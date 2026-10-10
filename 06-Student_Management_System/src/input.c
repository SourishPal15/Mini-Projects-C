#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "types.h"

void trimWhitespace(char *text)
{
    size_t start = 0;
    size_t length;

    while (text[start] != '\0' && isspace((unsigned char)text[start])) {
        start++;
    }
    if (start > 0) {
        memmove(text, text + start, strlen(text + start) + 1);
    }

    length = strlen(text);
    while (length > 0 && isspace((unsigned char)text[length - 1])) {
        text[length - 1] = '\0';
        length--;
    }
}

LineStatus readLine(const char *prompt, char *buffer, size_t size)
{
    size_t length;
    int ch;

    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return LINE_EOF;
    }

    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else if (!feof(stdin)) {
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
        return LINE_TOO_LONG;
    }

    trimWhitespace(buffer);
    return LINE_OK;
}

int parseInteger(const char *text, int min, int max, int *value)
{
    char *end;
    long number;

    if (text[0] == '\0') {
        return 0;
    }

    errno = 0;
    number = strtol(text, &end, 10);

    if (end == text || *end != '\0') {
        return 0;
    }
    if (errno == ERANGE || number < min || number > max) {
        return 0;
    }

    *value = (int)number;
    return 1;
}

InputStatus promptInt(const char *prompt, int min, int max, int *value)
{
    char line[LINE_LEN];
    LineStatus status;

    for (;;) {
        status = readLine(prompt, line, sizeof(line));
        if (status == LINE_EOF) {
            return INPUT_EOF;
        }
        if (status == LINE_TOO_LONG) {
            printf("  Error: that input is too long.\n");
            continue;
        }
        if (line[0] == '\0') {
            return INPUT_CANCELLED;
        }
        if (parseInteger(line, min, max, value)) {
            return INPUT_OK;
        }
        printf("  Error: enter a whole number from %d to %d.\n", min, max);
    }
}

InputStatus promptText(const char *prompt, char *destination, size_t size,
                       int (*isValid)(const char *), const char *errorMessage)
{
    char line[LINE_LEN];
    LineStatus status;

    for (;;) {
        status = readLine(prompt, line, sizeof(line));
        if (status == LINE_EOF) {
            return INPUT_EOF;
        }
        if (status == LINE_TOO_LONG) {
            printf("  Error: that input is too long.\n");
            continue;
        }
        if (line[0] == '\0') {
            return INPUT_CANCELLED;
        }
        if (strlen(line) >= size) {
            printf("  Error: enter at most %d characters.\n", (int)(size - 1));
            continue;
        }
        if (!isValid(line)) {
            printf("  Error: %s\n", errorMessage);
            continue;
        }
        snprintf(destination, size, "%s", line);
        return INPUT_OK;
    }
}

InputStatus promptMenuChoice(int min, int max, int *choice)
{
    char line[LINE_LEN];
    LineStatus status;

    for (;;) {
        status = readLine("Enter your choice: ", line, sizeof(line));
        if (status == LINE_EOF) {
            return INPUT_EOF;
        }
        if (status == LINE_TOO_LONG) {
            printf("  Error: that input is too long.\n");
            continue;
        }
        if (parseInteger(line, min, max, choice)) {
            return INPUT_OK;
        }
        printf("  Error: choose a number from %d to %d.\n", min, max);
    }
}

int confirmAction(const char *question)
{
    char line[LINE_LEN];
    char prompt[LINE_LEN];
    LineStatus status;

    snprintf(prompt, sizeof(prompt), "%s (y/n): ", question);

    for (;;) {
        status = readLine(prompt, line, sizeof(line));
        if (status == LINE_EOF) {
            return 0;
        }
        if (status == LINE_OK) {
            if (equalsIgnoreCase(line, "y") || equalsIgnoreCase(line, "yes")) {
                return 1;
            }
            if (equalsIgnoreCase(line, "n") || equalsIgnoreCase(line, "no")) {
                return 0;
            }
        }
        printf("  Please answer y or n.\n");
    }
}

void reportAborted(InputStatus status)
{
    if (status == INPUT_CANCELLED) {
        printf("Operation cancelled. No changes were made.\n");
    } else if (status == INPUT_EOF) {
        printf("\nInput ended. No changes were made.\n");
    }
}

void printCancelHint(void)
{
    printf("(Press Enter on an empty line at any prompt to cancel.)\n\n");
}

int isPrintableText(const char *text)
{
    size_t i;

    if (text[0] == '\0') {
        return 0;
    }
    for (i = 0; text[i] != '\0'; i++) {
        if (!isprint((unsigned char)text[i])) {
            return 0;
        }
    }
    return 1;
}

int compareIgnoreCase(const char *first, const char *second)
{
    int a;
    int b;

    while (*first != '\0' && *second != '\0') {
        a = tolower((unsigned char)*first);
        b = tolower((unsigned char)*second);
        if (a != b) {
            return a - b;
        }
        first++;
        second++;
    }
    return tolower((unsigned char)*first) - tolower((unsigned char)*second);
}

int equalsIgnoreCase(const char *first, const char *second)
{
    return compareIgnoreCase(first, second) == 0;
}

int containsIgnoreCase(const char *text, const char *term)
{
    size_t termLength = strlen(term);
    size_t i;
    size_t j;

    if (termLength == 0) {
        return 1;
    }

    for (i = 0; text[i] != '\0'; i++) {
        for (j = 0; j < termLength; j++) {
            if (text[i + j] == '\0') {
                return 0;
            }
            if (tolower((unsigned char)text[i + j]) != tolower((unsigned char)term[j])) {
                break;
            }
        }
        if (j == termLength) {
            return 1;
        }
    }
    return 0;
}

void printLine(FILE *out, char symbol)
{
    int i;

    for (i = 0; i < CONSOLE_WIDTH; i++) {
        fputc(symbol, out);
    }
    fputc('\n', out);
}

void printHeader(const char *title)
{
    printf("\n");
    printLine(stdout, '=');
    printf("%s\n", title);
    printLine(stdout, '=');
}