#include "Request.hpp"
#include "server.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <sstream>
/*
char**  cgi_cmd(Request* cmd)
{

}*/

char**  cgi_env(std::string cmd, std::string cgi_str, Request *request, Server* server)
{
    std::ostringstream convert;
    char **env = new char*[29];
    std::string str;
    str = "CONTENT_TYPE="; // The media type of the query data, such as text/html.
    if (request->get_method() == "GET")
        str += "";
    else
        str += request->get_header("Content-Type"); // ? 
    env[0] = new char[str.length() + 1];
    env[0] = std::strcpy(env[0], str.c_str());
    str = "CONTENT_LENGTH="; // The length of the query data (in bytes or the number of characters) passed to the CGI program through standard input (for POST).
    if (request->get_method() == "GET")
        str += "";
    else
        str += request->get_header("Content-Length"); // ?
    env[1] = new char[str.length() + 1];
    env[1] = std::strcpy(env[1], str.c_str());
    str = "GATEWAY_INTERFACE="; // The revision of the Common Gateway Interface that the server uses.
    str += "CGI/1.1";
    env[2] = new char[str.length() + 1];
    env[2] = std::strcpy(env[2], str.c_str());
    str = "PATH_INFO="; // Extra path information passed to a CGI program.
    if (request->get_method() == "GET")
        str += "/home/user42/Documents/Projets/webserv/groupe_git/www" + request->get_path().substr(0, request->get_path().find(".php?") + 4);
    else
        str += "/home/user42/Documents/Projets/webserv/groupe_git/www" + request->get_path();
    env[3] = new char[str.length() + 1];
    env[3] = std::strcpy(env[3], str.c_str());
    str = "PATH_TRANSLATED="; // The translated version of the path given by the variable PATH_INFO.
    if (request->get_method() == "GET")
        str += "/home/user42/Documents/Projets/webserv/groupe_git/www" + request->get_path().substr(0, request->get_path().find(".php?") + 4);
    else
        str += "/home/user42/Documents/Projets/webserv/groupe_git/www" + request->get_path();
    env[4] = new char[str.length() + 1];
    env[4] = std::strcpy(env[4], str.c_str());
    str = "QUERY_STRING="; // The query information passed to the program. It is appended to the URL following a question mark (?).
    str += cgi_str;
    env[5] = new char[str.length() + 1];
    env[5] = std::strcpy(env[5], str.c_str());
    str = "AUTH_TYPE="; // The authentication method used to validate a user. See REMOTE_IDENT and REMOTE_USER.
    str += "Basic";
    env[6] = new char[str.length() + 1];
    env[6] = std::strcpy(env[6], str.c_str());

    str = "REMOTE_ADDR="; // The remote IP address from which the user is making the request.
    str += inet_ntoa(request->get_sockaddr().sin_addr); // A CHANGER
    env[7] = new char[str.length() + 1];
    env[7] = std::strcpy(env[7], str.c_str());
    str = "REMOTE_PORT="; // The remote IP address from which the user is making the request.
    env[8] = new char[str.length() + 6];
    convert << static_cast<int>(ntohs(request->get_sockaddr().sin_port));
    str += convert.str();
    env[8] = std::strcpy(env[8], str.c_str());
    str = "REMOTE_HOST="; // The remote hostname from which the user is making the request.
    str += "";
    env[9] = new char[str.length() + 1];
    env[9] = std::strcpy(env[9], str.c_str());
    str = "REMOTE_USER="; // The authenticated name of the user making the query.
    str += request->get_header("From");
    env[10] = new char[str.length() + 1];
    env[10] = std::strcpy(env[10], str.c_str());

    str = "REQUEST_METHOD="; // The method with which the information request was issued (e.g., GET, POST, HEAD).
    str += request->get_method();
    env[11] = new char[str.length() + 1];
    env[11] = std::strcpy(env[11], str.c_str());
    str = "SCRIPT_NAME="; //The virtual path (e.g., /cgi-bin/program.pl) of the script being executed. 
    str += cmd.substr(cmd.find("/cgi-bin/"), cmd.length());
    env[12] = new char[str.length() + 1];
    env[12] = std::strcpy(env[12], str.c_str());

    str = "SERVER_NAME="; // The server's hostname or IP address. Equivalent HTTP_HOST
    str += server->ip; // OU HOST ??
    env[13] = new char[str.length() + 1];
    env[13] = std::strcpy(env[13], str.c_str());
    str = "SERVER_PROTOCOL="; // The name and revision number of the server protocol.
    str += "HTTP/1.1";
    env[14] = new char[str.length() + 1];
    env[14] = std::strcpy(env[14], str.c_str());
    str = "SERVER_PORT="; // The port number of the host on which the server is running.
    str += server->port;
    env[15] = new char[str.length() + 1];
    env[15] = std::strcpy(env[15], str.c_str());
    str = "SERVER_SOFTWARE="; // The name and version of the server software that is answering the client request
    str += "WebServ/1.0";
    env[16] = new char[str.length() + 1];
    env[16] = std::strcpy(env[16], str.c_str());

    str = "HTTP_ACCEPT="; // A list of the media types that the client can accept.
    str += request->get_header("Accept");
    env[17] = new char[str.length() + 1];
    env[17] = std::strcpy(env[17], str.c_str());
    str = "HTTP_COOKIE="; // A list of cookies defined for that URL.
    str += request->get_header("Cookie");
    env[18] = new char[str.length() + 1];
    env[18] = std::strcpy(env[18], str.c_str());
    str = "HTTP_ACCEPT_LANGUAGE="; // Langage accepté par le client.
    str += request->get_header("Accept-Language");
    env[19] = new char[str.length() + 1];
    env[19] = std::strcpy(env[19], str.c_str());
    str = "HTTP_REFERER="; // The URL of the document the client read before accessing the CGI program.
    str += request->get_header("Referer");
    env[20] = new char[str.length() + 1];
    env[20] = std::strcpy(env[20], str.c_str());
    str = "HTTP_USER_AGENT="; // The browser the client is using to issue the request.
    str += request->get_header("User-Agent");
    env[21] = new char[str.length() + 1];
    env[21] = std::strcpy(env[21], str.c_str());
    str = "HTTP_ACCEPT_ENCODING="; 
    str += request->get_header("Accept-Encoding");
    env[22] = new char[str.length() + 1];
    env[22] = std::strcpy(env[22], str.c_str());
    str = "HTTP_ACCEPT_CHARSET="; 
    str += request->get_header("Accept_Charsets");
    env[23] = new char[str.length() + 1];
    env[23] = std::strcpy(env[23], str.c_str());

    str = "DOCUMENT_ROOT="; // The directory from which web documents are served. //
    str += "/home/user42/Documents/Projets/webserv/groupe_git/www/"; // --> Besoin du chemin complet
    env[24] = new char[str.length() + 1];
    env[24] = std::strcpy(env[24], str.c_str());
    
    str = "REQUEST_URI="; // file_path //
    if (request->get_method() == "GET")
        str += request->get_path().substr(0, request->get_path().find(".php?") + 4);
    else
        str += request->get_path();
    env[25] = new char[str.length() + 1];
    env[25] = std::strcpy(env[25], str.c_str());
    str = "REDIRECT_STATUS=";
    str += "200";
    env[26] = new char[str.length() + 1];
    env[26] = std::strcpy(env[26], str.c_str());

    str = "SCRIPT_FILENAME=";
    if (request->get_method() == "GET")
        str += "/home/user42/Documents/Projets/webserv/groupe_git/www" + request->get_path().substr(0, request->get_path().find(".php?") + 4);
    else
        str += "/home/user42/Documents/Projets/webserv/groupe_git/www" + request->get_path();
    env[27] = new char[str.length() + 1];
    env[27] = std::strcpy(env[27], str.c_str());
    env[28] = NULL;

    return (env);
}
/*
  if (config_.getMethod() == "POST") {
		cgi_env_["CONTENT_TYPE"] = req_headers_["Content-Type"];
		cgi_env_["CONTENT_LENGTH"] = ft::to_string(req_body_.length());
	}
	cgi_env_["GATEWAY_INTERFACE"] = "CGI/1.1";
  cgi_env_["PATH_INFO"] = file_path_;
	cgi_env_["PATH_TRANSLATED"] = file_path_;
  cgi_env_["QUERY_STRING"] = config_.getQuery();
  cgi_env_["REMOTE_ADDR"] = config_.getClient().getAddr();

  if (config_.getAuth() != "off") {
    cgi_env_["AUTH_TYPE"] = "Basic";
    cgi_env_["REMOTE_IDENT"] = config_.getAuth().substr(0, config_.getAuth().find(':'));
    cgi_env_["REMOTE_USER"] = config_.getAuth().substr(0, config_.getAuth().find(':'));
  }

  cgi_env_["REQUEST_METHOD"] = config_.getMethod();
	cgi_env_["REQUEST_URI"] = file_path_;

  cgi_env_["SCRIPT_NAME"] = cgi_path_;
	cgi_env_["SERVER_NAME"] = config_.getHost();
	cgi_env_["SERVER_PROTOCOL"] = config_.getProtocol();
	cgi_env_["SERVER_PORT"] = ft::to_string(config_.getPort());
  cgi_env_["SERVER_SOFTWARE"] = "WEBSERV/1.0";
*/

