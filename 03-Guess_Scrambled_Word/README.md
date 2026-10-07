# Scrambled Word Guesser

A simple **Scrambled Word Guesser written in C** where the player chooses a difficulty, receives a randomly selected and scrambled word from a local word list, and tries to guess the original word within a limited number of attempts.

This project focuses on practicing fundamental C programming concepts such as:

* Functions
* Strings and character arrays
* `if-else`
* `while` loops
* File handling
* Input validation
* Random number generation
* String manipulation
* Character manipulation
* Function parameters and return values
* Basic program flow and control
* Fisher-Yates shuffle

---

## Setup

Before running the program, create a text file named:

```text
words.txt
```

The `words.txt` file must be placed in the **same folder as the program/executable**.

Add one word per line:

```text
apple
banana
computer
keyboard
school
mountain
programming
```

The program reads the words from this file and randomly selects one whenever a new round begins.

### Important

* Use **one word per line**.
* Words should contain letters only.
* Do not put spaces inside words.
* You can add as many words as you want.
* You do not need to modify the C source code when adding or changing words.

The basic project structure should look like:

```text
Scrambled-Word-Guesser/
│
├── main.c
├── words.txt
└── README.md
```

If using GCC:

```bash
gcc main.c -o scrambled_word_guesser
```

Then run:

```bash
./scrambled_word_guesser
```

On Windows:

```bash
scrambled_word_guesser.exe
```

---

## Game Preview

When the program starts, the player sees:

```text
================================
     Scrambled Word Guesser
================================

1) Instructions
2) Play now
3) Exit

Enter your choice:
```

After selecting **Play now**, the player chooses a difficulty:

```text
================================
      Choose your Difficulty
================================

1) Easy
2) Medium
3) Hard
4) Extra Hard

Enter your choice:
```

### Difficulty Levels

| Difficulty | Attempts |
| ---------- | -------- |
| Easy       | 15       |
| Medium     | 10       |
| Hard       | 5        |
| Extra Hard | 1        |

The selected difficulty determines the number of valid incorrect guesses allowed during the round.

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
  |       |
  |       +---- Play Now
  |       |
  |       +---- Exit
  |
  +---- Play Now
  |       |
  |       v
  |   Choose Difficulty
  |       |
  |       v
  |   Open words.txt
  |       |
  |       v
  |   Select Random Word
  |       |
  |       v
  |   Convert to Uppercase
  |       |
  |       v
  |   Scramble Word
  |       |
  |       v
  |   Display Scrambled Word
  |       |
  |       v
  |   Enter Guess
  |       |
  |       +---- Correct ----> Win
  |       |
  |       +---- Incorrect
  |                |
  |                v
  |          Attempts Remaining?
  |                |
  |          +-----+-----+
  |          |           |
  |         Yes          No
  |          |           |
  |          v           v
  |      Guess Again  Game Over
  |                      |
  |                      v
  |                After Round Menu
  |                      |
  |             +--------+--------+
  |             |        |        |
  |             v        v        v
  |        Play Again  Change    Exit
  |                    Difficulty
