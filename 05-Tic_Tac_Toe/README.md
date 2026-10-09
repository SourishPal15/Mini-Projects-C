# Tic-Tac-Toe

A simple **Tic-Tac-Toe game written in C** that allows two players to play against each other in the terminal. Players can choose their symbols, take turns making moves, and compete to get three symbols in a row, column, or diagonal.

The program uses standard C libraries and can be compiled and run on Windows, Linux, and macOS with a compatible C compiler.

## Features

* Two-player gameplay.
* Players can choose between X and O.
* X always makes the first move.
* A simple 3 × 3 game board displayed in the terminal.
* Automatic winner detection.
* Draw detection when all positions are filled without a winner.
* Input validation for menu choices and board positions.
* Prevention of moves in already occupied positions.
* Instructions menu explaining the rules.
* Option to play multiple rounds or exit the program.

**Note:** This version supports two human players playing on the same computer. It does not include a computer-controlled opponent.

## How the Game Works

The game board contains nine positions, numbered from 1 to 9.

```text
 1 | 2 | 3
---+---+---
 4 | 5 | 6
---+---+---
 7 | 8 | 9
```

Players choose their symbols, X or O, before the game begins. X always plays first, followed by O. Players enter the number corresponding to the position where they want to place their symbol.

The first player to complete a horizontal row, vertical column, or diagonal wins. If all nine positions are occupied and neither player wins, the game ends in a draw.

## Functions Explained

The program is divided into separate functions, each responsible for a specific task. This makes the code easier to understand, organise, and maintain.

### 1. `DisplayBoard()`

```c
void DisplayBoard(char b[3][3])
```

Displays the current game board in a readable 3 × 3 format.

* Accepts the game board as a two-dimensional character array.
* Uses nested `for` loops to display the rows and columns.
* Prints separators between positions.

The board is displayed after each valid move so players can see the current state of the game.

### 2. `CheckWin()`

```c
int CheckWin(char b[3][3], char p)
```

Checks whether a player has won the game.

It checks all possible winning combinations:

* Three matching symbols in any row.
* Three matching symbols in any column.
* Three matching symbols along either diagonal.

The function returns `1` if the specified player has won and `0` otherwise.

### 3. `Instructions()`

```c
void Instructions(void)
```

Displays the rules and instructions for playing the game.

It explains how to choose symbols, enter positions, win the game, and reach a draw.

### 4. `ReadNumber()`

```c
int ReadNumber(int min, int max)
```

Accepts and validates numerical input from the user.

* Ensures that the entered value falls within the specified range.
* Rejects non-numeric input.
* Rejects numbers outside the permitted range.
* Checks for unwanted extra characters after a number.
* Returns `-1` if the input stream reaches its end.

This function is used for menu choices and board positions, helping prevent invalid input from disrupting the game.

### 5. `PlayGame()`

```c
void PlayGame(void)
```

Contains the main gameplay logic.

Its responsibilities include:

* Initialising a new game board.
* Asking Player 1 to choose X or O.
* Assigning the remaining symbol to Player 2.
* Ensuring X makes the first move.
* Accepting and validating each player's move.
* Preventing players from occupying an already-filled position.
* Checking for a winner after every valid move.
* Declaring a draw if all nine positions are filled without a winner.

The function returns when a player wins, the game ends in a draw, or the input stream ends.

### 6. `main()`

```c
int main(void)
```

Controls the overall program flow.

It displays the main menu and allows the user to:

1. View the game instructions.
2. Start a game against a friend.
3. Exit the program.

After each round, a separate menu allows players to start another round immediately or exit the program.

The `do-while` loop makes it possible to play multiple rounds without returning to the main menu after every game.

## Program Flow

The program follows this sequence:

