
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_fd;
    struct sockaddr_in address;
    char buffer[1024];
    int bytes_read;

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

    // 5. Listen for incoming connections
    if (listen(server_fd, 1) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server waiting for a TCP connection...\n");

    // 6. Accept one client connection
    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == INVALID_SOCKET) {
        printf("Accept failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Client connected. Message:\n");

    // 7. Receive data until the client closes the connection
    while ((bytes_read = recv(client_fd, buffer,
                              sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_read, stdout);
        fflush(stdout);
    }

    if (bytes_read == SOCKET_ERROR)
        printf("\nReceive failed\n");

    printf("\nConnection closed.\n");

    // 8. Close both sockets
    closesocket(client_fd);
    closesocket(server_fd);

    // 9. Clean up Winsock
    WSACleanup();

    return 0;
}