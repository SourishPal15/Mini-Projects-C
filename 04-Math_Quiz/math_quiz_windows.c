#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#define EASY 1
#define MEDIUM 2
#define HARD 3

#define CORRECT_ANSWER 1
#define WRONG_ANSWER 0
#define TIMED_OUT -1

#define INPUT_SIZE 100
#define MAX_LIMIT 10000

void showMainMenu(void);
void showInstructions(void);
int chooseDifficulty(void);
void playQuizSessions(int difficulty);
void startQuiz(int difficulty);
int askQuestion(int number, int first, int second, int operation, int limitMillis);
void showResults(int score, int attempts, int skips, int total);
void showAfterQuizMenu(void);
int getUserChoice(int minChoice, int maxChoice);
int readLimit(const char *prompt);
void readLine(char *buffer, int size);
int parseWholeNumber(const char *text, int *value);
int isValidNumber(const char *text);
int randomNumber(int low, int high);
int generateSecondNumber(int low, int high, int operation);
char getOperatorSymbol(int operation);
int isCorrect(double userValue, int first, int second, int operation);
double absoluteValue(double value);
long toHundredths(double value);
int readLineWithTimeout(char *buffer, int size, int limitMillis);

int main(void)
{
    int choice;
    int difficulty;
    int running = 1;

    srand((unsigned int)time(NULL));

    while (running)
    {
        showMainMenu();
        choice = getUserChoice(1, 3);

        switch (choice)
        {
            case 1:
                showInstructions();
                break;

            case 2:
                difficulty = chooseDifficulty();
                playQuizSessions(difficulty);
                running = 0;
                break;

            case 3:
                running = 0;
                break;
        }
    }

    return 0;
}

void showMainMenu(void)
{
    printf("\n==== Math Quiz ====\n");
    printf("1) Instructions\n");
    printf("2) Take Quiz now\n");
    printf("3) Exit\n");
    printf("===================\n");
}

void showInstructions(void)
{
    printf("\n==== Instructions ====\n");
    printf("a. Choose a difficulty level before starting the quiz.\n");
    printf("b. Each difficulty has a different number of questions and time limit.\n");
    printf("c. Each question contains two randomly generated numbers and an operator.\n");
    printf("d. Enter your answer before the time limit expires.\n");
    printf("e. Addition, subtraction and multiplication answers must match exactly.\n");
    printf("f. Division answers are checked up to 2 decimal places.\n");
    printf("g. Only numeric answers are accepted.\n");
    printf("h. If time runs out, the question is counted as a skip.\n");
    printf("i. Your score, attempts and skips are displayed at the end.\n");
    printf("j. You can take the quiz again, change difficulty, or exit.\n");
    printf("======================\n");
}

int chooseDifficulty(void)
{
    printf("\n==== Choose your difficulty ====\n");
    printf("1) Easy\n");
    printf("2) Medium\n");
    printf("3) Hard\n");
    printf("================================\n");

    return getUserChoice(1, 3);
}

void playQuizSessions(int difficulty)
{
    int choice;
    int finished = 0;

    while (!finished)
    {
        startQuiz(difficulty);
        showAfterQuizMenu();
        choice = getUserChoice(1, 3);

        switch (choice)
        {
            case 1:
                break;

            case 2:
                difficulty = chooseDifficulty();
                break;

            case 3:
                finished = 1;
                break;
        }
    }
}

