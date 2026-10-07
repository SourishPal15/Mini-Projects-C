# Expense Tracker

A console-based expense management application written in C. The project currently implements user registration and login, persistent user-specific expense storage, expense creation, viewing, editing, deletion, input validation, and basic account management.

The project is being developed incrementally, with additional features planned for future versions.

## Current Version


### Implemented

* User registration
* User login
* Username validation
* Password validation
* Separate expense file for each user
* Add expenses
* View expenses
* Calculate total expenses
* Edit expenses
* Delete expenses
* Built-in expense categories
* Custom expense categories
* Date validation
* Automatic current-date detection
* Expense IDs
* Input validation
* Persistent file-based storage
* Login attempt limitation

## Project Overview

The program follows a simple file-based architecture.

Each registered user has:

1. A record in `users.txt`
2. A separate expense file based on their user ID

For example:

```text
users.txt
│
├── User 1
│   └── user_001.txt
│
├── User 2
│   └── user_002.txt
│
└── User 3
    └── user_003.txt
```

This allows the application to keep each user's expenses separate.

---
## Program Architecture

The overall flow of the application is:

```text
                         EXPENSE TRACKER
                                |
                                v
                       Welcome Main Menu
                                |
                  +-------------+-------------+
                  |             |             |
                  v             v             v
             Login User     New User        Exit
                  |             |
                  |             v
                  |         Register
                  |             |
                  +------+------+
                         |
                         v
                    User Dashboard
                         |
       +---------+-------+-------+---------+
       |         |       |       |         |
       v         v       v       v         v
      Add      View     Edit   Delete   Logout/Exit
       |         |       |       |
       +---------+-------+-------+
                         |
                         v
                  User Expense File
                         |
                  Load -> Modify
                         |
                         v
                       Save
```

The application is divided into several logical sections:

```text
Input Handling
      |
      v
Account System
      |
      v
Authentication
      |
      v
Expense Input Validation
      |
      v
File Handling
      |
      v
Expense Operations
      |
      v
User Dashboard
      |
      v
Main Program Control
```

---

# File Structure

A typical project directory will look like:

```text
Expense-Tracker/
│
├── expense_tracker.c
├── users.txt
├── user_001.txt
├── user_002.txt
├── user_003.txt
└── README.md
```

The exact number of `user_XXX.txt` files depends on the number of registered users.

## Source File

```text
expense_tracker.c
```

Contains the complete C program.

## User Database

```text
users.txt
```

Stores registered users.

Each line follows this format:

```text
ID|Username|Password
```

Example:

```text
1|Rohan Singh|Hello123
2|Amit Kumar|Amit456
```

## User Expense Files

Each user receives a separate file.

For example:

```text
user_001.txt
```

stores the expenses belonging to user ID `1`.

Each expense is stored as:

```text
ID|Date|Category|Amount|Description
```

Example:

```text
1|06/10/2026|Food|250.00|College lunch
2|06/10/2026|Travel|80.00|Bus
```

---

# Data Structures

The program uses two primary structures.

## `struct User`

```c
struct User
{
    int id;
    char username[MAX_NAME];
    char password[MAX_PASS];
};
```

This represents a registered user.

### Fields

| Field      | Type     | Purpose         |
| ---------- | -------- | --------------- |
| `id`       | `int`    | Unique user ID  |
| `username` | `char[]` | User's username |
| `password` | `char[]` | User's password |

The user ID is also used to create the user's expense file.

For example:

```text
User ID = 1
       |
       v
user_001.txt
```

---

## `struct Expense`

```c
struct Expense
{
    int   id;
    char  date[20];
    char  category[30];
    float amount;
    char  description[100];
};
```

This represents a single expense.

### Fields

| Field         | Type     | Purpose                    |
| ------------- | -------- | -------------------------- |
| `id`          | `int`    | Unique expense ID          |
| `date`        | `char[]` | Expense date               |
| `category`    | `char[]` | Expense category           |
| `amount`      | `float`  | Expense amount             |
| `description` | `char[]` | Description of the expense |

---

# Constants

The program uses `#define` constants to keep important limits and configuration values in one place.

```c
#define USERS_FILE      "users.txt"
#define INPUT_SIZE      100
#define MAX_NAME        30
#define MAX_PASS        30
#define MAX_ATTEMPTS    3
#define MAX_EXPENSES    500
#define MAX_AMOUNT      10000000.0f
#define NUM_CATEGORIES  8
#define MAX_CUSTOM_LEN  15
```

