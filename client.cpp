#include "tictactoe.h"

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
    server_address.sin_addr.s_addr = INADDR_ANY;                    //Valid for local host and LAN connection.
    //server_address.sin_addr.s_addr = inet_addr("97.88.25.171")    //Can insert public IP for internet play.

    //Attempt server connection.
    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    if (connection_status ==-1)
    {
        printf("There was an error making a connection to the remote socket \n");
    }
    else
    {
        printf("Server Connection established.\n\n");
    }

    //Begin send and receive loop
    while(1)
    {
        //Receive response from server and display to user.
        recv(network_socket, &server_response, sizeof(server_response) , 0);
        fflush(stdout);
        printf("%s\n", server_response);

        //If we receive the "disconnected" signal, close the socket descriptor and return
        if(strcmp(server_response, "You have been disconnected from the server.") == 0)
        {
            close(network_socket);  //Close socket.
            return 0;
        }

        //Get input from user, and send to server.
        fgets(user_input,sizeof(user_input), stdin);
        user_input[strlen(user_input) - 1] = '\0';
        send(network_socket, user_input, sizeof(user_input), 0);
    }

    close(network_socket);
    return 0;
}
