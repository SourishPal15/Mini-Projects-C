#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>
#include "types.h"

int isValidName(const char *text);
int isValidEmail(const char *text);

int findStudentIndex(const Database *db, int id);
int requireStudents(const Database *db);
int selectStudentById(const Database *db, int *index);

void printStudentDetails(const Student *student);
void printStudentTable(FILE *out, const Database *db, const int indices[], int count);

void addStudent(Database *db);
void viewAllStudents(const Database *db);
void searchStudentMenu(const Database *db);
void updateStudent(Database *db);
void deleteStudent(Database *db);

#endif