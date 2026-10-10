#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <stdio.h>
#include "types.h"

typedef enum {
    SORT_BY_ID,
    SORT_BY_NAME,
    SORT_PERCENT_DESC,
    SORT_PERCENT_ASC
} SortMode;

typedef struct {
    int totalStudents;
    int withRecords;
    int withoutRecords;
    int passed;
    int failed;
    double averagePercentage;
    double highestPercentage;
    double lowestPercentage;
    int bands[BAND_COUNT];
    double subjectAverage[MAX_SUBJECTS];
    int subjectHighest[MAX_SUBJECTS];
    int subjectLowest[MAX_SUBJECTS];
    int subjectPassed[MAX_SUBJECTS];
    int subjectFailed[MAX_SUBJECTS];
    int bestSubject;
    int weakestSubject;
} ClassStats;

ClassStats calculateClassStats(const Database *db);
void sortIndices(const Database *db, int indices[], SortMode mode);

void printClassOverview(FILE *out, const Database *db);
void printSubjectStatistics(FILE *out, const Database *db);

void analyticsMenu(const Database *db);
void sortStudentsMenu(const Database *db);

#endif