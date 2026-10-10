#ifndef ACADEMIC_H
#define ACADEMIC_H

#include <stdio.h>
#include "types.h"

typedef struct {
    int available;
    int total;
    int maxTotal;
    double percentage;
    int highest;
    int lowest;
    int passedSubjects;
    int failedSubjects;
    int passed;
    char grade[4];
} Result;

void setDefaultSubjects(SubjectConfig *config);
int isSubjectPassed(int marks, int maxMarks);
Result calculateResult(const Student *student, const SubjectConfig *config);
const char *resultLabel(const Result *result);

void printSubjectMarks(FILE *out, const Student *student, const SubjectConfig *config);
void printResultTotals(FILE *out, const Student *student, const SubjectConfig *config);
void printAcademicDetails(FILE *out, const Student *student, const SubjectConfig *config);

void manageAcademicMenu(Database *db);
void viewStudentResults(const Database *db);

#endif