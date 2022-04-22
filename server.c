// Server side C/C++ program to demonstrate Socket programming
// Source: https://www.geeksforgeeks.org/socket-programming-cc/
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 15635 // the port users will connect to

int getDigitsCount (int number) {
    int digits = 0;
    while (number > 0) {
        digits++;
        number /= 10;
    }
    return digits;
}

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

void sendResponse(char *filename, struct stat stats, int new_fd) {
    // open file to check if it exists
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) { // file does not exist
        send(new_fd, "HTTP/1.1 404 Not Found\n", 14, 0);
        send(new_fd, "Content-Length: 13\n", 19, 0);
        send(new_fd, "Content-Type: text/html\n\n", 25, 0);
        send(new_fd, "<h1>404 Not Found</h1>", 20, 0);
        close(new_fd);
        return;
    }

    // SEND http version and status code
    write(new_fd, "HTTP/1.1 200 OK\n", 16); 

    // acquire content-length
    stat(filename, &stats); //for timer
    int size = stats.st_size;
    int len = getDigitsCount(size) + 17; //size of message = size bytes + 17 for characters
    char* content_length = malloc(len);
    sprintf(content_length, "Content-Length: %d\n", size);

    // SEND content-length
    write(new_fd, content_length, size);

    // get file extension from filename
    char* extension = getFileExtension(filename);
    
    if (extension != NULL) printf("%s\n", extension);
    else printf("No extension\n");
    
    // SEND content-type based on extension
    if (extension != NULL && strcasecmp(extension, "html") == 0) 
        send(new_fd, "Content-Type: text/html\r\n\r\n", 27, 0);
    else if (extension != NULL && strcasecmp(extension, "txt") == 0) 
        send(new_fd, "Content-Type: text/plain\r\n\r\n", 28, 0);
    else if (extension != NULL && strcasecmp(extension, "jpg") == 0) 
        send(new_fd, "Content-Type: image/jpeg\r\n\r\n", 28, 0);
    else if (extension != NULL && strcasecmp(extension, "png") == 0) 
        send(new_fd, "Content-Type: image/png\r\n\r\n", 27, 0);
    else if (extension != NULL && strcasecmp(extension, "gif") == 0) 
        send(new_fd, "Content-Type: image/gif\r\n\r\n", 27, 0);
    else 
        send(new_fd, "Content-Type: application/octet-stream\r\n\r\n", 42, 0);

    // SEND the requested file 
    int bytesread;
    char file_buffer[8192] = {0};
    int fd = fileno(fp);

    printf("test\n");

    while ((bytesread = read(fd, file_buffer, 8192)) != 0) {
        if (bytesread > 0) {
            if (send(new_fd, file_buffer, bytesread, 0) == -1) {
                perror("write error");
                exit(EXIT_FAILURE);
            }
        }	
        else {
            perror("read error");   
            exit(EXIT_FAILURE);
        }
    }
}

/* Parses a request message for the filename */
char* parseForFileName (char* request) {
    int starti, endi;
    int i = 0;
    // skip request type word 
    while (request[i] != ' ') i++;
    i++; 
    // skip initial / of pathname
    // set start i to start of filename
    if (request[i] == '/') i++;
    starti = i;

    // pathname after request type (seperated by a space)
    // set end i to end of filename
    while (request[i] != ' ') i++;
    endi = i;

    // allocate mem for size of file name
    // +1 for zero bit
    int filename_size = endi - starti;
    char* filename_rough = malloc(filename_size);

    // set filename var to appropriate file name
    strncpy(filename_rough, request+starti, filename_size);

    // replace all "%20" substr with " "
    // replace all "%25" substr with "%"
    int p20occurences = 0;
    char* filename = malloc(filename_size);

    int j = 0, k = 0;
    while (filename_rough[j] != '\0') {
        if (filename_rough[j] == '%' && j <= filename_size - 3){ //char is '%' and enough room to replace "%20" with ' '
            filename[k] = ' ';
            j += 2; //iterate past "%2"
        } else {
            filename[k] = filename_rough[j];
        }
        j++;
        k++;
    }
    return filename;
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
    int valread; // store bytes read from client
    char buffer[8192] = {0};
    char *hello = "Hello from server";
    struct stat stats;

    while ((new_fd = accept(sock_fd, (struct sockaddr *) &client_addr, (socklen_t * ) & addrlen)) != -1) {
        // read request message from client
        // read() - https://man7.org/linux/man-pages/man2/read.2.html
        valread = read(new_fd, buffer, 8192);
        if (valread == -1) { // error handling for read
            close(sock_fd);
            close(new_fd);
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        else if (valread > 0) { // bytes were read
            // print request from client
            printf("%s\n", buffer);

            // parse request message for pathname
            // for this project, it would be a single filename
            char* filename = parseForFileName(buffer);
            printf("%s\n", filename);

            // send response to client
            sendResponse(filename, stats, new_fd);

            // close client socket
            // close () - https://man7.org/linux/man-pages/man2/close.2.html
            close(new_fd);
        }
    }
    close(sock_fd);
    close(new_fd);
    return 0;
}