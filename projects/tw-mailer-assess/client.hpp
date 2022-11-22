class Client
{
private:
    int create_socket;
    struct sockaddr_in address;
    int isQuit;

public:
    Client(const char *, int);
    ~Client();

    void connectClient();
    void communicate();
    void sendMail();
    void readMail();
    void listMails();
    void deleteMail();
};
