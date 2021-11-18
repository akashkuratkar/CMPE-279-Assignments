// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    pid_t split;

    if (argc == 2)
    {
        //Getting nobody user information
        struct passwd *pw = getpwnam("nobody");

        if (pw == NULL)
        {
            fprintf(stderr, "Get of user information failed.\n");
            exit(1);
        }
        int new_id = pw->pw_uid;
        if (setuid(new_id) < 0)
        {
            printf("Error dropping privilege\n");
            return 0;
        }
        else
        {
            new_socket = atoi(argv[1]);
            valread = read(new_socket, buffer, 1024);
            printf("Read %d bytes: %s\n", valread, buffer);
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello message sent\n");
        }
    }
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
    //&opt, sizeof(opt)))
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    split = fork();

    if (split == 0)
    {
        char new_socket_str[50];
        sprintf(new_socket_str, "%d", new_socket);
        char filename[20];
        strcpy(filename, argv[20]);
        char *new_argv[] = {filename, new_socket_str, NULL};
        execvp(argv[0], new_argv);
    }
    else if (split > 0)
    {
        wait(NULL);
        printf("Exiting after returning from child\n");
        return 0;
    }
    else
    {
        perror("Error occured while forking the process");
        _exit(2);
    }

    return 0;
}
