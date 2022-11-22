all: server client

server: server.cpp
	g++ -std=c++17 -Wall -Werror -o twmailer-server server.cpp src/Models/Mail.cpp src/Models/Request.cpp src/Mailspool.cpp src/Models/Response.cpp

client: client.cpp
	g++ -std=c++17 -Wall -Werror -o twmailer-client client.cpp src/Models/Mail.cpp src/Models/Request.cpp

clean:
	rm -f twmailer-server
	rm -f twmailer-client