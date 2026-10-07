#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void readLine(char *buf, int size)
{
    if (fgets(buf, size, stdin) == NULL)
    {
        printf("\n");
        exit(0);
    }

    if (strchr(buf, '\n') == NULL)
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
    }

    buf[strcspn(buf, "\r\n")] = '\0';
}

int askChoice(int low, int high)
{
    char line[20];

    while (1)
    {
        printf("Enter your choice: ");
        readLine(line, sizeof(line));

        if (strlen(line) == 1 && line[0] >= '0' + low && line[0] <= '0' + high)
        {
            return line[0] - '0';
        }

        printf("Invalid choice. Please try again.\n\n");
    }
}

void showMainMenu(void)
{
    printf("\n");
    printf("================================\n");
    printf("     Scrambled Word Guesser\n");
    printf("================================\n");
    printf("\n");
    printf("1) Instructions\n");
    printf("2) Play now\n");
    printf("3) Exit\n");
    printf("\n");
}

void showInstructions(void)
{
    printf("\n");
    printf("================================\n");
    printf("          Instructions\n");
    printf("================================\n");
    printf("\n");
    printf("a. A random word will be selected from the word list.\n");
    printf("b. The selected word will be scrambled randomly.\n");
    printf("c. You must guess the original word.\n");
    printf("d. Your guess is not case-sensitive.\n");
    printf("e. Spaces are not allowed in your guess.\n");
    printf("f. You must guess the word within the given number of attempts.\n");
    printf("g. The number of attempts depends on the difficulty level.\n");
    printf("\n");
    printf("2) Play now\n");
    printf("3) Exit\n");
    printf("================================\n");
    printf("\n");
}

int chooseDifficulty(void)
{
    int choice;

    printf("\n");
    printf("================================\n");
    printf("      Choose your Difficulty\n");
    printf("================================\n");
    printf("\n");
    printf("1) Easy\n");
    printf("2) Medium\n");
    printf("3) Hard\n");
    printf("4) Extra Hard\n");
    printf("\n");

    choice = askChoice(1, 4);

    if (choice == 1)
    {
        return 15;
    }
    if (choice == 2)
    {
        return 10;
    }
    if (choice == 3)
    {
        return 5;
    }
    return 1;
}

void showReplayMenu(void)
{
    printf("\n");
    printf("================================\n");
    printf("      Would you like to?\n");
    printf("================================\n");
    printf("\n");
    printf("1) Play Again\n");
    printf("2) Change Difficulty\n");
    printf("3) Exit\n");
    printf("\n");
    printf("================================\n");
    printf("\n");
}

void goodbye(void)
{
    printf("\nThank you for playing!\n");
}

void toUpperCase(char *s)
{
    int i;

    for (i = 0; s[i] != '\0'; i++)
    {
        s[i] = toupper((unsigned char)s[i]);
    }
}

int nextWord(FILE *f, char *word)
{
    char line[100];
    int len;
    int i;
    int ok;

    while (fgets(line, sizeof(line), f) != NULL)
    {
        line[strcspn(line, "\r\n")] = '\0';

        len = strlen(line);
        while (len > 0 && (line[len - 1] == ' ' || line[len - 1] == '\t'))
        {
            line[len - 1] = '\0';
            len--;
        }

        ok = (len > 0);
        for (i = 0; line[i] != '\0'; i++)
        {
            if (!isalpha((unsigned char)line[i]))
            {
                ok = 0;
            }
        }

        if (ok)
        {
            strcpy(word, line);
            return 1;
        }
    }

    return 0;
}

int pickWord(char *word)
{
    FILE *f;
    int count = 0;
    int pick;
    int i;

    f = fopen("words.txt", "r");
    if (f == NULL)
    {
        printf("\nError: could not open words.txt.\n");
        return 0;
    }

    while (nextWord(f, word))
    {
        count++;
    }

    if (count == 0)
    {
        fclose(f);
        printf("\nError: words.txt is empty or has no valid words.\n");
        return 0;
    }

    pick = rand() % count;
    rewind(f);

    for (i = 0; i <= pick; i++)
    {
        nextWord(f, word);
    }

    fclose(f);
    return 1;
}

