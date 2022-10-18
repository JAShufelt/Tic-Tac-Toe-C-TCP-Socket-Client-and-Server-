#include "tictactoe.h"

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
    int fd[2];
    int abc = 2;
    pipe(fd);

    char pipe_write[11];
    snprintf(pipe_write,10,"%d",fd[1]);

    pthread_t socket_manager;
    if(pthread_create(&socket_manager,NULL, &monitorSocketDescriptors, &fd[0]) != 0)
    {
        printf("Error creating a thread.");
    }

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
        if(err == -1)
        {
            printf("Process failed to fork\n");
            close(server_socket);
            exit(1);
        }

        if(err == 0)
        {
            execlp("./Server_G", "Server_G", client_socket_CHAR, client_id_CHAR, pipe_write, NULL);
        }
    }

    //close the socket
    close(server_socket);

    return 0;
}

void* monitorSocketDescriptors(void* pipe_read_end)
{
    int sd;
    int pipe_read = *(int*)pipe_read_end;
    char readingBuff[BUFFERSIZE];

    fflush(stdout);
    printf("Server C: New thread estabalished monitoring socket descriptors\n");

    while(1)
    {
        read(pipe_read, readingBuff, sizeof(readingBuff));
        fflush(stdout);

        printTime();
        printf("Closing Socket Descriptor: %s\n", readingBuff);
        sd = atoi(readingBuff);
        if(close(sd) != 0)
        {

            printf("Thread failed to close socket descriptor %d\n", sd);
        }
    }

    return 0;
}
