#include <stdio.h>

void DisplayBoard(char b[3][3])
{
    int i, j;

    printf("\n");

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            printf(" %c ", b[i][j]);

            if(j < 2)
                printf("|");
        }

        printf("\n");

        if(i < 2)
            printf("---+---+---\n");
    }

    printf("\n");
}

int CheckWin(char b[3][3], char p)
{
    int i;

    for(i = 0; i < 3; i++)
    {
        if(b[i][0] == p && b[i][1] == p && b[i][2] == p)
            return 1;

        if(b[0][i] == p && b[1][i] == p && b[2][i] == p)
            return 1;
    }

    if(b[0][0] == p && b[1][1] == p && b[2][2] == p)
        return 1;

    if(b[0][2] == p && b[1][1] == p && b[2][0] == p)
        return 1;

    return 0;
}

void Instructions(void)
{
    printf("\n========== INSTRUCTIONS ==========\n");
    printf("1. Tic-Tac-Toe is played between two players.\n");
    printf("2. Player 1 chooses X or O.\n");
    printf("3. X always makes the first move.\n");
    printf("4. Enter a position from 1 to 9 to make a move.\n");
    printf("5. The first player to get three symbols in a row,\n");
    printf("   column, or diagonal wins.\n");
    printf("6. If all nine positions are filled without a winner,\n");
    printf("   the game ends in a draw.\n");
    printf("==================================\n\n");
}

int ReadNumber(int min, int max)
{
    int num, ch, result;

    while(1)
    {
        result = scanf("%d", &num);

        if(result == EOF)
            return -1;

        if(result == 1)
        {
            ch = getchar();

            while(ch != '\n' && ch != EOF)
            {
                if(ch != ' ' && ch != '\t' && ch != '\r')
                {
                    while(ch != '\n' && ch != EOF)
                        ch = getchar();

                    printf("Invalid input! Enter a number between %d and %d: ", min, max);
                    break;
                }

                ch = getchar();
            }

            if(ch == '\n' || ch == EOF)
            {
                if(num >= min && num <= max)
                    return num;

                printf("Invalid choice! Enter a number between %d and %d: ", min, max);
            }
        }
        else
        {
            ch = getchar();

            while(ch != '\n' && ch != EOF)
                ch = getchar();

            printf("Invalid input! Enter a number between %d and %d: ", min, max);
        }
    }
}

void PlayGame(void)
{
    char b[3][3] = {
        {'1', '2', '3'},
        {'4', '5', '6'},
        {'7', '8', '9'}
    };

    char p1, p2, p;
    int pos, row, col, turn;

    printf("\nPlayer 1, choose your symbol (X/O): ");

    while(1)
    {
        if(scanf(" %c", &p1) != 1)
            return;

        if(p1 == 'x' || p1 == 'X')
        {
            p1 = 'X';
            p2 = 'O';
            break;
        }
        else if(p1 == 'o' || p1 == 'O')
        {
            p1 = 'O';
            p2 = 'X';
            break;
        }
        else
        {
            printf("Invalid choice! Please enter X or O: ");
        }
    }

    printf("\nPlayer 1: %c\n", p1);
    printf("Player 2: %c\n", p2);
    printf("X always starts the game.\n");

    for(turn = 0; turn < 9; turn++)
    {
        p = (turn % 2 == 0) ? 'X' : 'O';

        DisplayBoard(b);

        if(p == p1)
            printf("Player 1 (%c)", p);
        else
            printf("Player 2 (%c)", p);

        printf(", enter position (1-9): ");

        pos = ReadNumber(1, 9);

        if(pos == -1)
            return;

        row = (pos - 1) / 3;
        col = (pos - 1) % 3;

        if(b[row][col] == 'X' || b[row][col] == 'O')
        {
            printf("Position already occupied! Try again.\n");
            turn--;
            continue;
        }

        b[row][col] = p;

        if(CheckWin(b, p))
        {
            DisplayBoard(b);

            if(p == p1)
                printf("Player 1 (%c) wins!\n", p);
            else
                printf("Player 2 (%c) wins!\n", p);

            return;
        }
    }

    DisplayBoard(b);
    printf("The game is a draw!\n");
}

int main(void)
{
    int choice, again;

    while(1)
    {
        printf("==== Tic-Tac-Toe ====\n");
        printf("1) Instructions\n");
        printf("2) Play against Friend\n");
        printf("3) Exit\n");
        printf("=====================\n");
        printf("Enter your choice: ");

        choice = ReadNumber(1, 3);

        if(choice == -1)
        {
            printf("\nExiting game. Goodbye!\n");
            break;
        }

        switch(choice)
        {
            case 1:
                Instructions();
                break;

            case 2:
                do
                {
                    PlayGame();

                    printf("\n==== Would you like to? ====\n");
                    printf("1) Play again\n");
                    printf("2) Exit\n");
                    printf("============================\n");
                    printf("Enter your choice: ");

                    again = ReadNumber(1, 2);

                    if(again == -1)
                    {
                        printf("\nExiting game. Goodbye!\n");
                        return 0;
                    }

                    if(again == 2)
                    {
                        printf("\nThanks for playing Tic-Tac-Toe!\n");
                        return 0;
                    }

                } while(again == 1);

                break;

            case 3:
                printf("\nThanks for playing Tic-Tac-Toe!\n");
                return 0;
        }

        printf("\n");
    }

    return 0;
}