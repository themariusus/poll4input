#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define errExit(msg) { perror(msg); exit(EXIT_FAILURE); }

int
main(int argc, char *argv[])
{
	int				ready;
	nfds_t			num_open_fds, nfds;
	struct pollfd	*pfds;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s file...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	num_open_fds = nfds = argc - 1;
	pfds = calloc(nfds, sizeof(struct pollfd));
	if (pfds == NULL) {
		errExit("malloc");
	}

	/* Open each file on command line, and add it to 'pfds' array. */

	for (nfds_t j = 0; j < nfds; j++) {
		pfds[j].fd = open(argv[j + 1], O_RDONLY);
		if (pfds[j].fd == -1) {
			--num_open_fds;
			continue;
		}

		printf("Opened \"%s\" on fd %d\n", argv[j + 1], pfds[j].fd);

		pfds[j].events = POLLIN;
	}

	/* Keep calling poll() as long as at least one file descriptor is
	  open. */

	while (num_open_fds > 0) {
		printf("About to poll()\n");
		ready = poll(pfds, nfds, -1);
		if (ready == -1) {
			errExit("poll");
		}

		printf("Ready: %d\n", ready);

		/* Deal with array returned by poll(). */

		for (nfds_t j = 0; j < nfds; j++) {
			if (pfds[j].revents != 0) {
				printf("  fd=%d; events: %s%s%s\n", pfds[j].fd,
					  (pfds[j].revents & POLLIN)  ? "POLLIN "  : "",
					  (pfds[j].revents & POLLHUP) ? "POLLHUP " : "",
					  (pfds[j].revents & POLLERR) ? "POLLERR " : "");

				if (pfds[j].revents & POLLIN) {
					exit(EXIT_SUCCESS);
				} else {	/* POLLERR | POLLHUP */
					printf("	closing fd %d\n", pfds[j].fd);
					if (close(pfds[j].fd) == -1) {
						errExit("close");
					}
					num_open_fds--;
				}
			}
		}
	}

	printf("All file descriptors closed; bye\n");
	exit(EXIT_SUCCESS);
}
