#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define USERS_FILE     "users.txt"
#define INPUT_SIZE     100    
#define MAX_NAME       30     
#define MAX_PASS       30     
#define MAX_ATTEMPTS   3       

#define MAX_EXPENSES   500    
#define MAX_AMOUNT     10000000.0f
#define NUM_CATEGORIES 8
#define MAX_CUSTOM_LEN 15     

#define CURRENCY       "Rs"

#define DOUBLE_LINE    "========================================"
#define SINGLE_LINE    "----------------------------------------"
#define TABLE_LINE     "======================================================================"

#define RESULT_LOGOUT  1
#define RESULT_EXIT    2


struct User
{
    int  id;                   
    char username[MAX_NAME];  
    char password[MAX_PASS];   
};


struct Expense
{
    int   id;                  
    char  date[20];          
    char  category[30];       
    float amount;            
    char  description[100];  
};

const char categoryNames[NUM_CATEGORIES][20] =
{
    "Food", "Travel", "Education", "Shopping",
    "Entertainment", "Bills", "Health", "Other"
};


void readLine(char *buffer, int size);
int  readNumber(const char *prompt);
void getFirstName(const char *username, char *firstName);

int  validateUsername(const char *username);
int  validatePassword(const char *password);
int  findUser(const char *username, struct User *found);
int  usernameExists(const char *username);
int  getNextUserId(void);
void getUserFileName(int id, char *fileName);
void createUserFile(int id);
void saveUser(const struct User *user);
int  askTryAgain(void);
int  registerUser(struct User *current);
int  loginUser(struct User *current);

void  replacePipes(char *text);
float readAmount(const char *prompt);
int   isLeapYear(int year);
int   daysInMonth(int month, int year);
int   validateDate(const char *text);
void  getTodayDate(char *today);
void  readDate(const char *prompt, char *date);
void  chooseCategory(char *category);
void  readDescription(const char *prompt, char *description);

int  loadExpenses(const char *fileName, struct Expense list[]);
int  saveAllExpenses(const char *fileName, const struct Expense list[], int count);
int  getNextExpenseId(const struct Expense list[], int count);
int  findExpenseIndex(const struct Expense list[], int count, int id);

void printTableHeader(void);
void printExpenseRow(const struct Expense *e);
void addExpense(const char *fileName);
void viewExpenses(const char *fileName);
void editExpense(const char *fileName);
void deleteExpense(const char *fileName);

void showWelcomeMenu(void);
void showWelcomeBack(const char *fileName, const char *firstName);
int  userMenu(const struct User *user);


void readLine(char *buffer, int size)
{
    int length;
    int ch;

    if (fgets(buffer, size, stdin) == NULL)
    {
        printf("\nInput closed. Goodbye!\n");
        exit(0);
    }

    length = strlen(buffer);

    if (length > 0 && buffer[length - 1] == '\n')
    {
        buffer[length - 1] = '\0';  
    }
    else
    {
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
            /* do nothing, just discard characters */
        }
    }
}

int readNumber(const char *prompt)
{
    char line[INPUT_SIZE];
    int  number;

    while (1)
    {
        printf("%s", prompt);
        readLine(line, INPUT_SIZE);

        if (sscanf(line, "%d", &number) == 1)
        {
            return number;
        }

        printf("\nInvalid input!\nPlease enter a number.\n\n");
    }
}

void getFirstName(const char *username, char *firstName)
{
    int i = 0;

    while (username[i] != '\0' && username[i] != ' ')
    {
        firstName[i] = username[i];
        i++;
    }
    firstName[i] = '\0';
}


int validateUsername(const char *username)
{
    int i;
    int length = strlen(username);

    if (length == 0)
    {
        printf("\nUsername cannot be empty.\n\n");
        return 0;
    }

    if (length >= MAX_NAME)
    {
        printf("\nUsername is too long (maximum %d characters).\n\n", MAX_NAME - 1);
        return 0;
    }

    if (username[0] == ' ' || username[length - 1] == ' ')
    {
        printf("\nUsername cannot start or end with a space.\n\n");
        return 0;
    }

    for (i = 0; i < length; i++)
    {
        char c = username[i];

        if (!isalnum((unsigned char)c) && c != ' ' && c != '_')
        {
            printf("\nUsername can only contain letters, digits,\n");
            printf("spaces and underscores.\n\n");
            return 0;
        }
    }

    return 1;
}


