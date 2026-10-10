#include <stdio.h>
#include "types.h"
#include "academic.h"
#include "analytics.h"
#include "file.h"
#include "input.h"
#include "reports.h"
#include "student.h"

static void printMainMenu(void)
{
    printHeader("STUDENT MANAGEMENT SYSTEM");
    printf(" 1. Add New Student\n");
    printf(" 2. View All Students\n");
    printf(" 3. Search Student\n");
    printf(" 4. Update Student Details\n");
    printf(" 5. Delete Student Record\n");
    printf(" 6. Manage Academic Records\n");
    printf(" 7. View Student Results\n");
    printf(" 8. Class Statistics and Analytics\n");
    printf(" 9. Sort Student Records\n");
    printf("10. Generate Student Reports\n");
    printf("11. Manage Data and Backups\n");
    printf("12. About This Application\n");
    printf("13. Save and Exit\n");
    printLine(stdout, '=');
}

static void showAbout(void)
{
    printHeader("ABOUT THIS APPLICATION");
    printf("Student Management System - an educational console application in C.\n\n");
    printf("Each student is stored in their own text file: %s/student_<ID>.txt\n",
           DATA_DIR "/students");
    printf("An index file lists the registered IDs, and a subject file holds the\n");
    printf("subject names and maximum marks shared by all students.\n\n");
    printf("Example rules used by this program (not universal standards):\n");
    printf("  - A subject is passed with at least %d%% of its maximum marks.\n",
           SUBJECT_PASS_PERCENT);
    printf("  - A student passes only if every subject is passed.\n");
    printf("  - Grades: A+ 90, A 80, B 70, C 60, D 50, E below 50 (F if failed).\n\n");
    printf("This is a learning project, not a production college database.\n");
    printf("Files are plain text and are not encrypted.\n");
}

static int saveBeforeExit(const Database *db)
{
    if (!saveToLocation(&DATA_LOCATION, db)) {
        printf("Warning: the final save failed. Earlier changes were saved as you made them,\n");
        printf("but check the '%s' folder before relying on the data.\n", DATA_DIR);
        return 0;
    }
    return 1;
}

int main(void)
{
    Database db;
    LoadStatus status;
    int skipped = 0;
    int choice;
    int running = 1;

    printHeader("STUDENT MANAGEMENT SYSTEM");
    printf("Starting up...\n");

    if (!ensureStorageReady()) {
        return 1;
    }

    status = loadFromLocation(&DATA_LOCATION, &db, &skipped);
    if (status == LOAD_FATAL) {
        printf("The program cannot start safely. Your files were not modified.\n");
        printf("Repair or move the damaged file, or copy a backup into the data folder.\n");
        return 1;
    }

    if (status == LOAD_NEW) {
        printf("No existing data found. Creating a new database.\n");
        if (!saveToLocation(&DATA_LOCATION, &db)) {
            printf("Warning: the initial data files could not be created.\n");
        }
    } else {
        printf("Loaded %d student record(s).\n", db.count);
        if (skipped > 0) {
            printf("Warning: %d record(s) could not be loaded. Their files were left untouched.\n",
                   skipped);
        }
    }

    while (running) {
        printMainMenu();
        if (promptMenuChoice(1, 13, &choice) != INPUT_OK) {
            break;
        }

        switch (choice) {
        case 1:
            addStudent(&db);
            break;
        case 2:
            viewAllStudents(&db);
            break;
        case 3:
            searchStudentMenu(&db);
            break;
        case 4:
            updateStudent(&db);
            break;
        case 5:
            deleteStudent(&db);
            break;
        case 6:
            manageAcademicMenu(&db);
            break;
        case 7:
            viewStudentResults(&db);
            break;
        case 8:
            analyticsMenu(&db);
            break;
        case 9:
            sortStudentsMenu(&db);
            break;
        case 10:
            reportsMenu(&db);
            break;
        case 11:
            dataMenu(&db);
            break;
        case 12:
            showAbout();
            break;
        case 13:
            running = 0;
            break;
        }
    }

    if (!saveBeforeExit(&db)) {
        return 1;
    }

    printf("\nAll data saved. Goodbye!\n");
    return 0;
}