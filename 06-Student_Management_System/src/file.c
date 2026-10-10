#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "academic.h"
#include "input.h"
#include "student.h"

#define STUDENT_HEADER "STUDENT_RECORD_V1"
#define INDEX_HEADER "STUDENT_INDEX_V1"
#define SUBJECT_HEADER "SUBJECT_CONFIG_V1"

typedef enum {
    FILE_READ_OK,
    FILE_READ_MISSING,
    FILE_READ_INVALID
} FileState;

typedef enum {
    DATA_LINE_OK,
    DATA_LINE_END,
    DATA_LINE_BAD
} DataLineStatus;

enum {
    FIELD_ID = 1,
    FIELD_NAME = 2,
    FIELD_AGE = 4,
    FIELD_COURSE = 8,
    FIELD_SEMESTER = 16,
    FIELD_EMAIL = 32,
    FIELD_HAS_MARKS = 64,
    FIELD_MARKS = 128
};

const StorageLocation DATA_LOCATION = {
    DATA_DIR "/students",
    DATA_DIR "/index.txt",
    DATA_DIR "/subjects.txt",
    1
};

const StorageLocation BACKUP_LOCATION = {
    BACKUP_DIR "/students",
    BACKUP_DIR "/index_backup.txt",
    BACKUP_DIR "/subjects_backup.txt",
    0
};

static int buildStudentPath(char *path, size_t size, const char *directory, int id)
{
    int written = snprintf(path, size, "%s/student_%d.txt", directory, id);

    return written > 0 && (size_t)written < size;
}

static int fileIsReadable(const char *path)
{
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        return 0;
    }
    fclose(file);
    return 1;
}

static int probeDirectory(const char *directory)
{
    char path[PATH_LEN];
    FILE *file;

    snprintf(path, sizeof(path), "%s/.write_test", directory);
    file = fopen(path, "w");
    if (file == NULL) {
        return 0;
    }
    fclose(file);
    remove(path);
    return 1;
}

int ensureStorageReady(void)
{
    int ready = 1;

    if (!probeDirectory(DATA_DIR) || !probeDirectory(DATA_LOCATION.studentDir)) {
        printf("Error: the folders '%s' and '%s' must exist and be writable.\n", DATA_DIR,
               DATA_LOCATION.studentDir);
        printf("Run the program from the project's main folder, or create those folders.\n");
        ready = 0;
    }
    if (ready && !probeDirectory(BACKUP_LOCATION.studentDir)) {
        printf("Warning: the folder '%s' is missing or not writable. Backups will fail.\n",
               BACKUP_LOCATION.studentDir);
    }
    if (ready && !probeDirectory(REPORT_DIR)) {
        printf("Warning: the folder '%s' is missing or not writable. Reports cannot be saved.\n",
               REPORT_DIR);
    }
    return ready;
}

static int replaceFile(const char *tempPath, const char *finalPath)
{
    if (rename(tempPath, finalPath) == 0) {
        return 1;
    }
    if (remove(finalPath) == 0 && rename(tempPath, finalPath) == 0) {
        return 1;
    }
    return 0;
}

static FILE *openTempFile(const char *finalPath, char *tempPath, size_t size)
{
    int written = snprintf(tempPath, size, "%s.tmp", finalPath);

    if (written <= 0 || (size_t)written >= size) {
        return NULL;
    }
    return fopen(tempPath, "w");
}

static int finishTempFile(FILE *file, const char *tempPath, const char *finalPath)
{
    int failed = ferror(file);

    if (fclose(file) != 0) {
        failed = 1;
    }
    if (failed) {
        remove(tempPath);
        return 0;
    }
    return replaceFile(tempPath, finalPath);
}

static DataLineStatus readDataLine(FILE *file, char *buffer, size_t size)
{
    size_t length;
    int ch;

    while (fgets(buffer, (int)size, file) != NULL) {
        length = strlen(buffer);
        if (length > 0 && buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
        } else if (!feof(file)) {
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {
            }
            return DATA_LINE_BAD;
        }

        trimWhitespace(buffer);
        if (buffer[0] != '\0') {
            return DATA_LINE_OK;
        }
    }
    return ferror(file) ? DATA_LINE_BAD : DATA_LINE_END;
}

