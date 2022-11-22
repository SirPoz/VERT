#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <string>

#include "client.hpp"
#include "src/Models/Mail.hpp"
#include "src/Models/Request.hpp"

///////////////////////////////////////////////////////////////////////////////

#define PORT 6543

///////////////////////////////////////////////////////////////////////////////

void signalHandler(int sig)
{
    if (sig == SIGINT)
    {
        throw std::runtime_error("Abort requested");
    }
    else
    {
        exit(sig);
    }
}

Client::Client(const char *ip, int port)
{
    // CREATE A SOCKET
    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        throw std::runtime_error("Socket error");
    }

    // INIT ADDRESS
    memset(&address, 0, sizeof(address)); // init storage with 0
    address.sin_family = AF_INET;         // IPv4
    address.sin_port = htons(port);
    inet_aton(ip, &address.sin_addr);
}

Client::~Client()
{
    // CLOSES THE DESCRIPTOR
    if (create_socket != -1)
    {
        if (shutdown(create_socket, SHUT_RDWR) == -1)
        {
            // invalid in case the server is gone already
            perror("shutdown create_socket");
        }
        if (close(create_socket) == -1)
        {
            perror("close create_socket");
        }
        create_socket = -1;
    }
}

void Client::connectClient()
{
    // CREATE A CONNECTION
    if (connect(create_socket,
                (struct sockaddr *)&address,
                sizeof(address)) == -1)
    {
        throw std::runtime_error("Connect error - no server available");
    }

    printf("Connection with server (%s) established\n",
           inet_ntoa(address.sin_addr));
}

void Client::sendMail()
{
    // Responsible for sending Mails to Server

    Mail mail;

    // Fill Mail with information
    std::cout << "Enter your Username: ";
    std::getline(std::cin, mail.sender);
    std::cout << "Enter Receiver: ";
    std::getline(std::cin, mail.recipient);
    std::cout << "Enter Subject (max. 80 char): ";
    std::getline(std::cin, mail.subject);

    std::cout
        << "Enter Message: " << std::endl;

    // Read message line by line
    while (true)
    {
        std::string messageLine;
        std::getline(std::cin, messageLine);
        if (messageLine == ".")
        {
            break;
        }
        else
        {
            mail.body += messageLine + '\n';
        }
    }

    std::cout << "Sending mail..." << std::endl;

    // Prepare Request
    Request request("SEND", mail.getProtocol());

    // Transmit Request to server
    request.transmit(create_socket);
}

void Client::listMails()
{
    std::cout << "Enter your Username: ";
    std::string username;
    std::cin >> username;

    std::fflush(stdin);

    // Prepare Request
    Request request("LIST", username);

    // Transmit Request
    request.transmit(create_socket);

    // Parse response by server
    Request response(create_socket);

    // check if response was ok
    bool isOk = std::string(response.command).find("OK") != std::string::npos;
    if (isOk)
    {
        // parse mailcount from response
        int mailCount = stoi(response.payload.substr(0, response.payload.find("\n")));

        // remove OK and mail count from buffer
        std::string bufferString = std::string(response.payload);
        std::stringstream bufferStream(bufferString);
        std::cout << "You have " << mailCount << " mails:\n";

        for (int i = 0; i <= mailCount; i++)
        {
            std::string mailSubject;
            std::getline(bufferStream, mailSubject, '\n');
            if (i != 0)
            {
                std::cout << i << ": " << mailSubject << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Error: " << response.payload << std::endl;
        return;
    }
}

void Client::readMail()
{
    std::cout << "Enter your Username: ";
    std::string username;
    std::cin >> username;

    std::cout << "Enter Mail ID: ";
    std::string mailId;
    std::cin >> mailId;

    std::fflush(stdin);

    // Prepare Request
    Request request("READ", username + "\n" + mailId + "\n");
    request.transmit(create_socket);
}

void Client::deleteMail()
{
    std::cout << "Enter your Username: ";
    std::string username;
    std::cin >> username;

    std::cout << "Enter Mail ID: ";
    std::string mailId;
    std::cin >> mailId;

    std::fflush(stdin);

    // Prepare Request
    Request request("DEL", username + "\n" + mailId + "\n");
    request.transmit(create_socket);
}

void Client::communicate()
{
    // RECEIVE DATA

    Request request(create_socket);
    std::cout << request.command << '\n'
              << request.payload << '\n';

    do
    {
        printf(">> ");
        std::string command;
        std::cin >> command;

        std::fflush(stdin);

        if (command == "SEND")
        {
            sendMail();
        }
        else if (command == "LIST")
        {
            listMails();
            continue; // continue, becuase listMails() handles the response
        }
        else if (command == "READ")
        {
            readMail();
        }
        else if (command == "DEL")
        {
            deleteMail();
        }
        else if (command == "QUIT")
        {
            isQuit = true;
            Request quit("QUIT", ""); // transmit quit request to server to close session (for tw-mailer pro)
            quit.transmit(create_socket);
            break;
        }
        else
        {
            std::cout << "Unknown command" << std::endl;
            std::fflush(stdin);

            continue;
        }

        // RECEIVE FEEDBACK

        Request feedback(create_socket);

        bool isOk = feedback.command.find("OK") != std::string::npos;
        if (!isOk)
        {
            fprintf(stderr, "<< Server error occured, abort\n");
            break;
        }
        else
        {
            std::cout << feedback.payload << std::endl;
        }

    } while (!isQuit);
}

int main(int argc, char **argv)
{
    if (signal(SIGINT, signalHandler) == SIG_ERR)
    {
        throw std::runtime_error("Signal cannot be requested");
    }

    Client *client;
    std::string ip; // xxx.xxx.xxx.xxx
    int port;
    if (argc == 3)
    {
        ip = argv[1];
        port = atoi(argv[2]);

        std::cout << "IP: " << ip << "\nPort: " << port << '\n';
    }
    else
    {
        ip = "127.0.0.1";
        port = PORT;
    }

    try
    {
        client = new Client(ip.c_str(), port);

        client->connectClient();
        client->communicate();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    delete client;

    return 0;
}
