#pragma once

#include "server.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Request;
class Response;

class  Client
{
  public:
    Client();
    Client(int new_socket, struct sockaddr_in new_addr);
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