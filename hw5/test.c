#include <stdio.h>

#define MAX_U_INT 4294967295
#define BUF_MAX 11

const char d_to_char_h[16] = {
	[10] = 'a',
	[11] = 'b',
	[12] = 'c',
	[13] = 'd',
	[14] = 'e',
	[15] = 'f',
};

int my_strlen(char* str) {
	int c = -1;
	while (str[++c] != '\0');
	return c;
}

char d_to_h(int rem) {
	if (rem >= 10 && rem <= 15) return d_to_char_h[rem];
	return (char)(rem + '0');
}

void dec_to_hex(unsigned int dec, char buf[]) {
	if (dec < 0 || dec > MAX_U_INT) return;
	
	unsigned int tmp = dec;
	int c = 0;
	while (tmp != 0) {tmp /= 16; c++;}

	buf[0] = '0';
	buf[1] = 'x';
	
	if (c == 0) {
		buf[2] = '0';
		buf[3] = '\0';
		return;
	}

	int i = c + 1;
	while (i >= 2) {
		int rem = dec % 16;
		buf[i--] = d_to_h(rem);		
		dec /= 16;
	}

	buf[c+2] = '\0';
}

int main() {	
	char buf1[BUF_MAX] = {'0'};
	char buf2[BUF_MAX] = {'0'};
	char buf3[BUF_MAX] = {'0'};
	char* str = "";

	unsigned int x = 0;
	unsigned int y = 0x21;
	unsigned int z = 0x321;

	dec_to_hex(x, buf1);
	dec_to_hex(y, buf2);
	dec_to_hex(z, buf3);

	printf("%s\n", buf1);
	printf("%s\n", buf2);
	printf("%s\n", buf3);

	printf("strlen = %d\n", my_strlen(buf1));
	printf("strlen = %d\n", my_strlen(buf2));
	printf("strlen = %d\n", my_strlen(buf3));
	printf("strlen = %d\n", my_strlen(str));

	return 0;
}

	//printf("lower 16 bits %x\n", x & 0x0000ffff);
	//printf("upper 16 bits %x\n", (x & 0xffff0000) >> 16);

//	for (int i = 0; i < 8; ++i) {
//		printf("bit number %d is %x\n", i, (x & (0xf << 4*i)) >> 4*i);
//	}	
	
//	for (int i = 10; i < 16; ++i) {
//		printf("dec %d is hex %c\n", i, d_to_char_h[i]);
//	}
