#include "tictactoe.h"


int main(int argc, char* argv[])
{
    srand(time(NULL));  //Seed rand for AI play
    int sd; //Socket descriptor for communication with client
    int client_id;  //ClientId provided via CLA
    int game_over = 0;  //Variable to allow termination of gaming loop and process termination.
    int pipe_write; //Write end of pipe to parent process
    char writingBuff[BUFFERSIZE];   //Buffer for holding messages to be written to the pipe.
    int pid = getpid(); //Process ID
    char user_input[BUFFERSIZE];    //User input
    char server_message[BUFFERSIZE] = "Welcome to Tic-Tac-Toe!\nUse the following move guide to choose a move. Simply enter the number corresponding to your desired move. (You are \"X\")\n\n";
    char move_guide[BUFFERSIZE] = " 0 | 1 | 2 \n---|---|---\n 3 | 4 | 5 \n---|---|---\n 6 | 7 | 8 \n"; //Char[] for showing movement guide to player
    char boardRep[BUFFERSIZE];  //Char[] for storing character representation of the board.

    //Interpret command line arguments
    sd = atoi(argv[1]); //Receive socket descriptor
    client_id = atoi(argv[2]);  //receive client_id
    pipe_write = atoi(argv[3]); //receive writing pipe
    snprintf(writingBuff,10,"%d",sd);

    //Communicate established connection
    printf("Child Process Spawned, PID:%d\n", pid);
    printf("PID:%d established connection to Client %d across socket descriptor: %d\n\n", pid,client_id,sd);

    //Create a new gameBoard instance for play
    gameBoard game;

    //Create message introductory message for client
    boardtoCharArray((char *)&boardRep,&game);
    strcat(server_message, move_guide);
    strcat(server_message, "\n\nCurrent Game State:\n");
    strcat(server_message, boardRep);

    //Send introductory message to client and record communication
    send(sd, server_message, sizeof(server_message), 0);
    printTime();
    printf("Message sent to ClientID %d\n\n", client_id);
    while(game_over == 0)
    {
        //Receive input from client and record communication
        recv(sd, &user_input, sizeof(user_input), 0);
        fflush(stdout);
        printTime();
        printf("Message Received from ClientID %d\n\n", client_id);

        //Make movement on the gameBoard as determined by makeMove function
        makeMove(&game,(char *) &user_input, sizeof(user_input), (char *) &server_message, sizeof(server_message));

        //If the game is over... (win detected)
        if(game.win != 0)
        {
            //Contruct and send message asking user to replay.
            strcat(server_message, "\n\nWould you like to play again? Enter Y (yes) or N (no).");
            send(sd,server_message, sizeof(server_message), 0);
            fflush(stdout);
            printTime();
            printf("Message sent to ClientID %d\n\n", client_id);

            //Receive user response.
            recv(sd, &user_input, sizeof(user_input), 0);
            fflush(stdout);
            printTime();
            printf("Message Received from ClientID %d\n\n", client_id);

            //Determine input of user reponse
            if(strlen(user_input) > 0)
            {
                gameBoard newGame;
                game = newGame;
                switch(user_input[0])
                {
                case 'Y':
                case 'y':
                    //Cases in which user wants to play again, reset the board and print it
                    char temp[BUFFERSIZE];
                    boardtoCharArray((char *)&temp, &game);
                    strcpy(server_message, temp);

                    break;
                case 'N':
                case 'n':
                default:
                    //Cases in which user does not want to play again, disconnect and signal break in play loop.
                    game_over = 1;
                    strcpy(server_message, "You have been disconnected from the server.");
                    break;
                }
            }
        }

        //Send message to client and record communication
        send(sd,server_message, sizeof(server_message), 0);
        fflush(stdout);
        printTime();
        printf("Message sent to ClientID %d\n\n", client_id);
    }

    //User has chosen to not play again, record normal termination.
    printTime();
    printf("PID: %d, Server_G Instance exiting normally.\n\n", pid);
    write(pipe_write,writingBuff,sizeof(writingBuff));  //Send socket descriptor back to Server C for closing.
    close(sd);  //Close the socket descriptor
    return 0;
}

