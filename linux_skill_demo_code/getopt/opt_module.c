#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

static void usage(char *command)
{
    printf(
"Usage: %s [OPTION]... [argument]...\n"
"\n"
"-h, --help              help\n"
"    --version           print current version\n"
 
		, command);
}

enum {
	OPT_VERSION = 1,
};

int main(int argc, char **argv)
{
    opterr = 0;
    int option_index, c;
    int flag;
	char short_options[] = "h";
    struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"version", 0, 0, OPT_VERSION}
    };

    while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
            usage(argv[0]);
            break;

            case OPT_VERSION:
            printf("the program version is 0.0.0\r\n");
            break;

            default:
            printf("Try `%s --help' for more information.\n", argv[0]);
            return -1;
        }
    }
}