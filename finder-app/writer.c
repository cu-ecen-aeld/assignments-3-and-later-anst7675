#include <stdio.h>
#include <syslog.h>
#include <errno.h>



int main(int argc, char* argv[])
{
    openlog(NULL, 0 ,LOG_USER);
    if (argc < 3) {
        syslog(LOG_ERR, "Invalid number of arguments %d.", argc);
        return 1;
    }

    syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);


    const char *filename = argv[1];
    FILE *file = fopen(filename,"w");
    if(file == NULL) {
        fprintf(stderr, "Value of errno atempting to open file %s: %d\n", filename, errno);
        perror("perror returned");
        //fprintf(stderr, "Error opening file %s: %s\n", filename, strerror( errno ));
    }
    else {
        fprintf(file, "%s\n", argv[2]);
        fclose(file);
        return 0;
    }

}