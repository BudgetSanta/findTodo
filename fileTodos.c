#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_BUFF 1024
#define MAXDIRNAME 200
#define KEYWORD "TODO "

// PROTOTYPES
void printTodos(FILE *fp);
int hasTodos(FILE *fp);

int main(int argc, char *argv[]) {

    /*
        Revision 1
            - Search through file give as command line arg
            - Find single lines with TODOs in them
            - print out on single lines, info
                line No. > TODO the rest of the line comment

        Ideas for future
            - Multi line TODOs
            - this file can be used to search through folders or projects
                    FILE.c
                1 > TODO fix this
                8 > TODO fix that
                9 > TODO etc
            - Possibly also get branch name?
    */

    char input[MAXDIRNAME];                                     // DIRECTORY

    // collect the directory name, with "." as default
    if (argc < 2) {
        strlcpy(input, ".", MAXDIRNAME);
    } else {
        strlcpy(input, argv[1], MAXDIRNAME);
    }

    // check that the name really is a directory
    struct stat info;
    if (stat(input, &info) < 0) {                               // Trys to stat input
        perror(argv[0]); exit(EXIT_FAILURE);
    }
    if ((info.st_mode & S_IFMT) != S_IFDIR) {
        char const* const fileName = argv[1];                   /* should check that argc > 1 */
        FILE *file = fopen(fileName, "r");                      /* should check the result */
        printTodos(file);                                       // PRINT TODOs
        fclose(file);                                           // CLOSE FILE
    } else {

        // DIRECTORY SETUP
        DIR *df;                                                // DIR Type
        df = opendir(input);                                    // DIR is opened
        if (df == NULL) {exit(EXIT_FAILURE);}

        // ENTRY SETUP
        struct dirent *entry;                                   // For each entry
        struct stat details;                                    // File info
        char catPath[MAXDIRNAME];                                // Store file and fir

        // LOOPING THROUGH ENTRIES
        while ( (entry = readdir(df)) != NULL ) {

            if ( !(entry->d_name[0] == '.') ) {                 // Ignore DOT files

                // CONCATENATE ABSOLUTE FILEPATH
                strncpy(catPath, input, MAXDIRNAME);            // File path
                strlcat(catPath, "/", MAXDIRNAME);              // Add /
                strlcat(catPath, entry->d_name, MAXDIRNAME);    // Add on file name

                // STAT FILE
                if (lstat(catPath, &details) == -1) {
                    perror("stat");
                    exit(EXIT_FAILURE);
                }

                FILE *file = fopen(catPath, "r");               // OPEN FILE
                if (hasTodos(file)) {
                    printf("\t%s\n-----------------------\n", entry->d_name);
                    printTodos(file);                               // PRINT TODOS
                    printf("-----------------------\n");
                }
                fclose(file);                                   // CLOSE FILE

            }
        }

        // finish up
        closedir(df);
    }

    return 0;
}

void printTodos(FILE *fp) {

    char iter_buff[MAX_BUFF];                                   // Line buffer
    int  lineCount = 1;                                         // Line count
    char *ret;                                                  // Pointer to TODO in str

    rewind(fp);                                                 // Make sure it reads from 1
    while (fgets(iter_buff, sizeof(iter_buff), fp)) {
        if ( (ret = strstr(iter_buff, KEYWORD)) ) {
            printf("[%d] %s", lineCount, ret);
        }
        lineCount++;
    }
}

// RETURNS 1 IF FINDS KEYWORD IN FILE
int hasTodos(FILE *fp) {

    char iter_buff[MAX_BUFF];                                   // Line buffer

    rewind(fp);                                                 // Make sure it reads from 1
    while (fgets(iter_buff, sizeof(iter_buff), fp)) {
        if ( strstr(iter_buff, KEYWORD) ) {
            return 1;
        }
    }

    return 0;
}
