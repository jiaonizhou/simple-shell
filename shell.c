#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

// constants
#define CMD_MAX_LEN     1024
#define CMD_MAX_COUNT   32
#define PATH_MAX        1024
#define MAX_ARG         64
#define MAX_NUM_CMDS    2

extern char** environ;

// structs
struct Cmd {
    char *cmd;
    char *infile;
    char *outfile;
    int infd;
    int infdpipe;
    int outfd;
    int outfdpipe;
};

// global vars
char cdir[PATH_MAX];

char * trimSpace(char * buffer)
{
    char * new_buffer = NULL;
    for (int i = 0; buffer[i] != '\0'; ++i) {
        if (buffer[i] != ' ') {
            new_buffer = &buffer[i];
            break;
        }
    }
    for (int j = strlen(buffer) - 1; j >= 0; --j) {
        if (buffer[j] != ' ') {
            buffer[j + 1] = '\0';
            break;
        }
    }
    return new_buffer;
}

int parseCmd(char *rawCmd, struct Cmd *cmds) {
    memset(cmds, 0, sizeof(struct Cmd) * MAX_NUM_CMDS);

    int num_of_commands = 0;
    char * token[2];
    cmds[0].infile = NULL;
    cmds[0].outfile = NULL;
    cmds[1].infile = NULL;
    cmds[1].outfile = NULL;
    cmds[0].cmd = NULL;
    cmds[1].cmd = NULL;
    token[0] = strtok(rawCmd, "|");
    token[1] = strtok(NULL, "|");

    if (token[0] != NULL) {
        num_of_commands = 1;
        for (int i = 0; token[0][i] != '\0'; ++i) {
            cmds[0].cmd = &token[0][0];
            if (token[0][i] == '<') {
                token[0][i] = '\0';
                cmds[0].infile = &token[0][i + 1];
            }
            if (token[0][i] == '>') {
                token[0][i] = '\0';
                cmds[0].outfile = &token[0][i + 1];
            }
        }
    }

    if (token[1] != NULL) {
        num_of_commands = 2;
        for (int j = 0; token[1][j] != '\0'; ++j) {
            cmds[1].cmd = &token[1][0];
            if (token[1][j] == '>') {
                token[1][j] = '\0';
                cmds[1].outfile = &token[1][j + 1];
            }
        }
    }
    if (cmds[0].cmd != NULL) cmds[0].cmd = trimSpace(cmds[0].cmd);
    if (cmds[0].infile != NULL) cmds[0].infile = trimSpace(cmds[0].infile);
    if (cmds[0].outfile != NULL) cmds[0].outfile = trimSpace(cmds[0].outfile);
    if (cmds[1].cmd != NULL) cmds[1].cmd = trimSpace(cmds[1].cmd);
    if (cmds[1].infile != NULL) cmds[1].infile = trimSpace(cmds[1].infile);
    if (cmds[1].outfile != NULL) cmds[1].outfile = trimSpace(cmds[1].outfile);
    return num_of_commands;
}

void printPrompt() {
    printf("mysh$ ");
}

void readCmd(char *cmd) {
    fgets(cmd, CMD_MAX_LEN, stdin);
    // removing the trailing newline
    cmd[strlen(cmd) - 1] = '\0';
}