int validatePassword(const char *password)
{
    int i;
    int length     = strlen(password);
    int hasLetter  = 0;
    int hasDigit   = 0;
    int hasBadChar = 0;
    int valid      = 1;

    for (i = 0; i < length; i++)
    {
        if (isalpha((unsigned char)password[i]))
        {
            hasLetter = 1;
        }
        else if (isdigit((unsigned char)password[i]))
        {
            hasDigit = 1;
        }

        /* spaces and '|' would break our file format */
        if (password[i] == ' ' || password[i] == '|')
        {
            hasBadChar = 1;
        }
    }

    if (length < 6 || length >= MAX_PASS || !hasLetter || !hasDigit || hasBadChar)
    {
        valid = 0;
    }

    if (!valid)
    {
        printf("\n%s\n", DOUBLE_LINE);
        printf("            INVALID PASSWORD!\n");
        printf("%s\n\n", DOUBLE_LINE);

        if (length < 6)
            printf("- Password must contain at least 6 characters.\n");
        if (length >= MAX_PASS)
            printf("- Password is too long (maximum %d characters).\n", MAX_PASS - 1);
        if (!hasLetter)
            printf("- Password must contain at least one alphabet.\n");
        if (!hasDigit)
            printf("- Password must contain at least one digit.\n");
        if (hasBadChar)
            printf("- Password cannot contain spaces or the '|' symbol.\n");

        printf("\n");
    }

    return valid;
}


int findUser(const char *username, struct User *found)
{
    FILE *fp;
    char line[200];
    struct User temp;

    fp = fopen(USERS_FILE, "r");
    if (fp == NULL)
    {
        return 0;   /* file doesn't exist yet = nobody is registered */
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        /*
         * sscanf pattern explained:
         *   %d          read a whole number (the id)
         *   |           expect a literal '|' character
         *   %29[^|]     read up to 29 characters that are NOT '|'  (username)
         *   |           expect another literal '|'
         *   %29[^\n]    read up to 29 characters until the end of line (password)
         */
        if (sscanf(line, "%d|%29[^|]|%29[^\n]",
                   &temp.id, temp.username, temp.password) == 3)
        {
            if (strcmp(temp.username, username) == 0)
            {
                *found = temp;    /* copy the whole record to the caller */
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);
    return 0;
}

int usernameExists(const char *username)
{
    struct User temp;
    return findUser(username, &temp);
}

int getNextUserId(void)
{
    FILE *fp;
    char line[200];
    int  id;
    int  highest = 0;

    fp = fopen(USERS_FILE, "r");
    if (fp == NULL)
    {
        return 1;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line, "%d|", &id) == 1 && id > highest)
        {
            highest = id;
        }
    }

    fclose(fp);
    return highest + 1;
}

void getUserFileName(int id, char *fileName)
{
    sprintf(fileName, "user_%03d.txt", id);
}


void createUserFile(int id)
{
    char fileName[30];
    FILE *fp;

    getUserFileName(id, fileName);

    fp = fopen(fileName, "a");
    if (fp == NULL)
    {
        printf("Warning: could not create %s\n", fileName);
        return;
    }
    fclose(fp);
}


void saveUser(const struct User *user)
{
    FILE *fp = fopen(USERS_FILE, "a");

    if (fp == NULL)
    {
        printf("Error: could not open %s for writing.\n", USERS_FILE);
        return;
    }

    fprintf(fp, "%d|%s|%s\n", user->id, user->username, user->password);
    fclose(fp);
}


int askTryAgain(void)
{
    int choice;

    printf("1) Try again\n");
    printf("2) Back to main menu\n\n");

    while (1)
    {
        choice = readNumber("Enter your choice: ");

        if (choice == 1) return 1;
        if (choice == 2) return 0;

        printf("\nInvalid choice!\nPlease select an available option.\n\n");
    }
}


int registerUser(struct User *current)
{
    struct User newUser;
    char input[INPUT_SIZE];

    printf("\n========== CREATE ACCOUNT ==========\n\n");

    while (1)
    {
        printf("Enter your username: ");
        readLine(input, INPUT_SIZE);

        if (!validateUsername(input))
        {
            continue;               
        }

        if (usernameExists(input))
        {
            printf("\n%s\n", DOUBLE_LINE);
            printf("       USERNAME ALREADY EXISTS!\n");
            printf("%s\n\n", DOUBLE_LINE);
            printf("This username is already registered.\n");
            printf("Please choose another username.\n\n");
            continue;
        }

        break; 
    }
    strcpy(newUser.username, input);

    while (1)
    {
        printf("Enter a password: ");
        readLine(input, INPUT_SIZE);

        if (validatePassword(input))
        {
            break;
        }
    }
    strcpy(newUser.password, input);
    printf("\nPassword accepted!\n");

    newUser.id = getNextUserId();
    saveUser(&newUser);
    createUserFile(newUser.id);

    printf("\nAccount created successfully!\n");

    *current = newUser;
    return 1;
}


