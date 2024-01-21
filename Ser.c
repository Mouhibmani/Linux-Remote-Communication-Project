#include "S.h"

User validUsers[MAX_USERS] = {
    {"admin", "admin"},
    {"haithem", "isi"},
    {"mouhib", "helloworld"},
    {"aziz", "123"}
};

void *clientHandler(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int serverPort = atoi(argv[1]);

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", serverPort);

    while (1) {
        // Accept a connection
        int clientSocket = accept(sockfd, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocket == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        time(&connectionTime);

        printf("Connection accepted from %s\n", inet_ntoa(clientAddr.sin_addr));

        // Create a thread to handle the client
        pthread_t thread;
        if (pthread_create(&thread, NULL, clientHandler, (void *)&clientSocket) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }

        // Detach the thread
        pthread_detach(thread);
    }

    // Close the server socket
    close(sockfd);

    return 0;
}

void *clientHandler(void *arg) {
    int clientSocket = *((int *)arg);

    // Receive client's authentication
    int authenticated = 0;
	
    char username[MAX_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];
    recv(clientSocket, username, sizeof(username), 0);
    recv(clientSocket, password, sizeof(password), 0);

    printf("%s has authenticated\n", username);
    
    authenticated = authenticateUser(username, password);

    // Send authentication result to the client
    send(clientSocket, &authenticated, sizeof(authenticated), 0);

    if (!authenticated) {
    	printf("Authentication failed. Client dismissed.\n");
    	close(clientSocket);
    	pthread_exit(NULL);
    }
    // Receive client's choice
    int choice = 0;
    
    while(1){
    	recv(clientSocket, &choice, sizeof(choice), 0);
    	
    	printf("%s choice is %d\n", username, choice);
    	
    	// Handle client request
    	handleClientRequest(clientSocket, choice);
    	
    	if (choice == 5) {
    	    printf("%s has left\n", username);
            break;
        }
    }

    // Close the client socket
    close(clientSocket);

    // Exit the thread
    pthread_exit(NULL);
}

