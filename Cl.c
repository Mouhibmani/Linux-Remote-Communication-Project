#include "C.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *serverIP = argv[1];
    int serverPort = atoi(argv[2]);

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP, &serverAddr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Get user credentials
    char username[MAX_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    // Send credentials to server for authentication
    send(sockfd, username, strlen(username) + 1, 0);
    sleep(1);
    send(sockfd, password, strlen(password) + 1, 0);

    // Receive authentication result
    int authSuccess;
    recv(sockfd, &authSuccess, sizeof(authSuccess), 0);

    // Check authentication result
    if (!authSuccess) {
        printf("Authentication failed. Exiting...\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Client interface
    int choice;
    do {
        displayMenu();
        scanf("%d", &choice);

        // Send choice to server
        send(sockfd, &choice, sizeof(choice), 0);
	
	printf("\x1b[1m");
        // Handle user choice
        switch (choice) {
            case 1:
            case 4:
            case 5:
                // For cases where the server sends a simple response
                receiveAndPrint(sockfd);
                break;
            case 2: receiveAndPrintLarge(sockfd);
                break;

            case 3: {
                // Get Content of a File
                char filename[MAX_BUFFER_SIZE];
                printf("Enter filename from the current directory: ");
                scanf("%s", filename);

                // Send filename to the server
                send(sockfd, filename, strlen(filename) + 1, 0);

                // Receive and print file content from the server
                receiveAndPrintLarge(sockfd);
                break;
            }

            default:
                printf("Invalid choice. Please try again.\n");
        }
        printf("\x1b[0m\n"); 

    } while (choice != 5);

    // Close the socket
    close(sockfd);

    return 0;
}

