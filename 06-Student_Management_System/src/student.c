#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "student.h"
#include "academic.h"
#include "file.h"
#include "input.h"

typedef enum {
    MATCH_FULL_NAME,
    MATCH_PARTIAL_NAME,
    MATCH_COURSE
} TextSearchKind;

int isValidName(const char *text)
{
    size_t i;
    int letters = 0;
    unsigned char c;

    for (i = 0; text[i] != '\0'; i++) {
        c = (unsigned char)text[i];
        if (isalpha(c)) {
            letters++;
        } else if (c != ' ' && c != '.' && c != '-' && c != '\'') {
            return 0;
        }
    }
    return letters > 0;
}

int isValidEmail(const char *text)
{
    const char *at;
    const char *dot;
    size_t i;

    for (i = 0; text[i] != '\0'; i++) {
        if (!isprint((unsigned char)text[i]) || isspace((unsigned char)text[i])) {
            return 0;
        }
    }

    at = strchr(text, '@');
    if (at == NULL || at == text || strchr(at + 1, '@') != NULL) {
        return 0;
    }

    dot = strrchr(at, '.');
    return dot != NULL && dot > at + 1 && dot[1] != '\0';
}

int findStudentIndex(const Database *db, int id)
{
    int i;

    for (i = 0; i < db->count; i++) {
        if (db->students[i].id == id) {
            return i;
        }
    }
    return -1;
}

int requireStudents(const Database *db)
{
    if (db->count == 0) {
        printf("No student records exist yet. Add a student first.\n");
        return 0;
    }
    return 1;
}

int selectStudentById(const Database *db, int *index)
{
    int id;
    InputStatus status;

    status = promptInt("Student ID: ", MIN_STUDENT_ID, MAX_STUDENT_ID, &id);
    if (status != INPUT_OK) {
        reportAborted(status);
        return 0;
    }

    *index = findStudentIndex(db, id);
    if (*index < 0) {
        printf("Error: no student with ID %d exists.\n", id);
        return 0;
    }
    return 1;
}

void printStudentDetails(const Student *student)
{
    printLine(stdout, '-');
    printf("Student ID       : %d\n", student->id);
    printf("Full Name        : %s\n", student->name);
    printf("Age              : %d\n", student->age);
    printf("Course           : %s\n", student->course);
    printf("Semester         : %d\n", student->semester);
    printf("Email            : %s\n", student->email);
    printf("Academic Records : %s\n", student->hasMarks ? "Entered" : "Not entered");
    printLine(stdout, '-');
}

void printStudentTable(FILE *out, const Database *db, const int indices[], int count)
{
    int i;
    const Student *student;
    Result result;
    char percent[16];

    fprintf(out, "%-6s %-18s %-18s %-3s %-7s %-8s %-6s\n",
            "ID", "NAME", "COURSE", "SEM", "RECORDS", "PERCENT", "RESULT");
    printLine(out, '-');

    for (i = 0; i < count; i++) {
        student = &db->students[indices[i]];
        result = calculateResult(student, &db->subjects);

        if (result.available) {
            snprintf(percent, sizeof(percent), "%.2f%%", result.percentage);
        } else {
            snprintf(percent, sizeof(percent), "--");
        }

        fprintf(out, "%-6d %-18.18s %-18.18s %-3d %-7s %-8s %-6s\n",
                student->id, student->name, student->course, student->semester,
                student->hasMarks ? "Yes" : "No", percent, resultLabel(&result));
    }
    printLine(out, '-');
}

static InputStatus promptNewId(const Database *db, int *id)
{
    InputStatus status;

    for (;;) {
        status = promptInt("Student ID (1-999999): ", MIN_STUDENT_ID, MAX_STUDENT_ID, id);
        if (status != INPUT_OK) {
            return status;
        }
        if (findStudentIndex(db, *id) >= 0) {
            printf("  Error: ID %d is already registered.\n", *id);
            continue;
        }
        if (studentFileExists(*id)) {
            printf("  Error: a data file for ID %d already exists on disk.\n", *id);
            printf("         Choose another ID or remove that file manually.\n");
            continue;
        }
        return INPUT_OK;
    }
}

static InputStatus promptName(char *destination)
{
    return promptText("Full name: ", destination, NAME_LEN, isValidName,
                      "use letters, spaces, '.', '-' or an apostrophe only.");
}

static InputStatus promptAge(int *age)
{
    return promptInt("Age (15-100): ", MIN_AGE, MAX_AGE, age);
}

