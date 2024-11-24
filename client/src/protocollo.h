/*
 ============================================================================
 Name        : protocollo.h
 Author      : Lamonaca Niccolò
 ============================================================================
 */


#ifndef PROTOCOLLO_H_
#define PROTOCOLLO_H_

//This define set the default port to 60000
#define DEFAULT_PORT 60000
//This define set the default ip to the one written on the right between " "
#define DEFAULT_IP "127.0.0.1"
// This define set the size of a temp memory, the buffer, of 64
#define BUFFER_SIZE 64
//this define set the max lenght that the passwords can have
#define MAX 32
//This define set the min lenght that the passwords can have
#define MIN 6
#define QLEN 5

//here we define a struct named request
typedef struct {
    char request_type; // this variable will store the type that the password need to be betweed the following types: 'n', 'a', 'm', 's', or 'q'
    int length;        // this variable will store password length
    char password[MAX + 1]; // in this variable will be stored the generated password
} request;

#endif /* PROTOCOLLO_H_ */
