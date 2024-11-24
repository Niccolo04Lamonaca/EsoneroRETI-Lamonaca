/*
 ============================================================================
 Name        : client.c
 Author      : Lamonaca Niccolò
 ============================================================================
 */

#if defined WIN32
#include <winsock.h>	// For Windows-specific socket functions
#else
#include <string.h> // Provides string manipulation functions.
#include <unistd.h> // Provides POSIX API functions like close().
#include <sys/types.h> // Provides data types used in socket programming.
#include <sys/socket.h> // Provides the socket API.
#include <arpa/inet.h>	// For socket programming on Unix-like systems
#include <stdlib.h> // Provides general-purpose functions like malloc() and free().
#define closesocket close	// Define closesocket for compatibility with Unix-like systems
#endif
#include <time.h> // For time-related functions.
#include <stdio.h> // Provides general-purpose functions like malloc() and free().
#include "protocollo.h"	// Custom header file providing protocol-related definitions.

// Function to handle and print error messages
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
    // // Initialize Winsock on Windows systems. Initialize the Windows Sockets API
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        errorhandler("Error at WSAStartup()");
        return -1; // Exit if initialization fails.
    }
#endif

    // Create the client socket
    int c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (c_socket < 0) {
        errorhandler("Socket creation failed.");
        clearwinsock();
        return -1;
    }

    // Set server address and port
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;	// IPv4 protocol
    server_address.sin_addr.s_addr = inet_addr(DEFAULT_IP);
    server_address.sin_port = htons(DEFAULT_PORT);

    // Connect to the server
    if (connect(c_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        errorhandler("Connect failed.");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }else{
 //else if everything is good with the connection to the server, it will appear on screen the message between ()
    	printf("\n---- CONNECTED TO THE SERVER ----.\n");
    }



    // Initialize the request structure
    request request;
    printf("------ RANDOM PASSWORD GENERATOR ------\n\n");
    while (1) {
        // Get the user input
    	printf("\nWhat password you want to generate?\n-n numeric password (only numbers)\n"
    			"-a alphanumeric password (only lowercase letters)\n"
    			"-m mixed password (numbers and lowercase letters)\n"
    			"-s safe password(capital letters, lowercase letters, numbers and symbols)\n"
    			"***Enter the type of password and its length***\n");
        printf("Enter request or q to quit): ");
        char input[BUFFER_SIZE];
        fgets(input, BUFFER_SIZE, stdin);

        // Parse the input
        if (sscanf(input, "%c %d", &request.request_type, &request.length) < 1) {
            printf("\nInvalid input. Try again.\n");
            continue;
        }

        // Check if the user wants to quit
        if (request.request_type == 'q') {
//if the user digit the character q, that will let know the client that the connection will end. With that on screen will be printed the message between (" ")
            printf("\nExiting client...\n");
            send(c_socket, (const char *)&request, sizeof(request), 0); // Notify the server
            // using this the program will exit from the if and execute the piece of code after this piece of code
            break;
        }


// This if control the password length. If the password length is not correct and is smaller than the MIN MAX range, it will print the message between (" ")
        if (request.length < MIN || request.length > MAX) {
            printf("\nPassword length must be between %d and %d.\n", MIN, MAX  );
            continue;
        }

        // Send the request to the server
        if (send(c_socket,  (const char *)&request, sizeof(request), 0) <= 0) {
/*If send() returns a value ≤ 0, it means that the sending operation failed.
 * Then it will use the function errorhandler to show on screen the message between (" ").
 */
            errorhandler("\nFailed to send the request.");
            // using this the program will exit from the if and execute the piece of code after this piece of code
            break;
        }

        // Receive the response from the server
        if (recv(c_socket,  (char *)&request, sizeof(request), 0) <= 0) {
/* If recv() returns a value ≤ 0, it means that the sending operation failed.
 *Then it will use the function errorhandler to show on screen the message between (" ").
 */
            errorhandler("\nFailed to receive response from the server.");
 // using this the program will exit from the if and execute the piece of code after this piece of code
            break;
        }

        // Print the generated password
        printf("\ngenerated password --> %s\n", request.password);
    }

    // Close the socket
    closesocket(c_socket);
    //then it clear all the resources associated to the socket with the function clearwinsock
    clearwinsock(); // This work only in WINDOWS
    return 0;
}
