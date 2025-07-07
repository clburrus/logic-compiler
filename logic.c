#include <stdio.h>  // Required for file operations (fopen, fgets, fclose) and standard I/O (printf, fprintf)
#include <stdlib.h> // Required for exit()
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#define _CRT_SECURE_NO_WARNINGS 1
#define SIZE_LINE_BUFFER 256
#define SIZE_DICT_STRING 32


#define COMMENT_TOKENIZER_CHAR '#'
#define COMMENT_TOKENIZER_STRING  { "#" }
#define ASSIGNMENT_TOKENIZER_STRING "=~"
#define WHITESPACE_TOKENIZER_STRING " \t"


#if 0
config databits = 8
config addressbits = 16

#                              1 1   1 1 1 1
#    0 1 2 3    4 5 6 7    8 9 0 1   2 3 4 5
d0 = 1 x x x    x x x x    x x x x   x x x x    # buffer d0 = a0
d1 = x 0 x x    x x x x    x x x x   x x x x	# inverter d1 = !a1
d2 = x x 1 x    x x x x    x x x x   x x x x    #
d2 = x x x 1    x x x x    x x x x   x x x x    # d2 = a2 | a3
d3 = x x x x    1 1 x x    x x x x   x x x x    # d3 = a4 & a5
d4 = x x x x    x x 1 0    x x x x   x x x x    #
d4 = x x x x    x x 0 1    x x x x   x x x x    # d4 = a6 ^ a7(XOR)
d5 = x x x x    x x x x    0 0 1 x   x x x x    #
d6 = x x x x    x x x x    0 1 0 x   x x x x    # LUT
d7 = x x x x    x x x x    0 1 1 x   x x x x    #

# Other examples
d0 ~1 1 1 1    1 1 1 1    1 1 1 1   1 1 1 0    # d0 = ~(0x8000)
d1 = 1 0 x x    x x x x    x x x x   x x x x    # d1 = a0 & ~a1(a1 gates a0)

#  ...in this case...
#  d1 | a0  a1  a2  a3
#  ------------------ -
#  1 | 1   0   0   0
#  0 | 0   1   0   0
#  0 | 1   1   0   0
#  0 | 0   0   1   0
#  1 | 1   0   1   0
#  0 | 0   1   1   0
#  0 | 1   1   1   0
#  0 | 0   0   0   1
#  1 | 1   0   0   1

#  address[18..0]
#  data[7..0]

assign / rd 	a0
assign / wr 	a1
assign 	A18 	a18
assign 	A17 	a17
assign 	A16 	a16
assign 	A15 	a15
assign / wait 	d5
assign  RomSel	d1


d1 = A18
#endif













struct sConfig
    {
    int numDataBits;
    int numAddrBits;
    int Verbosity;
    };


struct sConfig Config;


struct sDictionaryItem
    {
    unsigned char String[SIZE_DICT_STRING];
    int AddrBit;
    };



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
        fprintf(stderr, "Usage: logic <filename>\n", argv[0]);
        // fprintf(stderr, ...) prints to the standard error stream, which is good for error messages.
        // argv[0] is the name of the executable itself.
        return EXIT_FAILURE; // Indicate an error occurred
        }

    while(ArgNum < argc)
        {
        if(strcmp(argv[ArgNum], "-v") == 0)
            {
            // Verbosity
            Config.Verbosity = 1;
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
int handleKeyword(char *buffer)
    {
    char *pWord;

    pWord = strtok(buffer, WHITESPACE_TOKENIZER_STRING);

    if(pWord)
        {
        if(strcmp(pWord, "alias") == 0)
            {
            // alias <alias name> <address id>
            }
        else if(strcmp(pWord, "databits") == 0)
            {

            }
        else if(strcmp(pWord, "addrbits") == 0)
            {

            }


        pWord = strtok(NULL, WHITESPACE_TOKENIZER_STRING);
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
    filePointer = fopen(argv[argc - 1], "r");

    // Check if the file was opened successfully
    if(filePointer == NULL)
        {
        fprintf(stderr, "Error opening '%s'", argv); // perror prints a system error message based on errno
        return EXIT_FAILURE;         // Indicate an error occurred
        }

    dbprintf("--- Contents of '%s' ---\n", argv[1]);

    // Read lines from the file until the end of the file (EOF) is reached or an error occurs
    char *Success = (char *)1;
    while(Success)
        {
        char *pToken;
        char *pLeft, *pRight;
        Success = fgets(buffer, sizeof(buffer), filePointer);
        
        if(Success != NULL)
            {
            int lineLength = strlen(buffer);

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