int loginUser(struct User *current)
{
    struct User found;
    char input[INPUT_SIZE];
    char firstName[MAX_NAME];
    int  attempt;
    int  remaining;

    printf("\n============== LOGIN ===============\n\n");

    while (1)
    {
        printf("Enter your username: ");
        readLine(input, INPUT_SIZE);

        if (findUser(input, &found))
        {
            break; 
        }

        printf("\n%s\n", DOUBLE_LINE);
        printf("        USER NOT IDENTIFIED!\n");
        printf("%s\n\n", DOUBLE_LINE);
        printf("The username does not exist.\n");
        printf("Please try again.\n\n");

        if (!askTryAgain())
        {
            return 0;  
        }
        printf("\n");
    }

    for (attempt = 1; attempt <= MAX_ATTEMPTS; attempt++)
    {
        printf("Enter your password: ");
        readLine(input, INPUT_SIZE);

        if (strcmp(input, found.password) == 0)
        {
            getFirstName(found.username, firstName);
            printf("\nLogin successful!\n\n");
            printf("Welcome, %s!\n", firstName);

            *current = found;
            return 1;
        }

        remaining = MAX_ATTEMPTS - attempt;
        if (remaining > 0)
        {
            printf("\nIncorrect password!\n");
            printf("Attempts remaining: %d\n\n", remaining);
        }
    }

    printf("\nToo many incorrect attempts.\n\n");
    printf("Returning to the main menu...\n");
    return 0;
}

void replacePipes(char *text)
{
    int i;

    for (i = 0; text[i] != '\0'; i++)
    {
        if (text[i] == '|')
        {
            text[i] = '-';
        }
    }
}


float readAmount(const char *prompt)
{
    char  line[INPUT_SIZE];
    int   i, start, length;
    int   digits, dots, ok;
    float amount;

    while (1)
    {
        printf("%s", prompt);
        readLine(line, INPUT_SIZE);

        length = strlen(line);
        start  = 0;
        digits = 0;
        dots   = 0;
        ok     = (length > 0);

        if (line[0] == '-')     
        {                       
            start = 1;
        }

        for (i = start; i < length; i++)
        {
            if (isdigit((unsigned char)line[i]))
                digits++;
            else if (line[i] == '.')
                dots++;
            else
                ok = 0;
        }

        if (!ok || digits == 0 || dots > 1)
        {
            printf("\nInvalid input!\nPlease enter a number.\n\n");
            continue;
        }

        amount = (float)atof(line);   

        if (amount <= 0)
        {
            printf("\nInvalid amount!\nExpense amount must be greater than 0.\n\n");
            continue;
        }

        if (amount > MAX_AMOUNT)
        {
            printf("\nInvalid amount!\nMaximum allowed is %.0f.\n\n", MAX_AMOUNT);
            continue;
        }

        return amount;
    }
}

int isLeapYear(int year)
{
    if (year % 400 == 0) return 1;
    if (year % 100 == 0) return 0;
    if (year % 4 == 0)   return 1;
    return 0;
}


int daysInMonth(int month, int year)
{
    switch (month)
    {
        case 2:
            return isLeapYear(year) ? 29 : 28;   
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        default:
            return 31;
    }
}


int validateDate(const char *text)
{
    int i;
    int day, month, year;

    if (strlen(text) != 10)
    {
        return 0;
    }

    for (i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5)
        {
            if (text[i] != '/') return 0;
        }
        else if (!isdigit((unsigned char)text[i]))
        {
            return 0;
        }
    }

    sscanf(text, "%d/%d/%d", &day, &month, &year);

    if (year < 2000 || year > 2100) return 0;
    if (month < 1 || month > 12)    return 0;
    if (day < 1 || day > daysInMonth(month, year)) return 0;

    return 1;
}


void getTodayDate(char *today)
{
    time_t now = time(NULL);
    struct tm *info = localtime(&now);

    strftime(today, 20, "%d/%m/%Y", info);
}

