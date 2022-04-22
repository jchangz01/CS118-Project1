// Server side C/C++ program to demonstrate Socket programming
// Source: https://www.geeksforgeeks.org/socket-programming-cc/
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 15635 // the port users will connect to

char* getFileExtension (char* filename) {
    char* extension = NULL;
    int len = strlen(filename);
    int i = len - 1;
    // traverse from r to l to find first .
    // substr right of . is our extension
    while (i >= 0 && filename[i] != '.' ) i--;
    
    // if a dot exists, extension returned
    // otherwise, null returned
    if (i != -1) {
        i++;
        extension = malloc(len);
        strncpy(extension, filename+i, len - i);
    }
    return extension;
}

void sendResponse(char *filename) {
    // get file extension from filename
    char* extension = getFileExtension(filename);

    
}

/* Parses a request message for the filename */
char* parseForFileName (char* request) {
    int starti, endi;
    int i = 0;
    // skip request type word 
    while (request[i] != ' ') i++;
    i++; 
    // skip initial / of pathname
    // set start i to start of pathname
    if (request[i] == '/') i++;
    starti = i;

    // pathname after request type (seperated by a space)
    // set end i to end of pathname
    while (request[i] != ' ') i++;
    endi = i;

    // allocate mem for size of path name
    // +1 for zero bit
    int filename_size = endi - starti;
    char* filename_rough = malloc(filename_size + 1);

    // set pathname var to appropriate path name
    strncpy(filename_rough, request+starti, filename_size);
    filename_rough[filename_size] = '\0';

    // replace all "%20" substr with " "
    // count number of %20 occurences 
    int p20occurences = 0;
    /*for (i = 0; filename_rough < filename_size - 2; i++) {
        printf()
    }*/
    return filename_rough;
}

int main(int argc, char const *argv[]) {
    int sock_fd, new_fd; // listen on sock_fd, new coonection on new_fd
    struct sockaddr_in server_addr, client_addr;
    int addrlen = sizeof(server_addr);
    int opt = 1;

    // Creating socket file descriptor
    // socket() - https://man7.org/linux/man-pages/man2/socket.2.html#RETURN_VALUE
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Setting socket options
    // Note: https://stackoverflow.com/questions/58599070/socket-programming-setsockopt-protocol-not-available
    // setsockopt() - https://man7.org/linux/man-pages/man3/setsockopt.3p.html
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
    int valread; // store bytes read from client
    char buffer[4096] = {0};
    char *hello = "Hello from server";

    while ((new_fd = accept(sock_fd, (struct sockaddr *) &client_addr, (socklen_t * ) & addrlen)) != -1) {
        // read request message from client
        // read() - https://man7.org/linux/man-pages/man2/read.2.html
        valread = read(new_fd, buffer, 1024);
        if (valread == -1) { // error handling for read
            close(sock_fd);
            close(new_fd);
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        else if (valread > 0) {// bytes were read
            // print request message from client
            printf("%s\n", buffer);

            // parse request message for pathname
            // for this project, it would be a single filename
            char* filename = parseForFileName(buffer);
            printf("%s\n", filename);

            printf("%s\n", getFileExtension(filename));
            // send response to client

            //send(new_fd, hello, strlen(hello), 0);
            //printf("Hello message sent\n");
        }
    }
    close(sock_fd);
    close(new_fd);
    return 0;
}