#include <stdio.h>

int main (int argc, char* argv[]) {
	// LS > SOUBOR.TXT

	pid_t pid = fork();
	if (pid == 0) {
		close(STDOUT_FILENO); // close(1)
		int fd = open("soubor.txt", O_CREAT|O_WRONLY|O_TRUNC);
		// should open on 1 cus weve just clossed it

		execlp("ls", "ls", NULL); //l = list of args, p = path, v = args in file(char**)
	}

	

	return 0;
}