void    afficher_env(char** env)
{
    std::string str;
    for (size_t i = 0; env[i] != NULL; i++)
    {
        std::cout << env[i] << std::endl;
    }
    std::cout << std::endl;
    return ;
}

std::string    cgi_exec(char **cmd, char **env, Request* request)
{
    pid_t   pid;
    int pfd[2];
    int status;
    char   ret[30001];
    int rd;

    //afficher_env(env);
    if (pipe(pfd) < 0)
    {
        exit(1);
    }
    if ((pid = fork()) < 0)
    {
        exit(1);
    }
    if (pid == 0)
    {
        close(pfd[0]);
        dup2(pfd[1], 1);
        if (request->get_method() == "POST")
        {
           write(0, request->get_body().c_str(), request->get_body().length());
        }
        if (execve(cmd[0], cmd, env) < 0)
        {
    		for (size_t k = 0; k < 24; k++)
			    delete [] env[k];
	    	delete [] env;
            delete [] cmd[1];
            delete [] cmd[0];
            delete [] cmd;
            close(pfd[1]);
            exit(1);
        }
        exit(1);
    }
    else
    {
        close(pfd[1]);
        wait(&status);
        if ((rd = read(pfd[0], ret, 30000)) < 0)
        {
            exit(1);
        }
        close(pfd[0]);
        ret[rd] = '\0';
		for (size_t k = 0; k < 28; k++)
			delete [] env[k];
		delete [] env;
    }
    return (ret);
}