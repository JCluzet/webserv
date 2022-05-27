#include "server.hpp"

bool is_cgi(Request* request, Server* conf)
{
    std::string str;
    if (request->get_path().find("/") == std::string::npos || request->get_path() == "")
        return false;
    for (size_t i = 0; i < conf->cgi.size(); i++)
    {
        if (request->get_method() == "GET" && request->get_path().find("." + conf->cgi[i].first) != std::string::npos)
            return true;
        if (request->get_method() == "POST" && request->get_path().find("." + conf->cgi[i].first) != std::string::npos)
            return true;
    }
    return false;
}

std::vector<std::string> cgi_env(std::string cgi_str, Client *client, Server *server, std::string fullpath)
{
    std::ostringstream convert;
    std::vector<std::string> env(28);
    std::string str;

    str = "CONTENT_TYPE="; // The media type of the query data, such as text/html.
    if (client->request->get_method() == "GET")
        str += "";
    else
        str += client->request->get_header("Content-Type");
    env[0] = str;
    str = "CONTENT_LENGTH="; // The length of the query data (in bytes or the number of characters) passed to the CGI program through standard input (for POST).
    if (client->request->get_method() == "GET")
        str += "";
    else
        str += client->request->get_header("Content-Length");
    env[1] = str;
    str = "GATEWAY_INTERFACE="; // The revision of the Common Gateway Interface that the server uses.
    str += "CGI/1.1";
    env[2] = str;
    str = "PATH_INFO="; // Extra path information passed to a CGI program.
    str += fullpath.substr(server->root.length());
    env[3] = str;
    str = "PATH_TRANSLATED="; // The translated version of the path given by the variable PATH_INFO.
    str += fullpath.substr(server->root.length());
    env[4] = str;
    str = "QUERY_STRING="; // The query information passed to the program. It is appended to the URL following a question mark (?).
    str += cgi_str;
    env[5] = str;
    str = "AUTH_TYPE="; // The authentication method used to validate a user. See REMOTE_IDENT and REMOTE_USER.
    str += "Basic";
    env[6] = str;
    str = "REMOTE_ADDR=";                        // The remote IP address from which the user is making the client->request.
    str += inet_ntoa(client->sockaddr.sin_addr);
    env[7] = str;
    str = "REMOTE_PORT="; // The remote IP address from which the user is making the client->request.
    convert << static_cast<int>(ntohs(client->sockaddr.sin_port));
    str += convert.str();
    env[8] = str;
    str = "REMOTE_HOST="; // The remote hostname from which the user is making the client->request.
    str += "";
    env[9] = str;
    str = "REMOTE_USER="; // The authenticated name of the user making the query.
    str += client->request->get_header("From");
    env[10] = str;

    str = "REQUEST_METHOD="; // The method with which the information client->request was issued (e.g., GET, POST, HEAD).
    str += client->request->get_method();
    env[11] = str;

    str = "SERVER_NAME="; // The server's hostname or IP address. Equivalent HTTP_HOST
    str += server->ip;
    env[12] = str;
    str = "SERVER_PROTOCOL="; // The name and revision number of the server protocol.
    str += "HTTP/1.1";
    env[13] = str;
    str = "SERVER_PORT="; // The port number of the host on which the server is running.
    str += server->port;
    env[14] = str;
    str = "SERVER_SOFTWARE="; // The name and version of the server software that is answering the client client->request
    str += "WebServ/1.0";
    env[15] = str;
    str = "HTTP_ACCEPT="; // A list of the media types that the client can accept.
    str += client->request->get_header("Accept");
    env[16] = str;
    str = "HTTP_COOKIE="; // A list of cookies defined for that URL.
    str += client->request->get_header("Cookie");
    env[17] = str;
    str = "HTTP_ACCEPT_LANGUAGE="; // Langage accepté par le client.
    str += client->request->get_header("Accept-Language");
    env[18] = str;
    str = "HTTP_REFERER="; // The URL of the document the client read before accessing the CGI program.
    str += client->request->get_header("Referer");
    env[19] = str;
    str = "HTTP_USER_AGENT="; // The browser the client is using to issue the client->request.
    str += client->request->get_header("User-Agent");
    env[20] = str;
    str = "HTTP_ACCEPT_ENCODING=";
    str += client->request->get_header("Accept-Encoding");
    env[21] = str;
    str = "HTTP_ACCEPT_CHARSET=";
    str += client->request->get_header("Accept_Charsets");
    env[22] = str;
    str = "DOCUMENT_ROOT="; // The directory from which web documents are served.
    str += server->root;
    env[23] = str;
    str = "REQUEST_URI=";
    str += client->request->get_path_o();
    env[24] = str;
    str = "REDIRECT_STATUS=";
    str += "200";
    env[25] = str;
    str = "SCRIPT_FILENAME=";
    str += fullpath;
    env[26] = str;
    str = "UPLOAD_PATH=";
    str += server->upload;
    env[27] = str;

    return (env);
}

