#pragma once
#include "server.hpp"

class  Client
{
  public:
    Client();
    Client(int new_socket, struct sockaddr_in new_addr, Server* server);
    Client(const Client &src);
    ~Client();

    Client& operator=(const Client &src);

    Request*            request;
    Response*           response;
    struct sockaddr_in  sockaddr;
    int                 socket;
    int                 pipe_cgi_in[2];
    int                 pipe_cgi_out[2];
    int                 fd_file;
};