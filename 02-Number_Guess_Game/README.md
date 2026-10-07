# Number Guesser Game

A simple **Number Guesser Game written in C** where the player chooses a difficulty, selects a number range, and tries to guess a randomly generated number within a limited number of attempts.

This project focuses on practicing fundamental C programming concepts such as:

* Functions
* `if-else`
* `while` loops
* Input validation
* Random number generation
* Variables and constants
* Function parameters and return values
* Basic program flow and control

---

## Game Preview

```text
==== Number Guesser Game ====
1) Instructions
2) Play Now
3) Exit
=============================
```

After selecting **Play Now**, the player chooses a difficulty:

```text
==== Choose Difficulty ====
1) Easy
2) Medium
3) Hard
===========================
```

### Difficulty Levels

| Difficulty | Attempts |
| ---------- | -------- |
| Easy       | 20       |
| Medium     | 10       |
| Hard       | 5        |

The player then enters the maximum number for the guessing range.

For example:

```text
Enter the maximum number: 100
```

The game generates a number between:

```text
1 and 100
```

---

## How the Game Works

The game follows this general flow:

```text
Start
  |
  v
Main Menu
  |
  +---- Instructions
  |
  +---- Play Now
  |       |
  |       v
  |   Choose Difficulty
  |       |
  |       v
  |   Enter Maximum
  |       |
  |       v
  |   Generate Random Number
  |       |
  |       v
  |   Guessing Round
  |       |
  |       +---- Correct ----> Win
  |       |
  |       +---- Attempts End -> Game Over
  |                         |
  |                         v
  |                   After Round Menu
  |                         |
  |              +----------+----------+
  |              |          |          |
  |              v          v          v
  |          Play Again  Change     Exit
  |                     Difficulty
```

---

## Main Features

### 1. Main Menu

When the program starts, the player gets three choices:

```text
1) Instructions
2) Play Now
3) Exit
```

The program keeps asking until a valid option between `1` and `3` is entered.

---

### 2. Instructions

The Instructions section explains the basic rules of the game.

It also provides:

```text
2) Play Now
3) Exit
```

This allows the player to start the game directly after reading the instructions.

---

### 3. Difficulty Selection

The player can select one of three difficulty levels.

#### Easy

```text
20 attempts
```

#### Medium

```text
10 attempts
```

#### Hard

```text
5 attempts
```

The selected difficulty is stored as the total number of attempts and is reused for subsequent rounds when the player chooses **Play Again**.

---

### 4. Custom Number Range

The player chooses the maximum number for the game.

For example:

```text
Enter the maximum number: 500
```

The game then generates a number between:

```text
1 and 500
```

The current program accepts maximum values from:

```text
2 to 10000
```

The upper limit is controlled by:

```c
#define MAX_LIMIT 10000
```

This makes it easy to change the maximum allowed range later.

---

### 5. Random Number Generation

The program uses:

```c
rand()
```

to generate the secret number.

The random number is generated using:

```c
secretNumber = rand() % maximum + 1;
```

This produces a number from:

```text
1 to maximum
```

The random number generator is seeded once at the beginning of the program using:

```c
srand((unsigned int)time(NULL));
```

This ensures that the game does not generate the same sequence of random numbers every time the program runs.

---

## Guessing System

During a round, the player enters guesses until:

* The correct number is guessed, or
* All available attempts are used.

For every valid guess, the program compares the player's guess with the secret number.

### Guess is Too Low

```text
Too low!
```

### Guess is Too High

```text
Too high!
```

### Guess is Correct

```text
Congratulations!
You guessed the correct number!
```

The program also displays the number of attempts used.

---

## Attempts System

Only valid guesses count as attempts.

For example, if the player has 20 attempts and makes a wrong guess:

```text
Wrong guess!
Attempts remaining: 19
```

An invalid guess outside the selected range does **not** consume an attempt.

For example:

```text
Invalid guess. Please enter a number between 1 and 100.
```

This allows the player to correct an invalid input without losing an attempt.

---

## Game Over

If the player uses all available attempts without finding the secret number, the game displays:

```text
Game Over!
You used all your attempts.
The correct number was: 73
```

The secret number is revealed at the end of the round.

---

## After-Round Menu

After every completed round, the player gets three choices:

```text
==== Would you like to? ====
1) Play again
2) Change difficulty
3) Exit
============================
```

### Play Again

Starts another round using the **same difficulty**.

A new maximum range can be entered and a new random number is generated.

### Change Difficulty

Returns to the difficulty selection menu.

The player can select Easy, Medium, or Hard before starting another round.

### Exit

Ends the game.

---

# Functions

The program is divided into several functions. Each function handles a specific part of the game.

---

## `clearInput()`

```c
void clearInput(void)
```

Clears unwanted characters remaining in the input buffer.

It reads characters until it reaches:

```text
\n
```

or:

```text
EOF
```

This helps prevent invalid input from interfering with the next input operation.

---

## `readNumber()`

```c
int readNumber(void)
```

Handles integer input throughout the program.

It uses:

```c
scanf("%d", &value);
```

The function checks whether the input was successfully read.

If the input is invalid, it returns:

```text
-1
```

If the input reaches `EOF`, the program exits.

This function also calls:

```c
clearInput();
```

after reading input.

Having one common input function keeps the rest of the program simpler.

---

## `showMainMenu()`

```c
int showMainMenu(void)
```

Displays the main menu:

```text
1) Instructions
2) Play Now
3) Exit
```

It continues asking until the player enters a valid option.

The function returns:

```text
1, 2, or 3
```

depending on the player's selection.

---

## `showInstructions()`

