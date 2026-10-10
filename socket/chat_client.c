
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>

SOCKET sock;

// Function to receive messages from the server
unsigned __stdcall receive_messages(void *arg) {
    char buffer[1024];
    int bytes_read;

    // Keep receiving messages until the server disconnects
    while ((bytes_read = recv(sock, buffer,
                              sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';

        printf("\nServer: %s\nYou: ", buffer);
        fflush(stdout);
    }

    printf("\nServer disconnected.\n");
    return 0;
}

int main() {
    WSADATA wsa;
    struct sockaddr_in server;
    char message[1024];
    uintptr_t thread;

    // 1. Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock initialization failed\n");
        return 1;
    }

    // 2. Create a TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // 3. Configure the server address
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1",
                  &server.sin_addr) != 1) {
        printf("Invalid server address\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 4. Connect to the server
    if (connect(sock, (struct sockaddr *)&server,
                sizeof(server)) == SOCKET_ERROR) {
        printf("Connection failed. Start the server first.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Connected to chat server. Type 'exit' to quit.\n");

    // 5. Start a thread to receive messages
    thread = _beginthreadex(NULL, 0, receive_messages,
                            NULL, 0, NULL);
    if (thread == 0) {
        printf("Thread creation failed\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 6. Read and send messages typed by the client user
    while (fgets(message, sizeof(message), stdin) != NULL) {
        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "exit") == 0)
            break;

        if (send(sock, message,
                 (int)strlen(message), 0) == SOCKET_ERROR) {
            printf("Send failed\n");
            break;
        }
    }

    // 7. Shut down communication and close the socket
    shutdown(sock, SD_BOTH);
    closesocket(sock);

    // 8. Wait for the receiving thread to finish
    WaitForSingleObject((HANDLE)thread, INFINITE);
    CloseHandle((HANDLE)thread);

    // 9. Clean up Winsock
    WSACleanup();

    return 0;
}