Some important limits are:

* Maximum username length: 29 characters
* Maximum password length: 29 characters
* Maximum login attempts: 3
* Maximum expenses per user: 500
* Maximum expense amount: 10,000,000
* Maximum custom category length: 15 characters

The program also defines result codes used by the dashboard:

```c
#define RESULT_LOGOUT 1
#define RESULT_EXIT   2
```

These allow `userMenu()` to tell `main()` what happened when the dashboard ends.

---

# Built-in Categories

The program stores eight predefined categories in a global constant array:

```c
const char categoryNames[NUM_CATEGORIES][20] =
{
    "Food", "Travel", "Education", "Shopping",
    "Entertainment", "Bills", "Health", "Other"
};
```

The user can select one of these categories or create a custom category.

---

# Function Reference

## Input Helper Functions

### `readLine()`

```c
void readLine(char *buffer, int size);
```

Reads a complete line from the keyboard using `fgets()`.

It removes the newline character added by `fgets()`.

This function is used instead of:

```c
scanf("%s", ...);
```

because `%s` stops reading at whitespace.

For example:

```text
Rohan Singh
```

would not be completely read using a simple `%s`.

`readLine()` also clears the remaining characters if the user enters more text than the buffer can hold.

---

### `readNumber()`

```c
int readNumber(const char *prompt);
```

Reads an integer from the user.

The function repeatedly asks for input until a valid integer is entered.

The input is first stored as text:

```c
char line[INPUT_SIZE];
```

and then converted using:

```c
sscanf(line, "%d", &number);
```

This prevents invalid text from causing problems when the program expects a number.

---

### `getFirstName()`

```c
void getFirstName(const char *username, char *firstName);
```

Extracts the first part of a username.

For example:

```text
Rohan Singh
```

becomes:

```text
Rohan
```

This is used for messages such as:

```text
Welcome, Rohan!
```

---

# Account Validation

## `validateUsername()`

```c
int validateUsername(const char *username);
```

Checks whether a username follows the application's rules.

A valid username:

* Cannot be empty
* Must be shorter than `MAX_NAME`
* Cannot start with a space
* Cannot end with a space
* Can contain letters
* Can contain digits
* Can contain spaces
* Can contain underscores
* Cannot contain `|`

The `|` restriction is important because it is used as the field separator in `users.txt`.

---

## `validatePassword()`

```c
int validatePassword(const char *password);
```

Checks the password against several requirements.

A valid password must:

* Contain at least 6 characters
* Contain at least one alphabetic character
* Contain at least one digit
* Be shorter than `MAX_PASS`
* Not contain spaces
* Not contain `|`

The function tracks these requirements independently:

```c
int hasLetter;
int hasDigit;
int hasBadChar;
```

This allows the program to report multiple problems at once.

---

# User File Management

## `findUser()`

```c
int findUser(const char *username, struct User *found);
```

Searches `users.txt` for a particular username.

The function reads the file line by line and extracts:

```text
ID
Username
Password
```

using `sscanf()`.

If a matching username is found:

```c
*found = temp;
```

copies the complete user structure to the caller.

The function returns:

```text
1 -> user found
0 -> user not found
```

---

## `usernameExists()`

```c
int usernameExists(const char *username);
```

A simple wrapper around `findUser()`.

It checks whether a username is already registered.

---

## `getNextUserId()`

```c
int getNextUserId(void);
```

Reads the existing user IDs and finds the highest one.

It then returns:

```text
highest ID + 1
```

For example:

```text
1
2
3
```

produces:

```text
4
```

This ID is then used to identify the new user.

---

## `getUserFileName()`

```c
void getUserFileName(int id, char *fileName);
```

Creates the filename associated with a user.

For example:

```text
ID 1  -> user_001.txt
ID 12 -> user_012.txt
ID 125 -> user_125.txt
```

The `%03d` format provides the three-digit numbering.

---

## `createUserFile()`

```c
void createUserFile(int id);
```

Creates an empty expense file for a new user.

It uses append mode:

```c
fopen(fileName, "a");
```

Using `"a"` creates the file if it does not exist without deleting existing contents.

---

## `saveUser()`

```c
void saveUser(const struct User *user);
```

Adds a new user to `users.txt`.

