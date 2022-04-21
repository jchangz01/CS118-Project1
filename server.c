// Server side C/C++ program to demonstrate Socket programming
// Source: https://www.geeksforgeeks.org/socket-programming-cc/
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 15635 // the port users will connect to

int main(int argc, char const *argv[]) {
    int sock_fd, new_fd, valread; // listen on sock_fd, new coonection on new_fd
    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    int addrlen = sizeof(server_addr);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    // socket() - https://man7.org/linux/man-pages/man2/socket.2.html#RETURN_VALUE
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Setting socket options
    // Note: https://stackoverflow.com/questions/58599070/socket-programming-setsockopt-protocol-not-available
    // setsockopt() - https://pubs.opengroup.org/onlinepubs/000095399/functions/setsockopt.html
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 15635
    // bind() - https://www.scottklement.com/rpg/socktut/bindapi.html
    //          https://man7.org/linux/man-pages/man2/bind.2.html
    if (bind(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections 
    // listen() - https://man7.org/linux/man-pages/man2/listen.2.html
    if (listen(sock_fd, 3) == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Listening on port %i \n", PORT);

    // Accept connections until session terminated
    // accept() - https://man7.org/linux/man-pages/man2/accept.2.html
    // TODO
    while ((new_fd = accept(sock_fd, (struct sockaddr *) &client_addr, (socklen_t * ) & addrlen)) != -1) {
        //read message from client
        valread = read(new_fd, buffer, 1024);
        printf("%s\n", buffer);
        send(new_fd, hello, strlen(hello), 0);
        printf("Hello message sent\n");
    }
    close(sock_fd);
    close(new_fd);
    return 0;
}