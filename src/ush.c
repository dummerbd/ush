/*
 * ush.c - micro shell with piping!
 * CS4520 - lab3
 * Benjamin Dummer
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_CMDS 64
#define MAX_ARGS 256
#define PIPE_CHAR '|'

void unix_error(char *msg);
void app_error(char *msg);
void eval_cmd(char *cmdline);
int is_valid_cmd(char *cmdname);

const char *WHICH_CMD = "which 2>/dev/null ";

int main()
{
    char cmdline[MAX_LINE];
    
    while (1)
    {
        printf("ush> ");
        fflush(stdout);

        if ((fgets(cmdline, MAX_LINE, stdin) == NULL) && ferror(stdin))
            app_error("fgets error");

        if (feof(stdin))
        {
            // End of file (ctrl-d)
            fflush(stdout);
            exit(0);
        }

        // Evaluate cmd line
        eval_cmd(cmdline);
        fflush(stdout);
    }
    return 0;
}

void eval_cmd(char *cmdline)
{
    int i = 0;
    char *tok = strtok(cmdline, " ");
    while (tok != NULL)
    {
        if (i == 0 && !is_valid_cmd(tok))
            printf("Command good!\n");
        else if (i > 0)
            printf("\t%s\n", tok);

        tok = strtok(NULL, " ");
        i++;
    }
}

/*
 * Check that the cmdname is in PATH and that the current user has
 * priveleges to execute it.
 * Returns -1 on error, 0 on success.
 */
int is_valid_cmd(char *cmdname)
{
    char which_cmd[256];
    char buf[256];
    FILE *p;

    strcpy(which_cmd, WHICH_CMD);
    strncpy(which_cmd + strlen(WHICH_CMD), cmdname, sizeof(which_cmd) - strlen(WHICH_CMD));
    
    p = popen(which_cmd, "r");
    if (fgets(buf, sizeof(buf), p) == NULL)
    {
        printf("Error: command not found\n");
        return -1;
    }
    pclose(p);

    return 0;
}

void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}
