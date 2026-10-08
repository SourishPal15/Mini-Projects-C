# Math Quiz

A simple **Math Quiz game written in C**.

This project is **cross-platform** and can run on **Windows, Linux, and macOS** using the same C source file. Platform-specific code is automatically selected using conditional compilation.

The quiz generates random mathematical questions and gives the player a limited amount of time to answer each question.

## Features

* Three difficulty levels:

  * Easy
  * Medium
  * Hard
* Randomly generated mathematical questions
* Addition, subtraction, multiplication, and division
* Different number ranges for each difficulty
* Different time limits for each difficulty
* Timed input
* Supports decimal answers for division
* Supports negative numbers in Hard mode
* Counts score, attempts, and skips
* Allows the player to replay the quiz
* Allows changing the difficulty
* Works on Windows, Linux, and macOS

## Difficulty Levels

| Difficulty | Questions | Number Range | Time per Question |
| ---------- | --------: | ------------ | ----------------: |
| Easy       |         5 | 1 - 100      |        15 seconds |
| Medium     |        10 | 50 - 100     |       7.5 seconds |
| Hard       |        15 | User-defined |         5 seconds |

Hard mode allows the player to enter a custom range from **-10000 to 10000**.

## How the Code Works

The program starts by seeding the random number generator:

```c
srand((unsigned int)time(NULL));
```

This makes the generated questions different each time the program runs.

The program then displays the main menu:

```text
==== Math Quiz ====
1) Instructions
2) Take Quiz now
3) Exit
===================
```

The player can read the instructions, start the quiz, or exit.

### Choosing Difficulty

When the player starts the quiz, they choose a difficulty:

```text
==== Choose your difficulty ====
1) Easy
2) Medium
3) Hard
================================
```

The selected difficulty determines:

* Number of questions
* Number range
* Time allowed for each question

The `Difficulty` structure stores these values.

## Question Generation

Two random numbers are generated using:

```c
generateNumber()
```

A random operator is selected using:

```c
generateOperator()
```

The available operators are:

```text
+
-
x
/
```

The `generateQuestion()` function combines the two numbers and the operator to create a question.

For division, the program makes sure that the second number is not zero.

## Calculating the Answer

The `calculateAnswer()` function calculates the correct answer based on the operator.

For example:

```text
12 + 13 = 25
20 - 7 = 13
8 x 5 = 40
22 / 7 = 3.142857...
```

Division answers are checked to **two decimal places**, so entering `3.14` for `22 / 7` is accepted.

## Timed Input

The program uses different input methods depending on the operating system.

Windows uses:

```c
<windows.h>
<conio.h>
```

Linux and macOS use:

```c
<unistd.h>
<termios.h>
<sys/select.h>
<sys/time.h>
```

Conditional compilation is used:

```c
#ifdef _WIN32
```

for Windows-specific code, while the Linux/macOS implementation is placed in:

```c
#else
```

This allows the same source file to work across all three operating systems.

## Input Validation

The program checks whether the entered answer is actually a number.

The `isValidNumber()` function accepts values such as:

```text
25
-12
3.14
0.5
```

Invalid input such as:

```text
abc
hello
12abc
```

is rejected.

The program displays:

```text
Only numbers are allowed!
```

Invalid input does not count as an attempt.

## Score, Attempts and Skips

The `QuizResult` structure stores:

```text
Score
Attempts
Skips
```

A correct answer increases the score and counts as an attempt.

A wrong numerical answer also counts as an attempt.

If the player does not answer before the timer runs out, the question is counted as a skip.

At the end of the quiz, the results are displayed:

```text
==== Results ====
1) Score: 2/5
2) Attempts: 3
3) Skips: 2
=================
```

## Functions

### `parseWholeNumber()`

Checks and converts a positive whole number from a string.

Used mainly for menu choices.

### `parseSignedWholeNumber()`

Checks and converts signed whole numbers.

This is used for the custom limits in Hard mode.

### `isValidNumber()`

Checks whether the player's answer contains a valid numerical value.

### `getTimeInSeconds()`

Gets the current time and is used by the timed input system.

The implementation changes depending on the operating system.

### `readLineWithTimeout()`

Reads the player's answer while the timer is running.

There are separate implementations for Windows and Linux/macOS, selected automatically during compilation.

### `toHundredths()`

Converts a decimal number into hundredths.

This allows division answers to be compared up to two decimal places.

### `checkAnswer()`

Checks whether the player's answer matches the correct answer.

Division is compared to two decimal places.

### `showInstructions()`

Displays the instructions screen.

### `readMenuChoice()`

Reads and validates menu choices.

### `getDifficulty()`

Sets the question count, number range, and time limit for Easy and Medium difficulty.

### `readLimit()`

Reads and validates the lower and upper limits for Hard mode.

### `setupHardDifficulty()`

Sets up Hard mode using the range entered by the player.

### `generateNumber()`

Generates a random number inside the selected range.

### `generateOperator()`

Randomly selects one of the four mathematical operators.

### `generateQuestion()`

Generates the two numbers and operator for a question.

### `calculateAnswer()`

Calculates the correct answer.

### `askQuestion()`

Displays one question, handles timed input, validates the answer, and updates the quiz result.