void boardtoCharArray(char *charArray, gameBoard* game)
{
    //temp is given a printout which displays a visual of the board
    char temp[256] = "   |   |   \n---|---|---\n   |   |   \n---|---|---\n   |   |   \n";

    //Occupy the board spaces with provided gameBoard data
    temp[1] = game->boardState[0];
    temp[5] = game->boardState[1];
    temp[9] = game->boardState[2];
    temp[25] = game->boardState[3];
    temp[29] = game->boardState[4];
    temp[33] = game->boardState[5];
    temp[49] = game->boardState[6];
    temp[53] = game->boardState[7];
    temp[57] = game->boardState[8];

    int index = 0;

    //Increment through characters of temp and store within charArray
    while(1)
    {
        if(temp[index] != '\0')
        {
            charArray[index] = temp[index];
        }
        else
        {
            break;
        }
        index++;
    }
}

void makeMove(gameBoard* game, char *user_input, int user_input_size, char *server_message, int server_message_size)
{
    int valid = 0; //Valid is 0 by default. Represents validity of user input. 0 equals invalid, 1 equals valid.
    int win_status = 0;

    //Determine if user made a valid input.
    if(user_input_size > 0)
    {
        switch(user_input[0])
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            valid = 1;
            break;
        }

    }

    //User entered valid input (0 through 8)
    if(valid)
    {
        int index = (user_input[0] - 48);    //Convert ASCII number to integer equivalent.

        //If desired move is available...
        if(game->boardState[index] == ' ')
        {
            game->boardState[index] = 'X';   //Place X in desired spot
            game->aiAvailSpaces[index] = ' ';   //Remove the desired space from the AI's available spaces via replacing its index with a space
            game->aiAvailSpaceNum--; //Decrement the number of available spaces

            win_status = detectWin(game);   //Detect win after player move.

            //If no win is detected...
            if(win_status == 0)
            {
                //No win detected after player move, AI will make a move.
                aiPly(game);
                win_status = detectWin(game);

                //If the AI has won...
                if(win_status == 2)
                {
                    //AI player has won.
                    char temp[BUFFERSIZE];
                    boardtoCharArray((char *)&temp, game);
                    strcpy(server_message, temp);
                    strcat(server_message, "\n");
                    strcat(server_message, "AI Player has won. You Lose!");
                }
                //No win detected after AI move...
                else
                {
                    //No Win detected after AI move, Return the board.
                    char temp[BUFFERSIZE];
                    boardtoCharArray((char *)&temp, game);
                    strcpy(server_message, temp);
                    strcat(server_message, "\n");
                }
            }
            //If player one has won...
            else if(win_status == 1)
            {
                //Player one has won.
                char temp[BUFFERSIZE];
                boardtoCharArray((char *)&temp, game);
                strcpy(server_message, temp);
                strcat(server_message, "\n");
                strcat(server_message, "You have won.");
            }
            //If there has been a tie...
            else if(win_status == 3)
            {
                //There is no more moves to be made, a catscratch.
                char temp[BUFFERSIZE];
                boardtoCharArray((char *)&temp, game);
                strcpy(server_message, temp);
                strcat(server_message, "\n");
                strcat(server_message, "There is no more moves, game ended in a tie.");
            }
        }
        //User provided valid input, but the desired spot was already taken previously.
        else
        {
            valid = 0;
        }

        //If player successfully made a move
        if(valid == 0)
        {
            strcpy(server_message, "The desired position has already been taken.\n");
        }
    }
    //User entered invalid input
    else
    {
        strcpy(server_message, "The entered input is invalid. Please enter a number between 0 and 8.\n");
    }
}

