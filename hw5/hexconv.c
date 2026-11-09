#define MAX_U_INT 4294967295
#define BUF_MAX 16
#define CLOBBERS "rcx", "r11", "memory"

enum std_nums {
	STDIN_FILENO = 0,
	STDOUT_FILENO = 1
};

enum syscall_nums {
	READ = 0,
	WRITE = 1,
	EXIT = 60
};

const char d_to_h[16] = {
	[10] = 'a',
	[11] = 'b',
	[12] = 'c',
	[13] = 'd',
	[14] = 'e',
	[15] = 'f',
};



// MY SYSCALLS ------------------------------------------------------

int my_read_write(long syscall_num, int fd, char* buf, int count)  {
	int ret;

	asm volatile (
	"syscall"
	: "=a" (ret)
	: "a" (syscall_num),
	  "D" (fd),
	  "S" (buf),
	  "d" (count)
	: CLOBBERS
	);
	
	return ret;
}

int my_exit(int status) {
	int ret;

	asm volatile (
	    "syscall"
	    : "=a" (ret)
	    : "a" (EXIT),
	      "D" (status)
	    : CLOBBERS
	);
	
	return ret;
}



// MY FUNCS ---------------------------------------------------------

int my_strlen(char* str) {
	int c = -1;
	while (str[++c] != '\0');
	return c;
}

char dec_to_hex(int rem) {
	if (rem >= 10 && rem <= 15) return d_to_h[rem];
	return (char)(rem + '0');
}

void my_sprintf(char* buf, unsigned int dec) {
	// if (dec < 0 || dec > MAX_U_INT) return;
	
	unsigned int tmp = dec;
	int c = 0;
	while (tmp != 0) {tmp /= 16; c++;}

	buf[0] = '0';
	buf[1] = 'x';
	
	if (c == 0) {
		buf[2] = '0';
		buf[3] = '\n';
		buf[4] = '\0';
		return;
	}

	int i = c + 1;
	while (i >= 2) {
		int rem = dec % 16;
		buf[i--] = dec_to_hex(rem);		
		dec /= 16;
	}

	buf[c+2] = '\n';
	buf[c+3] = '\0';
}



// TEMPLATE FUNCS ---------------------------------------------------

int isnum(char ch)
{
    return ch >= '0' && ch <= '9';
}

int isspc(char ch)
{
    return ch == ' ' || ch == '\n';
}

static void print(unsigned num)
{
    char buf[BUF_MAX];
    my_sprintf(buf, num);
    int ret = my_read_write(WRITE, STDOUT_FILENO, buf, my_strlen(buf));
    if (ret < 0)
        my_exit(1);
}



// MAIN -------------------------------------------------------------

void _start()
{
    char buf[BUF_MAX];
    unsigned num = 0;
    int i = 0;
    int num_digits = 0;
    unsigned chars_to_process = 0;

    for (/* no init */; /* no end condition */; i++, chars_to_process--) {
        if (chars_to_process == 0) {
            int ret = my_read_write(READ, STDIN_FILENO, buf, sizeof(buf));
            if (ret < 0) 
	    	my_exit(1);
            i = 0;
            chars_to_process = ret;
        }
        if (
            num_digits > 0
            && (chars_to_process == 0 /* EOF */ || !isnum(buf[i]))
        ) {
            print(num);
            num_digits = 0;
            num = 0;
        }
        if (
            chars_to_process == 0 /* EOF */
            || (!isspc(buf[i]) && !isnum(buf[i]))
        )
        	my_exit(0);

        if (isnum(buf[i])) {
            num = num * 10 + buf[i] - '0';
            num_digits++;
        }
    }
}

