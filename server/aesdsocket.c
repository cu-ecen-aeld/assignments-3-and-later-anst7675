#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <syslog.h>

#define PORT 9000  // the port users will be connecting to
#define OUTPUT_FILE "/var/tmp/aesdsocketdata"
#define BUFFER_SIZE 1024

void sigchld_handler(int s)
{
    if (s == SIGINT || s == SIGTERM)
    { 
        syslog(LOG_INFO, "Caught signal, exiting\n");
        remove(OUTPUT_FILE);
        shutdown(s, SHUT_RDWR);
        closelog();
        exit(1);
    }
}


int main(int argc, char *argv[])
{
    signal(SIGINT, sigchld_handler);
    signal(SIGTERM, sigchld_handler);
    
    openlog(NULL, 0 ,LOG_USER);

    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        exit(-1);
    }

    int socket_opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &socket_opt, sizeof(socket_opt))) 
    {
        exit(-1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        exit(-1);
    }
 
    int opt;
    while((opt = getopt(argc, argv, ":d")) != -1) 
    { 
        switch(opt) 
        { 
            case 'd': 
                daemon(0,0);
                break;
            case '?': 
                printf("unknown option: %c\n", optopt);
                break; 
        } 
    }

    if (listen(server_fd, 3) < 0) 
    {
        exit(-1);
    }

    while(1)
    {

        struct sockaddr_in client_addr;
        
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        
        if (client_socket == -1)
        {
            exit(-1);
        }

        syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(client_addr.sin_addr));

        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);

        int recv_bytes = 0;

        while ((recv_bytes = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)
        {
            FILE *file = fopen(OUTPUT_FILE, "a");
            if (file == NULL)
            {
                perror("Error opening file to write");
                close(client_socket);
                continue;
            }
            fwrite(buffer, sizeof(char), recv_bytes, file);
            fclose(file);

            char *newline = strchr(buffer, '\n');
            if (newline != NULL)
            {
                FILE *file = fopen(OUTPUT_FILE, "r");
                fseek(file, 0, SEEK_END);
                long file_size = ftell(file);
                rewind(file);
                char *file_contents = malloc(sizeof(char) * (file_size + 1));
                long bytes_read = fread(file_contents, sizeof(char), file_size, file);
                if (bytes_read != file_size)
                {
                    fclose(file);
                    free(file_contents);
                    exit(1);
                }

                file_contents[file_size] = '\0';
                fclose(file);
                send(client_socket, file_contents, strlen(file_contents), 0);
                free(file_contents);
            }

            memset(buffer, 0, BUFFER_SIZE);
        }

        syslog(LOG_INFO, "Closed connection from %s", inet_ntoa(client_addr.sin_addr));
        close(client_socket);
    }

    close(server_fd);
    remove(OUTPUT_FILE);
    closelog();

    return 0;

}