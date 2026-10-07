#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LIMIT 10000

void clearInput(void)
{
    int c;

    c = getchar();
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}

int readNumber(void)
{
    int value;
    int result;

    result = scanf("%d", &value);

    if (result == EOF)
    {
        exit(0);
    }

    clearInput();

    if (result != 1)
    {
        return -1;
    }

    return value;
}

int showMainMenu(void)
{
    int choice;

    while (1)
    {
        printf("\n==== Number Guesser Game ====\n");
        printf("1) Instructions\n");
        printf("2) Play Now\n");
        printf("3) Exit\n");
        printf("=============================\n");
        printf("Enter your choice: ");

        choice = readNumber();

        if (choice >= 1 && choice <= 3)
        {
            return choice;
        }

        printf("Invalid choice. Please enter 1, 2, or 3.\n");
    }
}

int showInstructions(void)
{
    int choice;

    printf("\n==== Instructions ====\n");
    printf("a. The game generates a random number within the range you choose.\n");
    printf("b. You must guess the number.\n");
    printf("c. The number of attempts depends on the difficulty you select.\n");
    printf("d. After every wrong guess, the attempts remaining are shown.\n");
    printf("e. The game tells you if your guess is too high or too low.\n");
    printf("f. You win if you guess the number within your attempts.\n");
    printf("g. If you use all your attempts, the correct number is revealed.\n");

    while (1)
    {
        printf("\n2) Play Now\n");
        printf("3) Exit\n");
        printf("=======================\n");
        printf("Enter your choice (2 or 3): ");

        choice = readNumber();

        if (choice == 2 || choice == 3)
        {
            return choice;
        }

        printf("Invalid choice. Please enter 2 or 3.\n");
    }
}

int chooseAttempts(void)
{
    int choice;

    while (1)
    {
        printf("\n==== Choose Difficulty ====\n");
        printf("1) Easy\n");
        printf("2) Medium\n");
        printf("3) Hard\n");
        printf("===========================\n");
        printf("Enter your choice: ");

        choice = readNumber();

        if (choice == 1)
        {
            return 20;
        }
        else if (choice == 2)
        {
            return 10;
        }
        else if (choice == 3)
        {
            return 5;
        }

        printf("Invalid choice. Please enter 1, 2, or 3.\n");
    }
}

int readMaximumNumber(void)
{
    int maximum;

    while (1)
    {
        printf("\nEnter the maximum number: ");

        maximum = readNumber();

        if (maximum >= 2 && maximum <= MAX_LIMIT)
        {
            return maximum;
        }

        printf("Invalid number. Please enter a number between 2 and %d.\n", MAX_LIMIT);
    }
}

void playRound(int totalAttempts)
{
    int maximum;
    int secretNumber;
    int guess;
    int attemptsUsed;
    int won;

    attemptsUsed = 0;
    won = 0;

    maximum = readMaximumNumber();
    secretNumber = rand() % maximum + 1;

    printf("\n==== Guess the Number ====\n");
    printf("Guess a number between 1 and %d.\n", maximum);
    printf("You have %d attempts.\n", totalAttempts);

    while (attemptsUsed < totalAttempts && won == 0)
    {
        printf("\nEnter your guess: ");

        guess = readNumber();

        if (guess < 1 || guess > maximum)
        {
            printf("Invalid guess. Please enter a number between 1 and %d.\n", maximum);
        }
        else
        {
            attemptsUsed++;

            if (guess == secretNumber)
            {
                won = 1;
            }
            else
            {
                if (guess < secretNumber)
                {
                    printf("Too low!\n");
                }
                else
                {
                    printf("Too high!\n");
                }

                printf("Wrong guess!\n");
                printf("Attempts remaining: %d\n", totalAttempts - attemptsUsed);
            }
        }
    }

    if (won == 1)
    {
        printf("\nCongratulations!\n");
        printf("You guessed the correct number!\n");

        if (attemptsUsed == 1)
        {
            printf("You guessed it in 1 attempt!\n");
        }
        else
        {
            printf("You guessed it in %d attempts!\n", attemptsUsed);
        }
    }
    else
    {
        printf("\nGame Over!\n");
        printf("You used all your attempts.\n");
        printf("The correct number was: %d\n", secretNumber);
    }
}

int showAfterRoundMenu(void)
{
    int choice;

    while (1)
    {
        printf("\n==== Would you like to? ====\n");
        printf("1) Play again\n");
        printf("2) Change difficulty\n");
        printf("3) Exit\n");
        printf("============================\n");
        printf("Enter your choice: ");

        choice = readNumber();

        if (choice >= 1 && choice <= 3)
        {
            return choice;
        }

        printf("Invalid choice. Please enter 1, 2, or 3.\n");
    }
}

void playGame(void)
{
    int totalAttempts;
    int choice;

    totalAttempts = chooseAttempts();
    choice = 1;

    while (choice != 3)
    {
        playRound(totalAttempts);

        choice = showAfterRoundMenu();

        if (choice == 2)
        {
            totalAttempts = chooseAttempts();
        }
    }
}

int main(void)
{
    int choice;

    srand((unsigned int)time(NULL));

    choice = showMainMenu();

    if (choice == 1)
    {
        choice = showInstructions();
    }

    if (choice == 2)
    {
        playGame();
    }

    printf("\nThanks for playing!\n");
    printf("Goodbye!\n");

    return 0;
}