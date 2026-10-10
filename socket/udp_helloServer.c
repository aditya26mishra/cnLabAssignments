
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    const char *message = "Hello, world!";

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
    server.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1",
                  &server.sin_addr) != 1) {
        printf("Invalid server address\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 4. Send the message as a UDP datagram
    if (sendto(sock, message, (int)strlen(message), 0,
               (struct sockaddr *)&server,
               sizeof(server)) == SOCKET_ERROR) {
        printf("Send failed\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Message sent: %s\n", message);

    // 5. Close the UDP socket
    closesocket(sock);

    // 6. Clean up Winsock
    WSACleanup();

    return 0;
}