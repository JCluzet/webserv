#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "server.hpp"

class  Client
{
  public:
    Client();
    Client(int new_socket, struct sockaddr_in new_addr, Server* server);
    Client(const Client &src);
    ~Client();

    Client& operator=(const Client &src);
    bool	operator==(const Client &c) const;

    Request*            request;
    Response*           response;
    int                 socket;
    int                 pipe_cgi_in[2];
    int                 pipe_cgi_out[2];
    struct sockaddr_in  sockaddr;
};

#endif