static int splitKeyValue(char *line, char **key, char **value)
{
    char *equals = strchr(line, '=');

    if (equals == NULL) {
        return 0;
    }
    *equals = '\0';
    *key = line;
    *value = equals + 1;
    trimWhitespace(*key);
    trimWhitespace(*value);
    return 1;
}

static int writeStudentFile(const char *directory, const Student *student)
{
    char path[PATH_LEN];
    char tempPath[PATH_LEN + 8];
    FILE *file;
    int i;

    if (!buildStudentPath(path, sizeof(path), directory, student->id)) {
        return 0;
    }
    file = openTempFile(path, tempPath, sizeof(tempPath));
    if (file == NULL) {
        return 0;
    }

    fprintf(file, "%s\n", STUDENT_HEADER);
    fprintf(file, "id=%d\n", student->id);
    fprintf(file, "name=%s\n", student->name);
    fprintf(file, "age=%d\n", student->age);
    fprintf(file, "course=%s\n", student->course);
    fprintf(file, "semester=%d\n", student->semester);
    fprintf(file, "email=%s\n", student->email);
    fprintf(file, "has_marks=%d\n", student->hasMarks);

    if (student->hasMarks) {
        fprintf(file, "marks=");
        for (i = 0; i < MAX_SUBJECTS; i++) {
            if (i > 0) {
                fprintf(file, ",");
            }
            fprintf(file, "%d", student->marks[i]);
        }
        fprintf(file, "\n");
    }

    return finishTempFile(file, tempPath, path);
}

static int writeIndexFile(const char *path, const Database *db, int includeUnreadable)
{
    char tempPath[PATH_LEN + 8];
    FILE *file;
    int i;

    file = openTempFile(path, tempPath, sizeof(tempPath));
    if (file == NULL) {
        return 0;
    }

    fprintf(file, "%s\n", INDEX_HEADER);
    for (i = 0; i < db->count; i++) {
        fprintf(file, "%d\n", db->students[i].id);
    }
    if (includeUnreadable) {
        for (i = 0; i < db->skippedCount; i++) {
            fprintf(file, "%d\n", db->skippedIds[i]);
        }
    }
    return finishTempFile(file, tempPath, path);
}

static int writeSubjectFile(const char *path, const SubjectConfig *config)
{
    char tempPath[PATH_LEN + 8];
    FILE *file;
    int i;

    file = openTempFile(path, tempPath, sizeof(tempPath));
    if (file == NULL) {
        return 0;
    }

    fprintf(file, "%s\n", SUBJECT_HEADER);
    for (i = 0; i < MAX_SUBJECTS; i++) {
        fprintf(file, "subject_%d_name=%s\n", i + 1, config->names[i]);
        fprintf(file, "subject_%d_max=%d\n", i + 1, config->maxMarks[i]);
    }
    return finishTempFile(file, tempPath, path);
}

static int fieldBitForKey(const char *key)
{
    if (strcmp(key, "id") == 0) {
        return FIELD_ID;
    }
    if (strcmp(key, "name") == 0) {
        return FIELD_NAME;
    }
    if (strcmp(key, "age") == 0) {
        return FIELD_AGE;
    }
    if (strcmp(key, "course") == 0) {
        return FIELD_COURSE;
    }
    if (strcmp(key, "semester") == 0) {
        return FIELD_SEMESTER;
    }
    if (strcmp(key, "email") == 0) {
        return FIELD_EMAIL;
    }
    if (strcmp(key, "has_marks") == 0) {
        return FIELD_HAS_MARKS;
    }
    if (strcmp(key, "marks") == 0) {
        return FIELD_MARKS;
    }
    return 0;
}

static int parseMarksList(const char *text, const SubjectConfig *config, int marks[])
{
    const char *cursor = text;
    char *end;
    long value;
    int i;

    for (i = 0; i < MAX_SUBJECTS; i++) {
        errno = 0;
        value = strtol(cursor, &end, 10);
        if (end == cursor || errno == ERANGE) {
            return 0;
        }
        if (value < 0 || value > config->maxMarks[i]) {
            return 0;
        }
        marks[i] = (int)value;
        cursor = end;

        if (i < MAX_SUBJECTS - 1) {
            if (*cursor != ',') {
                return 0;
            }
            cursor++;
        }
    }
    return *cursor == '\0';
}

