#include "Client.hpp"

Client::Client() : request(new Request()), response(new Response())
{
    socket = -1;
    pipe_cgi_in[0] = -1;
    pipe_cgi_in[1] = -1;
    pipe_cgi_out[0] = -1;
    pipe_cgi_out[1] = -1;
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
    sockaddr = op2.sockaddr;
    return (*this);
}

bool	Client::operator==(const Client &c) const
{ return (*request == *c.request && *response == *c.response && socket == c.socket && pipe_cgi_in[0] == c.pipe_cgi_in[0]
            && pipe_cgi_in[1] == c.pipe_cgi_in[1] && pipe_cgi_out[0] == c.pipe_cgi_out[0] && pipe_cgi_out[1] == c.pipe_cgi_out[1]
            && sockaddr.sin_port == c.sockaddr.sin_port); }