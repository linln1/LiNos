#include <ulib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <file.h>
#include <dir.h>
#include <unistd.h>
#include <stat.h>

#define printf(...)                 fprintf(1, __VA_ARGS__)




int main(void)
{
	char s[256], *sp = &s;
	char news[256], *newsp = &news;

	getcwd(sp, 256);
	printf("dir name : %s\n", sp);

	int ret = chcwd("test_dir");
	printf("ret = %d\n", ret);

	getcwd(newsp, 256);
	printf("new dir name : %s\n", newsp);

	return 0;
}
