#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFERSIZE 256

using namespace std;

int main()
{
    char server_message[BUFFERSIZE] = "You have reached the server!";

    //create the server socket
    int err;
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    listen(server_socket, 5);

    int client_id = -1;

    while(1)
    {
        int client_socket;
        client_id++;
        char client_socket_CHAR[11];
        char client_id_CHAR[11];

        client_socket = accept(server_socket, NULL, NULL);
        printf("Accepted Client connection. Communicating on socket descriptor: %d\n", client_socket);


        snprintf(client_socket_CHAR, 10, "%d", client_socket);
        snprintf(client_id_CHAR, 10, "%d", client_id);

        err = fork();

        if(err == 0)
        {
            execlp("./Server_G", "Server_G", client_socket_CHAR, client_id_CHAR, NULL);
        }

        //Send the message
        //send(client_socket,server_message,sizeof(server_message), 0);
    }

    //close the socket
    close(server_socket);

    return 0;
}

