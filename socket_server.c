#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib") // Link Winsock Library

#define NSTRS 1500         /* Number of strings */ 
#define PORT 2222          /* Port to listen on */

char strs[NSTRS] = "" ;
char mystring[50] = "This string is fifty bytes long----------------\n";

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code : %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed. Error Code : %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is listening on port %d...\n", PORT);

    // Prepare data to send
    for (int i = 0; i < 30; i++)
        strcat(strs, mystring);

    while (1) {
        client_addr_size = sizeof(client_addr);

        // Accept incoming connection
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size)) == INVALID_SOCKET) {
            printf("Accept failed. Error Code : %d\n", WSAGetLastError());
            continue;
        }

        printf("One connection initiated\n");

        // Receive data
        recv(client_socket, strs, NSTRS, 0);

        // Send data back
        send(client_socket, strs, NSTRS, 0);

        // Close the client socket
        closesocket(client_socket);
    }

    // Close server socket
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
