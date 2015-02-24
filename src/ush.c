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

#define MAXLINE 1024
#define MAXCMDS 64
#define MAXARGS 256

void unix_error(char *msg);
void app_error(char *msg);
void eval_cmd(char *cmdline);
int expand_cmd_path(char *cmdname, int size);

int main()
{
    char cmdline[MAXLINE];
    
    while (1)
    {
        printf("ush> ");
        fflush(stdout);

        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
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
    
    char *tok = strtok(cmdline, " ");
    while (tok != NULL)
    {
        // do stuff here...        
        tok = strtok(NULL, " ");
    }
}

int expand_cmd_path(char *cmdname, int size)
{
    char str[256];
    char *cmd = "which 2>/dev/null ";
    strcpy(str, cmd);
    strncpy(str + strlen(cmd), cmdname, sizeof(str) - strlen(cmd));
    FILE *p = popen(str, "r");
    if (fgets(cmdname, size, p) == NULL)
    {
        pclose(p);
        return -1;
    }
    cmdname[strlen(cmdname) - 1] = '\0';
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
