#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFERSIZE 1024

using namespace std;

int main()
{
    int network_socket;
    char server_response[BUFFERSIZE];
    char user_input[BUFFERSIZE];

    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;
    //server_address.sin_addr.s_addr = inet_addr("97.88.25.171") ;

    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    if (connection_status ==-1){
        printf("There was an error making a connection to the remote socket \n");
    }
    else{
        printf("Server Connection established.\n\n");
    }

    while(1)
    {
        recv(network_socket, &server_response, sizeof(server_response) , 0);
        fflush(stdout);
        printf("%s\n", server_response);
        //sleep(1);

        if(strcmp(server_response, "You have been disconnected from the server.") == 0)
        {
            close(network_socket);  //Close socket.
            return 0;
        }

        fgets(user_input,sizeof(user_input), stdin);
        fflush(stdout);
        user_input[strlen(user_input) - 1] = '\0';
        send(network_socket, user_input, sizeof(user_input), 0);
    }

    close(network_socket);
    return 0;
}
