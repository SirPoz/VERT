#include "Request.hpp"
#include <vector>
#include <stdexcept>

void Request::transmit(int socket)
{
    // handles the transmission of the request

    std::string request = getProtocol();
    if ((send(socket, request.c_str(), request.length(), 0) < 0))
    {
        throw std::runtime_error("Error sending request");
    }
}

std::string Request::getProtocol()
{
    std::string request;

    request += command + "\n";
    request += payload + "\n";
    request += ".\n";

    return request;
}

Request::Request(std::string command, std::string payload)
{
    this->command = command;
    this->payload = payload;
}

Request::Request(char *buffer)
{
    std::string request = buffer;

    command = request.substr(0, request.find("\n"));
    request.erase(0, request.find("\n") + 1);

    payload = request.substr(0, request.find(".\n"));
}

Request::Request(int socket)
{
    const unsigned int MAX_BUF_LENGTH = 4096;
    std::vector<char> buffer(MAX_BUF_LENGTH);
    std::string rcv;
    int bytesReceived = 0;

    // for larger requests that exceed the buffer
    do
    {
        bytesReceived = recv(socket, &buffer[0], buffer.size(), 0);
        // append string from buffer.
        if (bytesReceived == -1)
        {
            perror("Failed receiving request");
            break;
        }
        else
        {
            rcv.append(buffer.cbegin(), buffer.cend());
        }
    } while (bytesReceived == MAX_BUF_LENGTH);

    command = rcv.substr(0, rcv.find("\n"));
    rcv.erase(0, rcv.find("\n") + 1);

    payload = rcv.substr(0, rcv.find(".\n"));
}

std::string Request::getTrimmedPayload()
{
    // Remove trailing .\n from payload
    std::string trimmedPayload = payload;
    trimmedPayload.erase(trimmedPayload.find_last_not_of(".\n") + 1);

    return trimmedPayload;
}