void readDate(const char *prompt, char *date)
{
    char line[INPUT_SIZE];

    while (1)
    {
        printf("%s", prompt);
        readLine(line, INPUT_SIZE);

        if (strlen(line) == 0)
        {
            getTodayDate(date);
            printf("Using today's date: %s\n", date);
            return;
        }

        if (validateDate(line))
        {
            strcpy(date, line);
            return;
        }

        printf("\nInvalid date!\n");
        printf("Use the format DD/MM/YYYY (for example 06/10/2026).\n\n");
    }
}


void chooseCategory(char *category)
{
    char input[INPUT_SIZE];
    int  i;
    int  choice;

    printf("\nSelect a category:\n");
    for (i = 0; i < NUM_CATEGORIES; i++)
    {
        printf("%d) %s\n", i + 1, categoryNames[i]);
    }
    printf("9) Custom category\n\n");

    while (1)
    {
        choice = readNumber("Enter category number: ");

        if (choice >= 1 && choice <= NUM_CATEGORIES)
        {
            strcpy(category, categoryNames[choice - 1]);
            return;
        }

        if (choice == 9)
        {
            while (1)
            {
                printf("Enter custom category: ");
                readLine(input, INPUT_SIZE);
                replacePipes(input);

                if (strlen(input) == 0)
                {
                    printf("\nCategory cannot be empty.\n\n");
                }
                else if (strlen(input) > MAX_CUSTOM_LEN)
                {
                    printf("\nCategory is too long (maximum %d characters).\n\n",
                           MAX_CUSTOM_LEN);
                }
                else
                {
                    strcpy(category, input);
                    return;
                }
            }
        }

        printf("\nInvalid choice!\nPlease select an available option.\n\n");
    }
}


void readDescription(const char *prompt, char *description)
{
    char input[INPUT_SIZE];

    printf("%s", prompt);
    readLine(input, INPUT_SIZE);
    replacePipes(input);

    if (strlen(input) == 0)
    {
        strcpy(description, "-");   
    }
    else
    {
        strcpy(description, input);
    }
}


int loadExpenses(const char *fileName, struct Expense list[])
{
    FILE *fp;
    char line[300];
    int  count = 0;

    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        return 0;
    }

    while (count < MAX_EXPENSES && fgets(line, sizeof(line), fp) != NULL)
    {
        
        if (sscanf(line, "%d|%19[^|]|%29[^|]|%f|%99[^\n]",
                   &list[count].id,
                   list[count].date,
                   list[count].category,
                   &list[count].amount,
                   list[count].description) == 5)
        {
            count++;   
        }
    }

    fclose(fp);
    return count;
}


int saveAllExpenses(const char *fileName, const struct Expense list[], int count)
{
    FILE *fp;
    int   i;

    fp = fopen(fileName, "w");
    if (fp == NULL)
    {
        printf("Error: could not save to %s\n", fileName);
        return 0;
    }

    for (i = 0; i < count; i++)
    {
        fprintf(fp, "%d|%s|%s|%.2f|%s\n",
                list[i].id, list[i].date, list[i].category,
                list[i].amount, list[i].description);
    }

    fclose(fp);
    return 1;
}


int getNextExpenseId(const struct Expense list[], int count)
{
    int i;
    int highest = 0;

    for (i = 0; i < count; i++)
    {
        if (list[i].id > highest)
        {
            highest = list[i].id;
        }
    }

    return highest + 1;
}

int findExpenseIndex(const struct Expense list[], int count, int id)
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (list[i].id == id)
        {
            return i;
        }
    }

    return -1;
}


void printTableHeader(void)
{
    printf("\n%s\n", TABLE_LINE);
    printf("%-4s %-12s %-15s %-12s %s\n",
           "ID", "Date", "Category", "Amount", "Description");
    printf("%s\n", TABLE_LINE);
}


void printExpenseRow(const struct Expense *e)
{
    printf("%-4d %-12s %-15s %s%-11.2f %s\n",
           e->id, e->date, e->category, CURRENCY, e->amount, e->description);
}

