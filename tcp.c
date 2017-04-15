/* Created by Shengkai Zhang on 10/4/2017 */

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>


#define BUFFER_SIZE 1024

int main(int argc, char const *argv[])
{
	char buffer[BUFFER_SIZE];
	struct stat buf;
	int src_fd, tar_fd;

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
		/* Copy the content */
		while(1)
		{
			int rd = read(src_fd, buffer, BUFFER_SIZE);
			if (rd == -1)
			{
				fprintf(stderr, "Error while reading %s: %s\n", argv[1], strerror(errno));
				exit(EXIT_FAILURE);
			} else if (rd == 0)
			{
				printf("Copy %s success\n", argv[1]);
				break;
			} else if (rd > 0)
			{
				int wr = write(tar_fd, buffer, rd);
				if (wr != rd)
				{
					fprintf(stderr, "Error while writing %s: %s\n", argv[2], strerror(errno));
					exit(EXIT_FAILURE);
				}
			} else
			{
				printf("Unkonw error\n");
				exit(EXIT_FAILURE);
			}
		}
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
		/* Copy the content */
		while(1)
		{
			int rd = read(src_fd, buffer, BUFFER_SIZE);
			if (rd == -1)
			{
				fprintf(stderr, "Error while reading %s: %s\n", argv[1], strerror(errno));
				exit(EXIT_FAILURE);
			} else if (rd == 0)
			{
				printf("Copy %s success\n", argv[1]);
				break;
			} else if (rd > 0)
			{
				int wr = write(tar_fd, buffer, rd);
				if (wr != rd)
				{
					fprintf(stderr, "Error while writing %s: %s\n", argv[2], strerror(errno));
					exit(EXIT_FAILURE);
				}
			} else
			{
				printf("Unkonw error\n");
				return -1;
			}
		}
	}
	close(src_fd);
	close(tar_fd);
	return 0;
}