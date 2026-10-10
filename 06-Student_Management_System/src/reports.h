#ifndef REPORTS_H
#define REPORTS_H

#include <stdio.h>
#include "types.h"

void printStudentReport(FILE *out, const Student *student, const SubjectConfig *config);
void reportsMenu(const Database *db);

#endif