#pragma once
#include <string>

typedef std::string MailID;

class Mail
{
public:
    MailID id;
    std::string sender;
    std::string recipient;
    std::string subject;
    std::string body;
    time_t timestamp;

    const std::string getProtocol() const;
    Mail(const std::string protocol);
    Mail();
};