int detectWin(gameBoard* game)
{
    //Detect Top Horizontal Wins
    if((game->boardState[0] == 'X') && (game->boardState[1] == 'X') && (game->boardState[2] == 'X'))
    {
        game->win = 1;
        return 1;
    }
    if((game->boardState[0] == 'O') && (game->boardState[1] == 'O') && (game->boardState[2] == 'O'))
    {
        game->win = 2;
        return 2;
    }

    //Detect Middle Horizontal Wins
    if((game->boardState[3] == 'X') && (game->boardState[4] == 'X') && (game->boardState[5] == 'X'))
    {
        game->win = 1;
        return 1;
    }
    if((game->boardState[3] == 'O') && (game->boardState[4] == 'O') && (game->boardState[5] == 'O'))
    {
        game->win = 2;
        return 2;
    }

    //Detect Bottom Horizontal Wins
    if((game->boardState[6] == 'X') && (game->boardState[7] == 'X') && (game->boardState[8] == 'X'))
    {
        game->win = 1;
        return 1;
    }
    if((game->boardState[6] == 'O') && (game->boardState[7] == 'O') && (game->boardState[8] == 'O'))
    {
        game->win = 2;
        return 2;
    }

    //Detect Left Vertical Wins
    if((game->boardState[0] == 'X') && (game->boardState[3] == 'X') && (game->boardState[6] == 'X'))
    {
        game->win = 1;
        return 1;
    }
    if((game->boardState[0] == 'O') && (game->boardState[3] == 'O') && (game->boardState[6] == 'O'))
    {
        game->win = 2;
        return 2;
    }


    //Detect Middle Vertical Wins
    if((game->boardState[1] == 'X') && (game->boardState[4] == 'X') && (game->boardState[7] == 'X'))
    {
        game->win = 1;
        return 1;
    }
    if((game->boardState[1] == 'O') && (game->boardState[4] == 'O') && (game->boardState[7] == 'O'))
    {
        game->win = 2;
        return 2;
    }


    //Detect Right Vertical Wins
    if((game->boardState[2] == 'X') && (game->boardState[5] == 'X') && (game->boardState[8] == 'X'))
    {
        game->win = 1;
        return 1;
    }
    if((game->boardState[2] == 'O') && (game->boardState[5] == 'O') && (game->boardState[8] == 'O'))
    {
        game->win = 2;
        return 2;
    }

    //Detect Diagonal 1 Wins
    if((game->boardState[0] == 'X') && (game->boardState[4] == 'X') && (game->boardState[8] == 'X'))
    {
        game->win = 1;
        return 1;
    }
    if((game->boardState[0] == 'O') && (game->boardState[4] == 'O') && (game->boardState[8] == 'O'))
    {
        game->win = 2;
        return 2;
    }

    //Detect Diagonal 2 Wins
    if((game->boardState[6] == 'X') && (game->boardState[4] == 'X') && (game->boardState[2] == 'X'))
    {
        game->win = 1;
        return 1;
    }
    if((game->boardState[6] == 'O') && (game->boardState[4] == 'O') && (game->boardState[2] == 'O'))
    {
        game->win = 2;
        return 2;
    }

    //If there is no more available moves, game is a tie (return 3)
    if(game->aiAvailSpaceNum == 0)
    {
        game->win = 3;
        return 3;
    }
    else
    {
        return 0;   //Nobody won, and there is still available moves.
    }
}

void aiPly(gameBoard* game)
{
    int rand_index; //Random number selected to determine which valid input AI should choose
    rand_index = (rand() % game->aiAvailSpaceNum) + 1;


    int i = 0;  //Variable used for calculating j (represents number of valid spaces detected as while loop increments through aiAvailSpaces)
    int j = 0;  //Index of AI choice within aiAvailSpaces

    //Increment through available spaces only in the aiAvailSpaces array, skipping over elements which represent an already chosen space.
    while(i != rand_index)
    {
        if(game->aiAvailSpaces[j] != ' ')
        {
            i++;
            if(i == rand_index)
                break;
        }
        j++;
    }

    game->boardState[j] = 'O';  //Mark move with AI's "O" game piece
    game->aiAvailSpaces[j] = ' ';   //Mark the chosen space as taken
    game->aiAvailSpaceNum--;    //Decrement number of available spaces

}



