#include "Response.hpp"
#include <stdexcept>

Response::Response(std::string payload)
{
    this->payload = payload;
}

void Response::transmit(int socket)
{
    std::string response = "OK\n" + payload;
    if ((send(socket, response.c_str(), response.length(), 0) < 0))
    {
        throw std::runtime_error("Error sending response");
    }
}