#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "setuid.h"

#define BUFFER_SIZE 1024

char command[BUFFER_SIZE];

int main(int argc, char *argv[]) {
	if (!(initsetuid()))
		exit(1);

	// Check what command is asked
	if (argc == 1) {
		fprintf (stderr, "Missing smbctrl command!\n");
		return 1;

	} else if (strcmp(argv[1], "smbuserdisable") == 0) {
		if (!is_valid_argument_alnum(argv[2])) {
			fprintf(stderr, "Invalid username '%s'\n", argv[2]);
			exit(2);
		}

		snprintf(command, BUFFER_SIZE-1, "/usr/bin/smbpasswd -d %s >/dev/null", argv[2]);
		safe_system(command);

	} else if (strcmp(argv[1], "smbuserenable") == 0) {
		if (!is_valid_argument_alnum(argv[2])) {
			fprintf(stderr, "Invalid username '%s'\n", argv[2]);
			exit(2);
		}

		snprintf(command, BUFFER_SIZE-1, "/usr/bin/smbpasswd -e %s >/dev/null", argv[2]);
		safe_system(command);

	} else if (strcmp(argv[1], "smbuserdelete") == 0) {
		if (!is_valid_argument_alnum(argv[2])) {
			fprintf(stderr, "Invalid username '%s'\n", argv[2]);
			exit(2);
		}

		snprintf(command, BUFFER_SIZE-1, "/usr/bin/smbpasswd -x %s >/dev/null", argv[2]);
		safe_system(command);

		snprintf(command, BUFFER_SIZE-1, "/usr/sbin/userdel %s >/dev/null", argv[2]);
		safe_system(command);

	} else if (strcmp(argv[1], "smbsafeconf") == 0) {
		safe_system("/bin/cat /var/ipfire/samba/global /var/ipfire/samba/shares > /var/ipfire/samba/smb.conf");

	} else if (strcmp(argv[1], "smbstop") == 0) {
		safe_system("/etc/rc.d/init.d/samba stop >/dev/null");
		safe_system("/usr/local/bin/sambactrl disable");

	} else if (strcmp(argv[1], "smbstart") == 0) {
		safe_system("/etc/rc.d/init.d/samba start >/dev/null");
		safe_system("/usr/local/bin/sambactrl enable");

	} else if (strcmp(argv[1], "smbrestart") == 0) {
		safe_system("/etc/rc.d/init.d/samba restart >/dev/null");

	} else if (strcmp(argv[1], "smbreload") == 0) {
		safe_system("/etc/rc.d/init.d/samba reload >/dev/null");

	} else if (strcmp(argv[1], "smbstatus") == 0) {
		snprintf(command, BUFFER_SIZE-1, "/usr/bin/smbstatus 2>/dev/null");
		safe_system(command);

	} else if (strcmp(argv[1], "smbuseradd") == 0) {
		if (!is_valid_argument_alnum(argv[2])) {
			fprintf(stderr, "Invalid username '%s'\n", argv[2]);
			exit(2);
		}

		snprintf(command, BUFFER_SIZE-1, "/usr/sbin/groupadd sambauser >/dev/null");
		safe_system(command);

		snprintf(command, BUFFER_SIZE-1, "/usr/sbin/useradd -c 'Samba User' -m -g sambauser -s /bin/false %s >/dev/null", argv[2]);
		safe_system(command);

		run("/usr/sbin/samba-change-password", argv + 1);

	} else if (strcmp(argv[1], "smbchangepw") == 0) {
		if (!is_valid_argument_alnum(argv[2])) {
			fprintf(stderr, "Invalid username '%s'\n", argv[2]);
			exit(2);
		}

		run("/usr/sbin/samba-change-password", argv + 1);

	} else if (strcmp(argv[1], "readsmbpasswd") == 0) {
		safe_system("/bin/chown root:nobody /var/ipfire/samba/private >/dev/null");
		safe_system("/bin/chown root:nobody /var/ipfire/samba/private/smbpasswd >/dev/null");
		safe_system("/bin/chmod 640 /var/ipfire/samba/private/smbpasswd >/dev/null");
		safe_system("/bin/chmod 650 /var/ipfire/samba/private >/dev/null");

	} else if (strcmp(argv[1], "locksmbpasswd") == 0) {
		safe_system("/bin/chown root:root /var/ipfire/samba/private >/dev/null");
		safe_system("/bin/chown root:root /var/ipfire/samba/private/smbpasswd >/dev/null");
		safe_system("/bin/chmod 600 /var/ipfire/samba/private/smbpasswd >/dev/null");
		safe_system("/bin/chmod 600 /var/ipfire/samba/private >/dev/null");

	} else if (strcmp(argv[1], "enable") == 0) {
		safe_system("touch /var/ipfire/samba/enable");
		safe_system("ln -snf /etc/rc.d/init.d/samba /etc/rc.d/rc3.d/S45samba");
		safe_system("ln -snf /etc/rc.d/init.d/samba /etc/rc.d/rc0.d/K48samba");
		safe_system("ln -snf /etc/rc.d/init.d/samba /etc/rc.d/rc6.d/K48samba");

	} else if (strcmp(argv[1], "disable") == 0) {
		safe_system("unlink /var/ipfire/samba/enable");
		safe_system("rm -rf /etc/rc.d/rc*.d/*samba");

	} else if (strcmp(argv[1], "join") == 0) {
		if (argc == 4) {
			snprintf(command, BUFFER_SIZE - 1, "/usr/bin/net join -U \"%s%%%s\"",
				argv[2], argv[3]);
			return safe_system(command);
		} else {
			fprintf(stderr, "Wrong number of arguments. Need username and password.\n");
			return 1;
		}
	}

	return 0;
}