static const char *applyStudentField(Student *student, const SubjectConfig *config,
                                     const char *key, const char *value, int *seen)
{
    int bit = fieldBitForKey(key);
    int number;

    if (bit == 0) {
        return "unknown field";
    }
    if (*seen & bit) {
        return "field appears more than once";
    }

    switch (bit) {
    case FIELD_ID:
        if (!parseInteger(value, MIN_STUDENT_ID, MAX_STUDENT_ID, &number)) {
            return "invalid student ID";
        }
        student->id = number;
        break;
    case FIELD_NAME:
        if (!isValidName(value) || strlen(value) >= NAME_LEN) {
            return "invalid name";
        }
        snprintf(student->name, NAME_LEN, "%s", value);
        break;
    case FIELD_AGE:
        if (!parseInteger(value, MIN_AGE, MAX_AGE, &number)) {
            return "invalid age";
        }
        student->age = number;
        break;
    case FIELD_COURSE:
        if (!isPrintableText(value) || strlen(value) >= COURSE_LEN) {
            return "invalid course";
        }
        snprintf(student->course, COURSE_LEN, "%s", value);
        break;
    case FIELD_SEMESTER:
        if (!parseInteger(value, MIN_SEMESTER, MAX_SEMESTER, &number)) {
            return "invalid semester";
        }
        student->semester = number;
        break;
    case FIELD_EMAIL:
        if (!isValidEmail(value) || strlen(value) >= EMAIL_LEN) {
            return "invalid email";
        }
        snprintf(student->email, EMAIL_LEN, "%s", value);
        break;
    case FIELD_HAS_MARKS:
        if (!parseInteger(value, 0, 1, &number)) {
            return "has_marks must be 0 or 1";
        }
        student->hasMarks = number;
        break;
    case FIELD_MARKS:
        if (!parseMarksList(value, config, student->marks)) {
            return "marks are malformed or outside the allowed range";
        }
        break;
    }

    *seen |= bit;
    return NULL;
}

static FileState readStudentFile(const char *directory, int expectedId,
                                 const SubjectConfig *config, Student *student,
                                 char *problem, size_t problemSize)
{
    char path[PATH_LEN];
    char line[LINE_LEN];
    char *key;
    char *value;
    const char *error;
    FILE *file;
    DataLineStatus status;
    int seen = 0;
    int required = FIELD_ID | FIELD_NAME | FIELD_AGE | FIELD_COURSE | FIELD_SEMESTER |
                   FIELD_EMAIL | FIELD_HAS_MARKS;
    int openError;

    memset(student, 0, sizeof(*student));

    if (!buildStudentPath(path, sizeof(path), directory, expectedId)) {
        snprintf(problem, problemSize, "file path is too long");
        return FILE_READ_INVALID;
    }

    errno = 0;
    file = fopen(path, "r");
    openError = errno;
    if (file == NULL) {
        snprintf(problem, problemSize, "the file %s could not be opened", path);
        return (openError == ENOENT) ? FILE_READ_MISSING : FILE_READ_INVALID;
    }

    if (readDataLine(file, line, sizeof(line)) != DATA_LINE_OK ||
        strcmp(line, STUDENT_HEADER) != 0) {
        fclose(file);
        snprintf(problem, problemSize, "the header line is missing or wrong");
        return FILE_READ_INVALID;
    }

    while ((status = readDataLine(file, line, sizeof(line))) == DATA_LINE_OK) {
        if (!splitKeyValue(line, &key, &value)) {
            fclose(file);
            snprintf(problem, problemSize, "a line is not in key=value form");
            return FILE_READ_INVALID;
        }
        error = applyStudentField(student, config, key, value, &seen);
        if (error != NULL) {
            fclose(file);
            snprintf(problem, problemSize, "%s (field '%.40s')", error, key);
            return FILE_READ_INVALID;
        }
    }
    fclose(file);

    if (status == DATA_LINE_BAD) {
        snprintf(problem, problemSize, "the file has an unreadable or overlong line");
        return FILE_READ_INVALID;
    }
    if ((seen & required) != required) {
        snprintf(problem, problemSize, "required fields are missing");
        return FILE_READ_INVALID;
    }
    if (student->id != expectedId) {
        snprintf(problem, problemSize, "the ID inside the file does not match its name");
        return FILE_READ_INVALID;
    }
    if (student->hasMarks && !(seen & FIELD_MARKS)) {
        snprintf(problem, problemSize, "has_marks=1 but no marks line exists");
        return FILE_READ_INVALID;
    }
    if (!student->hasMarks && (seen & FIELD_MARKS)) {
        snprintf(problem, problemSize, "marks are present although has_marks=0");
        return FILE_READ_INVALID;
    }
    return FILE_READ_OK;
}

