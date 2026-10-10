# Student Management System in C

## Project Overview

A console application that lets an administrator manage the records of a small college: register students, store marks, calculate results, analyse class performance, generate reports and keep everything on disk between runs. It was written as a first-year Computer Science learning project to practise structures, arrays, functions, string handling, file handling, searching and sorting in one realistic program.

The project is designed to be cross-platform. It uses only standard C library features, so it should build on Windows, Linux and macOS. Actual compatibility depends on using a suitable C11 compiler and a filesystem that supports the standard file operations used here.

## Features

- **Student registration** with validation of ID, name, age, course, semester and email, and duplicate-ID rejection
- **Viewing** all students in an aligned table, with a clear difference between "no academic records" (N/A) and FAIL
- **Searching** by ID, full name, partial name, course or semester (name and course matching ignores case)
- **Updating** one field or all personal details, with cancel support
- **Deletion** with confirmation; the student's record file is removed too
- **Academic management**: enter marks, update one subject, view marks, edit subject names and maximum marks
- **Result calculation**: total, maximum, percentage, highest/lowest mark, subjects passed/failed, overall result and grade
- **Class statistics**: counts, pass/fail, average, highest/lowest, performance bands, per-subject statistics, top five, students at or above a percentage, grouping by course and semester
- **Sorting** by ID, name, or percentage in either direction (display only; stored order is not changed)
- **Reports**: individual student report and class report, shown on screen or saved as text files
- **Permanent storage**: one text file per registered student, plus an index file and a subject file
- **Backup and restore** with validation, overwrite warnings and confirmation
- **Input validation and error handling** throughout (letters in numbers, out-of-range values, overlong lines, end of input, missing or damaged files, failed writes)

## Technologies Used

- **C11** (`-std=c11`), standard C library only
- **Structures** (`Student`, `SubjectConfig`, `Database`, `Result`, `ClassStats`) and **arrays** for records and marks
- **Functions** split across modules; **pointers** for passing records and output parameters; one function pointer for reusable input validation
- **String handling**: `snprintf`, `strlen`, `strchr`, `strcmp`, `strtol`, and small case-insensitive helpers
- **File handling**: `fopen`, `fgets`, `fprintf`, `fclose`, `remove`, `rename`
- **Searching**: linear search. **Sorting**: Bubble Sort on an array of indices

## Project Structure

```
Student-Management-System/
|-- src/
|   |-- main.c        start-up, main menu, shutdown
|   |-- types.h       constants and the shared data structures
|   |-- student.c/.h  add, view, search, update, delete
|   |-- academic.c/.h subjects, marks, result calculation
|   |-- analytics.c/.h class statistics and sorting
|   |-- file.c/.h     load, save, backup, restore, storage status
|   |-- reports.c/.h  individual and class reports
|   |-- input.c/.h    safe input, validation helpers, small display helpers
|-- data/             runtime data (index.txt, subjects.txt, students/student_<ID>.txt)
|-- reports/          saved reports
|-- backups/          backup copy of the data
|-- Makefile
|-- CMakeLists.txt
|-- README.md
|-- TEST_CHECKLIST.md
|-- LICENSE
|-- .gitignore
```

`data/`, `data/students/`, `reports/`, `backups/` and `backups/students/` are kept in Git with empty `.gitkeep` files. The program cannot create folders using standard C alone, so these folders must exist.

## Requirements

- **Required:** a C compiler that supports C11 (GCC, Clang, or MinGW-w64 on Windows)
- **Optional:** `make` or CMake. They are conveniences; you can compile with one compiler command.

## Compilation and Execution

Always run the program **from the project's main folder**, because it uses relative paths such as `data/students/`.

**GCC (Linux, macOS with GCC, Windows with MinGW-w64):**

```
gcc -std=c11 -Wall -Wextra -Wpedantic -o student_manager src/main.c src/student.c src/academic.c src/analytics.c src/file.c src/reports.c src/input.c
```

**Clang (Linux, macOS):**

```
clang -std=c11 -Wall -Wextra -Wpedantic -o student_manager src/main.c src/student.c src/academic.c src/analytics.c src/file.c src/reports.c src/input.c
```

**Make:**

```
make
```

**CMake:**

```
cmake -S . -B build
cmake --build build
```

CMake places the executable inside `build/`, so run it from the project folder, for example `./build/student_manager` (Linux/macOS) or `build\student_manager.exe` (Windows, the exact path depends on the generator).

