#include <stdio.h>
#include <stdlib.h>

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


int parse_line(char *line, Command *cmd) {

}

int execute(Command *cmd) {

}



void free_memory(Command *cmd) { free(cmd->args); }




int main(int argc, char **argv)
{
    int status = 1;
    Command cmd;

    char *line = NULL;
    while (status) {
        printf("x > ");
        line = read_line();

    }

    return 0;
}