The data is written using:

```text
ID|Username|Password
```

The function receives a pointer to `struct User`, so the structure does not need to be copied into the function.

---

# Registration and Login

## `registerUser()`

```c
int registerUser(struct User *current);
```

Creates a new account.

The process is:

```text
Enter Username
      |
      v
Validate Username
      |
      v
Check Whether Username Exists
      |
      v
Enter Password
      |
      v
Validate Password
      |
      v
Generate User ID
      |
      v
Save User
      |
      v
Create Expense File
      |
      v
Log User In
```

If successful, the new user's information is copied into:

```c
current
```

The function returns `1` on success.

---

## `loginUser()`

```c
int loginUser(struct User *current);
```

Handles authentication.

The login process is divided into two stages:

### Stage 1: Username

The program searches `users.txt`.

If the username does not exist, the user can either:

```text
1) Try again
2) Back to main menu
```

### Stage 2: Password

Once the username is found, the program allows three password attempts.

The password is compared using:

```c
strcmp(input, found.password)
```

If the password matches, the user is logged in.

---

# Expense Input Helpers

## `replacePipes()`

```c
void replacePipes(char *text);
```

Replaces every:

```text
|
```

with:

```text
-
```

The program uses `|` as its file separator, so allowing users to insert it into descriptions or custom categories would corrupt the file format.

For example:

```text
Original:
Lunch | College

Stored:
Lunch - College
```

---

## `readAmount()`

```c
float readAmount(const char *prompt);
```

Reads and validates an expense amount.

The function checks the input manually before converting it using:

```c
atof()
```

It allows:

```text
250
99.50
1000.75
```

and rejects invalid values such as:

```text
abc
12abc
0
-50
```

The amount must also be less than or equal to:

```c
MAX_AMOUNT
```

This manual validation is important because directly using a conversion function could otherwise accept only the valid beginning of malformed input.

---

## `isLeapYear()`

```c
int isLeapYear(int year);
```

Determines whether a year is a leap year.

The standard Gregorian calendar rules are used:

```text
Divisible by 400 -> leap year
Divisible by 100 -> not a leap year
Divisible by 4   -> leap year
Otherwise        -> not a leap year
```

---

## `daysInMonth()`

```c
int daysInMonth(int month, int year);
```

Returns the number of days in a particular month.

February is handled using `isLeapYear()`.

For example:

```text
February 2024 -> 29
February 2025 -> 28
```

---

## `validateDate()`

```c
int validateDate(const char *text);
```

Checks whether a date follows:

```text
DD/MM/YYYY
```

For example:

```text
06/10/2026
```

The function checks:

1. Correct string length
2. `/` characters at the correct positions
3. All other characters are digits
4. Year range
5. Month range
6. Valid number of days for that month

This means dates such as:

```text
31/02/2026
```

are rejected.

---

## `getTodayDate()`

```c
void getTodayDate(char *today);
```

Uses `<time.h>` to obtain the computer's current local date.

The date is formatted as:

```text
DD/MM/YYYY
```

using `strftime()`.

---

## `readDate()`

```c
void readDate(const char *prompt, char *date);
```

Provides a convenient interface around date validation.

The user can:

* Enter a valid date
* Press Enter to automatically use today's date

Invalid dates cause the function to ask again.

---

## `chooseCategory()`

```c
void chooseCategory(char *category);
```

Displays the built-in category list:

```text
1) Food
2) Travel
3) Education
4) Shopping
5) Entertainment
6) Bills
7) Health
8) Other
9) Custom category
```

If the user chooses a built-in category, the corresponding name is copied into the expense.

If option `9` is selected, the program asks for a custom category and validates its length.

---

## `readDescription()`

```c
void readDescription(const char *prompt, char *description);
```

Reads an expense description.

If the user enters nothing, the program stores:

```text
-
```

instead of leaving the field empty.

The function also calls `replacePipes()` to protect the file format.

---

# Expense File Handling

The program uses a simple strategy for modifying expense data:

```text
                 User Expense File
                        |
                        v
                 loadExpenses()
                        |
                        v
              Array of struct Expense
                        |
             +----------+----------+
             |          |          |
            Add        Edit      Delete
             |          |          |
             +----------+----------+
                        |
                        v
                saveAllExpenses()
                        |
                        v
                 Updated File
```

This is one of the central design decisions in Version 2.

