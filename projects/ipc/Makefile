all: minimal_fifo myfifo mypipe mypopen myqueue_client myqueue_server signal_handler

minimal_fifo: minimal_fifo.c
	g++ -std=c++14 -Wall -Werror -o minimal_fifo minimal_fifo.c

myfifo: myfifo.c
	g++ -std=c++14 -Wall -Werror -o myfifo myfifo.c

mypipe: mypipe.c
	g++ -std=c++14 -Wall -Werror -o mypipe mypipe.c

mypopen: mypopen.c
	g++ -std=c++14 -Wall -Werror -o mypopen mypopen.c

myqueue_client: myqueue_client.c
	g++ -std=c++14 -Wall -Werror -o myqueue_client myqueue_client.c

myqueue_server: myqueue_server.c
	g++ -std=c++14 -Wall -Werror -o myqueue_server myqueue_server.c

signal_handler: signal_handler.c
	g++ -std=c++14 -Wall -Werror -o signal_handler signal_handler.c

clean:
	rm -f minimal_fifo myfifo mypipe mypopen myqueue_client myqueue_server signal_handler