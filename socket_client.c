#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>  // For Sleep()

#pragma comment(lib, "ws2_32.lib")

#define NSTRS 1500
#define PORT 2222
#define INTERVAL_MS 10 // 10 milliseconds

char strs[NSTRS] = "";
char mystring[50] = "This string is fifty bytes long----------------\n";

// Function to handle connection, send/receive data
void handle_connection(const char* server_ip) {
    WSADATA wsaData;
    SOCKET s;
    struct sockaddr_in serverAddr;
    char buffer[NSTRS] = {0};
    LARGE_INTEGER frequency, t1, t2;
    double elapsed_time;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return;
    }

    // Create TCP socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("client: socket creation failed\n");
        WSACleanup();
        return;
    }

    // Setup server address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(server_ip);  // FIX: Use inet_addr()

    // Connect to server
    if (connect(s, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("client: connection failed\n");
        closesocket(s);
        WSACleanup();
        return;
    }

    // Start timer
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);

    // Send data to server
    if (send(s, strs, strlen(strs), 0) == SOCKET_ERROR) {
        printf("client: send failed\n");
        closesocket(s);
        WSACleanup();
        return;
    }

    // Receive data from server
    if (recv(s, buffer, sizeof(buffer), 0) <= 0) {
        printf("client: recv failed\n");
        closesocket(s);
        WSACleanup();
        return;
    }

    // End timer
    QueryPerformanceCounter(&t2);
    elapsed_time = (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart * 1000000;

    fprintf(stderr, "Round-trip time: %.0lf microseconds\n", elapsed_time);

    // Close the socket
    closesocket(s);
    WSACleanup();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <server IP>\n", argv[0]);
        return 1;
    }

    // Prepare the message to send (30 repetitions)
    for (int i = 0; i < 30; i++) {
        strcat(strs, mystring);
    }

    printf("Client is creating a new connection every %d ms...\n", INTERVAL_MS);

    while (1) {
        handle_connection(argv[1]);
        Sleep(INTERVAL_MS);  // Wait for 10 ms before reconnecting
    }

    return 0;
}