void cgi_exec(std::vector<std::string> cmd, std::vector<std::string> env, Client *client)
{
    pid_t pid;

    char **cmd_execve = new char *[cmd.size() + 1];
    for (size_t i = 0; i < cmd.size(); i++)
    {
        cmd_execve[i] = new char[cmd[i].length() + 1];
        cmd_execve[i] = std::strcpy(cmd_execve[i], cmd[i].c_str());
    }
    cmd_execve[cmd.size()] = NULL;
    char **env_execve = new char *[env.size() + 1];
    for (size_t i = 0; i < env.size(); i++)
    {
        env_execve[i] = new char[env[i].length() + 1];
        env_execve[i] = std::strcpy(env_execve[i], env[i].c_str());
    }
    env_execve[env.size()] = NULL;
    if (pipe(client->pipe_cgi_out) < 0)
    {
        client->pipe_cgi_out[0] = -1;
        client->pipe_cgi_out[1] = -1;
        client->response->setStatus(500);
        client->fd_file = client->response->openFile();
        for (size_t k = 0; k < env.size(); k++)
            delete[] env_execve[k];
        delete[] env_execve;
        for (size_t k = 0; k < cmd.size(); k++)
            delete[] cmd_execve[k];
        delete[] cmd_execve;
        return;
    }
    if (client->request->get_method() == "POST" && pipe(client->pipe_cgi_in) < 0)
    {
        close(client->pipe_cgi_out[0]);
        close(client->pipe_cgi_out[1]);
        client->pipe_cgi_out[0] = -1;
        client->pipe_cgi_out[1] = -1;
        client->pipe_cgi_in[0] = -1;
        client->pipe_cgi_in[1] = -1;
        client->response->setStatus(500);
        client->fd_file = client->response->openFile();
        for (size_t k = 0; k < env.size(); k++)
            delete[] env_execve[k];
        delete[] env_execve;
        for (size_t k = 0; k < cmd.size(); k++)
            delete[] cmd_execve[k];
        delete[] cmd_execve;
        return;
    }
    if ((pid = fork()) < 0)
    {
        close(client->pipe_cgi_out[0]);
        close(client->pipe_cgi_out[1]);
        close(client->pipe_cgi_in[0]);
        close(client->pipe_cgi_in[1]);
        client->pipe_cgi_in[0] = -1;
        client->pipe_cgi_in[1] = -1;
        client->pipe_cgi_out[0] = -1;
        client->pipe_cgi_out[1] = -1;
        client->response->setStatus(500);
        client->fd_file = client->response->openFile();
        for (size_t k = 0; k < env.size(); k++)
            delete[] env_execve[k];
        delete[] env_execve;
        for (size_t k = 0; k < cmd.size(); k++)
            delete[] cmd_execve[k];
        delete[] cmd_execve;
        return;
    }
    if (pid == 0)
    {
        close(client->pipe_cgi_in[1]);
        close(client->pipe_cgi_out[0]);
        if (dup2(client->pipe_cgi_out[1], 1) < 0 || (client->request->get_method() == "POST" && dup2(client->pipe_cgi_in[0], 0) < 0))
        {
            close(client->pipe_cgi_in[0]);
            close(client->pipe_cgi_out[1]);
            for (size_t k = 0; k < env.size(); k++)
                delete[] env_execve[k];
            delete[] env_execve;
            for (size_t k = 0; k < cmd.size(); k++)
                delete[] cmd_execve[k];
            delete[] cmd_execve;
            exit(1);
        }

        if (execve(cmd[0].c_str(), cmd_execve, env_execve) < 0)
        {
            for (size_t k = 0; k < env.size(); k++)
                delete[] env_execve[k];
            delete[] env_execve;
            for (size_t k = 0; k < cmd.size(); k++)
                delete[] cmd_execve[k];
            delete[] cmd_execve;
            close(client->pipe_cgi_in[0]);
            close(client->pipe_cgi_out[1]);
        }
        exit(1);
    }
    else
    {
        close(client->pipe_cgi_in[0]);
        close(client->pipe_cgi_out[1]);
        for (size_t k = 0; k < env.size(); k++)
            delete[] env_execve[k];
        delete[] env_execve;
        for (size_t k = 0; k < cmd.size(); k++)
            delete[] cmd_execve[k];
        delete[] cmd_execve;
    }
    return;
}

