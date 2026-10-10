
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>

SOCKET client_fd;

// Function to receive messages from the client
unsigned __stdcall receive_messages(void *arg) {
    char buffer[1024];
    int bytes_read;

    // Keep receiving messages until the client disconnects
    while ((bytes_read = recv(client_fd, buffer,
                              sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';

        printf("\nClient: %s\nYou: ", buffer);
        fflush(stdout);
    }

    printf("\nClient disconnected.\n");
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET server_fd;
    struct sockaddr_in address;
    char message[1024];
    uintptr_t thread;

    // 1. Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock initialization failed\n");
        return 1;
    }

    // 2. Create a TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // 3. Configure the server address
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // 4. Bind the socket to port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // 5. Listen for a client
    if (listen(server_fd, 1) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Chat server waiting for client...\n");

    // 6. Accept a client connection
    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == INVALID_SOCKET) {
        printf("Accept failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Client connected. Type 'exit' to quit.\n");

    // 7. Start a thread to receive messages
    thread = _beginthreadex(NULL, 0, receive_messages,
                            NULL, 0, NULL);
    if (thread == 0) {
        printf("Thread creation failed\n");
        closesocket(client_fd);
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // 8. Read and send messages typed by the server user
    while (fgets(message, sizeof(message), stdin) != NULL) {
        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "exit") == 0)
            break;

        if (send(client_fd, message,
                 (int)strlen(message), 0) == SOCKET_ERROR) {
            printf("Send failed\n");
            break;
        }
    }

    // 9. Shut down communication and close sockets
    shutdown(client_fd, SD_BOTH);
    closesocket(client_fd);
    closesocket(server_fd);

    // 10. Wait for the receiving thread to finish
    WaitForSingleObject((HANDLE)thread, INFINITE);
    CloseHandle((HANDLE)thread);

    // 11. Clean up Winsock
    WSACleanup();

    return 0;
}