void startQuiz(int difficulty)
{
    int low = 1;
    int high = 100;
    int total = 5;
    int limitMillis = 15000;
    int score = 0;
    int attempts = 0;
    int skips = 0;
    int number;
    int first;
    int second;
    int operation;
    int result;

    switch (difficulty)
    {
        case EASY:
            printf("\n==== Quiz (Easy Difficulty) ====\n");
            break;

        case MEDIUM:
            low = 50;
            high = 100;
            total = 10;
            limitMillis = 7500;
            printf("\n==== Quiz (Medium Difficulty) ====\n");
            break;

        case HARD:
            total = 15;
            limitMillis = 5000;

            printf("\n");

            do
            {
                low = readLimit("Enter lower limit: ");
                high = readLimit("Enter upper limit: ");

                if (low >= high)
                    printf("Lower limit must be less than upper limit!\n");

            } while (low >= high);

            printf("\n==== Quiz (Hard Difficulty) ====\n");
            break;
    }

    for (number = 1; number <= total; number++)
    {
        first = randomNumber(low, high);
        operation = rand() % 4;
        second = generateSecondNumber(low, high, operation);

        result = askQuestion(
            number,
            first,
            second,
            operation,
            limitMillis
        );

        switch (result)
        {
            case CORRECT_ANSWER:
                attempts++;
                score++;
                break;

            case WRONG_ANSWER:
                attempts++;
                break;

            case TIMED_OUT:
                skips++;
                break;
        }

        printf("\n");
    }

    showResults(score, attempts, skips, total);
}

int askQuestion(
    int number,
    int first,
    int second,
    int operation,
    int limitMillis
)
{
    char input[INPUT_SIZE];
    int remaining = limitMillis;
    int used;

    printf("Q.%d) %d %c %d\n",
           number,
           first,
           getOperatorSymbol(operation),
           second);

    while (1)
    {
        printf("Answer: ");
        fflush(stdout);

        used = readLineWithTimeout(
            input,
            INPUT_SIZE,
            remaining
        );

        if (used < 0)
        {
            printf("\nUh oh! Time's up!\n");
            return TIMED_OUT;
        }

        remaining -= used;

        if (remaining <= 0)
        {
            printf("\nUh oh! Time's up!\n");
            return TIMED_OUT;
        }

        if (!isValidNumber(input))
        {
            printf("Only numbers are allowed!\n");
            continue;
        }

        if (isCorrect(
                atof(input),
                first,
                second,
                operation))
        {
            printf("Correct!\n");
            return CORRECT_ANSWER;
        }

        printf("Wrong\n");
        return WRONG_ANSWER;
    }
}

void showResults(int score, int attempts, int skips, int total)
{
    printf("===============================\n");
    printf("\n==== Results ====\n");
    printf("1) Score: %d/%d\n", score, total);
    printf("2) Attempts: %d\n", attempts);
    printf("3) Skips: %d\n", skips);
    printf("=================\n");
}

void showAfterQuizMenu(void)
{
    printf("\n==== Would you like to? ====\n");
    printf("1) Take quiz again\n");
    printf("2) Change Difficulty\n");
    printf("3) Exit\n");
    printf("============================\n");
}

int getUserChoice(int minChoice, int maxChoice)
{
    char line[INPUT_SIZE];
    int choice;

    while (1)
    {
        printf("Enter choice: ");
        fflush(stdout);

        readLine(line, INPUT_SIZE);

        if (parseWholeNumber(line, &choice) &&
            choice >= minChoice &&
            choice <= maxChoice)
        {
            return choice;
        }

        printf(
            "Invalid choice! Enter a number from %d to %d.\n",
            minChoice,
            maxChoice
        );
    }
}

int readLimit(const char *prompt)
{
    char line[INPUT_SIZE];
    int value;

    while (1)
    {
        printf("%s", prompt);
        fflush(stdout);

        readLine(line, INPUT_SIZE);

        if (parseWholeNumber(line, &value))
        {
            if (value <= MAX_LIMIT)
                return value;
        }

        if (line[0] == '-')
        {
            int i = 1;
            int valid = 1;
            int digits = 0;
            int result = 0;

            while (line[i] >= '0' && line[i] <= '9')
            {
                result = result * 10 + (line[i] - '0');
                digits++;
                i++;
            }

            while (line[i] == ' ' || line[i] == '\t')
                i++;

            if (digits > 0 && digits <= 5 && line[i] == '\0')
            {
                value = -result;

                if (value >= -MAX_LIMIT)
                    return value;
            }
        }

        printf(
            "Invalid input! Enter a whole number from -%d to %d.\n",
            MAX_LIMIT,
            MAX_LIMIT
        );
    }
}

