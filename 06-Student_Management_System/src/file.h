#ifndef FILE_H
#define FILE_H

#include "types.h"

typedef struct {
    const char *studentDir;
    const char *indexPath;
    const char *subjectPath;
    int keepUnreadableIds;
} StorageLocation;

typedef enum {
    LOAD_OK,
    LOAD_NEW,
    LOAD_FATAL
} LoadStatus;

extern const StorageLocation DATA_LOCATION;
extern const StorageLocation BACKUP_LOCATION;

int ensureStorageReady(void);

LoadStatus loadFromLocation(const StorageLocation *location, Database *db, int *skipped);
int saveToLocation(const StorageLocation *location, const Database *db);

int saveStudentFile(const Student *student);
int saveIndex(const Database *db);
int saveSubjects(const SubjectConfig *config);
int removeStudentFile(int id);
int studentFileExists(int id);

void dataMenu(Database *db);

#endif