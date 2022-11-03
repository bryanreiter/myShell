executable: myShell.c
	gcc -o myShell myShell.c -Wall -Werror
clean:
	rm *.o myShell