void treat_cgi(Server *server, Client *client, std::string cmd_path)
{
    std::string str = "";
    std::string cmd_cgi;
    std::vector<std::string> cmd(2);
    size_t i = 0;


    cmd_cgi = server->cgi_bin;
    for (; i < server->cgi.size(); i++)
    {
        if (cmd_path.find("." + server->cgi[i].first) != std::string::npos)
        {
            if (server->cgi_bin[server->cgi_bin.length() - 1] == '/' && server->cgi[i].second[0] == '/')
                server->cgi[i].second.erase(0, 1);
            else if (server->cgi_bin[server->cgi_bin.length() - 1] != '/' && server->cgi[i].second[0] != '/')
                server->cgi_bin += "/";
            cmd_cgi = server->cgi_bin + server->cgi[i].second;
            break ;
        }
    }
    if (i == server->cgi.size())
    {
        i = 0;
        cmd_cgi = server->cgi_bin + server->cgi[0].second;
    }
    if (client->request->get_method() == "POST")
    {
        if (cmd_path.find("." + server->cgi[i].first + "?") != std::string::npos)
        {
            str = cmd_path.substr(cmd_path.rfind(("." + server->cgi[i].first + "?")) + server->cgi[i].first.length() + 2);
            cmd_path = cmd_path.substr(0, cmd_path.rfind("." + server->cgi[i].first + "?") + server->cgi[i].first.length() + 1);
            cmd[0] = cmd_cgi;
            cmd[1] = cmd_path;  
        }
        else
        {
            cmd[0] = cmd_cgi;
            cmd[1] = cmd_path;
        } 
        cgi_exec(cmd, cgi_env(str, client, server, cmd_path), client);
    }
    if (client->request->get_method() == "GET")
    {
        if (cmd_path.find("." + server->cgi[i].first + "?") != std::string::npos)
        {
            cmd_path = cmd_path.substr(0, cmd_path.rfind("." + server->cgi[i].first + "?") + server->cgi[i].first.length() + 1);
            str = client->request->get_path().substr(client->request->get_path().rfind(("." + server->cgi[i].first + "?")) + server->cgi[i].first.length() + 2);
        }
        else
            str = "";
        cmd[0] = cmd_cgi;
        cmd[1] = cmd_path; 
        cgi_exec(cmd, cgi_env(str, client, server, cmd_path), client);
    }
    return;
}