static FileState readIndexFile(const char *path, int ids[], int *count)
{
    char line[LINE_LEN];
    FILE *file;
    DataLineStatus status;
    int id;
    int i;
    int openError;

    *count = 0;

    errno = 0;
    file = fopen(path, "r");
    openError = errno;
    if (file == NULL) {
        return (openError == ENOENT) ? FILE_READ_MISSING : FILE_READ_INVALID;
    }

    if (readDataLine(file, line, sizeof(line)) != DATA_LINE_OK ||
        strcmp(line, INDEX_HEADER) != 0) {
        fclose(file);
        return FILE_READ_INVALID;
    }

    while ((status = readDataLine(file, line, sizeof(line))) == DATA_LINE_OK) {
        if (*count >= MAX_STUDENTS || !parseInteger(line, MIN_STUDENT_ID, MAX_STUDENT_ID, &id)) {
            fclose(file);
            return FILE_READ_INVALID;
        }
        for (i = 0; i < *count; i++) {
            if (ids[i] == id) {
                fclose(file);
                return FILE_READ_INVALID;
            }
        }
        ids[*count] = id;
        (*count)++;
    }
    fclose(file);

    return (status == DATA_LINE_END) ? FILE_READ_OK : FILE_READ_INVALID;
}

static int readSubjectEntry(FILE *file, int index, SubjectConfig *config)
{
    char line[LINE_LEN];
    char expected[32];
    char *key;
    char *value;
    int maxMarks;

    snprintf(expected, sizeof(expected), "subject_%d_name", index + 1);
    if (readDataLine(file, line, sizeof(line)) != DATA_LINE_OK ||
        !splitKeyValue(line, &key, &value) || strcmp(key, expected) != 0) {
        return 0;
    }
    if (!isPrintableText(value) || strlen(value) >= SUBJECT_NAME_LEN) {
        return 0;
    }
    snprintf(config->names[index], SUBJECT_NAME_LEN, "%s", value);

    snprintf(expected, sizeof(expected), "subject_%d_max", index + 1);
    if (readDataLine(file, line, sizeof(line)) != DATA_LINE_OK ||
        !splitKeyValue(line, &key, &value) || strcmp(key, expected) != 0) {
        return 0;
    }
    if (!parseInteger(value, 1, MAX_SUBJECT_MARKS, &maxMarks)) {
        return 0;
    }
    config->maxMarks[index] = maxMarks;
    return 1;
}

static FileState readSubjectFile(const char *path, SubjectConfig *config)
{
    SubjectConfig loaded;
    char line[LINE_LEN];
    FILE *file;
    int i;
    int openError;

    errno = 0;
    file = fopen(path, "r");
    openError = errno;
    if (file == NULL) {
        return (openError == ENOENT) ? FILE_READ_MISSING : FILE_READ_INVALID;
    }

    memset(&loaded, 0, sizeof(loaded));
    if (readDataLine(file, line, sizeof(line)) != DATA_LINE_OK ||
        strcmp(line, SUBJECT_HEADER) != 0) {
        fclose(file);
        return FILE_READ_INVALID;
    }

    for (i = 0; i < MAX_SUBJECTS; i++) {
        if (!readSubjectEntry(file, i, &loaded)) {
            fclose(file);
            return FILE_READ_INVALID;
        }
    }

    if (readDataLine(file, line, sizeof(line)) != DATA_LINE_END) {
        fclose(file);
        return FILE_READ_INVALID;
    }
    fclose(file);

    *config = loaded;
    return FILE_READ_OK;
}