```

---

# Main Features

## 1. Main Menu

When the program starts, the player gets three choices:

```text
1) Instructions
2) Play now
3) Exit
```

The program keeps asking until a valid option between `1` and `3` is entered.

---

## 2. Instructions

The Instructions section explains the basic rules of the game:

```text
a. A random word will be selected from the word list.
b. The selected word will be scrambled randomly.
c. You must guess the original word.
d. Your guess is not case-sensitive.
e. Spaces are not allowed in your guess.
f. You must guess the word within the given number of attempts.
g. The number of attempts depends on the difficulty level.
```

The player can then choose:

```text
2) Play now
3) Exit
```

---

## 3. Difficulty Selection

The player can select one of four difficulty levels.

### Easy

```text
15 attempts
```

### Medium

```text
10 attempts
```

### Hard

```text
5 attempts
```

### Extra Hard

```text
1 attempt
```

The selected difficulty is converted into the corresponding number of attempts.

---

## 4. Reading Words from `words.txt`

The program does not hardcode the word list.

Instead, it reads words from:

```text
words.txt
```

For example:

```text
apple
banana
computer
keyboard
mountain
```

The program searches through the file and counts the valid words.

It then generates a random position and reads the word at that position.

This means new words can be added simply by editing `words.txt`.

---

## 5. Random Word Selection

Once a round begins, the program selects one valid word randomly.

For example, if `words.txt` contains:

```text
apple
banana
computer
keyboard
```

the program could randomly select:

```text
apple
```

The selected word is not shown directly to the player.

---

## 6. Converting the Word to Uppercase

After selecting a word, the program converts it to uppercase.

For example:

```text
apple
```

becomes:

```text
APPLE
```

This is done using the `toupper()` function from:

```c
ctype.h
```

The player's guess is also converted to uppercase before comparison.

This allows:

```text
APPLE
apple
Apple
aPpLE
```

to all be treated as the same answer.

---

## 7. Scrambling the Word

After converting the word to uppercase, the program creates a copy and randomly rearranges its characters.

For example:

```text
APPLE
```

could become:

```text
L E P A P
```

Another round could produce:

```text
P A P L E
```

The scrambling is performed using the **Fisher-Yates shuffle**.

The original word remains unchanged so that it can be compared with the player's guess.

---

## 8. Displaying the Scrambled Word

The scrambled word is displayed with spaces between its characters.

For example:

```text
Scrambled Word: L E P A P
```

The spaces are only used for visual presentation.

The actual scrambled string remains:

```text
LEPAP
```

---

# Guessing System

After displaying the scrambled word, the player enters a guess:

```text
Scrambled Word: L E P A P

Enter your Guess:
```

The program compares the player's guess with the original word.

### Correct Guess

If the player enters:

```text
APPLE
```

the game displays a success message:

```text
================================
          Correct!
================================

The word was: APPLE

You guessed the word correctly!
```

The round then ends.

---

## Incorrect Guess

If the guess is incorrect, the number of remaining attempts is reduced.

For example:

```text
Incorrect guess.

Attempts remaining: 4
```

The player can then try again.

---

# Input Validation

The program validates the player's guess before checking the answer.

## Case-Insensitive Input

All of these are valid ways of entering `APPLE`:

```text
APPLE
apple
Apple
aPpLE
ApPlE
```

The program converts the input to uppercase before comparing it.

---

## Spaces Are Not Allowed

The player cannot enter spaces inside the guess.

These are invalid:

```text
A P P L E
App Le
AP PLE
```

The program displays:

```text
Spaces are not allowed. Please enter the word without spaces.
```

The invalid input does **not** consume an attempt.

---

## Letters Only

The guess must contain letters only.

For example:

```text
APP123
APP!E
```

are rejected.

The program displays:

```text
Please use letters only.
```

These invalid inputs also do not consume an attempt.

---

# Attempts System

Only valid guesses count as attempts.

For example, on **Hard** difficulty:

```text
Attempts remaining: 5
```

After an incorrect valid guess:

```text
Attempts remaining: 4
```

Invalid inputs such as:

```text
A P P L E
```

or:

```text
APP123
```

do not reduce the number of attempts.

---

# Game Over

If the player uses all available attempts without guessing the word correctly, the game displays:

```text
================================
          Game Over
================================

The word was: APPLE

Better luck next time!
```

The original word is revealed after the round ends.

---

# After-Round Menu

After a successful guess or game over, the player gets three choices:

```text
================================
      Would you like to?
================================

1) Play Again
2) Change Difficulty
3) Exit

================================
```

## Play Again

Starts another round using the **same difficulty**.

For example, if the player selected Hard:

```text
Hard → 5 attempts
```

choosing **Play Again** keeps the player on Hard difficulty.

A new word is selected and scrambled.

---

## Change Difficulty

Returns to:

```text
================================
      Choose your Difficulty
