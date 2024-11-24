/*
 ============================================================================
 Name        : server.c
 Author      : Lamonaca Niccolò
 ============================================================================
 */
#include <time.h> // For time-related functions.
#if defined WIN32
#include <winsock.h> // Windows-specific socket library.
#else
#include <string.h>  // Provides string manipulation functions.
#include <unistd.h>  // Provides POSIX API functions like close().
#include <sys/types.h> // Provides data types used in socket programming.
#include <sys/socket.h> // Provides the socket API.
#include <arpa/inet.h>  // Provides functions for IP address manipulation.
#include <ctype.h>      // Provides character type checking functions.
#include <stdlib.h>     // Provides general-purpose functions like malloc() and free().
#include <time.h>       // For time-related functions.
#define closesocket close // Defines closesocket as close() for non-Windows systems.
#endif

#include <stdio.h> // Standard I/O library for functions like printf().
#include "protocollo.h" // Custom header file providing protocol-related definitions.

// Function prototypes for generating random passwords.
void generate_numeric(char *password, int length);
void generate_alpha(char *password, int length);
void generate_mixed(char *password, int length);
void generate_secure(char *password, int length);

// Error handler function to display error messages.
void errorhandler(const char *errorMessage) {
    perror(errorMessage);
}

// Clears Windows socket resources.This won't work on other OS besides of WINDOWS.
void clearwinsock() {
#if defined WIN32
    WSACleanup(); // Cleans up Windows socket resources.
#endif
}

// This is the main function of the program.
int main() {
#if defined WIN32
    WSADATA wsa_data;
    // Initialize Winsock on Windows systems. Initialize the Windows Sockets API
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        errorhandler("Error at WSAStartup()");
        return -1; // Exit if initialization fails.
    }
#endif

    // Create a socket for the server.
    // SOCK_STREAM indicates that the socket will be connection-oriented and use the TCP protocol.
    // IPPROTO_TCP specifies that the protocol to be used is TCP(it can be optional).
    // PF_INET indicates that the socket will use the IPv4 address family.
    int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //this if will check if the socket creation failed
    if (server_socket < 0) {
    //if the socket creaton failed it will use the function errorhandler to print the error text in ().
        errorhandler("Socket creation failed.");
        /*after showing the error message, the program will call the
        function clearwnisock and with that it will clear all the resources
        allocated to the socket.
        This work only on WINDOWS. */
        clearwinsock();
        return -1; // Exit on failure and end the program.
    }

    // Initialize the server address structure.
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); // Zero out the structure.
    server_address.sin_family = AF_INET; // Set the address family to IPv4.
    server_address.sin_addr.s_addr = inet_addr(DEFAULT_IP); // Set the server's IP address.
    server_address.sin_port = htons(DEFAULT_PORT); // Set the server's port number.

    // Bind the socket to the specified address and port.
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    //the program here call the function errorhandler to show on screen the message in the ().
        errorhandler("Bind failed.");
    //then it call the function closesocket to close the socket connection
        closesocket(server_socket);
    /*then it clear all the resources associated to the socket with the function clearwinsock
    This work only in WINDOWS*/
        clearwinsock();
        return -1; // Exit on failure.
    }

    // Put the socket in listening mode to accept incoming connections.
    if (listen(server_socket, QLEN) < 0) {
        errorhandler("Listen failed."); // Handle errors in setting up the listener.
    //then it call the function closesocket to close the socket connection
        closesocket(server_socket);
    /*then it clear all the resources associated to the socket with the function clearwinsock
    This work only in WINDOWS*/
        clearwinsock();
        return -1; // Exit on failure.
    }

    // Inform that the server is ready and listening for connections.
    printf("Server ready on %s:%d\n Waiting for clients...", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
    srand((unsigned int)time(NULL)); // Seed the random number generator.

    while (1) { // Server's main loop to handle incoming connections.
        struct sockaddr_in client_address; // Structure to hold client address information.
        int client_len = sizeof(client_address); // Length of the client address structure.

        // Accept a new client connection.
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket < 0) { // Check if accept() failed.
            errorhandler("Accept failed."); // Handle the error.
            continue; // Skip to the next iteration to accept a new connection.
        }

        // Display information about the new client connection.
        printf("\n\n - New connection from client %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // Handle client requests.
        request req; // Structure to store client requests.
        while (recv(client_socket, (char *)&req, sizeof(req), 0) > 0) { // Receive data from the client.
            if (req.request_type == 'q') { // Check if the client wants to disconnect.
                printf("\n - Client disconnected.\n");
                break; // Exit the loop.
            }

            // Validate the requested password length.
            if (req.length < MIN || req.length > MAX) {
//if the length of the password we choose to generate is smaller than MIN or is bigger than MAX it will show the error message between the ()
                printf("Invalid password length requested.\n");
                continue; // Skip to the next iteration.
            }

            // Process the client's request based on the request type.
            switch (req.request_type) {
                case 'n': generate_numeric(req.password, req.length); break; // Numeric password.
                case 'a': generate_alpha(req.password, req.length); break;  // Alphabetical password.
                case 'm': generate_mixed(req.password, req.length); break;  // Mixed password.
                case 's': generate_secure(req.password, req.length); break; // Secure password.
                default: // Handle invalid request types.
                    printf("Invalid request type received.\n");
                    continue; // Skip to the next iteration.
            }

            // Send the generated password back to the client.
            send(client_socket, (const char *)&req, sizeof(req), 0);
        }

        closesocket(client_socket); // Close the connection with the client.
    }

    clearwinsock(); // Clean up resources before exiting.
    return 0; // Indicate successful execution.
}

// Generate a numeric password of the specified length.
void generate_numeric(char *password, int length) {
    for (int i = 0; i < length; ++i)
        password[i] = '0' + (rand() % 10); // Random digit from 0 to 9.
    password[length] = '\0'; // Null-terminate the string.
}

// Generate an alphabetical password of the specified length.
void generate_alpha(char *password, int length) {
    for (int i = 0; i < length; ++i)
        password[i] = 'a' + (rand() % 26); // Random lowercase letter.
    password[length] = '\0'; // Null-terminate the string.
}

// Generate a mixed password (letters and digits) of the specified length.
void generate_mixed(char *password, int length) {
    for (int i = 0; i < length; ++i) {
        if (rand() % 2) // Randomly choose between letter and digit.
            password[i] = 'a' + (rand() % 26); // Random letter.
        else
            password[i] = '0' + (rand() % 10); // Random digit.
    }
    password[length] = '\0'; // Null-terminate the string.
}

// Generate a secure password with letters, digits, and special characters.
void generate_secure(char *password, int length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()";
    int charset_size = sizeof(charset) - 1; // Number of characters in the charset.
    for (int i = 0; i < length; ++i)
        password[i] = charset[rand() % charset_size]; // Random character from the charset.
    password[length] = '\0'; // Null-terminate the string.
}
