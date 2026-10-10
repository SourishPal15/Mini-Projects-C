#include <stdio.h>
#include <string.h>
#include "academic.h"
#include "student.h"
#include "file.h"
#include "input.h"

void setDefaultSubjects(SubjectConfig *config)
{
    int i;

    for (i = 0; i < MAX_SUBJECTS; i++) {
        snprintf(config->names[i], SUBJECT_NAME_LEN, "Subject %d", i + 1);
        config->maxMarks[i] = DEFAULT_MAX_MARKS;
    }
}

int isSubjectPassed(int marks, int maxMarks)
{
    return marks * 100 >= maxMarks * SUBJECT_PASS_PERCENT;
}

static void assignGrade(Result *result)
{
    const char *grade;

    if (!result->passed) {
        grade = "F";
    } else if (result->percentage >= 90.0) {
        grade = "A+";
    } else if (result->percentage >= 80.0) {
        grade = "A";
    } else if (result->percentage >= 70.0) {
        grade = "B";
    } else if (result->percentage >= 60.0) {
        grade = "C";
    } else if (result->percentage >= 50.0) {
        grade = "D";
    } else {
        grade = "E";
    }
    snprintf(result->grade, sizeof(result->grade), "%s", grade);
}

Result calculateResult(const Student *student, const SubjectConfig *config)
{
    Result result;
    int i;

    memset(&result, 0, sizeof(result));
    snprintf(result.grade, sizeof(result.grade), "N/A");

    if (!student->hasMarks) {
        return result;
    }

    result.available = 1;
    result.highest = student->marks[0];
    result.lowest = student->marks[0];

    for (i = 0; i < MAX_SUBJECTS; i++) {
        result.total += student->marks[i];
        result.maxTotal += config->maxMarks[i];

        if (student->marks[i] > result.highest) {
            result.highest = student->marks[i];
        }
        if (student->marks[i] < result.lowest) {
            result.lowest = student->marks[i];
        }

        if (isSubjectPassed(student->marks[i], config->maxMarks[i])) {
            result.passedSubjects++;
        } else {
            result.failedSubjects++;
        }
    }

    if (result.maxTotal > 0) {
        result.percentage = (double)result.total * 100.0 / (double)result.maxTotal;
    }
    result.passed = (result.failedSubjects == 0);
    assignGrade(&result);
    return result;
}

const char *resultLabel(const Result *result)
{
    if (!result->available) {
        return "N/A";
    }
    return result->passed ? "PASS" : "FAIL";
}

void printSubjectMarks(FILE *out, const Student *student, const SubjectConfig *config)
{
    int i;

    fprintf(out, "%-4s %-*s %7s %7s  %s\n", "NO.", SUBJECT_NAME_LEN - 1, "SUBJECT",
            "MARKS", "MAX", "STATUS");
    printLine(out, '-');

    for (i = 0; i < MAX_SUBJECTS; i++) {
        fprintf(out, "%-4d %-*s %7d %7d  %s\n", i + 1, SUBJECT_NAME_LEN - 1,
                config->names[i], student->marks[i], config->maxMarks[i],
                isSubjectPassed(student->marks[i], config->maxMarks[i]) ? "PASS" : "FAIL");
    }
    printLine(out, '-');
}

void printResultTotals(FILE *out, const Student *student, const SubjectConfig *config)
{
    Result result = calculateResult(student, config);

    if (!result.available) {
        fprintf(out, "Overall Result : NOT AVAILABLE (no academic records entered)\n");
        return;
    }

    fprintf(out, "Total Marks          : %d / %d\n", result.total, result.maxTotal);
    fprintf(out, "Percentage           : %.2f%%\n", result.percentage);
    fprintf(out, "Highest Subject Mark : %d\n", result.highest);
    fprintf(out, "Lowest Subject Mark  : %d\n", result.lowest);
    fprintf(out, "Subjects Passed      : %d\n", result.passedSubjects);
    fprintf(out, "Subjects Failed      : %d\n", result.failedSubjects);
    fprintf(out, "Overall Result       : %s\n", resultLabel(&result));
    fprintf(out, "Overall Grade        : %s\n", result.grade);
}

void printAcademicDetails(FILE *out, const Student *student, const SubjectConfig *config)
{
    if (!student->hasMarks) {
        fprintf(out, "Academic records have not been entered for this student.\n");
        fprintf(out, "Overall Result : NOT AVAILABLE\n");
        return;
    }

    printSubjectMarks(out, student, config);
    fprintf(out, "\n");
    printResultTotals(out, student, config);
}

static int selectStudentWithRecords(const Database *db, int *index)
{
    if (!requireStudents(db)) {
        return 0;
    }
    printCancelHint();
    if (!selectStudentById(db, index)) {
        return 0;
    }
    if (!db->students[*index].hasMarks) {
        printf("This student has no academic records yet. Use 'Enter Academic Records' first.\n");
        return 0;
    }
    return 1;
}