LoadStatus loadFromLocation(const StorageLocation *location, Database *db, int *skipped)
{
    int ids[MAX_STUDENTS];
    int idCount;
    int i;
    char problem[LINE_LEN];
    Student student;
    FileState state;

    memset(db, 0, sizeof(*db));
    *skipped = 0;
    setDefaultSubjects(&db->subjects);

    state = readIndexFile(location->indexPath, ids, &idCount);
    if (state == FILE_READ_MISSING) {
        return LOAD_NEW;
    }
    if (state == FILE_READ_INVALID) {
        printf("Error: the index file '%s' is damaged or unreadable.\n", location->indexPath);
        return LOAD_FATAL;
    }

    state = readSubjectFile(location->subjectPath, &db->subjects);
    if (state == FILE_READ_INVALID) {
        printf("Error: the subject file '%s' is damaged or unreadable.\n", location->subjectPath);
        return LOAD_FATAL;
    }
    if (state == FILE_READ_MISSING) {
        printf("Warning: '%s' is missing. Default subject settings are being used.\n",
               location->subjectPath);
    }

    for (i = 0; i < idCount; i++) {
        state = readStudentFile(location->studentDir, ids[i], &db->subjects, &student,
                                problem, sizeof(problem));
        if (state == FILE_READ_OK) {
            db->students[db->count] = student;
            db->count++;
        } else {
            db->skippedIds[db->skippedCount] = ids[i];
            db->skippedCount++;
            (*skipped)++;
            printf("Warning: student %d was skipped: %s.\n", ids[i], problem);
        }
    }
    return LOAD_OK;
}

int saveToLocation(const StorageLocation *location, const Database *db)
{
    int i;

    if (!writeSubjectFile(location->subjectPath, &db->subjects)) {
        return 0;
    }
    for (i = 0; i < db->count; i++) {
        if (!writeStudentFile(location->studentDir, &db->students[i])) {
            return 0;
        }
    }
    return writeIndexFile(location->indexPath, db, location->keepUnreadableIds);
}

int saveStudentFile(const Student *student)
{
    return writeStudentFile(DATA_LOCATION.studentDir, student);
}

int saveIndex(const Database *db)
{
    return writeIndexFile(DATA_LOCATION.indexPath, db, DATA_LOCATION.keepUnreadableIds);
}

int saveSubjects(const SubjectConfig *config)
{
    return writeSubjectFile(DATA_LOCATION.subjectPath, config);
}

int removeStudentFile(int id)
{
    char path[PATH_LEN];

    if (!buildStudentPath(path, sizeof(path), DATA_LOCATION.studentDir, id)) {
        return 0;
    }
    return remove(path) == 0;
}

int studentFileExists(int id)
{
    char path[PATH_LEN];

    if (!buildStudentPath(path, sizeof(path), DATA_LOCATION.studentDir, id)) {
        return 0;
    }
    return fileIsReadable(path);
}

static int backupExists(void)
{
    return fileIsReadable(BACKUP_LOCATION.indexPath) &&
           fileIsReadable(BACKUP_LOCATION.subjectPath);
}

static void removeStaleBackupFiles(const int oldIds[], int oldCount, const Database *db)
{
    char path[PATH_LEN];
    int i;

    for (i = 0; i < oldCount; i++) {
        if (findStudentIndex(db, oldIds[i]) < 0 &&
            buildStudentPath(path, sizeof(path), BACKUP_LOCATION.studentDir, oldIds[i])) {
            remove(path);
        }
    }
}

static void createBackup(const Database *db)
{
    Database check;
    int oldIds[MAX_STUDENTS];
    int oldCount = 0;
    int skipped = 0;

    printHeader("CREATE DATA BACKUP");

    if (backupExists()) {
        printf("A backup already exists in '%s/'.\n", BACKUP_DIR);
        if (!confirmAction("Overwrite the existing backup?")) {
            printf("Backup cancelled. The existing backup was kept.\n");
            return;
        }
        if (readIndexFile(BACKUP_LOCATION.indexPath, oldIds, &oldCount) != FILE_READ_OK) {
            oldCount = 0;
        }
    }

    if (!saveToLocation(&BACKUP_LOCATION, db)) {
        printf("Error: the backup could not be written to '%s/'.\n", BACKUP_DIR);
        printf("The backup may be incomplete. Fix the problem and run the backup again.\n");
        return;
    }

    if (loadFromLocation(&BACKUP_LOCATION, &check, &skipped) != LOAD_OK || skipped != 0 ||
        check.count != db->count) {
        printf("Error: the backup was written but could not be verified.\n");
        return;
    }

    removeStaleBackupFiles(oldIds, oldCount, db);

    printf("Success: backed up %d student(s) and the subject settings.\n", db->count);
    printf("Location: %s/ (index_backup.txt, subjects_backup.txt, students/)\n", BACKUP_DIR);
    printf("Note: a backup on the same computer does not protect against disk loss.\n");
    printf("      Copy the folder somewhere else for an independent copy.\n");
}

