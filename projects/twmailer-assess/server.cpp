#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <uuid/uuid.h>


#include "server.hpp"
#include "src/Models/Request.hpp"
#include "src/Models/Mail.hpp"
#include "src/Mailspool.hpp"
#include "src/Models/Response.hpp"

void *clientCommunication(void *data);
void signalHandler(int sig);

volatile sig_atomic_t abortRequested = 0;

int main(int argc, char *argv[])
{
    int port;
    std::filesystem::path path;

    if (argc == 3) // richtige Anzahl an Argumenten
    {
        port = atoi(argv[1]);
        path = argv[2];
    }
    else
    {
        throw std::invalid_argument("Please provide port and path to mail-spool directory");
    }
    try
    {
        Server server = Server(port, path);
        server.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

Server::Server(int newPort, std::filesystem::path newMailspoolDirectory)
{
    port = newPort;
    mailspool = new Mailspool(newMailspoolDirectory);

    if (signal(SIGINT, signalHandler) == SIG_ERR) // signalhandler
    {
        throw new std::runtime_error("Signal cannot be registered");
    }

    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) // creating socket
    {
        throw std::runtime_error("Socket error");
    }

    if (setsockopt(create_socket,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuseValue,
                   sizeof(reuseValue)) == -1) // setting up socket
    {
        throw std::runtime_error("set socket options - reuseAddr");
    }

    if (setsockopt(create_socket,
                   SOL_SOCKET,
                   SO_REUSEPORT,
                   &reuseValue,
                   sizeof(reuseValue)) == -1)
    {
        throw std::runtime_error("set socket options - reusePort");
    }

    // initiating address
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(create_socket, (struct sockaddr *)&address, sizeof(address)) == -1) // assigning Address with Port to Socket
    {
        throw std::runtime_error("bind error");
    }

    if (listen(create_socket, 5) == -1) // allow connection
    {
        throw std::runtime_error("listen error");
    }
}

Server::~Server()
{

    if (new_socket != -1)
    {
        if (shutdown(new_socket, SHUT_RDWR) == -1)
        {
            perror("shutdown new_socket");
        }
        if (close(new_socket) == -1)
        {
            perror("close new_socket");
        }
        new_socket = -1;
    }

    if (create_socket != -1)
    {
        if (shutdown(create_socket, SHUT_RDWR) == -1)
        {
            perror("shutdown create_socket");
        }
        if (close(create_socket) == -1)
        {
            perror("close create_socket");
        }
        create_socket = -1;
    }

    delete mailspool;
}

int Server::start()
{

    while (!abortRequested)
    {
        printf("Waiting for connections...\n");

        addrlen = sizeof(struct sockaddr_in);
        if ((new_socket = accept(create_socket,
                                 (struct sockaddr *)&cliaddress,
                                 &addrlen)) == -1)
        {
            if (abortRequested)
            {
                perror("accept error after aborted");
            }
            else
            {
                perror("accept error");
            }
            break;
        }

        printf("Client connected from %s:%d...\n",
               inet_ntoa(cliaddress.sin_addr),
               ntohs(cliaddress.sin_port));
        try
        {
            clientCommunication(&new_socket); // communication with client
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
        new_socket = -1;
    }

    if (create_socket != -1) // frees the descriptor
    {
        if (shutdown(create_socket, SHUT_RDWR) == -1)
        {
            perror("shutdown create_socket");
        }
        if (close(create_socket) == -1)
        {
            perror("close create_socket");
        }
        create_socket = -1;
    }

    return EXIT_SUCCESS;
}

void *Server::clientCommunication(void *data)
{
    int *current_socket = (int *)data;

    // Welcome message
    Response welcome("Welcome to myserver!\r\nPlease enter your commands...\r\n");
    welcome.transmit(*current_socket);

    do
    {
        // Receive request
        Request request(*current_socket);
        if (request.command == "") // init request failed -> end connection with client
        {
            break;
        }

        if (request.command == "SEND")
        {
            Mail mail(request.payload);
            mailspool->storeMail(mail);
        }
        else if (request.command == "LIST")
        {
            std::string user = request.getTrimmedPayload();
            std::vector<Mail> mails = mailspool->getMails(request.getTrimmedPayload());

            std::string List = std::to_string(mails.size()) + "\n";

            for (Mail mail : mails)
            {
                List += mail.subject + "\n";
            }

            Response response(List);
            response.transmit(*current_socket);
            continue;
        }
        else if (request.command == "READ")
        {
            std::string user = request.getTrimmedPayload().substr(0, request.getTrimmedPayload().find("\n"));
            int mailCount = stoi(request.getTrimmedPayload().substr(request.getTrimmedPayload().find("\n") + 1));

            try
            {
                Mail mail = mailspool->getMailByCount(user, mailCount);
                Response response(mail.getProtocol());
                response.transmit(*current_socket);
                continue;
            }
            catch (const std::exception &e)
            {
                Response response("ERROR " + std::string(e.what()));
                response.transmit(*current_socket);
                continue;
            }
        }
        else if (request.command == "DEL")
        {
            std::string user = request.getTrimmedPayload().substr(0, request.getTrimmedPayload().find("\n"));
            int mailCount = stoi(request.getTrimmedPayload().substr(request.getTrimmedPayload().find("\n") + 1));

            try
            {
                mailspool->deleteMail(user, mailCount);
                Response response("OK");
                response.transmit(*current_socket);
                continue;
            }
            catch (const std::exception &e)
            {
                Response response("ERROR " + std::string(e.what()));
                response.transmit(*current_socket);
                continue;
            }
        }
        else if (request.command == "QUIT")
        {
            break; // stop connection to client
        }
        else
        {
            Response response("ERROR unknown command");
            response.transmit(*current_socket);
            continue;
        }

        Response response("OK"); // success response
        response.transmit(*current_socket);

    } while (!abortRequested);

    // closes/frees the descriptor if not already
    if (*current_socket != -1)
    {
        if (shutdown(*current_socket, SHUT_RDWR) == -1)
        {
            perror("shutdown new_socket");
        }
        if (close(*current_socket) == -1)
        {
            perror("close new_socket");
        }
        *current_socket = -1;
    }

    return NULL;
}

void signalHandler(int sig)
{
    if (sig == SIGINT)
    {
        abortRequested = 1;
        throw std::runtime_error("Abort requested");
    }
    else
    {
        exit(sig);
    }
}