static void enterAcademicRecords(Database *db)
{
    int index;
    int i;
    int marks[MAX_SUBJECTS];
    char prompt[LINE_LEN];
    Student original;
    Student *student;
    InputStatus status;

    printHeader("ENTER ACADEMIC RECORDS");

    if (!requireStudents(db)) {
        return;
    }
    printCancelHint();
    if (!selectStudentById(db, &index)) {
        return;
    }

    student = &db->students[index];
    printf("Student: %s\n", student->name);

    if (student->hasMarks) {
        printf("This student already has academic records.\n");
        if (!confirmAction("Replace all existing marks with new ones?")) {
            printf("Cancelled. Existing marks were kept.\n");
            return;
        }
    }

    for (i = 0; i < MAX_SUBJECTS; i++) {
        snprintf(prompt, sizeof(prompt), "%s (0-%d): ", db->subjects.names[i],
                 db->subjects.maxMarks[i]);
        status = promptInt(prompt, 0, db->subjects.maxMarks[i], &marks[i]);
        if (status != INPUT_OK) {
            printf("The academic records were not saved.\n");
            reportAborted(status);
            return;
        }
    }

    original = *student;
    student->hasMarks = 1;
    for (i = 0; i < MAX_SUBJECTS; i++) {
        student->marks[i] = marks[i];
    }

    if (!saveStudentFile(student)) {
        *student = original;
        printf("Error: the marks could not be saved to disk. The record was left unchanged.\n");
        return;
    }

    printf("\nSuccess: academic records saved for student %d.\n\n", student->id);
    printResultTotals(stdout, student, &db->subjects);
}

static void updateSubjectMarks(Database *db)
{
    int index;
    int subject;
    int mark;
    char prompt[LINE_LEN];
    Student original;
    Student *student;
    InputStatus status;

    printHeader("UPDATE SUBJECT MARKS");

    if (!selectStudentWithRecords(db, &index)) {
        return;
    }

    student = &db->students[index];
    printf("\n");
    printSubjectMarks(stdout, student, &db->subjects);

    snprintf(prompt, sizeof(prompt), "Subject number to update (1-%d): ", MAX_SUBJECTS);
    status = promptInt(prompt, 1, MAX_SUBJECTS, &subject);
    if (status != INPUT_OK) {
        reportAborted(status);
        return;
    }

    snprintf(prompt, sizeof(prompt), "New marks for %s (0-%d): ",
             db->subjects.names[subject - 1], db->subjects.maxMarks[subject - 1]);
    status = promptInt(prompt, 0, db->subjects.maxMarks[subject - 1], &mark);
    if (status != INPUT_OK) {
        reportAborted(status);
        return;
    }

    original = *student;
    student->marks[subject - 1] = mark;

    if (!saveStudentFile(student)) {
        *student = original;
        printf("Error: the marks could not be saved to disk. The record was left unchanged.\n");
        return;
    }

    printf("\nSuccess: marks updated and saved.\n\n");
    printResultTotals(stdout, student, &db->subjects);
}

static void viewSubjectMarks(const Database *db)
{
    int index;

    printHeader("VIEW SUBJECT MARKS");

    if (!selectStudentWithRecords(db, &index)) {
        return;
    }

    printf("\nStudent: %s (ID %d)\n", db->students[index].name, db->students[index].id);
    printSubjectMarks(stdout, &db->students[index], &db->subjects);
}

static int highestMarkForSubject(const Database *db, int subject)
{
    int i;
    int highest = 0;

    for (i = 0; i < db->count; i++) {
        if (db->students[i].hasMarks && db->students[i].marks[subject] > highest) {
            highest = db->students[i].marks[subject];
        }
    }
    return highest;
}

static void printSubjectConfig(const SubjectConfig *config)
{
    int i;

    printf("%-4s %-*s %9s\n", "NO.", SUBJECT_NAME_LEN - 1, "SUBJECT", "MAX MARKS");
    printLine(stdout, '-');
    for (i = 0; i < MAX_SUBJECTS; i++) {
        printf("%-4d %-*s %9d\n", i + 1, SUBJECT_NAME_LEN - 1, config->names[i],
               config->maxMarks[i]);
    }
    printLine(stdout, '-');
}

