#include <stdio.h>

int main(int argc, char* argv[]) {
	// LS | SORT

	int pipe_fd[2]; // 2 konce
	// if (pipe(pipe_fd) != 0) return 2;
	pipe(pipe_fd);

	pid_t pls = fork();
	if (pls == 0) { // am i a child
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[0]);	
		close(pipe_fd[1]);
		ecexlp("ls", "ls", NULL);
	}
	
	pid_t psort = fork();
	if (psort == 0) {
		dup2(pipe_fd[0], STDIN_FILENO);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		execlp("sort", "sort", NULL); // maybe add smth
	}

	// close0, close1
// + osetreni chyb

	return 0;
}