static InputStatus promptCourse(char *destination)
{
    return promptText("Course: ", destination, COURSE_LEN, isPrintableText,
                      "use printable characters only.");
}

static InputStatus promptSemester(int *semester)
{
    return promptInt("Semester (1-10): ", MIN_SEMESTER, MAX_SEMESTER, semester);
}

static InputStatus promptEmail(char *destination)
{
    return promptText("Email address: ", destination, EMAIL_LEN, isValidEmail,
                      "enter an address like name@example.com (no spaces).");
}

static InputStatus promptPersonalFields(Student *student)
{
    InputStatus status;

    status = promptName(student->name);
    if (status != INPUT_OK) {
        return status;
    }
    status = promptAge(&student->age);
    if (status != INPUT_OK) {
        return status;
    }
    status = promptCourse(student->course);
    if (status != INPUT_OK) {
        return status;
    }
    status = promptSemester(&student->semester);
    if (status != INPUT_OK) {
        return status;
    }
    return promptEmail(student->email);
}

void addStudent(Database *db)
{
    Student student;
    InputStatus status;

    printHeader("ADD NEW STUDENT");

    if (db->count + db->skippedCount >= MAX_STUDENTS) {
        printf("Error: the database is full (%d students maximum).\n", MAX_STUDENTS);
        return;
    }

    printCancelHint();
    memset(&student, 0, sizeof(student));

    status = promptNewId(db, &student.id);
    if (status == INPUT_OK) {
        status = promptPersonalFields(&student);
    }
    if (status != INPUT_OK) {
        reportAborted(status);
        return;
    }

    db->students[db->count] = student;
    db->count++;

    if (!saveStudentFile(&student) || !saveIndex(db)) {
        db->count--;
        removeStudentFile(student.id);
        printf("Error: the record could not be saved to disk, so the student was not registered.\n");
        return;
    }

    printf("\nSuccess: student %d (%s) was registered.\n", student.id, student.name);
    printf("Record file: %s/student_%d.txt\n", DATA_DIR "/students", student.id);
}

void viewAllStudents(const Database *db)
{
    int indices[MAX_STUDENTS];
    int i;
    int withRecords = 0;

    printHeader("STUDENT RECORDS");

    if (db->count == 0) {
        printf("No student records found.\n");
        return;
    }

    for (i = 0; i < db->count; i++) {
        indices[i] = i;
        if (db->students[i].hasMarks) {
            withRecords++;
        }
    }

    printStudentTable(stdout, db, indices, db->count);
    printf("Total Registered Students: %d\n", db->count);
    printf("Students with academic records: %d\n", withRecords);
}

static void showMatches(const Database *db, const int matches[], int count)
{
    if (count == 0) {
        printf("\nNo matching students were found.\n");
        return;
    }
    printf("\n");
    printStudentTable(stdout, db, matches, count);
    printf("Matches found: %d\n", count);
}

static void searchById(const Database *db)
{
    int id;
    int index;
    InputStatus status;

    printCancelHint();
    status = promptInt("Student ID: ", MIN_STUDENT_ID, MAX_STUDENT_ID, &id);
    if (status != INPUT_OK) {
        reportAborted(status);
        return;
    }

    index = findStudentIndex(db, id);
    if (index < 0) {
        printf("\nNo student with ID %d was found.\n", id);
        return;
    }
    showMatches(db, &index, 1);
    printStudentDetails(&db->students[index]);
}

static int collectTextMatches(const Database *db, TextSearchKind kind,
                              const char *term, int matches[])
{
    int i;
    int found = 0;
    int hit;
    const Student *student;

    for (i = 0; i < db->count; i++) {
        student = &db->students[i];
        hit = 0;
        switch (kind) {
        case MATCH_FULL_NAME:
            hit = equalsIgnoreCase(student->name, term);
            break;
        case MATCH_PARTIAL_NAME:
            hit = containsIgnoreCase(student->name, term);
            break;
        case MATCH_COURSE:
            hit = containsIgnoreCase(student->course, term);
            break;
        }
        if (hit) {
            matches[found] = i;
            found++;
        }
    }
    return found;
}

static void searchByText(const Database *db, TextSearchKind kind, const char *prompt)
{
    char term[NAME_LEN];
    int matches[MAX_STUDENTS];
    int count;
    InputStatus status;

    printCancelHint();
    status = promptText(prompt, term, sizeof(term), isPrintableText,
                        "use printable characters only.");
    if (status != INPUT_OK) {
        reportAborted(status);
        return;
    }

    count = collectTextMatches(db, kind, term, matches);
    showMatches(db, matches, count);
}

