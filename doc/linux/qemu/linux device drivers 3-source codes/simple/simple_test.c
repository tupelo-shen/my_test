#include <fcntl.h>
#include <sys/mman.h>
#include <stddef.h>

/*
 * 说明：
 * 		1. 首先打开设备文件/dev/simple，打开方式为只读（O_RDONLY）
 */
int main(int argc, char *argv[])
{
	int            fdin, fdout;
	void           *src, *dst;
	// struct stat    statbuf;
	unsigned char  sz[1024] = {0};

	if ((fdin = open("/dev/simple", O_RDONLY)) < 0)
		printf("can't open /dev/simple for reading");

	if ((src = mmap(NULL, 4096*2, PROT_READ, MAP_SHARED,fdin, 0)) == MAP_FAILED)
		printf("mmap error for simplen");

	memcpy(sz, src, 11);
	sz[10]='\0';
	printf("%x\n", src);
	printf("%s\n\n", sz);

	memcpy(sz, src+4096, 11);
	printf("%x\n", src+4096);
	printf("%s\n", sz);

	exit(0);
}
