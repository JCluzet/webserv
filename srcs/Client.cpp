#include "Client.hpp"

Client::Client() : request(new Request()), response(new Response())
{
    socket = -1;
    pipe_cgi_in[0] = -1;
    pipe_cgi_in[1] = -1;
    pipe_cgi_out[0] = -1;
    pipe_cgi_out[1] = -1;
    fd_file = -1;
    sockaddr = sockaddr_in();
    return ;
}

Client::Client(int new_socket, sockaddr_in new_addr, Server *server) : request(new Request())
{
    response = new Response(request, server);
    socket = new_socket;
    pipe_cgi_in[0] = -1;
    pipe_cgi_in[1] = -1;
    pipe_cgi_out[0] = -1;
    pipe_cgi_out[1] = -1;
    fd_file = -1;
    sockaddr = new_addr;
    return ;
}

Client::Client(const Client& src) : request(new Request()), response(new Response())
{
    *this = src;
    return ;
}

Client::~Client()
{
    delete request;
    delete response;
    if (pipe_cgi_in[0] != -1)
        close(pipe_cgi_in[0]);
    if (pipe_cgi_in[1] != -1)
        close(pipe_cgi_in[1]);
    if (pipe_cgi_out[0] != -1)
        close(pipe_cgi_out[0]);
    if (pipe_cgi_out[1] != -1)
        close(pipe_cgi_out[1]);
    if (fd_file != -1)
        close(fd_file);
    return ;
}

Client& Client::operator=(const Client& op2)
{
    delete request;
    delete response;
    request = new Request(*op2.request);
    response = new Response(*op2.response);
    response->setRequest(request);
    socket = op2.socket;
    pipe_cgi_in[0] = op2.pipe_cgi_in[0];
    pipe_cgi_in[1] = op2.pipe_cgi_in[1];
    pipe_cgi_out[0] = op2.pipe_cgi_out[0];
    pipe_cgi_out[1] = op2.pipe_cgi_out[1];
    fd_file = op2.fd_file;
    sockaddr = op2.sockaddr;
    return (*this);
}