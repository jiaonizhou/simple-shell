#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

// constants
#define CMD_MAX_LEN     1024
#define CMD_MAX_COUNT   32
#define PATH_MAX        1024
#define MAX_ARG         64

// structs
struct Cmd {
    char cmd[CMD_MAX_LEN];
    int infd;
    int outfd;
};

// global vars
char cdir[PATH_MAX];
struct Cmd cmds[256];

void printPrompt() {
    printf("%s $ ", cdir);
}

void readCmd(char *cmd) {
    fgets(cmd, CMD_MAX_LEN, stdin);
    // removing the trailing newline
    cmd[strlen(cmd)-1] = '\0';
}

int parseCmd(char *rawCmd, struct Cmd *cmds) {
    return 0;
}

void evalCmd(char *cmd, int infd, int outfd) {
    if (!strncmp(cmd, "cd ", 3)) {
        // builtin function for changing directory
        char *newDir = cmd + 3;
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
    } else if (!strcmp(cmd, "about")) {
        // about
        printf("Jiaoni Zhou\nW1189742\n");
    } else {
        // fork-exec
        int pid;
        if ((pid = fork())) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            char *argv[MAX_ARG];
            int i = 0;
            char *token = strtok(cmd, " ");
            while(token) {
                argv[i++] = token;
                token = strtok(NULL, " " );
            }
            argv[i] = NULL;
            execvp(argv[0], argv);
        }
    }
}

void shell() {
    getcwd(cdir, PATH_MAX);
    char rawCmd[CMD_MAX_LEN];
    while(1) {
        printPrompt();
        readCmd(rawCmd);
        //int inputFd = getInputFromCmd(rawCmd);
        //int outputFd = getOutputFromCmd(rawCmd);
        //char *realCmd = 
        evalCmd(rawCmd, 1, 0);
    }
}

int main() {
    shell();
}
