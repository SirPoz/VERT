#pragma once
#include <string>
#include <sys/socket.h>

class Request
{
public:
    std::string command;
    std::string payload;

    Request(std::string command, std::string payload);
    Request(char *buffer);
    Request(int socket);

    std::string getProtocol();

    std::string getTrimmedPayload();

    void transmit(int socket);
};