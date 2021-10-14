#include <ulib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <file.h>
#include <dir.h>
#include <unistd.h>

#define printf(...)                 fprintf(1, __VA_ARGS__)

static int safe_open(const char *path, int open_flags)
{
	int fd = open(path, open_flags);
	printf("fd is %d\n",fd);
	assert(fd >= 0);
	return fd;
}

static struct stat *safe_fstat(int fd)
{
	static struct stat __stat, *stat = &__stat;
	int ret = fstat(fd, stat);
	assert(ret == 0);
	return stat;
}


static void safe_read(int fd, void *data, size_t len)
{
	int ret = read(fd, data, len);
	assert(ret == len);
}


int main(int argc, char** argv)
{
	if(argc != 2){
		printf("[error] : wrong number of params! \n");	
	}
	int fd1 = safe_open(argv[1], O_RDONLY);
	struct stat *stat = safe_fstat(fd1);
	assert(stat->st_size >= 0 && stat->st_blocks >= 0);
	printf("file %s pass.\n", argv[1]);

	char filename[64];
	getcwd(&filename, 30);
	printf("cwd = %s\n", filename);

	close(fd1);
	printf("file closed\n");
	

	return 0;
}
