#include <stdio.h>
#include "reports.h"
#include "academic.h"
#include "analytics.h"
#include "student.h"
#include "input.h"

void printStudentReport(FILE *out, const Student *student, const SubjectConfig *config)
{
    printLine(out, '=');
    fprintf(out, "STUDENT REPORT\n");
    printLine(out, '=');
    fprintf(out, "Student ID : %d\n", student->id);
    fprintf(out, "Full Name  : %s\n", student->name);
    fprintf(out, "Age        : %d\n", student->age);
    fprintf(out, "Course     : %s\n", student->course);
    fprintf(out, "Semester   : %d\n", student->semester);
    fprintf(out, "Email      : %s\n", student->email);
    printLine(out, '-');
    fprintf(out, "ACADEMIC RECORD\n");
    printLine(out, '-');
    printAcademicDetails(out, student, config);
    printLine(out, '=');
}

static void printClassReport(FILE *out, const Database *db)
{
    int order[MAX_STUDENTS];
    int i;
    const Student *student;
    Result result;
    char percent[16];

    printLine(out, '=');
    fprintf(out, "CLASS REPORT\n");
    printLine(out, '=');

    if (db->count == 0) {
        fprintf(out, "No student records exist.\n");
        return;
    }

    sortIndices(db, order, SORT_BY_ID);

    fprintf(out, "%-6s %-*s %-*s %-3s %-8s %-6s\n", "ID", NAME_LEN - 1, "NAME",
            COURSE_LEN - 1, "COURSE", "SEM", "PERCENT", "RESULT");
    printLine(out, '-');

    for (i = 0; i < db->count; i++) {
        student = &db->students[order[i]];
        result = calculateResult(student, &db->subjects);
        if (result.available) {
            snprintf(percent, sizeof(percent), "%.2f%%", result.percentage);
        } else {
            snprintf(percent, sizeof(percent), "--");
        }
        fprintf(out, "%-6d %-*s %-*s %-3d %-8s %-6s\n", student->id, NAME_LEN - 1,
                student->name, COURSE_LEN - 1, student->course, student->semester, percent,
                resultLabel(&result));
    }
    printLine(out, '-');

    fprintf(out, "\nSUMMARY STATISTICS\n");
    printLine(out, '-');
    printClassOverview(out, db);

    fprintf(out, "\nSUBJECT STATISTICS\n");
    printLine(out, '-');
    printSubjectStatistics(out, db);
    printLine(out, '=');
}

static int finishReportFile(FILE *file, const char *path)
{
    int failed = ferror(file);

    if (fclose(file) != 0) {
        failed = 1;
    }
    if (failed) {
        printf("Error: writing '%s' failed. The report may be incomplete.\n", path);
        return 0;
    }
    printf("Success: report saved to %s\n", path);
    return 1;
}

static FILE *openReportFile(const char *path)
{
    FILE *file = fopen(path, "w");

    if (file == NULL) {
        printf("Error: could not create '%s'.\n", path);
        printf("Make sure the '%s' folder exists and is writable.\n", REPORT_DIR);
    }
    return file;
}

static void saveStudentReport(const Database *db, int index)
{
    char path[PATH_LEN];
    FILE *file;

    snprintf(path, sizeof(path), "%s/Student_%d.txt", REPORT_DIR, db->students[index].id);
    file = openReportFile(path);
    if (file == NULL) {
        return;
    }
    printStudentReport(file, &db->students[index], &db->subjects);
    finishReportFile(file, path);
}

static void saveClassReport(const Database *db)
{
    char path[PATH_LEN];
    FILE *file;

    snprintf(path, sizeof(path), "%s/Class_Report.txt", REPORT_DIR);
    file = openReportFile(path);
    if (file == NULL) {
        return;
    }
    printClassReport(file, db);
    finishReportFile(file, path);
}

static void individualReport(const Database *db)
{
    int index;
    int choice;

    printHeader("INDIVIDUAL STUDENT REPORT");

    if (!requireStudents(db)) {
        return;
    }
    printCancelHint();
    if (!selectStudentById(db, &index)) {
        return;
    }

    printf("\n 1. Display the report in the console\n");
    printf(" 2. Save the report to a text file\n");
    printf(" 3. Return to the report menu\n");
    printLine(stdout, '-');

    if (promptMenuChoice(1, 3, &choice) != INPUT_OK || choice == 3) {
        return;
    }

    if (choice == 1) {
        printf("\n");
        printStudentReport(stdout, &db->students[index], &db->subjects);
    } else {
        saveStudentReport(db, index);
    }
}

static void classReport(const Database *db)
{
    int choice;

    printHeader("CLASS REPORT");

    if (!requireStudents(db)) {
        return;
    }

    printf(" 1. Display the class report in the console\n");
    printf(" 2. Save the class report to a text file\n");
    printf(" 3. Return to the report menu\n");
    printLine(stdout, '-');

    if (promptMenuChoice(1, 3, &choice) != INPUT_OK || choice == 3) {
        return;
    }

    if (choice == 1) {
        printf("\n");
        printClassReport(stdout, db);
    } else {
        saveClassReport(db);
    }
}

void reportsMenu(const Database *db)
{
    int choice;

    for (;;) {
        printHeader("GENERATE STUDENT REPORTS");
        printf(" 1. Individual Student Report\n");
        printf(" 2. Class Report\n");
        printf(" 3. Return to Main Menu\n");
        printLine(stdout, '=');

        if (promptMenuChoice(1, 3, &choice) != INPUT_OK || choice == 3) {
            return;
        }

        if (choice == 1) {
            individualReport(db);
        } else {
            classReport(db);
        }
    }
}