static void editSubjects(Database *db)
{
    int subject;
    int maxMarks;
    int lowestAllowed;
    char prompt[LINE_LEN];
    char name[SUBJECT_NAME_LEN];
    SubjectConfig original;
    InputStatus status;

    printHeader("ADD OR EDIT SUBJECT NAMES");
    printf("These subjects apply to every student.\n\n");
    printSubjectConfig(&db->subjects);
    printCancelHint();

    snprintf(prompt, sizeof(prompt), "Subject number to edit (1-%d): ", MAX_SUBJECTS);
    status = promptInt(prompt, 1, MAX_SUBJECTS, &subject);
    if (status != INPUT_OK) {
        reportAborted(status);
        return;
    }
    subject--;

    status = promptText("New subject name: ", name, sizeof(name), isPrintableText,
                        "use printable characters only.");
    if (status != INPUT_OK) {
        reportAborted(status);
        return;
    }

    lowestAllowed = highestMarkForSubject(db, subject);
    if (lowestAllowed < 1) {
        lowestAllowed = 1;
    }
    printf("Existing marks for this subject go up to %d, so the maximum cannot be lower.\n",
           highestMarkForSubject(db, subject));

    snprintf(prompt, sizeof(prompt), "Maximum marks (%d-%d): ", lowestAllowed, MAX_SUBJECT_MARKS);
    status = promptInt(prompt, lowestAllowed, MAX_SUBJECT_MARKS, &maxMarks);
    if (status != INPUT_OK) {
        reportAborted(status);
        return;
    }

    original = db->subjects;
    snprintf(db->subjects.names[subject], SUBJECT_NAME_LEN, "%s", name);
    db->subjects.maxMarks[subject] = maxMarks;

    if (!saveSubjects(&db->subjects)) {
        db->subjects = original;
        printf("Error: the subject settings could not be saved. Nothing was changed.\n");
        return;
    }

    printf("\nSuccess: subject settings saved. Results are recalculated automatically.\n");
}

static void viewAcademicSummary(const Database *db)
{
    int index;

    printHeader("ACADEMIC SUMMARY");

    if (!requireStudents(db)) {
        return;
    }
    printCancelHint();
    if (!selectStudentById(db, &index)) {
        return;
    }

    printf("\nStudent: %s (ID %d)\n", db->students[index].name, db->students[index].id);
    printAcademicDetails(stdout, &db->students[index], &db->subjects);
}

void manageAcademicMenu(Database *db)
{
    int choice;

    for (;;) {
        printHeader("MANAGE ACADEMIC RECORDS");
        printf(" 1. Enter Academic Records\n");
        printf(" 2. Update Subject Marks\n");
        printf(" 3. View Subject Marks\n");
        printf(" 4. Add or Edit Subject Names\n");
        printf(" 5. View Academic Summary\n");
        printf(" 6. Return to Main Menu\n");
        printLine(stdout, '=');

        if (promptMenuChoice(1, 6, &choice) != INPUT_OK || choice == 6) {
            return;
        }

        switch (choice) {
        case 1:
            enterAcademicRecords(db);
            break;
        case 2:
            updateSubjectMarks(db);
            break;
        case 3:
            viewSubjectMarks(db);
            break;
        case 4:
            editSubjects(db);
            break;
        case 5:
            viewAcademicSummary(db);
            break;
        }
    }
}

static void viewOneResult(const Database *db)
{
    int index;

    if (!requireStudents(db)) {
        return;
    }
    printCancelHint();
    if (!selectStudentById(db, &index)) {
        return;
    }

    printHeader("RESULT SUMMARY");
    printf("Student ID : %d\n", db->students[index].id);
    printf("Name       : %s\n", db->students[index].name);
    printf("Course     : %s\n", db->students[index].course);
    printLine(stdout, '-');
    printAcademicDetails(stdout, &db->students[index], &db->subjects);
}

static void viewAllResults(const Database *db)
{
    int i;
    int listed = 0;
    int without = 0;
    Result result;

    printHeader("ALL STUDENT RESULTS");

    if (!requireStudents(db)) {
        return;
    }

    printf("%-6s %-24s %-11s %-8s %-6s %-5s\n", "ID", "NAME", "TOTAL", "PERCENT", "RESULT",
           "GRADE");
    printLine(stdout, '-');

    for (i = 0; i < db->count; i++) {
        result = calculateResult(&db->students[i], &db->subjects);
        if (!result.available) {
            without++;
            continue;
        }
        printf("%-6d %-24.24s %4d/%-6d %7.2f%% %-6s %-5s\n", db->students[i].id,
               db->students[i].name, result.total, result.maxTotal, result.percentage,
               resultLabel(&result), result.grade);
        listed++;
    }
    printLine(stdout, '-');

    if (listed == 0) {
        printf("No student has academic records yet.\n");
    }
    printf("Students with results: %d\n", listed);
    printf("Students without academic records (not listed): %d\n", without);
}

void viewStudentResults(const Database *db)
{
    int choice;

    for (;;) {
        printHeader("VIEW STUDENT RESULTS");
        printf(" 1. View One Student's Result\n");
        printf(" 2. View All Results\n");
        printf(" 3. Return to Main Menu\n");
        printLine(stdout, '=');

        if (promptMenuChoice(1, 3, &choice) != INPUT_OK || choice == 3) {
            return;
        }

        if (choice == 1) {
            viewOneResult(db);
        } else {
            viewAllResults(db);
        }
    }
}