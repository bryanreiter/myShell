# Project 2: Developing a Linux Shell

## **NOTE**: 
Unfortunately I do not have the rights to make the original repository public, so it documented here. The commits are public for the original project.


## Details

This project was written for my Operating Systems course (CIS 3207) at Temple University.
It is a basic unix like shell that can perform redirection, piping, and basic background processing.

There is functionality that I didn't have time to add that I may add in the future.


## Function
  ### `void checkIOorPipe(char *input, char **envp)`
  
  This function acts as a finder for some of the special characters like: `>, <, >>, |`
  We check if the input string contains one of those substrings, and then perform correct file table manipulation
  Then we call the execArg() function. All the filetable set up is done in this function, while execution
  takes place in execArg().
  
  ### `void execArg(char **args, int count, char *input, char **envp)`
  
  This function drives the execution of commands, either builtin or external. We begin by checking each builtin command
  that could be used, and then execute them. If it is not a builtin, then it is an external program. We start by checking
  the `/bin/` PATH. To do this, we set the PATH environment variable equal to `/bin/`
  
  If that fails, we try the current working directory (cwd or pwd). We change the environment PATH to the value returned by`getcwd()`. We then
  check if the program exists there. If it doesn't, throw an error message.
  
  ## Notes 
  My original pseudocode has changed, and I've diverged here and there.
  
  A more general much simplier explanation (basically the pseudocode without all the logistics of piping and redirection
  
  Take the input
  Check on a few simple builtins (ones that don't print or shouldn't be used in redirection.
  Then run checkIOorPipe. We check for each type of special symbol and then set up file table correctly if needed and call execArg().
  Then if none of those, its a basic command that requires no file table manipulation, so just call execArg().
  In execArg() first check if its a builtin, then check if its an external command.
  We first check the bin directory, than the current directory. if neither, error.
  If one of them, we fork and execute the arguments in a child, so we don't overlay parents
  
  ## Testing
  Testing basic commands was as simple as checking if worked properly, outputing or doing the correct action.
  
  Specials are a little more involved.
  I will document what I tested each for.
  
  ### Testing Piping:
  First I tested any misuse. For example I tested:
  
  `|`
  `ls |`
  `| ls`
  
  These all get catched.
  
  Next I tested with a correct command. I tested:
  
  `ls -la | grep foo`
  This would print only the files/directories in my directory that contain the string `foo` in their name. This worked correctly.

### Testing Redirection

Redirection took some time, since there was so many difference cases to try, but I first tested the misuse cases first just like in piping:

  `>`
  `ls <`
  `>> output.txt`
  
  Although I show only 1 test for each of these, they have all been tested for each variation.
  
  I then tested different commands such as:
  `ls >> output.txt`
  `grep foo < foo.txt >> output.txt`
  `cd > output.txt`
  `grep foo < foo.txt > output.txt`
  
  These all worked as expected.

### Testing Background
  This was as simple as saying: `sleep 2 &` and seeing if I got access to the shell back.
  
  ### Batchmode
  To test batchmode this was as simple as creating a batchfile that contains different commands to run.
  I tested with commands from above, and noted the output to ensure it was correct.
  
  ### Misc. Testing
  Lastly I tested anything else left to test, such as external commands. These commands are found in `\bin` or the `PWD`.
  I first test the bin and then test the current directory. These have been tested as well.

  **KNOWN BUGS**
  It is also important to note that if a user inputs a wrong command with specials, it tends to bug out a little but can be   put back on track sometimes. For example if they say: `a | grep foo` and the shell tends to bug out a little with how it
  displays text. I have not found the fix.

  Also, batchmode seg faults at the end of the file, I could not decipher why.
  

  ## Problems That Arose

  I had trouble for a while with cleaning up the zombie processes, but I figured it out using `waitpid()`.

  My biggest problem that I had was 100% Background processing. When I submit this, it will probably not allow background processing since I ran into a lot of issues  
  which caused the shell to bug out. I will document what I tried and what issues it caused.

  **UPDATE** I fixed background processing, but my shell only allows basic background processing. Meaning it only works for 1 process.

  I will keep what is below as it is what was logged originally.
  
  ```
  int status;
  pid_t p;
   while ((p = waitpid(-1, &status, WNOHANG)) > 0)
   {
        pid_t cpid = getpid();
        printf("harvested: %d\n", cpid);// child terminated
   }
getline(&input, &len, stdin);
  ```
  Here I attempt to check if any children have changed state, if they did, reap them. I thought this would work but it does not.
  My testing for it was as follows:

  **NOTE THIS HAS BEEN FIXED!**
  
  **TEST ONE**
  ```
  /home/breiter/CIS3107/Lab2:myShell~$sleep 2 &
/home/breiter/CIS3107/Lab2:myShell~$created: 586
/home/breiter/CIS3107/Lab2:myShell~$ps
  PID TTY          TIME CMD
  246 pts/0    00:00:00 bash
  584 pts/0    00:00:00 myShell3
  586 pts/0    00:00:00 myShell3
  588 pts/0    00:00:00 ps
/home/breiter/CIS3107/Lab2:myShell~$ps
  PID TTY          TIME CMD
  246 pts/0    00:00:00 bash
  584 pts/0    00:00:00 myShell3
  586 pts/0    00:00:00 myShell3
  589 pts/0    00:00:00 ps
/home/breiter/CIS3107/Lab2:myShell~$exit
Exiting...
breiter@Bryan-PC:~/CIS3107/Lab2$
  ```
  As you can see, a second process is created, but when the command finished, it does not exit or get reaped. But for some reason, we only need to call `exit` once. I could not find a solution to this so I left it out.
  
  **SECOND TEST**:
  ```
  /home/breiter/CIS3107/Lab2:myShell~$sleep 2 &
/home/breiter/CIS3107/Lab2:myShell~$created: 601
/home/breiter/CIS3107/Lab2:myShell~$ps
  PID TTY          TIME CMD
  246 pts/0    00:00:00 bash
  600 pts/0    00:00:00 myShell3
  601 pts/0    00:00:00 myShell3
  603 pts/0    00:00:00 ps
/home/breiter/CIS3107/Lab2:myShell~$exit
Exiting...
exit
Exiting...
breiter@Bryan-PC:~/CIS3107/Lab2$
  ```
  You can see here again, 2 processe are running, but the child is never reaped. I then have to call `exit` twice for some reason in this.

  Due to the unpredictable logic, I had to keep it out from the final submission.
  
  This was the only huge issue I ran into involving background processing and children, unfortunately it caused me to have to exclude this.
