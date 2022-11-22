#include "Mailspool.hpp"

Mailspool::Mailspool(std::filesystem::path mailspoolDirectory)
{
    this->mailspoolDirectory = mailspoolDirectory;

    // Create mailspool directory if it doesn't exist
    if (!std::filesystem::exists(mailspoolDirectory))
    {
        std::cout << "Mailspool directory doesn't exist. Creating it..." << std::endl;
        std::filesystem::create_directory(mailspoolDirectory);
    }
}

void Mailspool::storeMail(Mail mail)
{
    // check if receiver directory exists
    std::filesystem::path receiverDir = mailspoolDirectory / mail.recipient;
    if (!std::filesystem::exists(receiverDir))
    {
        // create receiver directory
        std::filesystem::create_directory(receiverDir);
    }

    // timestamp
    std::time_t timestamp = std::time(nullptr);

    // generate uuid
    std::string uuid = generateUUID();
    mail.id = uuid;

    // create mail file
    std::filesystem::path mailFile = receiverDir / (std::to_string(timestamp) + "_" + uuid + ".mail");

    std::ofstream mailStream(mailFile);
    mailStream << "ID: " << mail.id << std::endl;
    mailStream << "From: " << mail.sender << std::endl;
    mailStream << "To: " << mail.recipient << std::endl;
    mailStream << "Timestamp: " << timestamp << std::endl;
    mailStream << "Subject: " << mail.subject << std::endl;
    mailStream << std::endl;
    mailStream << mail.body << std::endl;
    mailStream.close();

    std::cout << "Mail from " << mail.sender << " to " << mail.recipient << " stored in mailspool" << std::endl;
}

std::string Mailspool::generateUUID()
{
    uuid_t uuid;
    uuid_generate(uuid);
    char uuidString[37];
    uuid_unparse(uuid, uuidString);
    return std::string(uuidString);
}

std::vector<Mail> Mailspool::getMails(std::string recipient)
{
    std::vector<Mail> mails;

    std::filesystem::path receiverDir = mailspoolDirectory / recipient;
    if (!std::filesystem::exists(receiverDir))
    {
        return mails;
    }

    for (const auto &entry : std::filesystem::directory_iterator(receiverDir))
    {
        mails.push_back(openMail(entry.path()));
    }

    return mails;
}

Mail Mailspool::openMail(std::filesystem::path mailFile)
{
    std::ifstream mailStream(mailFile);
    std::string line;
    Mail mail;

    while (std::getline(mailStream, line))
    {
        if (line.find("ID: ") == 0)
        {
            mail.id = line.substr(4);
        }
        if (line.find("From: ") == 0)
        {
            mail.sender = line.substr(6);
        }
        else if (line.find("To: ") == 0)
        {
            mail.recipient = line.substr(4);
        }
        else if (line.find("Timestamp: ") == 0)
        {
            mail.timestamp = std::stoi(line.substr(11));
        }
        else if (line.find("Subject: ") == 0)
        {
            mail.subject = line.substr(9);
        }
        else if (line == "")
        {
            break;
        }
    }

    std::string body;
    while (std::getline(mailStream, line))
    {
        body += line;
    }

    mail.body = body;

    return mail;
}

Mail Mailspool::getMailBySubject(std::string recipient, std::string subject)
{
    std::filesystem::path receiverDir = mailspoolDirectory / recipient;
    if (!std::filesystem::exists(receiverDir))
    {
        return Mail();
    }

    for (const auto &entry : std::filesystem::directory_iterator(receiverDir))
    {
        Mail mail = openMail(entry.path());
        if (mail.subject == subject)
        {
            return mail;
        }
    }

    throw std::runtime_error("Mail not found");
}

Mail Mailspool::getMailByID(std::string recipient, std::string id)
{
    std::filesystem::path receiverDir = mailspoolDirectory / recipient;
    if (!std::filesystem::exists(receiverDir))
    {
        return Mail();
    }

    for (const auto &entry : std::filesystem::directory_iterator(receiverDir))
    {
        Mail mail = openMail(entry.path());
        if (mail.id == id)
        {
            return mail;
        }
    }

    throw std::runtime_error("Mail not found");
}

Mail Mailspool::getMailByCount(std::string recipient, int count)
{
    std::filesystem::path receiverDir = mailspoolDirectory / recipient;
    if (!std::filesystem::exists(receiverDir))
    {
        return Mail();
    }

    int i = 0;
    for (const auto &entry : std::filesystem::directory_iterator(receiverDir))
    {
        if (i == count - 1)
        {
            return openMail(entry.path());
        }
        i++;
    }

    throw std::runtime_error("Mail not found");
}

void Mailspool::deleteMail(std::string recipient, std::string id)
{
    std::filesystem::path receiverDir = mailspoolDirectory / recipient;
    if (!std::filesystem::exists(receiverDir))
    {
        return;
    }

    for (const auto &entry : std::filesystem::directory_iterator(receiverDir))
    {
        Mail mail = openMail(entry.path());
        if (mail.id == id)
        {
            std::filesystem::remove(entry.path());
            return;
        }
    }

    throw std::runtime_error("Mail not found");
}

void Mailspool::deleteMail(std::string recipient, int mailCount)
{
    std::filesystem::path receiverDir = mailspoolDirectory / recipient;
    if (!std::filesystem::exists(receiverDir))
    {
        return;
    }

    int i = 0;
    for (const auto &entry : std::filesystem::directory_iterator(receiverDir))
    {
        if (i == mailCount - 1)
        {
            std::filesystem::remove(entry.path());
            return;
        }
        i++;
    }

    throw std::runtime_error("Mail not found");
}