Instead of trying to modify individual lines directly inside the text file, the program loads the entire dataset into memory, modifies the array, and then rewrites the file.

---

## `loadExpenses()`

```c
int loadExpenses(const char *fileName, struct Expense list[]);
```

Reads the user's expense file into an array of `struct Expense`.

Each line is parsed into:

```text
ID
Date
Category
Amount
Description
```

The function stops loading after `MAX_EXPENSES` records.

It returns the number of successfully loaded expenses.

---

## `saveAllExpenses()`

```c
int saveAllExpenses(
    const char *fileName,
    const struct Expense list[],
    int count
);
```

Writes the entire expense array back to the user's file.

The file is opened using:

```c
fopen(fileName, "w");
```

This clears the previous contents before the current array is written.

For every expense, the function writes:

```text
ID|Date|Category|Amount|Description
```

This approach keeps the file synchronized with the in-memory array.

---

## `getNextExpenseId()`

```c
int getNextExpenseId(
    const struct Expense list[],
    int count
);
```

Finds the highest existing expense ID and returns:

```text
highest ID + 1
```

Deleted IDs are not reused.

For example:

```text
1
2
3
```

If expense `2` is deleted:

```text
1
3
```

The next expense receives:

```text
4
```

This keeps expense IDs stable and avoids giving a different expense an old ID.

---

## `findExpenseIndex()`

```c
int findExpenseIndex(
    const struct Expense list[],
    int count,
    int id
);
```

Searches the expense array for a particular ID.

It returns the array position if the expense is found.

If the ID does not exist:

```c
return -1;
```

This function is used by both the edit and delete operations.

---

# Expense Features

## `printTableHeader()`

Prints the header used by the expense table.

```text
ID   Date         Category        Amount       Description
======================================================================
```

Keeping this in a separate function avoids repeating the same formatting code.

---

## `printExpenseRow()`

```c
void printExpenseRow(const struct Expense *e);
```

Prints one expense in table format.

The function receives a pointer to an expense:

```c
const struct Expense *e
```

and therefore accesses its members using:

```c
e->id
e->date
e->category
e->amount
e->description
```

---

# Adding an Expense

## `addExpense()`

```c
void addExpense(const char *fileName);
```

The function:

1. Loads existing expenses
2. Checks whether the maximum capacity has been reached
3. Reads the amount
4. Selects the category
5. Reads the date
6. Reads the description
7. Generates a new expense ID
8. Adds the expense to the array
9. Saves the complete array back to the file
10. Displays the newly created expense

The process is:

```text
Load Existing Expenses
          |
          v
Check Capacity
          |
          v
Read Amount
          |
          v
Choose Category
          |
          v
Read Date
          |
          v
Read Description
          |
          v
Generate ID
          |
          v
Add to Array
          |
          v
Save File
```

---

# Viewing Expenses

## `viewExpenses()`

```c
void viewExpenses(const char *fileName);
```

Loads all expenses and displays them in a table.

While displaying the records, it also calculates the total:

```c
total = total + list[i].amount;
```

The final result is displayed as:

```text
Total Expenses: ₹XXXX.XX
```

The function does not modify the file.

---

# Editing an Expense

## `editExpense()`

```c
void editExpense(const char *fileName);
```

Allows the user to modify:

```text
1) Amount
2) Category
3) Date
4) Description
5) Everything
6) Cancel
```

The function first searches for the requested expense ID using:

```c
findExpenseIndex()
```

If the expense exists, only the selected fields are modified.

For example, choosing `Edit Amount` does not affect the date, category, or description.

After the modification:

```text
Array
  |
  v
saveAllExpenses()
  |
  v
File updated
```

---

# Deleting an Expense

## `deleteExpense()`

```c
void deleteExpense(const char *fileName);
```

The deletion process is:

```text
Enter Expense ID
        |
        v
Find Array Index
        |
        v
Display Expense
        |
        v
Ask for Confirmation
        |
        +------ No ------> Cancel
        |
       Yes
        |
        v
Shift Following Elements
        |
        v
Decrease Count
        |
        v
Save Updated Array
```

The actual removal is performed by shifting every element after the deleted expense one position to the left.

For example:

```text
Before:

[Expense 1]
[Expense 2]
[Expense 3]
[Expense 4]

Delete Expense 2

After:

[Expense 1]
[Expense 3]
[Expense 4]
```

