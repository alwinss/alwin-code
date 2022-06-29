#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int ch;
    //opterr = 0;
   
    while((ch = getopt(argc,argv,":a:bcdef::"))!= -1)
    {
        switch(ch)
        {
            case 'a': printf("option a:’%s’\n",optarg); break;
            case 'b': printf("option b :b\n"); break;
            case 'c': printf("option c :c\n"); break;
            case 'd': printf("option d :d\n"); break;
            case 'e': printf("option e :e\n"); break;
            case 'f': printf("option f :’%s’\n",optarg); break;
            default: printf("other option :%c\n",ch);
        }
        printf("the optind is %d\n", optind);
        printf("the next process is %s\n", argv[optind]);
        printf("optopt %c\n",optopt);
    }
    return 0;
}