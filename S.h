#ifndef S_H
#define S_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 4096

time_t connectionTime;

#define MAX_USERS 4

typedef struct {
    const char* username;
    const char* password;
} User;

extern User validUsers[MAX_USERS];

int authenticateUser(const char* username, const char* password) {
    for (int i = 0; i < MAX_USERS; ++i) {
        if (strcmp(username, validUsers[i].username) == 0 && strcmp(password, validUsers[i].password) == 0) {
            return 1;  // Authentication success
        }
    }
    return 0;  // Authentication failure
}

void sendElapsedTimeInterval(int clientSocket) {
    time_t currentTime;
    time(&currentTime);

    double elapsedTime = difftime(currentTime, connectionTime);

    char elapsedTimeStr[MAX_BUFFER_SIZE];
    snprintf(elapsedTimeStr, sizeof(elapsedTimeStr), "Elapsed time since connection: %.2f seconds\n", elapsedTime);

    send(clientSocket, elapsedTimeStr, strlen(elapsedTimeStr), 0);
}

void sendFileContent(int clientSocket, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
    	send(clientSocket, "File does not exist", 20, 0);
        perror("Error opening file");
        return;
    }

    char buffer[MAX_BUFFER_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(clientSocket, buffer, bytesRead, 0);
    }

    fclose(file);
}

void handleClientRequest(int clientSocket, int choice) {
    const char *message;
    switch (choice) {
        case 1: {
            // Get Date and Time
            time_t currentTime;
            time(&currentTime);

            char timeStr[MAX_BUFFER_SIZE];
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));

            send(clientSocket, timeStr, strlen(timeStr), 0);
            break;
        }

        case 2: {
            // List Files in Server Directory
            DIR *directory = opendir(".");
            if (directory == NULL) {
                perror("Error opening directory");
                break;
            }

            struct dirent *entry;
            while ((entry = readdir(directory)) != NULL) {
                send(clientSocket, entry->d_name, strlen(entry->d_name), 0);
                send(clientSocket, "\n", 1, 0);
            }
	    send(clientSocket, "###", 3, 0);
            closedir(directory);
            break;
        }

        case 3: {
            // Get Content of a File
            char filename[MAX_BUFFER_SIZE];
            recv(clientSocket, filename, sizeof(filename), 0);

            sendFileContent(clientSocket, filename);
            send(clientSocket, "###", 3, 0);
            break;
        }

        case 4: {
            // Get Elapsed Time Since Connection
            // You may need to keep track of the connection time in the server
            // For simplicity, we'll just send a message for now
            sendElapsedTimeInterval(clientSocket);
            break;
        }

        case 5: {
            // Exit
            message = "Goodbye!";
            send(clientSocket, message, strlen(message), 0);
            break;
        }

        default:
            // Invalid choice
            message = "Invalid choice. Please try again.";
            send(clientSocket, message, strlen(message), 0);
    }
}

#endif
