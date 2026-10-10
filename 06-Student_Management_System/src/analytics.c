#include <stdio.h>
#include <string.h>
#include "analytics.h"
#include "academic.h"
#include "student.h"
#include "input.h"

typedef struct {
    int total;
    int withRecords;
    int passed;
    int failed;
    double percentSum;
} GroupTotals;

static const char *BAND_LABELS[BAND_COUNT] = {
    "90% and above",
    "80% to below 90%",
    "70% to below 80%",
    "60% to below 70%",
    "Below 60%"
};

static int bandIndex(double percentage)
{
    if (percentage >= 90.0) {
        return 0;
    }
    if (percentage >= 80.0) {
        return 1;
    }
    if (percentage >= 70.0) {
        return 2;
    }
    if (percentage >= 60.0) {
        return 3;
    }
    return 4;
}

ClassStats calculateClassStats(const Database *db)
{
    ClassStats stats;
    double percentSum = 0.0;
    double subjectSum[MAX_SUBJECTS];
    double bestRatio;
    double worstRatio;
    double ratio;
    int i;
    int s;
    int first;
    int mark;
    Result result;
    const Student *student;

    memset(&stats, 0, sizeof(stats));
    memset(subjectSum, 0, sizeof(subjectSum));
    stats.totalStudents = db->count;

    for (i = 0; i < db->count; i++) {
        student = &db->students[i];
        if (!student->hasMarks) {
            stats.withoutRecords++;
            continue;
        }

        result = calculateResult(student, &db->subjects);
        first = (stats.withRecords == 0);
        stats.withRecords++;
        percentSum += result.percentage;

        if (result.passed) {
            stats.passed++;
        } else {
            stats.failed++;
        }

        if (first || result.percentage > stats.highestPercentage) {
            stats.highestPercentage = result.percentage;
        }
        if (first || result.percentage < stats.lowestPercentage) {
            stats.lowestPercentage = result.percentage;
        }
        stats.bands[bandIndex(result.percentage)]++;

        for (s = 0; s < MAX_SUBJECTS; s++) {
            mark = student->marks[s];
            subjectSum[s] += mark;
            if (first || mark > stats.subjectHighest[s]) {
                stats.subjectHighest[s] = mark;
            }
            if (first || mark < stats.subjectLowest[s]) {
                stats.subjectLowest[s] = mark;
            }
            if (isSubjectPassed(mark, db->subjects.maxMarks[s])) {
                stats.subjectPassed[s]++;
            } else {
                stats.subjectFailed[s]++;
            }
        }
    }

    if (stats.withRecords == 0) {
        return stats;
    }

    stats.averagePercentage = percentSum / stats.withRecords;
    for (s = 0; s < MAX_SUBJECTS; s++) {
        stats.subjectAverage[s] = subjectSum[s] / stats.withRecords;
    }

    bestRatio = stats.subjectAverage[0] / db->subjects.maxMarks[0];
    worstRatio = bestRatio;
    for (s = 1; s < MAX_SUBJECTS; s++) {
        ratio = stats.subjectAverage[s] / db->subjects.maxMarks[s];
        if (ratio > bestRatio) {
            bestRatio = ratio;
            stats.bestSubject = s;
        }
        if (ratio < worstRatio) {
            worstRatio = ratio;
            stats.weakestSubject = s;
        }
    }
    return stats;
}

static int compareRecords(const Database *db, int a, int b, SortMode mode)
{
    const Student *first = &db->students[a];
    const Student *second = &db->students[b];
    Result resultA;
    Result resultB;
    long left;
    long right;
    int cmp = 0;

    if (mode == SORT_BY_NAME) {
        cmp = compareIgnoreCase(first->name, second->name);
    } else if (mode == SORT_PERCENT_DESC || mode == SORT_PERCENT_ASC) {
        resultA = calculateResult(first, &db->subjects);
        resultB = calculateResult(second, &db->subjects);

        if (!resultA.available && !resultB.available) {
            cmp = 0;
        } else if (!resultA.available) {
            cmp = 1;
        } else if (!resultB.available) {
            cmp = -1;
        } else {
            left = (long)resultA.total * resultB.maxTotal;
            right = (long)resultB.total * resultA.maxTotal;
            if (left != right) {
                cmp = (left > right) ? -1 : 1;
                if (mode == SORT_PERCENT_ASC) {
                    cmp = -cmp;
                }
            }
        }
    }

    if (cmp == 0) {
        cmp = first->id - second->id;
    }
    return cmp;
}