static void searchBySemester(const Database *db)
{
    int semester;
    int i;
    int count = 0;
    int matches[MAX_STUDENTS];
    InputStatus status;

    printCancelHint();
    status = promptSemester(&semester);
    if (status != INPUT_OK) {
        reportAborted(status);
        return;
    }

    for (i = 0; i < db->count; i++) {
        if (db->students[i].semester == semester) {
            matches[count] = i;
            count++;
        }
    }
    showMatches(db, matches, count);
}

void searchStudentMenu(const Database *db)
{
    int choice;

    for (;;) {
        printHeader("SEARCH STUDENT");
        printf(" 1. Search by Student ID\n");
        printf(" 2. Search by Full Name\n");
        printf(" 3. Search by Partial Name\n");
        printf(" 4. Search by Course\n");
        printf(" 5. Search by Semester\n");
        printf(" 6. Return to Main Menu\n");
        printLine(stdout, '=');

        if (promptMenuChoice(1, 6, &choice) != INPUT_OK || choice == 6) {
            return;
        }
        if (!requireStudents(db)) {
            continue;
        }

        switch (choice) {
        case 1:
            searchById(db);
            break;
        case 2:
            searchByText(db, MATCH_FULL_NAME, "Full name to find: ");
            break;
        case 3:
            searchByText(db, MATCH_PARTIAL_NAME, "Part of the name: ");
            break;
        case 4:
            searchByText(db, MATCH_COURSE, "Course (or part of it): ");
            break;
        case 5:
            searchBySemester(db);
            break;
        }
    }
}

void updateStudent(Database *db)
{
    int index;
    int choice;
    Student original;
    Student edited;
    Student *student;
    InputStatus status = INPUT_OK;

    printHeader("UPDATE STUDENT DETAILS");

    if (!requireStudents(db)) {
        return;
    }
    printCancelHint();
    if (!selectStudentById(db, &index)) {
        return;
    }

    student = &db->students[index];
    printf("\nCurrent details:\n");
    printStudentDetails(student);

    printf(" 1. Update Name\n");
    printf(" 2. Update Age\n");
    printf(" 3. Update Course\n");
    printf(" 4. Update Semester\n");
    printf(" 5. Update Email\n");
    printf(" 6. Update All Personal Details\n");
    printf(" 7. Cancel\n");
    printLine(stdout, '-');

    if (promptMenuChoice(1, 7, &choice) != INPUT_OK || choice == 7) {
        printf("Update cancelled. No changes were made.\n");
        return;
    }

    edited = *student;
    switch (choice) {
    case 1:
        status = promptName(edited.name);
        break;
    case 2:
        status = promptAge(&edited.age);
        break;
    case 3:
        status = promptCourse(edited.course);
        break;
    case 4:
        status = promptSemester(&edited.semester);
        break;
    case 5:
        status = promptEmail(edited.email);
        break;
    case 6:
        status = promptPersonalFields(&edited);
        break;
    }

    if (status != INPUT_OK) {
        reportAborted(status);
        return;
    }

    original = *student;
    *student = edited;

    if (!saveStudentFile(student)) {
        *student = original;
        printf("Error: the changes could not be saved to disk. The record was left unchanged.\n");
        return;
    }

    printf("\nSuccess: student %d was updated and saved.\n", student->id);
    printStudentDetails(student);
}

void deleteStudent(Database *db)
{
    int index;
    int i;
    int id;
    Student removed;

    printHeader("DELETE STUDENT RECORD");

    if (!requireStudents(db)) {
        return;
    }
    printCancelHint();
    if (!selectStudentById(db, &index)) {
        return;
    }

    printStudentDetails(&db->students[index]);
    if (!confirmAction("Permanently delete this student and all academic records?")) {
        printf("Deletion cancelled. No changes were made.\n");
        return;
    }

    removed = db->students[index];
    id = removed.id;

    for (i = index; i < db->count - 1; i++) {
        db->students[i] = db->students[i + 1];
    }
    db->count--;

    if (!saveIndex(db)) {
        for (i = db->count; i > index; i--) {
            db->students[i] = db->students[i - 1];
        }
        db->students[index] = removed;
        db->count++;
        printf("Error: the change could not be saved to disk. The student was not deleted.\n");
        return;
    }

    if (!removeStudentFile(id)) {
        printf("Warning: student %d was removed from the index, but the file\n", id);
        printf("         %s/student_%d.txt could not be deleted. Remove it manually.\n",
               DATA_DIR "/students", id);
    }

    printf("Success: student %d was deleted.\n", id);
}