#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

// constants
#define CMD_MAX_LEN     1024
#define CMD_MAX_COUNT   32
#define PATH_MAX        1024
#define MAX_ARG         64
#define MAX_NUM_CMDS    2

// structs
struct Cmd {
    char cmd[CMD_MAX_LEN];
    char *infile;
    char *outfile;
    int infd;
    int outfd;
};

// global vars
char cdir[PATH_MAX];

void printPrompt() {
    printf("%s $ ", cdir);
}

void readCmd(char *cmd) {
    fgets(cmd, CMD_MAX_LEN, stdin);
    // removing the trailing newline
    cmd[strlen(cmd)-1] = '\0';
}

int parseCmd(char *rawCmd, struct Cmd *cmds) {
    memset(cmds, 0, sizeof(struct Cmd) * MAX_NUM_CMDS);
    return 0;
}

void runCmd(struct Cmd *c) {
    if (!strncmp(c->cmd, "cd ", 3)) {
        // builtin function for changing directory
        char *newDir = c->cmd + 3;
        if (!newDir) {
            return;
        } else if (newDir[0] != '/') {
            char tempPath[PATH_MAX];
            strcpy(tempPath, cdir);
            int len = strlen(tempPath);
            sprintf(tempPath, "%s/%s", tempPath, newDir);
            chdir(tempPath);
        } else {
            chdir(newDir);
        }
        getcwd(cdir, PATH_MAX);
    } else if (!strcmp(c->cmd, "about")) {
        // about
        printf("Jiaoni Zhou\nW1189742\n");
    } else {
        // fork-exec
        int pid;
        if ((pid = fork())) {
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror(strerror(status));
                exit(-1);
            }
        } else if (pid > 0) {
            // setup the correct fd to implement pipe and redirection
            if (c->infd != -1) {
                if (dup2(c->infd, 0) == -1) {
                    perror(strerror(errno));
                    exit(-1);
                } else {
                    close(c->infd);
                }
            }
            if (c->outfd != -1) {
                if (dup2(c->outfd, 1) == -1) {
                    perror(strerror(errno));
                    exit(-1);
                } else {
                    close(c->outfd);
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
                perror(strerror(errno));
                exit(-1);
            };
        } else {
            perror(strerror(errno));
            exit(-1);
        }
    }
}

void evalCmds(struct Cmd *cmds, int numCmd) {
    for(int i = 0; i < numCmd; ++i) {
        struct Cmd *cmd = &cmds[i];
        if (cmd->infile) {
            cmd->infd = open(cmd->infile, O_RDONLY);
        } else {
            cmd->infd = -1;
        }
        if (cmd->outfile) {
            cmd->outfd = open(cmd->outfile, O_WRONLY);
        } else {
            cmd->outfd = -1;
        }
    }
    if (numCmd == 2) {
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror(strerror(errno));
            exit(-1);
        }
        cmds[0].outfd = pipefd[0];
        cmds[1].infd = pipefd[1];
    }
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
        evalCmds(cmds, numCmd);

        for(int i = 0; i < numCmd; ++i) {
            runCmd(&cmds[i]);
        }
        // Close the fd if there is any
        for(int i = 0; i < numCmd; ++i) {
            if (cmds[i].infd != -1) {
                close(cmds[i].infd);
            }
            if (cmds[i].outfd != -1) {
                close(cmds[i].outfd);
            }
        }

    }
}

int main() {
    shell();
}