### `showResults()`

Displays the final score, attempts, and skips.

### `runQuiz()`

Runs all the questions for the selected difficulty.

### `playQuizSessions()`

Handles replaying the quiz and changing difficulty.

## Program Flow

The overall program flow is:

```text
Start
  |
  v
Main Menu
  |
  +---- Instructions
  |        |
  |        v
  |     Main Menu
  |
  +---- Take Quiz
  |        |
  |        v
  |   Choose Difficulty
  |        |
  |        v
  |    Generate Question
  |        |
  |        v
  |     Start Timer
  |        |
  |        v
  |    Read Answer
  |        |
  |   +----+----+
  |   |         |
  | Correct    Wrong
  |   |         |
  |   +----+----+
  |        |
  |        v
  |      Next Question
  |        |
  |        v
  |      Results
  |        |
  |        v
  |   Quiz Options
  |        |
  |   +----+---------+
  |   |              |
  | Again       Change Difficulty
  |   |              |
  |   +--------------+
  |
  +---- Exit
           |
           v
          End
```

## Gameplay Demo

Example gameplay:

```text
==== Math Quiz ====
1) Instructions
2) Take Quiz now
3) Exit
===================
Enter your choice: 2

==== Choose your difficulty ====
1) Easy
2) Medium
3) Hard
================================
Enter your choice: 1

==== Quiz (Easy Difficulty) ====

Q.1) 5 - 3
Answer: 2
Correct!

Q.2) 44 x 23
Answer: 1000
Wrong

Q.3) 12 + 13
Answer: 25
Correct!

Q.4) 11 x 10
Answer: abc
Only numbers are allowed!
Answer: 110
Correct!

Q.5) 22 / 7
Answer: 3.14
Correct!
===============================

==== Results ====
1) Score: 4/5
2) Attempts: 5
3) Skips: 0
=================

==== Would you like to? ====
1) Take quiz again
2) Change Difficulty
3) Exit
============================
```

## Concepts Practiced

This project combines several C programming concepts into one practical application.

### C Fundamentals

* Variables and data types
* Constants and macros
* Input and output
* Arithmetic operators
* Conditional statements
* Loops

### Functions

The program is divided into multiple functions instead of putting all the logic inside `main()`.

This provides practice with:

* Function declaration
* Function definition
* Function parameters
* Return values
* Passing values between functions
* Organizing a larger program into smaller tasks

### Structures

The project uses structures to group related information.

For example, the `Difficulty` structure stores:

```text
Number of questions
Time limit
Minimum number
Maximum number
```

The `QuizResult` structure stores:

```text
Score
Attempts
Skips
```

### Random Number Generation

The project uses `rand()` and `srand()` to generate random questions.

It also uses:

```c
time(NULL)
```

to seed the random number generator.

### String Handling

User input is temporarily stored as strings and then validated before being converted into numerical values.

This provides practice with:

* Character-by-character validation
* Strings
* String arrays
* Converting strings into numbers

### Input Validation

The project handles different types of invalid input, including:

* Invalid menu choices
* Invalid numerical answers
* Invalid Hard mode ranges
* Negative values
* Decimal values

### Mathematical Logic

The project implements:

* Addition
* Subtraction
* Multiplication
* Division
* Decimal comparison
* Rounding to two decimal places
* Random number ranges

### Time and Timed Input

One of the main concepts practiced in this project is handling input with a time limit.

The program checks how much time has passed while the player is entering an answer and stops accepting input when the time limit is reached.

### Cross-Platform Programming

The project also introduces basic cross-platform programming using conditional compilation.

The program detects Windows using:

```c
#ifdef _WIN32
```

and uses a different implementation for Linux and macOS.

This allows platform-specific functionality to exist inside a single C source file.

### Program Design

The project provides practice with designing a complete program rather than solving a single isolated problem.

It includes:

* Menu systems
* Multiple functions
* Data structures
* Validation
* Random generation
* Timers
* Game logic
* Result tracking
* Program flow

## What This Project Helped Me Learn

Building this project helped bring several C concepts together into a single working application.

Instead of practicing functions, structures, random numbers, input validation, and loops separately, they are used together to create an interactive program.

The project especially helped with understanding how to:

* Break a large problem into smaller functions
* Design a menu-driven program
* Handle and validate user input
* Generate random values within a range
* Work with structures
* Track multiple values during program execution
* Work with time-related functionality
* Handle platform-specific code
* Use conditional compilation
* Organize the logic of a complete C project
* Build a program that interacts with the user continuously

## Compilation

### Windows

```bash
gcc math_quiz.c -o math_quiz.exe
```

### Linux

```bash
gcc math_quiz.c -o math_quiz
```

### macOS

```bash
gcc math_quiz.c -o math_quiz
```

The same source file is used on all three operating systems.

## Conclusion

The **Math Quiz** project is a practical C application that combines fundamental programming concepts with operating-system-specific timed input.

It started with simple ideas such as generating random numbers and checking answers, but brings together functions, structures, loops, input validation, random number generation, mathematical operations, timing, and conditional compilation into one complete program.

The project provided practical experience in turning individual C concepts into a structured, interactive, and cross-platform application.
