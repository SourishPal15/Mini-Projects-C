#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>
#endif

#define MIN_HARD_LIMIT -10000
#define MAX_HARD_LIMIT 10000

typedef struct
{
    int totalQuestions;
    double timeLimit;
    int minNumber;
    int maxNumber;
} Difficulty;

typedef struct
{
    int score;
    int attempts;
    int skips;
} QuizResult;

int parseWholeNumber(const char *text, int *value)
{
    int i = 0;
    int number = 0;

    if (text[0] == '\0')
        return 0;

    while (text[i] != '\0')
    {
        if (text[i] < '0' || text[i] > '9')
            return 0;

        number = number * 10 + (text[i] - '0');
        i++;
    }

    *value = number;
    return 1;
}

int parseSignedWholeNumber(const char *text, int *value)
{
    int i = 0;
    int sign = 1;
    int number = 0;

    if (text[0] == '\0')
        return 0;

    if (text[i] == '-')
    {
        sign = -1;
        i++;
    }
    else if (text[i] == '+')
    {
        i++;
    }

    if (text[i] == '\0')
        return 0;

    while (text[i] != '\0')
    {
        if (text[i] < '0' || text[i] > '9')
            return 0;

        number = number * 10 + (text[i] - '0');
        i++;
    }

    *value = number * sign;
    return 1;
}

int isValidNumber(const char *text)
{
    int i = 0;
    int digitFound = 0;
    int dotFound = 0;

    if (text[0] == '\0')
        return 0;

    if (text[0] == '+' || text[0] == '-')
        i++;

    if (text[i] == '\0')
        return 0;

    while (text[i] != '\0')
    {
        if (text[i] >= '0' && text[i] <= '9')
        {
            digitFound = 1;
        }
        else if (text[i] == '.' && !dotFound)
        {
            dotFound = 1;
        }
        else
        {
            return 0;
        }

        i++;
    }

    return digitFound;
}

#ifndef _WIN32

double getTimeInSeconds(void)
{
    struct timeval currentTime;

    gettimeofday(&currentTime, NULL);

    return currentTime.tv_sec + currentTime.tv_usec / 1000000.0;
}

int readLineWithTimeout(char *buffer, int size, double seconds)
{
    struct termios oldSettings;
    struct termios newSettings;

    tcgetattr(STDIN_FILENO, &oldSettings);

    newSettings = oldSettings;
    newSettings.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);

    int index = 0;
    double startTime = getTimeInSeconds();

    while (1)
    {
        double elapsed = getTimeInSeconds() - startTime;

        if (elapsed >= seconds)
        {
            buffer[index] = '\0';
            tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
            printf("\n");
            return -1;
        }

        fd_set inputSet;
        struct timeval timeout;

        FD_ZERO(&inputSet);
        FD_SET(STDIN_FILENO, &inputSet);

        double remaining = seconds - elapsed;

        timeout.tv_sec = (int)remaining;
        timeout.tv_usec = (int)((remaining - timeout.tv_sec) * 1000000);

        int result = select(STDIN_FILENO + 1, &inputSet, NULL, NULL, &timeout);

        if (result < 0)
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
            return -1;
        }

        if (result == 0)
        {
            buffer[index] = '\0';
            tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
            printf("\n");
            return -1;
        }

        char ch;

        if (read(STDIN_FILENO, &ch, 1) <= 0)
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
            return -1;
        }

        if (ch == '\n' || ch == '\r')
        {
            buffer[index] = '\0';
            tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
            printf("\n");
            return 1;
        }

        if (ch == '\b' || ch == 127)
        {
            if (index > 0)
            {
                index--;
                printf("\b \b");
                fflush(stdout);
            }
        }
        else if (index < size - 1)
        {
            buffer[index++] = ch;
            printf("%c", ch);
            fflush(stdout);
        }
    }
}

#else

double getTimeInSeconds(void)
{
    return GetTickCount() / 1000.0;
}