The array count is then reduced by one.

---

# Menu System

## `showWelcomeMenu()`

Displays the application's initial menu:

```text
1) Registered User
2) New User
3) Exit
```

---

## `showWelcomeBack()`

Checks whether the logged-in user already has saved expenses.

If expenses exist, it displays the number of previously saved records.

---

## `userMenu()`

```c
int userMenu(const struct User *user);
```

Controls the dashboard after successful login or registration.

The current menu is:

```text
1) Add Expense
2) View Expenses
3) Edit Expense
4) Delete Expense
5) Search Expenses
6) Expense Summary
7) Budget Management
8) Logout
9) Exit
```

Options 5, 6, and 7 are placeholders for future versions.

The function returns either:

```c
RESULT_LOGOUT
```

or:

```c
RESULT_EXIT
```

This allows `main()` to determine whether it should return to the welcome menu or terminate the application.

---

# Main Program Flow

## `main()`

```c
int main(void);
```

`main()` is responsible for the highest-level program flow.

It continuously displays the welcome menu while the program is running.

The structure is essentially:

```text
Start
  |
  v
Show Welcome Menu
  |
  v
Read Choice
  |
  +---- Registered User ----> Login
  |
  +---- New User -----------> Register
  |
  +---- Exit ---------------> End
  |
  v
Successful Authentication?
  |
 Yes
  |
  v
User Dashboard
  |
  +---- Logout -------------> Welcome Menu
  |
  +---- Exit ---------------> End
```

The variable:

```c
int running = 1;
```

controls whether the main loop continues.

---

# File Storage Design

The program uses plain text files instead of a database.

## User Database

```text
users.txt
```

Example:

```text
1|Rohan Singh|Hello123
2|Amit Kumar|Amit456
```

## Expense Database

A user's file may look like:

```text
user_001.txt
```

with:

```text
1|06/10/2026|Food|250.00|College lunch
2|06/10/2026|Travel|80.00|Bus
3|05/10/2026|Education|500.00|Books
```

The `|` character acts as a delimiter.

Conceptually:

```text
1 | 06/10/2026 | Food | 250.00 | College lunch
|     |           |       |          |
ID   Date      Category Amount   Description
```

---

# Why the Program Uses Arrays

Expenses are stored temporarily in:

```c
struct Expense list[MAX_EXPENSES];
```

This provides a straightforward way to perform operations such as:

* Searching
* Adding
* Editing
* Deleting
* Calculating totals

The maximum number of expenses per user is currently:

```text
500
```

The application therefore uses a fixed-size array rather than dynamically allocating memory.

---

# Important C Concepts Demonstrated

This project combines several important C programming concepts.

## Structures

```c
struct User
struct Expense
```

Structures allow related pieces of data to be grouped together.

---

## Arrays

The expense records are stored in:

```c
struct Expense list[MAX_EXPENSES];
```

The category system also uses a two-dimensional character array.

---

## Pointers

Several functions receive pointers to structures:

```c
struct User *current
```

and:

```c
const struct Expense *e
```

This allows functions to work directly with existing structures.

The `->` operator is used when accessing members through structure pointers.

---

## Strings

The project heavily uses:

```c
char[]
```

along with functions from `<string.h>` such as:

```c
strlen()
strcpy()
strcmp()
```

---

## File Handling

The program uses:

```c
FILE *
fopen()
fclose()
fgets()
fprintf()
sscanf()
```

to create, read, search, and rewrite text files.

---

## Input Validation

The project does not simply trust user input.

It validates:

* Numbers
* Usernames
* Passwords
* Amounts
* Dates
* Categories
* Expense IDs
* Menu choices

This makes the console application much more robust than a simple input/output program.

---

## Character Classification

The `<ctype.h>` functions are used for validation:

```c
isdigit()
isalpha()
isalnum()
```

For example:

```c
isdigit((unsigned char)c)
```

checks whether a character is a digit.

---

## Time Handling

The `<time.h>` library is used to obtain the current date.

The relevant functions include:

```c
time()
localtime()
strftime()
```

---

## Conditional Logic

The program uses:

* `if`
* `else`
* `switch`
* `while`
* `for`

throughout the application.

---

## Formatted Input and Output

The project makes extensive use of:

```c
printf()
sscanf()
sprintf()
fprintf()
```

particularly for its file format and table-based console interface.