void scramble(char *s)
{
    int i;
    int j;
    char temp;

    for (i = strlen(s) - 1; i > 0; i--)
    {
        j = rand() % (i + 1);
        temp = s[i];
        s[i] = s[j];
        s[j] = temp;
    }
}

void showScrambled(char *s)
{
    int i;

    printf("Scrambled Word: ");
    for (i = 0; s[i] != '\0'; i++)
    {
        printf("%c", s[i]);
        if (s[i + 1] != '\0')
        {
            printf(" ");
        }
    }
    printf("\n\n");
}

void readGuess(char *guess)
{
    int i;
    int hasSpace;
    int onlyLetters;

    while (1)
    {
        printf("Enter your Guess: ");
        readLine(guess, 100);

        if (guess[0] == '\0')
        {
            printf("Your guess cannot be empty. Please try again.\n\n");
            continue;
        }

        hasSpace = 0;
        onlyLetters = 1;
        for (i = 0; guess[i] != '\0'; i++)
        {
            if (guess[i] == ' ')
            {
                hasSpace = 1;
            }
            else if (!isalpha((unsigned char)guess[i]))
            {
                onlyLetters = 0;
            }
        }

        if (hasSpace)
        {
            printf("Spaces are not allowed. Please enter the word without spaces.\n\n");
            continue;
        }

        if (!onlyLetters)
        {
            printf("Please use letters only.\n\n");
            continue;
        }

        toUpperCase(guess);
        return;
    }
}

int playRound(int attempts)
{
    char word[100];
    char scrambled[100];
    char guess[100];

    if (!pickWord(word))
    {
        return 0;
    }

    toUpperCase(word);
    strcpy(scrambled, word);
    scramble(scrambled);

    printf("\nAttempts remaining: %d\n\n", attempts);

    while (attempts > 0)
    {
        showScrambled(scrambled);
        readGuess(guess);

        if (strcmp(guess, word) == 0)
        {
            printf("\n");
            printf("================================\n");
            printf("          Correct!\n");
            printf("================================\n");
            printf("\n");
            printf("The word was: %s\n", word);
            printf("\n");
            printf("You guessed the word correctly!\n");
            return 1;
        }

        attempts--;

        if (attempts > 0)
        {
            printf("\nIncorrect guess.\n\n");
            printf("Attempts remaining: %d\n\n", attempts);
        }
    }

    printf("\n");
    printf("================================\n");
    printf("          Game Over\n");
    printf("================================\n");
    printf("\n");
    printf("The word was: %s\n", word);
    printf("\n");
    printf("Better luck next time!\n");
    return 1;
}

void playGame(void)
{
    int attempts;
    int choice;

    attempts = chooseDifficulty();

    while (1)
    {
        if (!playRound(attempts))
        {
            return;
        }

        showReplayMenu();
        choice = askChoice(1, 3);

        if (choice == 2)
        {
            attempts = chooseDifficulty();
        }
        else if (choice == 3)
        {
            goodbye();
            return;
        }
    }
}

int main(void)
{
    int choice;

    srand((unsigned)time(NULL));

    while (1)
    {
        showMainMenu();
        choice = askChoice(1, 3);

        if (choice == 1)
        {
            showInstructions();
            choice = askChoice(2, 3);
        }

        if (choice == 2)
        {
            playGame();
            return 0;
        }

        if (choice == 3)
        {
            goodbye();
            return 0;
        }
    }
}

/*

MAKE A FILE EXACTLY NAMED 'words.txt' AND PASTE THESE WORDS HERE:
START COPYING FROM THE START OF 'apple' AND END AT 'dream'  
MAKE SURE THERE ARE NO EXTRA SPACES IN BETWEEN 

*/