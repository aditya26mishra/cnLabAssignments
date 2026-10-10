
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server, client;
    int client_len = sizeof(client);
    char buffer[1024];
    int bytes_read;
    DWORD timeout = 3000;

    // 1. Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock initialization failed\n");
        return 1;
    }

    // 2. Create a UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // 3. Configure the server address
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    // 4. Bind the socket to port 8080
    if (bind(sock, (struct sockaddr *)&server,
             sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 5. Set a receive timeout of three seconds
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                   (const char *)&timeout,
                   sizeof(timeout)) == SOCKET_ERROR) {
        printf("Timeout setup failed\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("UDP server waiting for messages...\n");

    // 6. Receive datagrams until a timeout occurs
    while (1) {
        bytes_read = recvfrom(sock, buffer,
                              sizeof(buffer) - 1, 0,
                              (struct sockaddr *)&client,
                              &client_len);

        if (bytes_read == SOCKET_ERROR) {
            printf("No more messages or receive error.\n");
            break;
        }

        buffer[bytes_read] = '\0';
        printf("Received: %s\n", buffer);
    }

    // 7. Close the socket
    closesocket(sock);

    // 8. Clean up Winsock
    WSACleanup();

    return 0;
}