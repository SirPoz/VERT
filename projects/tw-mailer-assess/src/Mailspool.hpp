#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "Models/Mail.hpp"
#include <uuid/uuid.h>
#include <vector>

class Mailspool
{
private:
    std::filesystem::path mailspoolDirectory;
    std::string generateUUID();

public:
    Mailspool(std::filesystem::path mailspoolDirectory);
    Mailspool() = delete;

    void storeMail(Mail mail);
    Mail openMail(std::filesystem::path mailFile);

    void deleteMail(std::string recipient, std::string mailID);
    void deleteMail(std::string recipient, int mailCount);

    std::vector<Mail> getMails(std::string recipient);
    Mail getMailBySubject(std::string recipient, std::string subject);
    Mail getMailByID(std::string recipient, std::string id);
    Mail getMailByCount(std::string recipient, int count);
};