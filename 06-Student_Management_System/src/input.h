#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>
#include <stdio.h>

typedef enum {
    INPUT_OK,
    INPUT_CANCELLED,
    INPUT_EOF
} InputStatus;

typedef enum {
    LINE_OK,
    LINE_TOO_LONG,
    LINE_EOF
} LineStatus;

LineStatus readLine(const char *prompt, char *buffer, size_t size);
int parseInteger(const char *text, int min, int max, int *value);
InputStatus promptInt(const char *prompt, int min, int max, int *value);
InputStatus promptText(const char *prompt, char *destination, size_t size,
                       int (*isValid)(const char *), const char *errorMessage);
InputStatus promptMenuChoice(int min, int max, int *choice);
int confirmAction(const char *question);
void reportAborted(InputStatus status);
void printCancelHint(void);

void trimWhitespace(char *text);
int isPrintableText(const char *text);
int compareIgnoreCase(const char *first, const char *second);
int equalsIgnoreCase(const char *first, const char *second);
int containsIgnoreCase(const char *text, const char *term);

void printLine(FILE *out, char symbol);
void printHeader(const char *title);

#endif