int readLineWithTimeout(char *buffer, int size, double seconds)
{
    int index = 0;
    double startTime = getTimeInSeconds();

    while (1)
    {
        double elapsed = getTimeInSeconds() - startTime;

        if (elapsed >= seconds)
        {
            buffer[index] = '\0';
            printf("\n");
            return -1;
        }

        if (_kbhit())
        {
            char ch = _getch();

            if (ch == '\r')
            {
                buffer[index] = '\0';
                printf("\n");
                return 1;
            }

            if (ch == '\b')
            {
                if (index > 0)
                {
                    index--;
                    printf("\b \b");
                }
            }
            else if (index < size - 1)
            {
                buffer[index++] = ch;
                printf("%c", ch);
            }
        }

        Sleep(10);
    }
}

#endif

long toHundredths(double value)
{
    if (value < 0)
        return (long)(value * 100 - 0.5);

    return (long)(value * 100 + 0.5);
}

int checkAnswer(double userAnswer, double correctAnswer, char operator)
{
    if (operator == '/')
    {
        return toHundredths(userAnswer) == toHundredths(correctAnswer);
    }

    return fabs(userAnswer - correctAnswer) < 0.000001;
}

void showInstructions(void)
{
    printf("\n==== Instructions ====\n");
    printf("a. Choose a difficulty level before starting the quiz.\n");
    printf("b. Each difficulty has a different number range and time limit.\n");
    printf("c. Enter your answer before the time runs out.\n");
    printf("d. Correct answers increase your score.\n");
    printf("e. Wrong numerical answers count as attempts.\n");
    printf("f. Invalid input does not count as an attempt.\n");
    printf("g. If time runs out, the question is counted as a skip.\n");
    printf("h. Division answers are checked up to 2 decimal places.\n");
    printf("======================\n");
}

int readMenuChoice(int minimum, int maximum)
{
    char input[100];
    int choice;

    while (1)
    {
        printf("Enter your choice: ");
        scanf("%99s", input);

        if (parseWholeNumber(input, &choice) &&
            choice >= minimum &&
            choice <= maximum)
        {
            return choice;
        }

        printf("Invalid choice! Try again.\n");
    }
}

void getDifficulty(int choice, Difficulty *difficulty)
{
    if (choice == 1)
    {
        difficulty->totalQuestions = 5;
        difficulty->timeLimit = 15.0;
        difficulty->minNumber = 1;
        difficulty->maxNumber = 100;
    }
    else if (choice == 2)
    {
        difficulty->totalQuestions = 10;
        difficulty->timeLimit = 7.5;
        difficulty->minNumber = 50;
        difficulty->maxNumber = 100;
    }
}

int readLimit(char *message)
{
    char input[100];
    int value;

    while (1)
    {
        printf("%s", message);
        scanf("%99s", input);

        if (parseSignedWholeNumber(input, &value))
        {
            if (value >= MIN_HARD_LIMIT && value <= MAX_HARD_LIMIT)
                return value;
        }

        printf("Invalid limit! Enter a whole number between -10000 and 10000.\n");
    }
}

void setupHardDifficulty(Difficulty *difficulty)
{
    int lower;
    int upper;

    while (1)
    {
        lower = readLimit("Enter lower limit: ");
        upper = readLimit("Enter upper limit: ");

        if (lower < upper)
            break;

        printf("Lower limit must be smaller than upper limit!\n");
    }

    difficulty->totalQuestions = 15;
    difficulty->timeLimit = 5.0;
    difficulty->minNumber = lower;
    difficulty->maxNumber = upper;
}

int generateNumber(int minimum, int maximum)
{
    return minimum + rand() % (maximum - minimum + 1);
}

char generateOperator(void)
{
    char operators[] = {'+', '-', 'x', '/'};

    return operators[rand() % 4];
}

void generateQuestion(int minimum, int maximum, int *first, int *second, char *operator)
{
    *first = generateNumber(minimum, maximum);
    *second = generateNumber(minimum, maximum);

    *operator = generateOperator();

    if (*operator == '/')
    {
        while (*second == 0)
            *second = generateNumber(minimum, maximum);
    }
}

double calculateAnswer(int first, int second, char operator)
{
    if (operator == '+')
        return first + second;

    if (operator == '-')
        return first - second;

    if (operator == 'x')
        return first * second;

    return (double)first / second;
}