void addExpense(const char *fileName)
{
    struct Expense list[MAX_EXPENSES];
    struct Expense newExpense;
    int count;

    count = loadExpenses(fileName, list);

    printf("\n%s\n", DOUBLE_LINE);
    printf("             ADD EXPENSE\n");
    printf("%s\n\n", DOUBLE_LINE);

    if (count >= MAX_EXPENSES)
    {
        printf("Your expense list is full (%d expenses).\n", MAX_EXPENSES);
        printf("Please delete some old expenses first.\n");
        return;
    }

    
    newExpense.amount = readAmount("Enter amount: ");
    chooseCategory(newExpense.category);
    printf("\n");
    readDate("Enter date (DD/MM/YYYY, or press Enter for today): ", newExpense.date);
    readDescription("Enter description: ", newExpense.description);

    newExpense.id = getNextExpenseId(list, count);

    list[count] = newExpense;    
    count++;

    if (saveAllExpenses(fileName, list, count))
    {
        printf("\nExpense added successfully!\n");
        printf("%s\n", SINGLE_LINE);
        printf("ID          : %d\n", newExpense.id);
        printf("Date        : %s\n", newExpense.date);
        printf("Category    : %s\n", newExpense.category);
        printf("Amount      : %s%.2f\n", CURRENCY, newExpense.amount);
        printf("Description : %s\n", newExpense.description);
        printf("%s\n", SINGLE_LINE);
    }
}


void viewExpenses(const char *fileName)
{
    struct Expense list[MAX_EXPENSES];
    int   count;
    int   i;
    float total = 0;

    count = loadExpenses(fileName, list);

    printf("\n%s\n", DOUBLE_LINE);
    printf("            YOUR EXPENSES\n");
    printf("%s\n", DOUBLE_LINE);

    if (count == 0)
    {
        printf("\nNo expenses recorded yet.\n");
        return;
    }

    printTableHeader();

    for (i = 0; i < count; i++)
    {
        printExpenseRow(&list[i]);
        total = total + list[i].amount;
    }

    printf("%s\n", TABLE_LINE);
    printf("Total Expenses: %s%.2f\n", CURRENCY, total);
}


void editExpense(const char *fileName)
{
    struct Expense list[MAX_EXPENSES];
    int count;
    int id, index, choice;

    count = loadExpenses(fileName, list);

    printf("\n%s\n", DOUBLE_LINE);
    printf("            EDIT EXPENSE\n");
    printf("%s\n", DOUBLE_LINE);

    if (count == 0)
    {
        printf("\nNo expenses recorded yet.\n");
        return;
    }

    id = readNumber("\nEnter Expense ID to edit: ");
    index = findExpenseIndex(list, count, id);

    if (index == -1)
    {
        printf("\nNo expense found with ID %d.\n", id);
        return;
    }

    printf("\nCurrent details:");
    printTableHeader();
    printExpenseRow(&list[index]);
    printf("%s\n\n", TABLE_LINE);

    printf("1) Edit Amount\n");
    printf("2) Edit Category\n");
    printf("3) Edit Date\n");
    printf("4) Edit Description\n");
    printf("5) Edit Everything\n");
    printf("6) Cancel\n\n");

    while (1)
    {
        choice = readNumber("Enter your choice: ");
        if (choice >= 1 && choice <= 6)
        {
            break;
        }
        printf("\nInvalid choice!\nPlease select an available option.\n\n");
    }

    if (choice == 6)
    {
        printf("\nEdit cancelled. Nothing was changed.\n");
        return;
    }

    printf("\n");

    
    if (choice == 1 || choice == 5)
    {
        list[index].amount = readAmount("Enter new amount: ");
    }
    if (choice == 2 || choice == 5)
    {
        chooseCategory(list[index].category);
        printf("\n");
    }
    if (choice == 3 || choice == 5)
    {
        readDate("Enter new date (DD/MM/YYYY, or press Enter for today): ",
                 list[index].date);
    }
    if (choice == 4 || choice == 5)
    {
        readDescription("Enter new description: ", list[index].description);
    }

    if (saveAllExpenses(fileName, list, count))
    {
        printf("\nExpense updated successfully!\n");
        printTableHeader();
        printExpenseRow(&list[index]);
        printf("%s\n", TABLE_LINE);
    }
}

