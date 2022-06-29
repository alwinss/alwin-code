#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

enum {
	OPT_VERSION = 1
};

 static struct option long_options[] = {
		{"version", 0, 0, OPT_VERSION}
    };
int main (int argc, char ** argv)
{
    while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (c) {
        case OPT_VERSION:
            printf("the program version is 0.0.0\r\n");
        break;

        default:
            printf("Try `%s --version' for get version.\n", argv[0]);
            return -1;
        }
    }
}
