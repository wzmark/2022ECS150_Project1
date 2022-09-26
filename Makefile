all: sshell

sshell: sshell.o
	gcc -Wall -Wextra -Werror -o sshell sshell.o

sshell.o: sshell.c sshell.h sshellModel.h
	gcc -Wall -Wextra -Werror -c -o sshell.o sshell.c

clean:
	rm -f shell sshell.o