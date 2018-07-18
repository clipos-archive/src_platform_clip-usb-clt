// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2007-2018 ANSSI. All Rights Reserved.

/**
 * usbclt.c
 *
 * @brief usbclt connects to /var/run/usbxxx and sends a command request to a usbadmin server. Command can be to mount, unmount or initialize a usb key.
 * @see usbadmin
 *
 **/


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

static int usage(const char *s)
{
	fprintf(stderr, "Usage: %s action\n\
where action can have the following values :\n\
\tumount\tumounts a USB token\n\
\tinit\tinitialize a USB token\n\
\tinit-cleartext\tinitialize a USB token without encryption\n\
\tgenerate\tgenerate new RSA keys\n\
\texport\texport RSA keys to USB\n\
\timport\timport RSA keys from USB\n\
\tclear\tclear out a USB token\n\
\tcdmount\tmount a CD-ROM\n\
\tcdumount\tunmount a CD-ROM\n", s);
	return -1;
}

int main(int argc, char *argv[])
{
	int s;
	char c;
	struct sockaddr_un sau;
	char command = 0;
	const char *exe;

	if (argv[0]) {
		exe = basename(argv[0]);
	} else {
		exe = "usbclt";
	}

	if (argc != 2)
		exit(usage(exe));

	if (!strcmp (argv[1], "init-cleartext"))
		command = 'i';
	else if (!strcmp (argv[1], "mount"))
		command = 'M';
	else if (!strcmp (argv[1], "umount"))
		command = 'U';
	else if (!strcmp (argv[1], "init"))
		command = 'I';
	else if (!strcmp (argv[1], "generate"))
		command = 'G';
	else if (!strcmp (argv[1], "export"))
		command = 'E';
	else if (!strcmp (argv[1], "import"))
		command = 'D';
	else if (!strcmp (argv[1], "clear"))
		command = 'e';
	else if (!strcmp (argv[1], "cdmount"))
		command = 'C';
	else if (!strcmp (argv[1], "cdumount"))
		command = 'c';
	else
		exit(usage(argv[0]));

	if (sizeof(sau.sun_path) < sizeof(USB_SOCK_PATH)) {
		return EXIT_FAILURE;
	}
	sau.sun_family = AF_UNIX;
	snprintf(sau.sun_path, sizeof(sau.sun_path), "%s", USB_SOCK_PATH);

	s = socket(PF_UNIX, SOCK_STREAM, 0);
	if (s < 0) {
		perror("socket");
		return -1;
	}
	
	if (connect(s, (struct sockaddr *)&sau, 
			sizeof(struct sockaddr_un)) < 0) {
		perror("connect");
		close(s);
		return -1;
	}

	if (write(s,&command,1) != 1) {
		perror("write");
		close(s);
		return -1;
	}

	if (read(s, &c, 1) != 1) {
		perror("read");
		close(s);
		return -1;
	}

	close(s);
	if (c == 'Y') {
		printf("Action successfully completed\n");
		return EXIT_SUCCESS;
	} else {
		printf("Error during the execution\n");
		return EXIT_FAILURE;
	}
}
