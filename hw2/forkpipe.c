#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int check(int res);
void handle_sigterm();

int main() {
	int pipe_fd[2];
	check(pipe(pipe_fd));
	
	// GEN
	pid_t gen = check(fork());
	// child
	if (gen == 0) {
		// handle SIGTERM
		if (signal(SIGTERM, handle_sigterm) == SIG_ERR) exit(2);

		check(dup2(pipe_fd[1], STDOUT_FILENO));
		check(close(pipe_fd[0]));
		check(close(pipe_fd[1]));
		int cnt = 0;
		while (1) {
			printf("OSY count %d\n", cnt++);
			fflush(stdout);
			sleep(1);
		}
		exit(0);
	}

	// XXD
	pid_t xxd = check(fork());
	// child
	if (xxd == 0) {
		check(dup2(pipe_fd[0], STDIN_FILENO));
		check(close(pipe_fd[0]));
		check(close(pipe_fd[1]));
		execlp("xxd", "xxd", "-p", NULL);
		exit(2);
	}

	//parent
	check(close(pipe_fd[0]));
	check(close(pipe_fd[1]));
	sleep(5);
	check(kill(gen, SIGTERM));
	
	int gen_status, xxd_status;
	check(waitpid(gen, &gen_status, 0));
	check(waitpid(xxd, &xxd_status, 0));

	if (WIFEXITED(gen_status) && WEXITSTATUS(gen_status) == 0 &&
	    WIFEXITED(xxd_status) && WEXITSTATUS(xxd_status) == 0) {
		check(write(STDOUT_FILENO, "OK\n", sizeof("OK")));
		return 0;
	} else {
		check(write(STDOUT_FILENO, "ERROR\n", sizeof("ERROR")));
		return 1;
	}
}

int check(int res) {
	if (res < 0) exit(2);
	return res;
}

void handle_sigterm() {
	if (write(STDERR_FILENO, "GEN TERMINATED\n", sizeof("GEN TERMINATED")) < 0) _exit(2);
	_exit(0);
}