================================

1) Easy
2) Medium
3) Hard
4) Extra Hard
```

The player can then select a different difficulty.

---

## Exit

Ends the game and displays:

```text
Thank you for playing!
```

---

# Functions

The program is divided into several functions. Each function handles a specific part of the game.

---

## `readLine()`

```c
void readLine(char *buf, int size)
```

Reads a complete line of input using `fgets()`.

It also:

* Removes the newline character.
* Handles input that is longer than the provided buffer.
* Prevents leftover characters from interfering with the next input.
* Exits cleanly if `EOF` is encountered.

This function is used as the main method for reading user input.

---

## `askChoice()`

```c
int askChoice(int low, int high)
```

Handles menu choices.

It asks the user for a choice and checks whether the input falls within the specified range.

For example:

```c
askChoice(1, 3);
```

accepts:

```text
1
2
3
```

and rejects invalid choices.

---

## `showMainMenu()`

```c
void showMainMenu(void)
```

Displays the main menu:

```text
1) Instructions
2) Play now
3) Exit
```

---

## `showInstructions()`

```c
void showInstructions(void)
```

Displays the game instructions and provides options to start playing or exit.

---

## `chooseDifficulty()`

```c
int chooseDifficulty(void)
```

Displays the difficulty selection menu.

The function converts the selected difficulty into the corresponding number of attempts:

```text
Easy       → 15
Medium     → 10
Hard       → 5
Extra Hard → 1
```

The actual game only needs the number of attempts, so the function returns that value.

---

## `showReplayMenu()`

```c
void showReplayMenu(void)
```

Displays the menu shown after a round:

```text
1) Play Again
2) Change Difficulty
3) Exit
```

---

## `goodbye()`

```c
void goodbye(void)
```

Displays the message shown when the player exits the game:

```text
Thank you for playing!
```

---

## `toUpperCase()`

```c
void toUpperCase(char *s)
```

Converts every character in a string to uppercase using `toupper()`.

For example:

```text
apple
```

becomes:

```text
APPLE
```

This is used for both the selected word and the player's guess.

---

## `nextWord()`

```c
int nextWord(FILE *f, char *word)
```

Reads the next valid word from `words.txt`.

It:

1. Reads a line from the file.
2. Removes the newline.
3. Removes trailing spaces and tabs.
4. Checks that the line is not empty.
5. Checks that every character is a letter.
6. Copies the valid word into `word`.

If a valid word is found, the function returns:

```text
1
```

Otherwise, it returns:

```text
0
```

---

## `pickWord()`

```c
int pickWord(char *word)
```

Selects a random word from `words.txt`.

The function first counts the valid words in the file.

It then generates a random position:

```c
pick = rand() % count;
```

The file is rewound and the selected word is read.

The function also handles errors such as:

```text
words.txt
```

not being available or containing no valid words.

---

## `scramble()`

```c
void scramble(char *s)
```

Randomly rearranges the characters in a string.

It uses the **Fisher-Yates shuffle** to produce a random arrangement of the characters.

For example:

```text
APPLE
```

could become:

```text
LEPAP
```

---

## `showScrambled()`

```c
void showScrambled(char *s)
```

Displays the scrambled word with spaces between its characters.

For example:

```text
LEPAP
```

is displayed as:

```text
L E P A P
```

---

## `readGuess()`

```c
void readGuess(char *guess)
```

Reads and validates the player's guess.

It checks for:

* Empty input
* Spaces
* Non-letter characters

After the input passes validation, it is converted to uppercase.

---

## `playRound()`

```c
int playRound(int attempts)
```

Controls one complete round.

It:

1. Selects a random word.
2. Converts the word to uppercase.
3. Creates a copy of the word.
4. Scrambles the copy.
5. Displays the scrambled word.
6. Reads the player's guess.
7. Compares the guess with the original word.
8. Reduces attempts after incorrect valid guesses.
9. Displays the result.

The function returns when the player either guesses the word correctly or runs out of attempts.

---

## `playGame()`

```c
void playGame(void)
```

Controls the complete gameplay session.

It:

1. Asks the player to choose a difficulty.
2. Starts a round.
3. Shows the result.
4. Displays the after-round menu.
5. Starts another round if **Play Again** is selected.
6. Changes difficulty if **Change Difficulty** is selected.
7. Exits when requested.

When **Play Again** is selected, the current difficulty is preserved.

When **Change Difficulty** is selected, `chooseDifficulty()` is called again.

---

## `main()`

```c
int main(void)
```

This is where program execution begins.

The random number generator is seeded once:

```c
srand((unsigned)time(NULL));
```

The program then repeatedly displays the main menu and handles the player's choice.

The main menu controls access to:

```text
Instructions
Play Now
Exit
```

---

# Random Number Generation

The program uses:

```c
rand()
```

for random word selection and word scrambling.

The random number generator is seeded once at the beginning:

```c
srand((unsigned)time(NULL));
```

This allows different random results each time the program is started.

The same random number generator is then used throughout the program for:

* Selecting a random word
* Scrambling the selected word

---

# File Handling

The program uses standard C file handling to read `words.txt`.

The file is opened using:

```c
fopen("words.txt", "r");
```

The program reads words using:

```c
fgets()
```

and closes the file using:

```c
fclose()
```

If the file cannot be opened, the program displays:

```text
Error: could not open words.txt.
```

If the file contains no valid words:

```text
Error: words.txt is empty or has no valid words.
```

---

# Libraries Used

The program uses the following standard C libraries:

| Library    | Purpose                                     |
| ---------- | ------------------------------------------- |
| `stdio.h`  | Input, output, and file handling            |
| `stdlib.h` | Random numbers and program control          |
| `string.h` | String operations                           |
| `ctype.h`  | Character checking and uppercase conversion |
| `time.h`   | Seeding the random number generator         |

No external libraries are required.

---

# Concepts Practiced

This project brings several fundamental C concepts together:

```text
Variables
Functions
Function parameters
Return values
if / else
while loops
for loops
Strings
Character arrays
String comparison
String copying
File handling
fopen()
fgets()
fclose()
rand()
srand()
time()
toupper()
isalpha()
Input validation
Randomization
Fisher-Yates shuffle
Menu-driven programs
Basic error handling
```

---

# Project Structure

```text
Scrambled-Word-Guesser/
│
├── main.c
├── words.txt
└── README.md
```

### `main.c`

Contains the complete game logic and all functions required to run the game.

### `words.txt`

Contains the word list used by the game.

### `README.md`

Contains the documentation for the project.

---

# Example Gameplay

```text
================================
     Scrambled Word Guesser
================================

1) Instructions
2) Play now
3) Exit

Enter your choice: 2

================================
      Choose your Difficulty
================================

1) Easy
2) Medium
3) Hard
4) Extra Hard

Enter your choice: 3

Attempts remaining: 5

Scrambled Word: L E P A P

Enter your Guess: banana

Incorrect guess.

Attempts remaining: 4

Scrambled Word: L E P A P

Enter your Guess: APPLE

================================
          Correct!
================================

The word was: APPLE

You guessed the word correctly!

================================
      Would you like to?
================================

1) Play Again
2) Change Difficulty
3) Exit

================================
```

---

# Why This Project Is Useful

This project combines several basic C concepts into one complete interactive program.

Instead of practicing functions, strings, file handling, random numbers, and input validation separately, they are all used together to create a working terminal game.

It also provides practical experience with reading external data from a file and processing that data inside a C program.

The project demonstrates the basic idea of breaking a larger program into smaller functions, where each function has a specific responsibility.

---

## Author

A C programming project created as part of a journey from learning C fundamentals to building practical terminal-based projects.