---

# Error Handling and Validation

The program repeatedly validates user input instead of immediately accepting it.

For example:

```text
Invalid Menu Choice
        |
        v
Display Error
        |
        v
Ask Again
```

The same pattern is used for:

* Invalid usernames
* Duplicate usernames
* Invalid passwords
* Invalid amounts
* Invalid dates
* Invalid categories
* Invalid expense IDs
* Invalid confirmation choices

This makes the program resilient to common user mistakes.

---

# Security Note

This project is intended for **educational purposes**.

Passwords are currently stored directly in:

```text
users.txt
```

as plain text.

For example:

```text
1|Rohan Singh|Hello123
```

This is **not suitable for a real financial application**.

A production application should use proper password hashing, secure authentication, protected storage, and stronger access controls.

Similarly, this project should not be used to store real financial information.

---

# Current Limitations

The current Version 2 implementation has several deliberate limitations.

### Storage

Data is stored in plain text files rather than a database.

### Password Security

Passwords are stored in plain text.

### Expense Capacity

Each user can store up to:

```text
500 expenses
```

because a fixed-size array is used.

### Search

Search functionality has not yet been implemented.

### Summary

Advanced expense summaries have not yet been implemented.

### Budget

Budget management has not yet been implemented.

### Data Validation

The application performs useful validation, but it is still a console-based educational application and does not provide production-level data integrity or security.




---

# Compilation

The program can be compiled using a C compiler such as GCC.

```bash
gcc expense_tracker.c -o expense_tracker
```

Run the program with:

### Windows

```bash
expense_tracker.exe
```

### Linux/macOS

```bash
./expense_tracker
```

The generated executable and the text files should remain in the same working directory so the program can access its data files.

---

# Example User Flow

A typical first-time user interaction is:

```text
Expense Tracker
       |
       v
New User
       |
       v
Enter Username
       |
       v
Enter Password
       |
       v
Account Created
       |
       v
User Dashboard
       |
       v
Add Expense
       |
       +--> Amount
       +--> Category
       +--> Date
       +--> Description
       |
       v
Expense Saved
       |
       v
user_001.txt
```

On a later launch:

```text
Expense Tracker
       |
       v
Registered User
       |
       v
Login
       |
       v
user_001.txt
       |
       v
Previous Expenses Loaded
       |
       v
Dashboard
```

This demonstrates the persistent-storage aspect of the project: expenses remain available after the program is closed and reopened.

---

# Project Design Summary

The core design can be summarized as:

```text
                    +-------------------+
                    |    main()         |
                    +---------+---------+
                              |
                              v
                    +-------------------+
                    | Authentication    |
                    | Login / Register  |
                    +---------+---------+
                              |
                              v
                    +-------------------+
                    |   userMenu()      |
                    +---------+---------+
                              |
            +-----------------+-----------------+
            |                 |                 |
            v                 v                 v
       Add / View         Edit / Delete     Future Features
            |                 |                 |
            +-----------------+-----------------+
                              |
                              v
                    +-------------------+
                    | Expense Array      |
                    | struct Expense[]   |
                    +---------+---------+
                              |
                              v
                    +-------------------+
                    | File Storage       |
                    | user_XXX.txt       |
                    +-------------------+
```

The application therefore follows a simple cycle:

```text
Read Data
   |
   v
Validate Data
   |
   v
Process Data
   |
   v
Store Data
   |
   v
Display Result
```

---

# Learning Outcomes

This project demonstrates a transition from basic C programming to a more complete application-oriented program.

The major concepts practiced include:

* Modular programming
* Function decomposition
* Structures
* Arrays of structures
* Pointers
* String manipulation
* File handling
* Input validation
* Searching
* Array modification
* Data persistence
* Date validation
* Character classification
* Formatted file storage
* Menu-driven program design
* Separation of responsibilities between functions

The most significant step beyond basic C exercises is the combination of these concepts into a single persistent application.

---

# Future Direction

The current architecture is intentionally simple enough to understand while still providing a foundation for future development.

The project can later be extended with more advanced C concepts such as dynamic memory allocation, improved data structures, stronger file handling, modular source files, and eventually database-backed storage.

---

# Disclaimer

This project is an educational C programming project.

It is not intended to be used as a real financial management system. Passwords are stored as plain text and the application does not implement the security mechanisms required for handling sensitive financial or authentication data.
