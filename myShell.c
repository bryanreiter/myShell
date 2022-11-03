/*
Bryan Reiter
CIS3107
Project 2: myShell

This is a basic shell that can do redirection, piping and background processes.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
void checkIOorPipe(char *input, char **envp);
void execArg(char **args, int count, char *input, char **envp);
char *startingDirectory = " ";
char error_message[60] = "Incorrect Usage. See 'help' for some assistance.\n";
int main(int argc, char **argv, char **envp)
{
    // initiate a buffer for cwd
    char *buffer = (char *)malloc(250 * sizeof(char));
    getcwd(buffer, 250);
    setenv("PWD", buffer, 1); // set PWD to current working directory
    startingDirectory = buffer;
    if (argc == 2)
    {
        setenv("PATH", "/bin/", 1);
        char line[250];
        FILE *f = fopen(argv[1], "r");
        if (f == NULL)
        {
            printf("%s\n", strerror(errno));
            return (EXIT_FAILURE);
        }
        while (fgets(line, 250, f) != NULL)
        {
            int status = 0;
            pid_t p = waitpid(-1, &status, WNOHANG);
            if (p > 0)
            {
                // harvest the children
            }
            printf("\n");
            setenv("PATH", "/bin/", 1);
            int length = strlen(line);
            int found = 0;
            char toFind = '&';
            for (int i = 0; i < length; i++) // count the number of found ampersands.
            {
                if (line[i] == toFind)
                {
                    found++;
                }
            }
            if (line[length - 1] == '\n')
            {
                line[length - 1] = '\0';
            }
            if (strcmp(line, "pause") == 0)
            {
                printf("Press Enter to Continue");
                while (getchar() != '\n')
                { // while loop waits until getchar() gets a newline (enter key is pressed)
                  // do nothing
                }
            }
            else if (strcmp(line, "exit") == 0)
            {
                printf("Exiting...\n");
                exit(0);
            }
            else if (found == 1)
            {
                for (int i = 0; i < length; i++)
                {
                    if (line[i] == toFind)
                    {
                        if (line[i + 1] == '\0')
                        { // jut at the end, so fork and don't wait.
                            pid_t pid = fork();
                            if (pid == 0)
                            {
                                line[i] = '\0';
                                checkIOorPipe(line, envp);
                                exit(0);
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else // if not newline, means we somewhere in the middle of string.
                        {
                            // more complicated.
                        }
                    }
                }
            }
            else
            {
                checkIOorPipe(line, envp);
            }
        }
        //kill any left over processes.
        int status;
        pid_t p = waitpid(-1, &status, WNOHANG);
        if (p > 0)
        {
            // harvest the children
        }
    }
    else if (argc == 1)
    {                           // run in interactive mode
        printf("\e[1;1H\e[2J"); // runs some magic and clears the screen
        while (1)               // run forever
        {
            setenv("PATH", "/bin/", 1);
            char *input = NULL; // input pointer is a null pointer.
            size_t len = 0;
            printf("%s:myShell~$", getenv("PWD")); // print cwd with ~$
            // read the user line
            // check to see if we have some zombies (children who status has changed. Yes? Clean them up)
            int status = 0;
            pid_t p = waitpid(-1, &status, WNOHANG);
            if (p > 0)
            {
                // harvest the children
            }
            getline(&input, &len, stdin);
            // input by default ends with a newline
            // replace that newline character

            int found = 0;
            char toFind = '&';
            for (int i = 0; i < strlen(input); i++) // count the number of found ampersands.
            {
                if (input[i] == toFind)
                {
                    found++;
                }
            }
            if (strcmp(input, "\n") == 0) // no input
            {
                continue;
            }

            int length = strlen(input);

            if (input[length - 1] == '\n')
            {
                input[length - 1] = '\0';
            }
            if (strcmp(input, "pause") == 0)
            {
                printf("Press Enter to Continue");
                while (getchar() != '\n')
                { // while loop waits until getchar() gets a newline (enter key is pressed)
                  // do nothing
                }
            }
            else if (strcmp(input, "exit") == 0)
            {
                printf("Exiting...\n");
                exit(0);
            }
            else if (found == 1)
            {
                for (int i = 0; i < strlen(input); i++)
                {
                    if (input[i] == toFind)
                    {
                        if (input[i + 1] == '\0')
                        { // just at the end, so fork and don't wait.
                            pid_t pid = fork();
                            if (pid == 0)
                            {
                                input[i] = '\0';
                                checkIOorPipe(input, envp);
                                exit(0);
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else // if not newline, means we somewhere in the middle of string.
                        {
                            // more complicated.
                        }
                    }
                }
            }
            else
            {
                checkIOorPipe(input, envp);
            }
            free(input); // free up memory
        }
    }
    else
    {
        printf("Too many arguments!\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}
void execArg(char **args, int count, char *input, char **envp)
{
    char *filePath;
    int command = 0;
    int current = 0;
    while (args[current] != NULL)
    {
        if (strcmp(args[current], "cd") == 0) // is cd
        {
            command = 1;
            if (args[current + 1] != NULL)
            {
                if (args[current + 2] != NULL)
                {
                    printf("Too many arguments!\n");
                    break;
                }
                if (chdir(args[current + 1]) != 0)
                { // change dir
                    printf("%s\n", strerror(errno));
                }
                else
                {
                    char *buffer = (char *)malloc(250 * sizeof(char));
                    getcwd(buffer, 250);      // store new directory into buffer.
                    setenv("PWD", buffer, 1); // set PWD to new directory
                }
            }
            else
            {
                printf("%s\n", getenv("PWD"));
            }
        }
        else if (strcmp(args[current], "dir") == 0) // is dir
        {
            command = 1;
            struct dirent **list;
            int num = 0;
            if (args[current + 1] == NULL)
            { // no args, throw error message
                printf("Please specify a directory with the command.\n");
            }
            else
            {
                if (args[current + 2] != NULL)
                {
                    printf("Too many arguments!\n");
                    break;
                }
                else
                {
                    num = scandir(args[current + 1], &list, NULL, alphasort);
                }
            }
            if (num == -1)
            { // check if we can open the directory
                printf("Directory cannot be opened.\n");
                break;
            }
            else
            {
                while (num--)
                {
                    if (strcmp(list[num]->d_name, ".") != 0 && strcmp(list[num]->d_name, "..") != 0)
                    {
                        printf("%s ", list[num]->d_name);
                    }
                }
                printf("\n");
            }
        }
        else if (strcmp(args[current], "echo") == 0)
        {
            command = 1;
            if (args[current + 1] != NULL)
            {
                int pos = current + 1;
                while (args[pos] != NULL)
                {
                    printf("%s ", args[pos]);
                    pos++;
                }
                printf("\n");
            }
            else
            {
                printf("\n");
                break;
            }
        }
        else if (strcmp(args[current], "help") == 0) // TODO
        {
            int status;
            command = 1;
            if (args[current + 1] != NULL)
            {
                printf("Too many arguments!\n");
            }
            else
            {
                pid_t pid = fork();
                char *helpArgs[3];
                chdir(startingDirectory);
                helpArgs[0] = "more";
                helpArgs[1] = "readme_doc";
                helpArgs[2] = NULL;
                if (pid == 0)
                { // is child
                    setenv("PATH", "/bin/", 1);
                    filePath = getenv("PATH");
                    strcat(filePath, "more");
                    execv(filePath, helpArgs);
                    exit(0);
                }
                else
                {
                    waitpid(pid, &status, 0);
                }
            }
            chdir(getenv("PWD"));
        }
        else if (strcmp(args[current], "environ") == 0) // TODO
        {
            command = 1;
            if (args[current + 1] != NULL)
            {
                printf("Too many arguments!\n");
            }
            else
            {
                for (char **env = envp; *env != 0; env++) // this grabs each enviroment variable, one at a time and prints.
                {
                    char *cur = *env;
                    printf("%s\n", cur);
                }
            }
        }
        else if (strcmp(input, "clr") == 0)
        {
            printf("\e[1;1H\e[2J"); // runs some magic and clears the screen
        }
        current++;
    }
    if (command == 0)
    {
        filePath = getenv("PATH");
        strcat(filePath, args[0]);
        pid_t pid = fork();
        int status;
        if (pid == 0)
        { // is child
            // check if exists in bin
            if (execv(filePath, args) != -1)
            {
                // it exists in bin!
                //  runs successfully
                exit(0); // exit child 1
            }
            else
            {
                // doesn't exist in bin, try current directory!
                char *buffer = (char *)malloc(250 * sizeof(char));
                getcwd(buffer, 250);
                setenv("PATH", buffer, 1);
                filePath = getenv("PATH"); // set up environment and file path correctly.
                strcat(filePath, "/");
                strcat(filePath, args[0]);
                if (execv(filePath, args) != -1)
                {
                    // it exists in current directory!
                    //  runs successfully
                    // kill off children
                    exit(0);
                    exit(0);
                }
                else
                {
                    // we couldn't find that command anywhere :(
                    printf("%s\n", strerror(errno)); // print error message
                    // kill off children
                    exit(0);
                    exit(0);
                }
            }
        }
        else
        {
            waitpid(pid, &status, 0);
        }
    }
}

void checkIOorPipe(char *input, char **envp)
{
    int count = 0;
    char last;
    int len = strlen(input);
    int pos = 0;
    if (len > 0)
    {
        last = input[0];
    }
    for (int k = 0; k <= len; k++)
    {
        if (((input[k] == ' ' || input[k] == '\0') && last != ' '))
        {
            count++;
        }
        last = input[k];
    }

    count++; // add another for NULL
    char *arguments[count];
    char *token;
    char *copy = (char *)malloc(strlen(input) + 1); // setting up a copy of input, so we can preserve the original
    strcpy(copy, input);                            // copy original into copy
    token = strtok(copy, " \n\t\r");                // split string
    int i = 0;
    int j = 0;
    while (token != NULL)
    {
        arguments[i] = token;
        token = strtok(NULL, " \n\t\r");
        i++;
    }
    arguments[i] = NULL; // execv expects NULL array

    if (strstr(input, "|") != NULL) // ls -la | grep foo
    {
        for (j = 0; j < count - 1; j++)
        {
            if ((strcmp(arguments[j], "|") == 0))
            {
                arguments[j] = NULL;
                pos = j;
            }
        }
        // Error catching
        if (pos == 0)
        { // nothing on left side
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        else if ((arguments[pos + 1] == NULL) || (arguments[pos - 1] == NULL))
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        pos++; // next argument after pipe
        int size = pos;
        int fds[2];
        if (pipe(fds) != 0)
        {
            printf("%s\n", strerror(errno));
        }
        else
        {
            pid_t pid1 = fork();
            int status;
            if (pid1 == -1)
            {
                printf("%s\n", strerror(errno));
            }
            if (pid1 == 0)
            {                    // in child
                close(1);        // close stdout
                dup2(fds[1], 1); // replace stdout with pipe write
                close(fds[0]);   // close read end of pipe
                execArg(arguments, count, input, envp);
                exit(0);
            }
            else
            {
                int newArrSize = count - pos;
                char *restArgs[newArrSize];
                for (int k = 0; k < newArrSize; k++)
                {
                    restArgs[k] = arguments[pos];
                    pos++;
                }
                pid_t pid2 = fork();
                if (pid2 == 0)
                {                    // is 2nd child
                    close(0);        // close stdin
                    dup2(fds[0], 0); // replace stdin with pipe read end
                    close(fds[1]);   // close writing end of pipe
                    execArg(restArgs, size, input, envp);
                    exit(0);
                }
                else
                {
                    waitpid(-1, &status, 0);
                }
            }
        }
        close(fds[0]);
        close(fds[1]);
    }
    else if (strstr(input, ">>") != NULL) // Tested with ls >> output.txt, appends
    {                                     // redirect output. append don't overwrite.
        if (strstr(input, "<") != NULL)   // testing on grep foo < foo.txt >> output.txt
        {                                 // input and output redirection
            for (j = 0; j < count - 1; j++)
            {
                if ((strcmp(arguments[j], "<") == 0) || (strcmp(arguments[j], ">>") == 0))
                {
                    arguments[j] = NULL;
                    pos = j;
                }
            }
            // Error catching
            if (pos == 0)
            { // nothing on left side
                write(STDERR_FILENO, error_message, strlen(error_message));
                return;
            }
            else if ((arguments[pos + 1] == NULL) || (arguments[pos - 1] == NULL))
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                return;
            }
            int fd1 = open(arguments[count - 2], O_CREAT | O_WRONLY | O_APPEND, S_IRWXU, S_IRWXO); // opens the first
            int sout = dup(1);                                                                     // dup stdout
            dup2(fd1, 1);
            int fd2 = open(arguments[count - 4], O_RDONLY);
            int sin = dup(0);
            dup2(fd2, 0);
            execArg(arguments, count, input, envp);
            dup2(sout, 1);
            dup2(sin, 0);
            close(fd1);
            close(fd2);
        }
        else // just output and append
        {
            for (j = 0; j < count - 1; j++) // tested
            {
                if (strcmp(arguments[j], ">>") == 0)
                {
                    arguments[j] = NULL;
                    pos = j;
                }
            }
            // Error catching
            if (pos == 0)
            { // nothing on left side
                write(STDERR_FILENO, error_message, strlen(error_message));
                return;
            }
            else if ((arguments[pos + 1] == NULL) || (arguments[pos - 1] == NULL))
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                return;
            }
            int fd = open(arguments[count - 2], O_CREAT | O_WRONLY | O_APPEND, S_IRWXU, S_IRWXO);
            int sout = dup(1); // store stdout fd
            dup2(fd, 1);
            execArg(arguments, count, input, envp);
            dup2(sout, 1); // restore
            close(fd);
        }
    }
    else if (strstr(input, ">") != NULL) // tested with cd > output.txt, overwrites.
    {                                    // redirect output, overwrite
        if (strstr(input, "<") != NULL)  // tested
        {                                // both input and output redirect
            for (j = 0; j < count - 1; j++)
            {
                if ((strcmp(arguments[j], "<") == 0) || (strcmp(arguments[j], ">") == 0))
                {
                    arguments[j] = NULL;
                    pos = j;
                }
            }
            // Error catching
            if (pos == 0)
            { // nothing on left side
                write(STDERR_FILENO, error_message, strlen(error_message));
                return;
            }
            else if ((arguments[pos + 1] == NULL) || (arguments[pos - 1] == NULL))
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                return;
            }
            int fd1 = open(arguments[count - 2], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU, S_IRWXO); // opens the first
            int sout = dup(1);                                                                    // dup stdout
            dup2(fd1, 1);
            int fd2 = open(arguments[count - 4], O_RDONLY);
            int sin = dup(0);
            dup2(fd2, 0);
            execArg(arguments, count, input, envp);
            dup2(sout, 1);
            dup2(sin, 0);
            close(fd1);
            close(fd2);
        }
        else
        {                                   // just simply output redirect, no input
            for (j = 0; j < count - 1; j++) // tested
            {
                if (strcmp(arguments[j], ">") == 0)
                {
                    arguments[j] = NULL;
                    pos = j;
                }
            }
            // Error catching
            if (pos == 0)
            { // nothing on left side
                write(STDERR_FILENO, error_message, strlen(error_message));
                return;
            }
            else if ((arguments[pos + 1] == NULL) || (arguments[pos - 1] == NULL))
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                return;
            }
            int fd = open(arguments[count - 2], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU, S_IRWXO);
            int sout = dup(1); // store stdout fd
            dup2(fd, 1);
            execArg(arguments, count, input, envp);
            dup2(sout, 1); // restore
            close(fd);
        }
    }
    else if (strstr(input, "<") != NULL) // tested with grep foo < foobar.txt    WORKS
    {                                    // input redirection
        for (j = 0; j < count - 1; j++)
        {
            if (strcmp(arguments[j], "<") == 0)
            {
                arguments[j] = NULL;
                pos = j;
            }
        }
        // Error catching
        if (pos == 0)
        { // nothing on left side
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        else if ((arguments[pos + 1] == NULL) || (arguments[pos - 1] == NULL))
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        int fd = open(arguments[count - 2], O_RDONLY); // open the file
        int sin = dup(0);                              // stores stdin fd
        dup2(fd, 0);                                   // overwrties stdin with file's fd
        execArg(arguments, count, input, envp);
        dup2(sin, 0); // restore file table back to normal
        close(fd);
    }
    else
    { // no special just run normally :) call execArg
        execArg(arguments, count, input, envp);
    }
    free(copy);
}