void askQuestion(int questionNumber, Difficulty difficulty, QuizResult *result)
{
    int first;
    int second;
    char operator;

    char input[100];

    double correctAnswer;
    double userAnswer;

    double remaining;
    int inputResult;

    generateQuestion(
        difficulty.minNumber,
        difficulty.maxNumber,
        &first,
        &second,
        &operator
    );

    correctAnswer = calculateAnswer(first, second, operator);

    printf("\nQ.%d) %d %c %d\n",
           questionNumber,
           first,
           operator,
           second);

    remaining = difficulty.timeLimit;

    while (remaining > 0)
    {
        printf("Answer: ");
        fflush(stdout);

        inputResult = readLineWithTimeout(input, sizeof(input), remaining);

        if (inputResult == -1)
        {
            printf("Uh oh! Time's up!\n");
            result->skips++;
            return;
        }

        if (!isValidNumber(input))
        {
            printf("Only numbers are allowed!\n");

            double usedTime;

            usedTime = difficulty.timeLimit;

            if (difficulty.timeLimit > 0)
                remaining = remaining - 0.1;

            if (remaining <= 0)
            {
                printf("Uh oh! Time's up!\n");
                result->skips++;
                return;
            }

            continue;
        }

        userAnswer = atof(input);

        result->attempts++;

        if (checkAnswer(userAnswer, correctAnswer, operator))
        {
            printf("Correct!\n");
            result->score++;
        }
        else
        {
            printf("Wrong\n");
        }

        return;
    }

    printf("Uh oh! Time's up!\n");
    result->skips++;
}

void showResults(QuizResult result, Difficulty difficulty)
{
    printf("\n==== Results ====\n");
    printf("1) Score: %d/%d\n",
           result.score,
           difficulty.totalQuestions);

    printf("2) Attempts: %d\n", result.attempts);
    printf("3) Skips: %d\n", result.skips);
    printf("=================\n");
}

void runQuiz(Difficulty difficulty)
{
    QuizResult result;

    result.score = 0;
    result.attempts = 0;
    result.skips = 0;

    for (int question = 1;
         question <= difficulty.totalQuestions;
         question++)
    {
        askQuestion(question, difficulty, &result);
    }

    showResults(result, difficulty);
}

void playQuizSessions(int difficultyChoice)
{
    Difficulty difficulty;
    int choice;

    while (1)
    {
        if (difficultyChoice == 1 || difficultyChoice == 2)
        {
            getDifficulty(difficultyChoice, &difficulty);
        }
        else
        {
            setupHardDifficulty(&difficulty);
        }

        while (1)
        {
            printf("\n==== Quiz ");

            if (difficultyChoice == 1)
                printf("(Easy Difficulty) ====\n");
            else if (difficultyChoice == 2)
                printf("(Medium Difficulty) ====\n");
            else
                printf("(Hard Difficulty) ====\n");

            runQuiz(difficulty);

            printf("\n==== Would you like to? ====\n");
            printf("1) Take quiz again\n");
            printf("2) Change Difficulty\n");
            printf("3) Exit\n");
            printf("============================\n");

            choice = readMenuChoice(1, 3);

            if (choice == 1)
            {
                continue;
            }

            if (choice == 2)
            {
                printf("\n==== Choose your difficulty ====\n");
                printf("1) Easy\n");
                printf("2) Medium\n");
                printf("3) Hard\n");
                printf("================================\n");

                difficultyChoice = readMenuChoice(1, 3);
                break;
            }

            return;
        }
    }
}

int main(void)
{
    int choice;
    int difficultyChoice;

    srand((unsigned int)time(NULL));

    while (1)
    {
        printf("\n==== Math Quiz ====\n");
        printf("1) Instructions\n");
        printf("2) Take Quiz now\n");
        printf("3) Exit\n");
        printf("===================\n");

        choice = readMenuChoice(1, 3);

        if (choice == 1)
        {
            showInstructions();
        }
        else if (choice == 2)
        {
            printf("\n==== Choose your difficulty ====\n");
            printf("1) Easy\n");
            printf("2) Medium\n");
            printf("3) Hard\n");
            printf("================================\n");

            difficultyChoice = readMenuChoice(1, 3);

            if (difficultyChoice == 3)
            {
                Difficulty hardDifficulty;
                setupHardDifficulty(&hardDifficulty);
            }

            playQuizSessions(difficultyChoice);
            break;
        }
        else
        {
            break;
        }
    }

    return 0;
}