#ifndef C_H
#define C_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

void displayMenu() {
    printf("\n==== Menu ====\n");
    printf("1. Get Server Date and Time\n");
    printf("2. List Files in Server Directory\n");
    printf("3. Get Content of a File\n");
    printf("4. Get Elapsed Time Since Connection\n");
    printf("5. Exit\n");
    printf("==============\n");
    printf("Enter your choice: ");
}

void receiveAndPrint(int sockfd) {
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // Receive data from the server
    recv(sockfd, buffer, sizeof(buffer), 0);

    // Print the received data
    printf("Server response: %s\n", buffer);
    }
    
void receiveAndPrintLarge(int sockfd) {
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';  // Null-terminate the received data
        printf("%s", buffer);

        // Check if the received data contains the end marker "###"
        if (strstr(buffer, "###") != NULL) {
            break;  // Exit the loop if the end marker is found
        }
    }

    if (bytesRead == -1) {
        perror("Error receiving data");
    }
}

#endif