```text
          Start Program
                |
                v
          Display Menu
                |
        +-------+-------+
        |       |       |
        v       v       v
   Instructions Play   Exit
        |      Game      
        |        |       
        |        v       
        |   Choose X/O   
        |        |       
        |        v       
        |   Display Board
        |        |       
        |        v       
        |   Enter Position
        |        |
        |        v
        |   Validate Move
        |        |
        |        v
        |   Check Winner
        |        |
        |   +----+----+
        |   |         |
        |   v         v
        |  Winner   No Winner
        |   |         |
        |   |         v
        |   |    Board Full?
        |   |      /    \
        |   |    Yes     No
        |   |     |       |
        |   |    Draw   Next Turn
        |   |     |
        |   +-----+
        |         |
        |         v
        |     Play Again?
        |       /     \
        |     Yes      No
        |      |        |
        |      v        v
        |   New Game   Exit
        |
        v
    Main Menu
```

## Demo Gameplay

The following example demonstrates a game in which Player 1 chooses X and Player 2 receives O.

### Step 1: Main Menu

```text
==== Tic-Tac-Toe ====
1) Instructions
2) Play against Friend
3) Exit
=====================
Enter your choice: 2
```

### Step 2: Choose Symbols

```text
Player 1, choose your symbol (X/O): X

Player 1: X
Player 2: O
X always starts the game.
```

### Step 3: Play the Game

Suppose the players make these moves:

* Player 1 places X at position 1.
* Player 2 places O at position 4.
* Player 1 places X at position 2.
* Player 2 places O at position 5.
* Player 1 places X at position 3.

The final board looks like this:

```text
 X | X | X
---+---+---
 O | O | 6
---+---+---
 7 | 8 | 9
```

Since Player 1 has completed the first row, the program declares Player 1 the winner.

```text
Player 1 (X) wins!
```

### Step 4: Play Again or Exit

```text
==== Would you like to? ====
1) Play again
2) Exit
============================
Enter your choice:
```

Choosing `1` immediately starts a new round with an empty board. Choosing `2` terminates the program.

## Concepts Practised

This project helped me practise several fundamental C programming concepts by applying them to a complete, interactive game.

| Concept                | How It Was Used                                                            |
| ---------------------- | -------------------------------------------------------------------------- |
| Functions              | Divided the program into smaller, reusable tasks.                          |
| Two-dimensional arrays | Stored the 3 × 3 game board.                                               |
| Character variables    | Represented the X and O symbols.                                           |
| `if-else` statements   | Handled decisions such as winner detection and input validation.           |
| `for` loops            | Traversed the board and checked winning combinations.                      |
| `while` loops          | Repeated menu operations and validated user input.                         |
| `do-while` loops       | Allowed players to start another round immediately.                        |
| `switch-case`          | Handled the main menu choices.                                             |
| Ternary operator       | Selected the symbol for each turn.                                         |
| Input validation       | Prevented invalid menu choices and board positions.                        |
| Logical operators      | Checked rows, columns, diagonals, and occupied positions.                  |
| Return values          | Communicated whether a player had won or whether valid input was received. |
| Game logic             | Managed turns, moves, wins, draws, and restarting rounds.                  |

## What I Learned From This Project

Building this project helped me understand how individual C programming concepts work together to create a complete application.

Some of the key things I learned were:

* **Working with two-dimensional arrays:** I gained practical experience storing and accessing elements of a game board using rows and columns.
* **Breaking a problem into functions:** I learned how to separate the display, validation, instruction, and gameplay logic into different functions.
* **Implementing game logic:** I practised checking multiple winning conditions and managing the sequence of turns.
* **Handling user input:** I learned how to validate input, handle unexpected characters, and prevent players from selecting occupied positions.
* **Using loops effectively:** I understood how different loops can control menus, gameplay, and repeated rounds.
* **Managing program flow:** I gained a better understanding of how `return`, `break`, and loop conditions affect the execution of a program.
* **Combining concepts into a project:** Instead of practising each concept separately, I applied them together to build a functional game.

This project also helped me appreciate the importance of planning program flow and testing different scenarios, including wins, draws, invalid inputs, and repeated games.

## Conclusion

Tic-Tac-Toe was a practical project in my journey of learning C programming. It allowed me to move beyond individual practice programs and apply fundamental programming concepts to build an interactive game.

Through this project, I gained experience with functions, arrays, loops, conditional statements, input validation, and problem-solving. It also helped me understand how to organise code into smaller, manageable parts.

Although this version supports only two human players and does not include a computer-controlled opponent, it provides a solid foundation for understanding game logic and developing more complex programming projects in the future.