static void removeUnlistedDataFiles(const Database *current, const Database *restored)
{
    int i;

    for (i = 0; i < current->count; i++) {
        if (findStudentIndex(restored, current->students[i].id) < 0) {
            if (!removeStudentFile(current->students[i].id)) {
                printf("Warning: old file for student %d could not be removed.\n",
                       current->students[i].id);
            }
        }
    }
}

static void restoreBackup(Database *db)
{
    Database restored;
    int skipped = 0;
    LoadStatus status;

    printHeader("RESTORE FROM BACKUP");

    if (!backupExists()) {
        printf("No complete backup was found in '%s/'. Create a backup first.\n", BACKUP_DIR);
        return;
    }

    status = loadFromLocation(&BACKUP_LOCATION, &restored, &skipped);
    if (status != LOAD_OK) {
        printf("Restore cancelled: the backup could not be read. Current data is unchanged.\n");
        return;
    }
    if (skipped > 0) {
        printf("Restore cancelled: %d backup record(s) are invalid. Current data is unchanged.\n",
               skipped);
        return;
    }

    printf("The backup is valid and holds %d student(s).\n", restored.count);
    printf("Current data holds %d student(s).\n", db->count);
    if (!confirmAction("Replace ALL current data with the backup?")) {
        printf("Restore cancelled. Current data is unchanged.\n");
        return;
    }

    if (!saveToLocation(&DATA_LOCATION, &restored)) {
        printf("Error: writing the restored data failed part-way.\n");
        printf("The data files on disk may now be a mix of old and restored records.\n");
        printf("Fix the problem (for example free disk space) and run the restore again.\n");
        return;
    }

    removeUnlistedDataFiles(db, &restored);
    *db = restored;
    printf("Success: restored %d student(s) from the backup.\n", db->count);
}

static void showStorageStatus(const Database *db)
{
    int ids[MAX_STUDENTS];
    int count = 0;
    int missing = 0;
    int i;

    printHeader("DATA STORAGE STATUS");

    for (i = 0; i < db->count; i++) {
        if (!studentFileExists(db->students[i].id)) {
            missing++;
        }
    }

    printf("Data folder          : %s/\n", DATA_DIR);
    printf("Student record files : %s/student_<ID>.txt (one per student)\n",
           DATA_LOCATION.studentDir);
    printf("Index file           : %s (%s)\n", DATA_LOCATION.indexPath,
           fileIsReadable(DATA_LOCATION.indexPath) ? "present" : "MISSING");
    printf("Subject file         : %s (%s)\n", DATA_LOCATION.subjectPath,
           fileIsReadable(DATA_LOCATION.subjectPath) ? "present" : "MISSING");
    printf("Students in memory   : %d\n", db->count);
    printf("Record files missing : %d\n", missing);
    printLine(stdout, '-');

    printf("Backup folder        : %s/\n", BACKUP_DIR);
    if (!backupExists()) {
        printf("Backup status        : no complete backup exists\n");
    } else if (readIndexFile(BACKUP_LOCATION.indexPath, ids, &count) == FILE_READ_OK) {
        printf("Backup status        : present, lists %d student(s)\n", count);
    } else {
        printf("Backup status        : present but its index is damaged\n");
    }
    printf("Reports folder       : %s/\n", REPORT_DIR);
}

void dataMenu(Database *db)
{
    int choice;

    for (;;) {
        printHeader("MANAGE DATA AND BACKUPS");
        printf(" 1. Create Data Backup\n");
        printf(" 2. Restore from Backup\n");
        printf(" 3. View Data Storage Status\n");
        printf(" 4. Return to Main Menu\n");
        printLine(stdout, '=');

        if (promptMenuChoice(1, 4, &choice) != INPUT_OK || choice == 4) {
            return;
        }

        switch (choice) {
        case 1:
            createBackup(db);
            break;
        case 2:
            restoreBackup(db);
            break;
        case 3:
            showStorageStatus(db);
            break;
        }
    }
}