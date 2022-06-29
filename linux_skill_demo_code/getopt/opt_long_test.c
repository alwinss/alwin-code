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
    OPT_SUBVERSION,
    OPT_NAME
};

int main(int argc, char **argv)
{
    opterr = 0;
    int option_index, c;
    int flag;
	char short_options[] = "ha:";
    struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"version", 0, 0, OPT_VERSION},
        {"subversion", 0, &flag, OPT_SUBVERSION},
        {"name", 0, &flag, OPT_NAME},
        {"set", required_argument, 0, 'a'}
    };

    while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
            usage(argv[0]);
            break;

            case OPT_VERSION:
            printf("the program version is 0.0.0\r\n");
            break;

            case 'a':
            printf("set the name %s\n", optarg);
            break;

            case 0:
                if (flag == OPT_SUBVERSION) {
                     printf("get the subversion %d\n", flag);
                } else if (flag == OPT_NAME){
                     printf("get the name %d\n", flag);
                }
           
            break;

            default:
            printf("Try `%s --help' for more information.\n", argv[0]);
            return -1;
            break;
        }
        printf("index is %d", option_index);
    }
}