int runCmd(struct Cmd *c) {
    int pid = -1;
    if (!strncmp(c->cmd, "cd", 2)) {
        // builtin function for changing directory
        char *newDir = c->cmd + 3;
        if (!*(c->cmd + 2)) {
            printf("%s\n", cdir);
            return pid;
        } else if (newDir[0] != '/') {
            char tempPath[PATH_MAX];
            strcpy(tempPath, cdir);
            int len = strlen(tempPath);
            sprintf(tempPath, "%s/%s", tempPath, newDir);
            if (chdir(tempPath) == -1) {
                perror(tempPath);
            }
        } else {
            if (chdir(newDir) == -1) {
                perror(newDir);
            }
        }
        if (!getcwd(cdir, PATH_MAX)) {
            perror("getcwd");
            exit(-1);
        }

    } else if (!strcmp(c->cmd, "about")) {
        // about
        printf("Jiaoni Zhou W1189742\n Chenjun Ling W1189446\n");
    } else if (!strcmp(c->cmd, "clr")) {
        system("clear");
    } else if (!strcmp(c->cmd, "dir")) {
        system("ls");
    } else if (!strcmp(c->cmd, "environ")) {
        char **var;
        for(var = environ; *var != NULL; ++var) {
            printf("%s\n", *var);
        }
    } else if (!strcmp(c->cmd, "help")) {
        system("help");
    } else if (!strcmp(c->cmd, "exit")) {
        kill(0, SIGKILL);
    }else {
        // fork-exec
        pid = fork();
        if (pid == 0) {
            // setup the correct fd to implement pipe and redirection
            if (c->infd != -1) {
                if (dup2(c->infd, 0) == -1) {
                    perror("dup2");
                    exit(-1);
                } else {
                    close(c->infd);
                    if (c->infdpipe != -1) {
                        close(c->infdpipe);
                    }
                }
            }
            if (c->outfd != -1) {
                if (dup2(c->outfd, 1) == -1) {
                    perror("dup2");
                    exit(-1);
                } else {
                    close(c->outfd);
                    if (c->outfdpipe != -1) {
                        close(c->outfdpipe);
                    }
                }
            }

            char *argv[MAX_ARG];
            int i = 0;
            char *token = strtok(c->cmd, " ");
            while(token) {
                argv[i++] = token;
                token = strtok(NULL, " " );
            }
            argv[i] = NULL;
            if (execvp(argv[0], argv) == -1) {
                perror(argv[0]);
                exit(-1);
            };
        } else if (pid < 0) {
            perror("fork failed");
            exit(-1);
        }
    }
    return pid;
}

int evalCmds(struct Cmd *cmds, int numCmd) {
    for(int i = 0; i < numCmd; ++i) {
        struct Cmd *cmd = &cmds[i];
        cmd->infd = -1;
        cmd->infdpipe = -1;
        cmd->outfd = -1;
        cmd->outfdpipe = -1;
        if (cmd->infile) {
            if ((cmd->infd = open(cmd->infile, O_RDONLY)) == -1) {
                perror(cmd->infile);
                return -1;
            }
        }
        if (cmd->outfile) {
            if ((cmd->outfd = open(cmd->outfile, O_RDWR | O_CREAT, 0644)) == -1) {
                perror(cmd->outfile);
                return -1;
            }
        }
    }
    if (numCmd == 2) {
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe failed");
            exit(-1);
        }
        cmds[0].outfd = pipefd[1];
        cmds[0].outfdpipe = pipefd[0];
        cmds[1].infd = pipefd[0];
        cmds[1].infdpipe = pipefd[1];
    }
    return 0;
}


void shell() {
    getcwd(cdir, PATH_MAX);
    char rawCmd[CMD_MAX_LEN];
    struct Cmd cmds[MAX_NUM_CMDS];
    while(1) {
        printPrompt();
        readCmd(rawCmd);
        int numCmd = parseCmd(rawCmd, cmds);
        if (numCmd > 2) {
            fprintf(stderr, "Sorry, only support two commands connecting through pipe");
        }

        // empty command
        if (numCmd == 0) {
            continue;
        }

        // Figure out the input fd and output fd for each command. Set up pipe if necessary
        if (evalCmds(cmds, numCmd) == -1) {
            continue;
        }

        int pid[2];
        for(int i = 0; i < numCmd; ++i) {
            pid[i] = runCmd(&cmds[i]);
        }
        for(int i = 0; i < numCmd; ++i) {
            int status;
            if (pid[i] != -1) {
                if (waitpid(pid[i], &status, 0) == -1) {
                    perror("waitpid");
                } else {
                    if (cmds[i].infd != -1) {
                        close(cmds[i].infd);
                    }
                    if (cmds[i].outfd != -1) {
                        close(cmds[i].outfd);
                    }
                }
            }
        }
    }
}

int main() {
    shell();
}
