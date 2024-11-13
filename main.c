#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64
#define DELIM " \t\r\n\a"

/**
 * Command structure:
 * name: A pointer to a character array representing the command name.
 * args: An array of strings for command-line arguments, terminated by NULL.
 * arg_count: The number of command-line arguments.
 */
typedef struct {
    char *name;
    char **args;
    char arg_count;
} Command;

char *read_line(void);
int parse_line(char *line, Command *cmd);
void free_command(Command *cmd);
int execute(Command *cmd);


int main(void)
{
    int status = 1;
    Command cmd;

    char *line = NULL;
    while (status) {
        printf("x > ");
        line = read_line();
        if(parse_line(line, &cmd)) {
            status = execute(&cmd);
            free_command(&cmd);
        }
    }

    free(line);
    return 0;
}


/**
 * Reads a line of input from standard input (stdin).
 *
 * This function dynamically allocates memory to store the input line and
 * reads it from stdin using getline(). If an error occurs or end-of-file
 * (EOF) is reached, the function handles it accordingly.
 *
 * @return A pointer to the dynamically allocated string containing the
 *         input line. The caller is responsible for freeing this memory.
 *
 * @note If end-of-file (EOF) is encountered, the function exits the program
 *       with a status of 0. If any other error occurs during reading, the
 *       function prints an error message and exits the program with a
 *       status of 1.
 */
char * read_line(void) {
    char *line = NULL;
    size_t bufsize = 0;

    if( getline(&line, &bufsize, stdin) == -1 ) {
        if(feof(stdin)) {
            exit(0);
        }
        perror("read_line");
        exit(1);
    }
    return line;
}

/**
 * Parses a line of input into a Command structure.
 *
 * This function takes an input line and a pointer to a Command structure,
 * then splits the line into tokens based on delimiters defined in DELIM.
 * Each token is stored in the args array of the Command structure. The first
 * token is considered as the command name.
 *
 * @param line A pointer to the character array containing the input line.
 * @param cmd A pointer to the Command structure where parsed tokens and
 *            command details will be stored.
 * @return An integer value indicating success or failure. It returns 1 on
 *         success and 0 if no tokens were parsed (empty input line).
 *
 * @note The function dynamically allocates memory for the tokens array.
 *       It is the caller's responsibility to free this memory using the
 *       free_command function.
 */
int parse_line(char *line, Command *cmd) {
    int bufsize = MAX_ARGS;
    int pos = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if(!token) {
        fprintf(stderr, "Memory Allocation Error!\n");
        exit(1);
    }

    token = strtok(line, DELIM);

    while(token != NULL) {
        tokens[pos++] = token;
        if(pos >= bufsize) {
            bufsize += MAX_ARGS;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if(!tokens) {
                fprintf(stderr, "Memory Allocation Error!\n");
                exit(1);
            }
        }
        token = strtok(NULL, DELIM);
    }
    tokens[pos] = NULL;
    if(pos == 0) {
        free(tokens);
        return 0;
    }

    cmd->args = tokens;
    cmd->arg_count = pos;
    cmd->name = tokens[0];

    return 1;
}

/**
 * @brief Executes the specified shell command.
 *
 * This function takes a command structure, parses it, and executes the command
 * in the current shell environment. The Command structure is expected to contain
 * all necessary information for the execution, such as the command name, arguments,
 * and any relevant options.
 *
 * @param cmd A pointer to a Command structure containing the details of the command to execute.
 * @return int Returns 0 on successful execution, or a non-zero error code on failure.
 *
 * @example
 *      Command myCommand;
 *      myCommand.name = "ls";
 *      char *args[] = {"-l", "/home/user", NULL};
 *      myCommand.args = args;
 *      myCommand.numArgs = 2;
 *      int status = execute(&myCommand);
 */
int execute(Command *cmd) {

    pid_t pid;

    if(strcmp(cmd->name, "exit") == 0) { return 0; }

    pid = fork();

    if(pid == -1) {
        fprintf(stderr, "Fork Failed!\n");
        exit(1);
    }

    if(pid == 0) { execvp(cmd->name, cmd->args); }

    int status;
    do {
        (void) waitpid(pid, &status, WUNTRACED);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));

    return 1;

}


void free_command(Command *cmd) { free(cmd->args); }