void sortIndices(const Database *db, int indices[], SortMode mode)
{
    int i;
    int j;
    int temp;
    int swapped;

    for (i = 0; i < db->count; i++) {
        indices[i] = i;
    }

    for (i = 0; i < db->count - 1; i++) {
        swapped = 0;
        for (j = 0; j < db->count - 1 - i; j++) {
            if (compareRecords(db, indices[j], indices[j + 1], mode) > 0) {
                temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

void printClassOverview(FILE *out, const Database *db)
{
    ClassStats stats = calculateClassStats(db);
    int i;

    fprintf(out, "Total registered students        : %d\n", stats.totalStudents);
    fprintf(out, "Students with academic records   : %d\n", stats.withRecords);
    fprintf(out, "Students without academic records: %d\n", stats.withoutRecords);

    if (stats.withRecords == 0) {
        fprintf(out, "\nNo academic records exist, so averages and distributions are unavailable.\n");
        return;
    }

    fprintf(out, "Students who passed              : %d\n", stats.passed);
    fprintf(out, "Students who failed              : %d\n", stats.failed);
    fprintf(out, "Class average percentage         : %.2f%%\n", stats.averagePercentage);
    fprintf(out, "Highest student percentage       : %.2f%%\n", stats.highestPercentage);
    fprintf(out, "Lowest student percentage        : %.2f%%\n", stats.lowestPercentage);
    fprintf(out, "(Averages include only students who have academic records.)\n");

    fprintf(out, "\nPerformance distribution\n");
    printLine(out, '-');
    for (i = 0; i < BAND_COUNT; i++) {
        fprintf(out, "%-20s : %d\n", BAND_LABELS[i], stats.bands[i]);
    }
}

void printSubjectStatistics(FILE *out, const Database *db)
{
    ClassStats stats = calculateClassStats(db);
    int s;

    if (stats.withRecords == 0) {
        fprintf(out, "No academic records exist, so subject statistics are unavailable.\n");
        return;
    }

    fprintf(out, "%-*s %5s %8s %6s %6s %7s %7s\n", SUBJECT_NAME_LEN - 1, "SUBJECT", "MAX",
            "AVERAGE", "HIGH", "LOW", "PASSED", "FAILED");
    printLine(out, '-');

    for (s = 0; s < MAX_SUBJECTS; s++) {
        fprintf(out, "%-*s %5d %8.2f %6d %6d %7d %7d\n", SUBJECT_NAME_LEN - 1,
                db->subjects.names[s], db->subjects.maxMarks[s], stats.subjectAverage[s],
                stats.subjectHighest[s], stats.subjectLowest[s], stats.subjectPassed[s],
                stats.subjectFailed[s]);
    }
    printLine(out, '-');

    fprintf(out, "Highest-average subject: %s\n", db->subjects.names[stats.bestSubject]);
    fprintf(out, "Lowest-average subject : %s\n", db->subjects.names[stats.weakestSubject]);
    fprintf(out, "(Subjects are ranked by average marks as a share of their maximum marks.)\n");
}

static void showClassOverview(const Database *db)
{
    printHeader("CLASS STATISTICS");
    if (!requireStudents(db)) {
        return;
    }
    printClassOverview(stdout, db);
}

static void showSubjectStatistics(const Database *db)
{
    printHeader("SUBJECT STATISTICS");
    if (!requireStudents(db)) {
        return;
    }
    printSubjectStatistics(stdout, db);
}

static void showTopStudents(const Database *db)
{
    int order[MAX_STUDENTS];
    int top[TOP_COUNT];
    int topCount = 0;
    int i;

    printHeader("TOP STUDENTS BY PERCENTAGE");
    if (!requireStudents(db)) {
        return;
    }

    sortIndices(db, order, SORT_PERCENT_DESC);
    for (i = 0; i < db->count && topCount < TOP_COUNT; i++) {
        if (db->students[order[i]].hasMarks) {
            top[topCount] = order[i];
            topCount++;
        }
    }

    if (topCount == 0) {
        printf("No student has academic records yet, so no ranking is available.\n");
        return;
    }

    printStudentTable(stdout, db, top, topCount);
    printf("Showing %d of at most %d. Equal percentages are listed by ascending ID.\n",
           topCount, TOP_COUNT);
}

static void showStudentsAbove(const Database *db)
{
    int order[MAX_STUDENTS];
    int selected[MAX_STUDENTS];
    int count = 0;
    int threshold;
    int i;
    Result result;
    InputStatus status;

    printHeader("STUDENTS AT OR ABOVE A PERCENTAGE");
    if (!requireStudents(db)) {
        return;
    }

    printCancelHint();
    status = promptInt("Minimum percentage (0-100): ", 0, 100, &threshold);
    if (status != INPUT_OK) {
        reportAborted(status);
        return;
    }

    sortIndices(db, order, SORT_PERCENT_DESC);
    for (i = 0; i < db->count; i++) {
        result = calculateResult(&db->students[order[i]], &db->subjects);
        if (result.available && result.percentage >= (double)threshold) {
            selected[count] = order[i];
            count++;
        }
    }

    if (count == 0) {
        printf("\nNo student has a percentage of %d%% or more.\n", threshold);
        return;
    }
    printf("\n");
    printStudentTable(stdout, db, selected, count);
    printf("Students found: %d\n", count);
}

static void addToGroup(GroupTotals *group, const Database *db, int index)
{
    Result result;

    group->total++;
    if (!db->students[index].hasMarks) {
        return;
    }

    result = calculateResult(&db->students[index], &db->subjects);
    group->withRecords++;
    group->percentSum += result.percentage;
    if (result.passed) {
        group->passed++;
    } else {
        group->failed++;
    }
}

static void printGroupHeader(const char *label)
{
    printf("%-24s %8s %8s %8s %6s %6s\n", label, "STUDENTS", "RESULTS", "AVG %", "PASS",
           "FAIL");
    printLine(stdout, '-');
}

static void printGroupRow(const char *label, const GroupTotals *group)
{
    char average[16];

    if (group->withRecords > 0) {
        snprintf(average, sizeof(average), "%.2f", group->percentSum / group->withRecords);
    } else {
        snprintf(average, sizeof(average), "--");
    }

    printf("%-24.24s %8d %8d %8s %6d %6d\n", label, group->total, group->withRecords,
           average, group->passed, group->failed);
}

static void showGroupedByCourse(const Database *db)
{
    int i;
    int j;
    int seenBefore;
    GroupTotals group;

    printHeader("STUDENTS GROUPED BY COURSE");
    if (!requireStudents(db)) {
        return;
    }

    printGroupHeader("COURSE");
    for (i = 0; i < db->count; i++) {
        seenBefore = 0;
        for (j = 0; j < i; j++) {
            if (equalsIgnoreCase(db->students[j].course, db->students[i].course)) {
                seenBefore = 1;
                break;
            }
        }
        if (seenBefore) {
            continue;
        }

        memset(&group, 0, sizeof(group));
        for (j = 0; j < db->count; j++) {
            if (equalsIgnoreCase(db->students[j].course, db->students[i].course)) {
                addToGroup(&group, db, j);
            }
        }
        printGroupRow(db->students[i].course, &group);
    }
    printLine(stdout, '-');
}

static void showGroupedBySemester(const Database *db)
{
    int semester;
    int i;
    char label[32];
    GroupTotals group;

    printHeader("STUDENTS GROUPED BY SEMESTER");
    if (!requireStudents(db)) {
        return;
    }

    printGroupHeader("SEMESTER");
    for (semester = MIN_SEMESTER; semester <= MAX_SEMESTER; semester++) {
        memset(&group, 0, sizeof(group));
        for (i = 0; i < db->count; i++) {
            if (db->students[i].semester == semester) {
                addToGroup(&group, db, i);
            }
        }
        if (group.total > 0) {
            snprintf(label, sizeof(label), "Semester %d", semester);
            printGroupRow(label, &group);
        }
    }
    printLine(stdout, '-');
}

void analyticsMenu(const Database *db)
{
    int choice;

    for (;;) {
        printHeader("CLASS STATISTICS AND ANALYTICS");
        printf(" 1. Class Overview and Performance Distribution\n");
        printf(" 2. Subject Statistics\n");
        printf(" 3. Top Five Students\n");
        printf(" 4. Students At or Above a Percentage\n");
        printf(" 5. Students Grouped by Course\n");
        printf(" 6. Students Grouped by Semester\n");
        printf(" 7. Return to Main Menu\n");
        printLine(stdout, '=');

        if (promptMenuChoice(1, 7, &choice) != INPUT_OK || choice == 7) {
            return;
        }

        switch (choice) {
        case 1:
            showClassOverview(db);
            break;
        case 2:
            showSubjectStatistics(db);
            break;
        case 3:
            showTopStudents(db);
            break;
        case 4:
            showStudentsAbove(db);
            break;
        case 5:
            showGroupedByCourse(db);
            break;
        case 6:
            showGroupedBySemester(db);
            break;
        }
    }
}

void sortStudentsMenu(const Database *db)
{
    int choice;
    int order[MAX_STUDENTS];
    SortMode mode = SORT_BY_ID;

    for (;;) {
        printHeader("SORT STUDENT RECORDS");
        printf(" 1. Sort by Student ID (ascending)\n");
        printf(" 2. Sort by Name (alphabetical)\n");
        printf(" 3. Sort by Percentage (highest to lowest)\n");
        printf(" 4. Sort by Percentage (lowest to highest)\n");
        printf(" 5. Return to Main Menu\n");
        printLine(stdout, '=');

        if (promptMenuChoice(1, 5, &choice) != INPUT_OK || choice == 5) {
            return;
        }
        if (!requireStudents(db)) {
            continue;
        }

        switch (choice) {
        case 1:
            mode = SORT_BY_ID;
            break;
        case 2:
            mode = SORT_BY_NAME;
            break;
        case 3:
            mode = SORT_PERCENT_DESC;
            break;
        case 4:
            mode = SORT_PERCENT_ASC;
            break;
        }

        sortIndices(db, order, mode);
        printf("\n");
        printStudentTable(stdout, db, order, db->count);
        printf("Students: %d\n", db->count);
        if (mode == SORT_PERCENT_DESC || mode == SORT_PERCENT_ASC) {
            printf("Students without academic records are listed last.\n");
        }
        printf("Equal values are ordered by ascending Student ID.\n");
    }
}