void deleteExpense(const char *fileName)
{
    struct Expense list[MAX_EXPENSES];
    int count;
    int id, index, choice, i;

    count = loadExpenses(fileName, list);

    printf("\n%s\n", DOUBLE_LINE);
    printf("           DELETE EXPENSE\n");
    printf("%s\n", DOUBLE_LINE);

    if (count == 0)
    {
        printf("\nNo expenses recorded yet.\n");
        return;
    }

    id = readNumber("\nEnter Expense ID to delete: ");
    index = findExpenseIndex(list, count, id);

    if (index == -1)
    {
        printf("\nNo expense found with ID %d.\n", id);
        return;
    }

    printTableHeader();
    printExpenseRow(&list[index]);
    printf("%s\n\n", TABLE_LINE);

    printf("Are you sure you want to delete this expense?\n\n");
    printf("1) Yes\n");
    printf("2) No\n\n");

    while (1)
    {
        choice = readNumber("Enter your choice: ");
        if (choice == 1 || choice == 2)
        {
            break;
        }
        printf("\nInvalid choice!\nPlease select an available option.\n\n");
    }

    if (choice == 2)
    {
        printf("\nDeletion cancelled. Nothing was changed.\n");
        return;
    }


    for (i = index; i < count - 1; i++)
    {
        list[i] = list[i + 1];
    }
    count--;

    if (saveAllExpenses(fileName, list, count))
    {
        printf("\nExpense deleted successfully!\n");
    }
}


void showWelcomeMenu(void)
{
    printf("\n%s\n", DOUBLE_LINE);
    printf("             EXPENSE TRACKER\n");
    printf("%s\n\n", DOUBLE_LINE);
    printf("1) Registered User\n");
    printf("2) New User\n");
    printf("3) Exit\n\n");
    printf("%s\n", DOUBLE_LINE);
}


void showWelcomeBack(const char *fileName, const char *firstName)
{
    struct Expense list[MAX_EXPENSES];
    int count = loadExpenses(fileName, list);

    if (count > 0)
    {
        printf("\nWelcome back, %s!\n", firstName);
        printf("Previous expenses loaded successfully (%d saved).\n", count);
    }
}


int userMenu(const struct User *user)
{
    char firstName[MAX_NAME];
    char fileName[30];
    int  choice;

    getFirstName(user->username, firstName);
    getUserFileName(user->id, fileName);   

    showWelcomeBack(fileName, firstName);

    while (1)
    {
        printf("\n%s\n", DOUBLE_LINE);
        printf("             EXPENSE TRACKER\n");
        printf("%s\n\n", DOUBLE_LINE);
        printf("Welcome, %s!\n\n", firstName);
        printf("1) Add Expense\n");
        printf("2) View Expenses\n");
        printf("3) Edit Expense\n");
        printf("4) Delete Expense\n");
        printf("5) Search Expenses\n");
        printf("6) Expense Summary\n");
        printf("7) Budget Management\n");
        printf("8) Logout\n");
        printf("9) Exit\n\n");
        printf("%s\n", DOUBLE_LINE);

        choice = readNumber("Enter your choice: ");

        switch (choice)
        {
            case 1:
                addExpense(fileName);
                break;
            case 2:
                viewExpenses(fileName);
                break;
            case 3:
                editExpense(fileName);
                break;
            case 4:
                deleteExpense(fileName);
                break;
            case 5:
            case 6:
                printf("\nComing in Version 3 (Search & Summary).\n");
                break;
            case 7:
                printf("\nComing in Version 4 (Budget).\n");
                break;
            case 8:
                printf("\nLogging out...\n\n");
                printf("Goodbye, %s!\n", firstName);
                return RESULT_LOGOUT;
            case 9:
                printf("\n%s\n\n", DOUBLE_LINE);
                printf("Thank you, %s, for working with us today!\n\n", firstName);
                printf("%s\n", DOUBLE_LINE);
                return RESULT_EXIT;
            default:
                printf("\nInvalid choice!\n");
                printf("Please select an available option.\n");
        }
    }
}


int main(void)
{
    struct User currentUser;
    int running = 1;
    int choice;
    int loggedIn;
    int result;

    while (running)
    {
        showWelcomeMenu();
        choice = readNumber("Enter your choice: ");

        switch (choice)
        {
            case 1:
                loggedIn = loginUser(&currentUser);
                break;
            case 2:
                loggedIn = registerUser(&currentUser);
                break;
            case 3:
                printf("\n%s\n\n", DOUBLE_LINE);
                printf("   Thank you for using Expense Tracker!\n\n");
                printf("%s\n", DOUBLE_LINE);
                running = 0;
                loggedIn = 0;
                break;
            default:
                printf("\nInvalid choice!\n");
                printf("Please select an available option.\n");
                loggedIn = 0;
        }
        
        if (loggedIn)
        {
            result = userMenu(&currentUser);

            if (result == RESULT_EXIT)
            {
                running = 0;
            }            
        }
    }

    return 0;
}