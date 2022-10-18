all: Client Server_C Server_G

Client: client.cpp tictactoe.h
	g++ client.cpp tictactoe.h -std=c++11 -o Client

Server_C: server.cpp tictactoe.h
	g++ server.cpp tictactoe.h -std=c++11 -pthread -o Server_C

Server_G: server_g.cpp tictactoe.h
	g++ server_g.cpp tictactoe.h -std=c++11 -o Server_G
