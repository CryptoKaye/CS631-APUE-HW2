/* Created by Shengkai Zhang on 12/4/2017 */

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

int main(int argc, char const *argv[])
{
	int src_fd, tar_fd;
	void *src, *tar;
	struct stat statbuf;
	struct stat buf;

	/* Open the source file */
	if ((src_fd = open(argv[1], O_RDONLY)) == -1)
	{
		fprintf(stderr, "Error while opening %s: %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Check if target is a directory */
	if (lstat(argv[2], &buf) < 0)
	{
		fprintf(stderr, "lstat Error");
		exit(EXIT_FAILURE);
	}

	// if target is a directory
	if (S_ISDIR(buf.st_mode))
	{
		/* Create a file */
		tar_fd = openat(dirfd(opendir(argv[2])), argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		if (tar_fd == -1)
		{
			fprintf(stderr, "Error while creating %s: %s\n", argv[1], strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (fstat(src_fd, &statbuf) < 0)
		{
			fprintf(stderr, "fstat error");
			exit(EXIT_FAILURE);
		}

		/* Set the size of output file */
		if (lseek(tar_fd, statbuf.st_size-1, SEEK_SET) == -1)
		{
			fprintf(stderr, "lseek error");
			exit(EXIT_FAILURE);
		}

		if (write(tar_fd, " ", 1) != 1)
		{
			fprintf(stderr, "write error");
			exit(EXIT_FAILURE);
		}

		/* mmap */
		src = mmap(0, statbuf.st_size, PROT_READ , MAP_SHARED, src_fd, 0);
		if (src == MAP_FAILED)
		{
			fprintf(stderr, "mmap error for input");
			exit(EXIT_FAILURE);
		}

		tar = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, tar_fd, 0);
		if (tar == MAP_FAILED)
		{
			fprintf(stderr, "mmap error for output");
			exit(EXIT_FAILURE);
		}

		memcpy(tar, src, statbuf.st_size);
		printf("Copy %s success\n", argv[1]);
	}
	/* if target is a regular file */
	else if (S_ISREG(buf.st_mode))
	{
		/* Append */
		tar_fd = open(argv[2], O_RDWR | O_APPEND);
		if (tar_fd == -1)
		{
			fprintf(stderr, "Error while opening %s: %s\n", argv[2], strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (fstat(src_fd, &statbuf) < 0)
		{
			fprintf(stderr, "fstat error");
			exit(EXIT_FAILURE);
		}

		/* Set the size of output file */
		if (lseek(tar_fd, statbuf.st_size-1, SEEK_SET) == -1)
		{
			fprintf(stderr, "lseek error");
			exit(EXIT_FAILURE);
		}

		if (write(tar_fd, " ", 1) != 1)
		{
			fprintf(stderr, "write error");
			exit(EXIT_FAILURE);
		}

		/* mmap */
		src = mmap(0, statbuf.st_size, PROT_READ , MAP_SHARED, src_fd, 0);
		if (src == MAP_FAILED)
		{
			fprintf(stderr, "mmap error for input");
			exit(EXIT_FAILURE);
		}

		tar = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, tar_fd, 10);
		if (tar == MAP_FAILED)
		{
			fprintf(stderr, "mmap error for output");
			exit(EXIT_FAILURE);
		}

		memcpy(tar, src, statbuf.st_size);
		printf("Copy %s success\n", argv[1]);
	}

	close(src_fd);
	close(tar_fd);
	return 0;
}