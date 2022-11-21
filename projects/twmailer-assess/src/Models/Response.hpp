#pragma once
#include <string>
#include <sys/socket.h>

class Response
{
public:
    std::string payload;

    Response(std::string payload);
    void transmit(int socket);
};