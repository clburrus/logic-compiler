#include <stdio.h>  // Required for file operations (fopen, fgets, fclose) and standard I/O (printf, fprintf)
#include <stdlib.h> // Required for exit()
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#define _CRT_SECURE_NO_WARNINGS 1
#define SIZE_LINE_BUFFER 256
#define SIZE_DICT_STRING 32
#define SIZE_FILE_PATH 4096
#define NUM_DICTIONARY_LINES 100

#define COMMENT_TOKENIZER_CHAR '#'
#define COMMENT_TOKENIZER_STRING  { COMMENT_TOKENIZER_CHAR, 0 }
#define ASSIGNMENT_TOKENIZER_STRING "=~"
#define WHITESPACE_TOKENIZER_STRING " \t"



struct sConfig
    {
    int numDataBits;
    int numAddrBits;
    int Verbosity;
    char Filename[SIZE_FILE_PATH];
    };


struct sConfig Config;


struct sDictionaryItem
    {
    char String[SIZE_DICT_STRING];
    int AddrBit;
    };


struct sDictionary
    {
    struct sDictionaryItem Item[NUM_DICTIONARY_LINES];
    int Index;
    } Dict;


void dbprintf(const char *format, ...)
    {
    if(Config.Verbosity)
        {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        }
    }



// *****************************************************************************
// *****************************************************************************
int handleArgs(int argc, char *argv[])
    {
    // arg[0] is always the command name.
    int ArgNum = 1;

    // Check if a filename was provided as a command-line argument
    if(argc < 2)
        {
        fprintf(stderr, "Usage: logic <filename>\n");
        // fprintf(stderr, ...) prints to the standard error stream, which is good for error messages.
        // argv[0] is the name of the executable itself.
        return EXIT_FAILURE; // Indicate an error occurred
        }

    while(ArgNum < argc)
        {
        // Handle any command line options.
        if(argv[ArgNum][0] == '-')
            {
            if(strcmp(argv[ArgNum], "-v") == 0)
                {
                // Verbosity
                Config.Verbosity = 1;
                }
            }
        else
            {
            // Assume this is the file name.
            strncpy(Config.Filename, argv[ArgNum]);
            }

        ArgNum++;
        }

    return 0;
    }


// *****************************************************************************
// *****************************************************************************
static void stringLower(char *str)
    {
    while(*str)
        {
        *str = tolower(*str);
        str++;
        }
    }



// *****************************************************************************
// *****************************************************************************
int addAliasToDictionary(char *pAlias, char *pTarget)
    {
    if(pAlias && pTarget)
        {
        if(Dict.Index < (NUM_DICTIONARY_LINES - 1))
            {
            for(int i = 0; i < NUM_DICTIONARY_LINES; i++)
                {
                if(Dict.Item[i].String[0] == 0 || strcmp((const char *)Dict.Item[i].String, pAlias) == 0)
                    {
                    // Add this alias to the dictionary
                    }
                }
            }
        }
    }


// *****************************************************************************
// *****************************************************************************
int handleKeyword(char *buffer)
    {
    char *pWord;

    pWord = strtok(buffer, WHITESPACE_TOKENIZER_STRING);

    if(pWord)
        {
        if(strcmp(pWord, "alias") == 0)
            {
            char *pAlias, *pTarget;

            // alias <alias name> <address id>
            pAlias = strtok(NULL, WHITESPACE_TOKENIZER_STRING);

            if(pAlias)
                {
                pTarget = strtok(NULL, WHITESPACE_TOKENIZER_STRING);

                if(pTarget)
                    {
                    if(addAliasToDictionary(pAlias, pTarget) != 0)
                        {

                        }

                    }
                else
                    {
                    // No target for the alias.

                    }
                }
            }
        else if(strcmp(pWord, "databits") == 0)
            {
            // databits <num data bits>
            char *pValue = strtok(NULL, WHITESPACE_TOKENIZER_STRING);

            if(pValue)
                {
                Config.numDataBits = atoi(pValue);

                return 1;
                }

            }
        else if(strcmp(pWord, "addrbits") == 0)
            {
            // addrbits <num address bits>
            char *pValue = strtok(NULL, WHITESPACE_TOKENIZER_STRING);

            if(pValue)
                {
                Config.numAddrBits = atoi(pValue);

                return 1;
                }
            }
        }

    return -1;
    }



// *****************************************************************************
// *****************************************************************************
int main(int argc, char *argv[])
    {
    FILE *filePointer; 
    char buffer[SIZE_LINE_BUFFER];
    int lineCounter = 0;

    if(handleArgs(argc, argv) != 0)
        {
        return EXIT_FAILURE;
        }

    // Attempt to open the file in read mode ("r")
    filePointer = fopen(Config.Filename, "r");

    // Check if the file was opened successfully
    if(filePointer == NULL)
        {
        // fprintf(stderr, "Error opening '%s'", argv[]); // perror prints a system error message based on errno
        return EXIT_FAILURE;         // Indicate an error occurred
        }

    dbprintf("--- Contents of '%s' ---\n", Config.Filename);

    // Read lines from the file until the end of the file (EOF) is reached or an error occurs
    char *Success = (char *)1;
    while(Success)
        {
        char *pToken;
        char *pLeft, *pRight;
        Success = fgets(buffer, sizeof(buffer), filePointer);
        
        if(Success != NULL)
            {
            int lineLength = (int)strlen(buffer);

            // Use this for indicating errors.
            lineCounter++;
            
            // Convert the entire buffer to lowercase.
            stringLower(buffer);

            dbprintf("%s", buffer); // Print the line read from the file
            
            // As we read each line, tokenize with this priority:
            //  * Comments (#)
            //  * Assignments (=)
            //  * Symbols (a0, d0, etc)
            
            // Zero out the string at the comment character, get rid of
            // any other unwanted characters while we're at it.
            for(int i = 0; i < lineLength; i++)
                {
                if(buffer[i] == COMMENT_TOKENIZER_CHAR || 
                   buffer[i] == '\r' || 
                   buffer[i] == '\n')
                    {
                    buffer[i] = 0;
                    break;
                    }
                }

            // Any comments will be removed now.
            dbprintf("Found '%s'\n", buffer);

            // What we have now is a string without comments. Check for
            // assignment.
            pLeft = strtok(buffer, ASSIGNMENT_TOKENIZER_STRING);

            if(pLeft)
                {
                // There was something to the left, see if there is
                // also something to the right.
                pRight = strtok(NULL, ASSIGNMENT_TOKENIZER_STRING);

                if(pRight)
                    {
                    // This is an assignment
                    dbprintf("Left : '%s'\n", pLeft);
                    dbprintf("Right : '%s'\n", pRight);
                    }
                else
                    {
                    if(handleKeyword(pLeft) == -1)
                        {
                        printf("Line %i", lineCounter);
                        return EXIT_FAILURE;         // Indicate an error occurred
                        }
                    }
                }
            }
        }

    dbprintf("--- End of file ---\n");

    // Close the file
    fclose(filePointer);

    return EXIT_SUCCESS; // Indicate successful execution
    }