
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

    // Convert the server IP address to the required format
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (server.sin_addr.s_addr == INADDR_NONE) {
        printf("Invalid address.\n");
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

    // 5. Send the message
    if (send(sock, message, (int)strlen(message), 0)
        == SOCKET_ERROR) {
        printf("Send failed\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Message sent: %s\n", message);

    // 6. Close the sending side of the connection
    shutdown(sock, SD_SEND);

    // 7. Close the socket
    closesocket(sock);

    // 8. Clean up Winsock
    WSACleanup();

    return 0;
}