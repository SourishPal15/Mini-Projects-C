#ifndef TYPES_H
#define TYPES_H

#define MAX_STUDENTS 300
#define MAX_SUBJECTS 5
#define NAME_LEN 50
#define COURSE_LEN 40
#define EMAIL_LEN 60
#define SUBJECT_NAME_LEN 30
#define LINE_LEN 256
#define PATH_LEN 200
#define CONSOLE_WIDTH 72

#define MIN_STUDENT_ID 1
#define MAX_STUDENT_ID 999999
#define MIN_AGE 15
#define MAX_AGE 100
#define MIN_SEMESTER 1
#define MAX_SEMESTER 10

#define DEFAULT_MAX_MARKS 100
#define MAX_SUBJECT_MARKS 1000
#define SUBJECT_PASS_PERCENT 40

#define BAND_COUNT 5
#define TOP_COUNT 5

#define DATA_DIR "data"
#define BACKUP_DIR "backups"
#define REPORT_DIR "reports"

typedef struct {
    int id;
    char name[NAME_LEN];
    int age;
    char course[COURSE_LEN];
    int semester;
    char email[EMAIL_LEN];
    int hasMarks;
    int marks[MAX_SUBJECTS];
} Student;

typedef struct {
    char names[MAX_SUBJECTS][SUBJECT_NAME_LEN];
    int maxMarks[MAX_SUBJECTS];
} SubjectConfig;

typedef struct {
    Student students[MAX_STUDENTS];
    int count;
    int skippedIds[MAX_STUDENTS];
    int skippedCount;
    SubjectConfig subjects;
} Database;

#endif