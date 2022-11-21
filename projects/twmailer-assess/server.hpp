#include <sys/socket.h>
#include <filesystem>
#include "src/Mailspool.hpp"

class Server
{
private:
    socklen_t addrlen;
    struct sockaddr_in address, cliaddress;
    int reuseValue = 1;
    int port;
    int create_socket = -1;
    int new_socket = -1;
    Mailspool *mailspool;

public:
    Server(int port, std::filesystem::path mailspoolDirectory);
    ~Server();
    int start();
    void *clientCommunication(void *data);
};
