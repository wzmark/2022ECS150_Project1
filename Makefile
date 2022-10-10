all: sshell

sshell: sshell.o
	gcc -Wall -Wextra -Werror -g -o sshell sshell.o

sshell.o: sshell.c sshell.h sshellModel.h
	gcc -Wall -Wextra -Werror -g -c -o sshell.o sshell.c

clean:
	rm shell 
	rm shell.o