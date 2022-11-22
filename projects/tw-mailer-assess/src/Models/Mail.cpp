#include "Mail.hpp"
#include <iostream>

const std::string Mail::getProtocol() const
{
    std::string request;

    request += sender + "\n";
    request += recipient + "\n";
    request += subject + "\n";
    request += body + "\n";
    request += ".\n";

    return request;
}

Mail::Mail(const std::string protocol)
{
    // Parses mail from protocol

    std::string request = protocol;
    std::string delimiter = "\n";

    // Sender
    sender = request.substr(0, request.find(delimiter));
    request.erase(0, request.find(delimiter) + delimiter.length());

    // Recipient
    recipient = request.substr(0, request.find(delimiter));
    request.erase(0, request.find(delimiter) + delimiter.length());

    // Subject
    subject = request.substr(0, request.find(delimiter));
    request.erase(0, request.find(delimiter) + delimiter.length());

    // Body
    body = request.substr(0, request.find(".\n"));
}

Mail::Mail()
{
    id = "";
    sender = "";
    recipient = "";
    subject = "";
    body = "";
}