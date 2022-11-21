
all: tw_client tw_server

tw_client:
	g++ -o tw_client tw_client.cpp

tw_server:
	g++ -std=c++11 -Wall -o tw_server tw_server.cpp -lldap -llber

clean:
	rm -f *~ tw_client tw_server