**Run:**

- Linux / macOS: `./student_manager`
- Windows (PowerShell / Command Prompt): `student_manager.exe`

## How the Application Works

**Main menu.** All 13 features are reachable from the main menu. Every submenu has a "Return" option. Invalid menu input only shows an error and asks again.

**Registration.** Each prompt is validated until it is acceptable. Pressing Enter on an empty line cancels. After the record is saved to its own file and the index, a confirmation is shown. If saving fails, the student is not registered.

**Searching and updating.** Searches use a linear scan through the student array. Updating works on a copy of the record; the real record changes only after every new value is valid, and it is rolled back if saving fails.

**Academic records.** Every student has the same five subject slots (names and maximum marks are editable). Marks are entered for all subjects together, so a record is never half-saved. A student with no marks has `hasMarks = 0` and is shown as N/A, never as zero or failed.

**Result calculation.** Results are never stored; they are calculated from the marks each time, so they cannot become inconsistent. Example rules used by this program (they are not universal standards):

- A subject is passed with at least 40% of its maximum marks
- A student passes only if every subject is passed
- Percentage = total marks / total maximum marks x 100, shown with two decimals
- Grades: A+ at 90% or more, A 80, B 70, C 60, D 50, E below 50; any failed subject gives F

**Class analytics.** Only students with academic records take part in averages, bands and subject statistics. Percentage bands do not overlap: 90 and above, 80 to below 90, 70 to below 80, 60 to below 70, below 60.

**Sorting.** Bubble Sort is applied to an array of record positions, so the stored order never changes. Equal values are ordered by ascending ID. Students without results are listed last in both percentage orders.

**Reports.** Individual reports are saved as `reports/Student_<ID>.txt` and the class report as `reports/Class_Report.txt`. File names are built only from the numeric ID, never from typed text.

## Data Storage

Every registered student gets their own text file when they are registered:

```
data/index.txt                   list of registered student IDs
data/subjects.txt                subject names and maximum marks
data/students/student_1001.txt   one file per student
```

A student file looks like this (fictional data):

```
STUDENT_RECORD_V1
id=1001
name=Asha Verma
age=18
course=BSc Computer Science
semester=1
email=asha@example.com
has_marks=1
marks=85,90,78,66,92
```

- Files are plain `key=value` text, so they are easy to read and debug. Names cannot contain line breaks, and the key is everything before the first `=`.
- Limits: name 49 characters, course 39, email 59, subject name 29.
- Each change is saved immediately. A file is written to a `.tmp` file first, checked, and then renamed over the real file. On Linux and macOS the rename replaces the file in one step. On Windows `rename` cannot replace an existing file, so the program removes the old file and then renames; a crash between those two steps could leave only the `.tmp` file.
- On start-up the index is read, then each listed student file is validated. A missing or damaged student file is skipped with a warning and left untouched, and its ID stays in the index so it loads again once the file is repaired. A damaged index or subject file stops the program from starting, so it can never replace good data with empty data.
- **Backups** copy the same structure into `backups/` (`index_backup.txt`, `subjects_backup.txt`, `students/`). An existing backup is not overwritten without asking. A backup is re-read and checked before success is reported. Restore validates the whole backup first and only then asks for confirmation.
- A backup on the same computer does not replace a copy stored somewhere else.

**Limitations:** copying many files cannot be made fully atomic with standard C, so a restore or backup that fails half-way can leave a mix of files and the program says so. Data files are plain text and are not encrypted.

## Concepts Learned and Practised

Variables and data types, conditional statements, loops, functions, arrays, strings, structures, pointers (parameters, output values, one function pointer), file handling, input validation, linear search, Bubble Sort, modular programming with header files and include guards, and error handling with return codes.


## Limitations

This is an educational file-based program, not a production college database. There is no database server, no support for several people using it at once, no real access control, and no encryption. Names and text are limited to plain ASCII characters, the database holds at most 300 students, and the same five subject slots apply to every course.

## Future Improvements

- Different subject sets per course or semester
- Pagination for long student lists
- Importing and exporting CSV files
- A simple password prompt (which would only be a demonstration, not real security)
- Automated unit tests for the calculation functions

## Conclusion

The project combines structures, arrays, string handling, file handling, searching, sorting and modular design in one working program. It shows how a real task can be split into small, readable modules, and why validating input and checking every file operation matters.