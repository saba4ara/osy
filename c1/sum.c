#include <stdio.h>

int main(int argc, char* argv[]) {
	int num = 0;
	int sum = 0;
	int cur_num;

	while (scanf("%d", &cur_num) == 1) {
		num++;
		sum += cur_num;
	}

	printf("%d\n", sum);
	fprintf(stderr, "Sum of %d numbers is %d\n", num, sum);
	
	return 0;
}

