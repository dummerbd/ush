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

#define MAX_LINE 2048
#define MAX_CMDS 512
#define MAX_ARGS 64
#define PIPE_CHAR '|'


typedef struct
{
    char *cmd_name;
    char **cmd_argv;
} cmd_t;

void init_cmd_list(cmd_t *cmd_list);
int build_cmd_list(cmd_t *cmd_list, char *cmd_line);
void eval_cmd_list(cmd_t *cmd_list);
void clear_cmd_list(cmd_t *cmd_list);
int is_valid_cmd(char *cmd_name);
void unix_error(char *msg);
void app_error(char *msg);


const char *WHICH_CMD = "which 2>/dev/null ";

int main()
{
    char cmd_line[MAX_LINE];
    cmd_t cmd_list[MAX_CMDS];

    init_cmd_list(cmd_list);

    while (1)
    {
        printf("ush> ");
        fflush(stdout);

        if ((fgets(cmd_line, MAX_LINE, stdin) == NULL) && ferror(stdin))
            app_error("fgets error");

        if (feof(stdin))
        {
            // End of file (ctrl-d)
            fflush(stdout);
            exit(0);
        }

        if (build_cmd_list(cmd_list, cmd_line))
            eval_cmd_list(cmd_list);

        clear_cmd_list(cmd_list);
        fflush(stdout);
    }
    return 0;
}

/*
 * Initialize the command list for use.
 */
void init_cmd_list(cmd_t *cmd_list)
{
    int i;

    for (i = 0; i < MAX_CMDS; i++)
    {
        cmd_list[i].cmd_name = NULL;
        cmd_list[i].cmd_argv = (char **)calloc(MAX_ARGS, sizeof(char *));
        if (cmd_list[i].cmd_argv == NULL)
            unix_error("Could not allocate memory.");
    }
}

/*
 * Clear up the command list.
 */
void clear_cmd_list(cmd_t *cmd_list)
{
    int i, j;

    for (i = 0; i < MAX_CMDS; i++)
    {
        cmd_list[i].cmd_name = NULL;
        for (j = 0; j < MAX_ARGS; j++)
            cmd_list[i].cmd_argv[j] = NULL;
    }
}

/*
 * Construct a command list from the command line.
 * Returns 0 on error (too many commands or arguments), 1 on succes.
 */
int build_cmd_list(cmd_t *cmd_list, char *cmd_line)
{
    int fsm[4][2] = {{1, 3}, {2, 0}, {2, 0}, {3, 3}};
    int state, is_pipe, cmd_i, arg_i;
    char *tok;
    cmd_t *cmd;

    tok = strtok(cmd_line, " ");
    cmd = cmd_list;
    state = 0;
    is_pipe = 0;
    cmd_i = 0;
    arg_i = 0;
    while (tok != NULL)
    {
        is_pipe = tok[0] == PIPE_CHAR ? 1 : 0;
        state = fsm[state][is_pipe];
        switch (state)
        {
            case 0: // start
                break;

            case 1: // cmd name
                cmd = cmd_list + cmd_i;
                cmd_i++;
                arg_i = 0;
                cmd->cmd_name = tok;

                if (cmd_i > MAX_CMDS)
                {
                    printf("Error: too many commands\n");
                    return 0;
                }
                if (!is_valid_cmd(cmd->cmd_name))
                    return 0;
                break;

            case 2: // arg list
                cmd->cmd_argv[arg_i] = tok;
                arg_i++;
                if (arg_i > MAX_ARGS - 1)
                {
                    printf("Error: too many command arguments\n");
                    return 0;
                }
                break;

            case 3: // syntax error (trap state)
                break;
        }
        tok = strtok(NULL, " ");
    }
    if (state == 1 || state == 2)
        return 1;
    printf("Error: invalid syntax\n");
    return 0;
}

/*
 * Evaluate the commands in the command list, building pipes inbetween
 * each command as needed.
 */
void eval_cmd_list(cmd_t *cmd_list)
{
    int i, j;
    for (i = 0; i < MAX_ARGS; i++)
    {
        if (cmd_list[i].cmd_name == NULL)
            return;
        printf("Command %s\n", cmd_list[i].cmd_name);
        for (j = 0; j < MAX_ARGS; j++)
        {
            if (cmd_list[i].cmd_argv[j] == NULL)
                break;
            printf("\t%s\n", cmd_list[i].cmd_argv[j]);
        }
    }
}

/*
 * Check that the cmdname is in PATH and that the current user has
 * priveleges to execute it.
 * Returns 0 on error, 1 on success.
 */
int is_valid_cmd(char *cmd_name)
{
    char which_cmd[256];
    char buf[256];
    FILE *p;

    strcpy(which_cmd, WHICH_CMD);
    strncpy(which_cmd + strlen(WHICH_CMD), cmd_name, sizeof(which_cmd) - strlen(WHICH_CMD));
    
    p = popen(which_cmd, "r");
    if (fgets(buf, sizeof(buf), p) == NULL)
    {
        printf("Error: command not found\n");
        return 0;
    }
    pclose(p);

    return 1;
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