```c
int showInstructions(void)
```

Displays the instructions for the game.

After displaying the instructions, it provides:

```text
2) Play Now
3) Exit
```

The function returns the selected option.

---

## `chooseAttempts()`

```c
int chooseAttempts(void)
```

Displays the difficulty menu.

The function converts the selected difficulty into the corresponding number of attempts:

```text
Easy   → 20
Medium → 10
Hard   → 5
```

Instead of storing the words `"Easy"`, `"Medium"`, and `"Hard"`, the program only needs the number of attempts for the actual game logic.

---

## `readMaximumNumber()`

```c
int readMaximumNumber(void)
```

Asks the player for the maximum number used for the guessing range.

The function only accepts values between:

```text
2 and 10000
```

The upper limit comes from:

```c
#define MAX_LIMIT 10000
```

Invalid values cause the function to ask again.

---

## `playRound()`

```c
void playRound(int totalAttempts)
```

This is the main gameplay function.

It:

1. Gets the maximum number.
2. Generates the secret number.
3. Displays the guessing range.
4. Accepts guesses.
5. Checks whether guesses are valid.
6. Compares guesses with the secret number.
7. Displays whether the guess is too high or too low.
8. Tracks attempts.
9. Displays attempts remaining.
10. Determines whether the player wins or loses.

The function receives the number of available attempts through:

```c
totalAttempts
```

This allows the same gameplay function to work with Easy, Medium, and Hard difficulty.

---

## `showAfterRoundMenu()`

```c
int showAfterRoundMenu(void)
```

Displays the menu shown after a round:

```text
1) Play again
2) Change difficulty
3) Exit
```

It validates the player's choice and returns the selected option.

---

## `playGame()`

```c
void playGame(void)
```

Controls the overall game session.

It first asks the player to select a difficulty.

Then it repeatedly:

1. Starts a round.
2. Displays the result.
3. Shows the after-round menu.
4. Starts another round if requested.
5. Changes difficulty if requested.
6. Exits when the player chooses Exit.

One important part of this function is:

```c
if (choice == 2)
{
    totalAttempts = chooseAttempts();
}
```

This means the difficulty only changes when the player explicitly chooses **Change difficulty**.

If the player chooses **Play again**, the existing `totalAttempts` value is kept.

---

## `main()`

```c
int main(void)
```

This is where program execution begins.

It first seeds the random number generator:

```c
srand((unsigned int)time(NULL));
```

Then it displays the main menu and handles the initial choice.

If the player chooses:

```text
Instructions
```

the instructions are displayed.

If the player chooses:

```text
Play Now
```

the game begins.

At the end, the program displays:

```text
Thanks for playing!
Goodbye!
```

---

# Input Validation

The program performs validation at multiple levels.

### Main Menu

Valid:

```text
1, 2, 3
```

### Difficulty Menu

Valid:

```text
1, 2, 3
```

### After-Round Menu

Valid:

```text
1, 2, 3
```

### Maximum Number

Valid:

```text
2 to 10000
```

### Guess

The guess must be within:

```text
1 to maximum
```

Invalid guesses do not consume an attempt.

---

# Constants

The program defines:

```c
#define MAX_LIMIT 10000
```

This is the maximum value the player can enter as the upper limit.

Using a constant instead of directly writing `10000` throughout the program makes the limit easier to modify.

For example:

```c
#define MAX_LIMIT 50000
```

could be used later if a larger range is desired.

---

# Libraries Used

The program uses three standard C libraries.

### `stdio.h`

Used for input and output functions such as:

```c
printf()
scanf()
getchar()
```

### `stdlib.h`

Used for:

```c
rand()
srand()
exit()
```

### `time.h`

Used for:

```c
time()
```

which provides the value used to seed the random number generator.

---

# Concepts Practiced

This project provides practice with several fundamental C concepts:

```text
Variables
Constants
Functions
Function parameters
Return values
if / else if / else
while loops
Input validation
scanf()
getchar()
rand()
srand()
time()
Boolean-style flags
Random number generation
Program flow
Menu-driven programs
```

It is especially useful for understanding how multiple functions can work together to create a complete terminal application.

---

# Project Structure

The repository can be kept simple:

```text
Number-Guesser/
│
├── number_guesser.c
└── README.md
```

If compiled using GCC:

```bash
gcc number_guesser.c -o number_guesser.exe
```

Then run:

```bash
number_guesser.exe
```

---

# Example Gameplay

```text
==== Number Guesser Game ====
1) Instructions
2) Play Now
3) Exit
=============================
Enter your choice: 2

==== Choose Difficulty ====
1) Easy
2) Medium
3) Hard
===========================
Enter your choice: 1

Enter the maximum number: 100

==== Guess the Number ====
Guess a number between 1 and 100.
You have 20 attempts.

Enter your guess: 50
Too high!
Wrong guess!
Attempts remaining: 19

Enter your guess: 25
Too low!
Wrong guess!
Attempts remaining: 18

Enter your guess: 37
Congratulations!
You guessed the correct number!
You guessed it in 3 attempts!

==== Would you like to? ====
1) Play again
2) Change difficulty
3) Exit
============================
```

---

# Why This Project Is Useful

Although the game itself is simple, it is a good beginner C project because it combines several individual concepts into one working program.

Instead of writing isolated programs for loops, conditions, functions, and random numbers, this project uses them together to create an interactive application.

The project also demonstrates an important programming principle:

> **Break a larger problem into smaller functions.**

Each function has a specific responsibility, making the overall program easier to understand and maintain.

---

## Author

A beginner-friendly C programming project created as part of a journey toward learning C fundamentals and building practical terminal-based projects.
