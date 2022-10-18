#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#ifndef BUFFERSIZE
#define BUFFERSIZE 1024
#endif // BUFFERSIZE

//gameBoard represents the data stored within a game of Tic-Tac-Toe, aswell as components necessary for an AI opponent interaction.
typedef struct gameBoard
{
    int turn = 1; //1: Human Player's Turn 2: AI Player's Turn
    int win = 0; //0: No winner 1: player won 2: AI won 3: draw
    char boardState[10] = "         "; //boardState initialized as 10 spaces to represent all 9 board spaces and a null terminator.
    int aiAvailSpaces[9] = {0,1,2,3,4,5,6,7,8}; //Array containing all of the indexes of board spaces that are available for the AI to choose
    int aiAvailSpaceNum = 9;    //Number of options for the AI to choose from

}gameBoard;

//printTime function is to be used before an action to document it with a timestamp.
void printTime()
{
    time_t current_time = time(NULL);
    printf("Action Documented: %s", ctime(&current_time));
}

//monitorSocketDescriptors is to be called by a thread in the parent server process for monitoring closure of socket descriptors
void* monitorSocketDescriptors(void* pipe_read_end);

//boardToCharArray receives a gameBoard and stores its visual representation into charAarray so that it may be printed to the user.
void boardtoCharArray(char* charArray, gameBoard* game);

//makeMove receives user input to make a player move on the gameBoard and subsequently makes an AI move, storing the user's response in server_message
void makeMove(gameBoard* game, char *user_input, int user_input_size, char *server_message, int server_message_size);

//aiPly is called from within makeMove as a subroutine defining the logic of AI movement.
void aiPly(gameBoard* game);

//detectWin receives a gameBoard and will change its win variable if a win is detected. It will also return the win value.
int detectWin(gameBoard* game);