void readLine(char *buffer, int size)
{
    int length = 0;
    int ch;

    if (fgets(buffer, size, stdin) == NULL)
        exit(0);

    while (buffer[length] != '\0' &&
           buffer[length] != '\n')
    {
        length++;
    }

    if (buffer[length] == '\n')
    {
        buffer[length] = '\0';
    }
    else
    {
        ch = getchar();

        while (ch != '\n' && ch != EOF)
            ch = getchar();
    }
}

int parseWholeNumber(const char *text, int *value)
{
    int i = 0;
    int digits = 0;
    int result = 0;

    while (text[i] == ' ' || text[i] == '\t')
        i++;

    while (text[i] >= '0' && text[i] <= '9')
    {
        if (digits < 9)
            result = result * 10 + (text[i] - '0');

        digits++;
        i++;
    }

    while (text[i] == ' ' ||
           text[i] == '\t' ||
           text[i] == '\r' ||
           text[i] == '\n')
    {
        i++;
    }

    if (digits == 0 ||
        digits > 9 ||
        text[i] != '\0')
    {
        return 0;
    }

    *value = result;

    return 1;
}

int isValidNumber(const char *text)
{
    int i = 0;
    int digits = 0;
    int dots = 0;

    while (text[i] == ' ' || text[i] == '\t')
        i++;

    if (text[i] == '-' || text[i] == '+')
        i++;

    while ((text[i] >= '0' && text[i] <= '9') ||
           text[i] == '.')
    {
        if (text[i] == '.')
            dots++;
        else
            digits++;

        i++;
    }

    while (text[i] == ' ' ||
           text[i] == '\t' ||
           text[i] == '\r' ||
           text[i] == '\n')
    {
        i++;
    }

    return text[i] == '\0' &&
           digits > 0 &&
           dots <= 1;
}

int randomNumber(int low, int high)
{
    return low + rand() % (high - low + 1);
}

int generateSecondNumber(int low, int high, int operation)
{
    int number = randomNumber(low, high);

    while (operation == 3 && number == 0)
        number = randomNumber(low, high);

    return number;
}

char getOperatorSymbol(int operation)
{
    switch (operation)
    {
        case 0:
            return '+';

        case 1:
            return '-';

        case 2:
            return 'x';

        default:
            return '/';
    }
}

int isCorrect(
    double userValue,
    int first,
    int second,
    int operation
)
{
    double expected;
    long userHundredths;
    long expectedHundredths;

    switch (operation)
    {
        case 0:
            expected = (double)first + second;
            break;

        case 1:
            expected = (double)first - second;
            break;

        case 2:
            expected = (double)first * second;
            break;

        default:
            expected = (double)first / second;

            userHundredths = toHundredths(userValue);
            expectedHundredths = toHundredths(expected);

            return userHundredths == expectedHundredths;
    }

    return absoluteValue(userValue - expected) < 0.000001;
}

double absoluteValue(double value)
{
    if (value < 0)
        return -value;

    return value;
}

long toHundredths(double value)
{
    if (value < 0)
        return (long)(value * 100 - 0.5);

    return (long)(value * 100 + 0.5);
}

int readLineWithTimeout(
    char *buffer,
    int size,
    int limitMillis
)
{
    DWORD startTime;
    DWORD currentTime;
    DWORD elapsed;
    int length = 0;
    int key;

    while (_kbhit())
        _getch();

    startTime = GetTickCount();

    while (1)
    {
        currentTime = GetTickCount();
        elapsed = currentTime - startTime;

        if (elapsed >= (DWORD)limitMillis)
            return -1;

        if (!_kbhit())
        {
            Sleep(10);
            continue;
        }

        key = _getch();

        if (key == '\r' || key == '\n')
        {
            putchar('\n');
            fflush(stdout);

            buffer[length] = '\0';

            return (int)elapsed;
        }

        if (key == '\b')
        {
            if (length > 0)
            {
                length--;
                printf("\b \b");
                fflush(stdout);
            }

            continue;
        }

        if (key == 0 || key == 224)
        {
            _getch();
            continue;
        }

        if (key >= 32 &&
            key < 127 &&
            length < size - 1)
        {
            buffer[length] = (char)key;
            length++;

            putchar(key);
            fflush(stdout);
        }
    }
}
