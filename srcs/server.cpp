// Server side C program to demonstrate HTTP Server programming
#include "server.hpp"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <arpa/inet.h>

#define CO_MAX 20

bool exit_status = false;

void launch_browser(int port)
{
	std::string test, o;
	std::cout << std::endl;
	std::cout << BLUE << "[⊛] => " << WHITE << "Want to open page on browser on first port? (y/n)";
	while (1)
	{
		std::cin >> test;
		if (test == "y")
		{
			// std::cout << "Opening page on port " << port << std::endl;
			if (MAC == 1)
				o = "open http://localhost:"; // --> mac
			else
				o = "xdg-open http://localhost:"; // --> linux
			o += intToStr(port);
			system(o.c_str());
			break;
		}
		if (test == "n")
		{
			break;
		}
	}
	system("clear");
	std::cout << RED << "   _      __    __   ____            \n  | | /| / /__ / /  / __/__ _____  __\n  | |/ |/ / -_) _ \\_\\ \\/ -_) __/ |/ /\n  |__/|__/\\__/_.__/___/\\__/_/  |___/ \n " << BLUE << "\n⎯⎯  jcluzet  ⎯  alebross ⎯  amanchon  ⎯⎯\n\n"
			  << RESET;
}

Config check_config(int argc, char const *argv[])
{
	std::string tmp;
	// std::cout << "___       __    ______ ________                               \n__ |     / /_______  /___  ___/______________   ______________\n__ | /| / /_  _ \\_  __ \\____ \\_  _ \\_  ___/_ | / /  _ \\_  ___/\n__ |/ |/ / /  __/  /_/ /___/ //  __/  /   __ |/ //  __/  /    \n____/|__/  \\___//_.___//____/ \\___//_/    _____/ \\___//_/     \n\n";

	std::cout << WHITE << "___       __    ______ " << RED << "________                               \n"
			  << WHITE << "__ |     / /_______  /" << RED << "___  ___/______________   ______________\n"
			  << WHITE << "__ | /| / /_  _ \\_  __ \\" << RED << "____ \\_  _ \\_  ___/_ | / /  _ \\_  ___/\n"
			  << WHITE << "__ |/ |/ / /  __/  /_/ /" << RED << "___/ //  __/  /   __ |/ //  __/  /    \n"
			  << WHITE << "____/|__/  \\___//_.___/" << RED << "/____/ \\___//_/    _____/ \\___//_/     \n\n"
			  << WHITE;
	if (argc == 1 || strcmp("--debug", argv[1]) == 0)
	{
		tmp = "config/default.conf";
	}
	else
		tmp = argv[1];
	if (access(tmp.c_str(), F_OK) == -1)
	{
		std::cout << WHITE << "[" << BLUE << "⊛" << WHITE << "] => " << WHITE << "Config file " << RESET << tmp << WHITE << " not found" << std::endl;
		exit(EXIT_FAILURE);
	}

	Config conf(tmp);
	if (!conf.valid)
	{
		// std::cout << WHITE << "[" << RED << "⊛" << WHITE << "] => " << RESET << tmp << WHITE << " Configuration ERROR" << std::endl;
		std::cout << std::endl
				  << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv" << WHITE << " : " << RESET << "Configuration ERROR" << std::endl;

		exit(EXIT_FAILURE);
	}
	std::cout << WHITE << "[" << getHour() << "] START Web" << RED << "Serv" << WHITE << "   ";
	return conf;
}

void output(std::string client_data, std::string server_data, std::string request_cast)
{
	(void)server_data;
	(void)client_data;
	std::cout << WHITE << "\nRequest: \n"
			  << RESET << client_data << std::endl;
	std::cout << WHITE << "\nRequest CAST: \n"
			  << RESET << request_cast << std::endl;

	std::cout << WHITE << "\nOUR RESPONSE: " << RESET << std::endl
			  << server_data << std::endl;
}

sockaddr_in SocketAssign(int port, int *server_fd)
{
	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	// bind of the socket to assign the port
	std::string load = "\\-/|\\";

	if (bind(*server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		while (bind(*server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		{
			std::cout << RED << "[⊛] => " << WHITE << "PORT " << port << " Already in use." << RESET << std::endl;
			port++;
			address.sin_port = htons(port);
		}
		std::cout << GREEN << "[⊛] => " << WHITE << "We have change the port number to " << GREEN << port << RESET << std::endl;
	}
	if (listen(*server_fd, 3) < 0)
	{
		perror("In listen");
		std::cout << std::endl
				  << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv" << RESET << std::endl;

		exit(EXIT_FAILURE);
	}

	// open browser at launch
	return (address);
}

void build_fd_set(int *listen_sock, Config* conf, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds)
{
	size_t i;
	size_t j;
	FD_ZERO(read_fds);
	for (j = 0; j < conf->server.size(); ++j)
	{
		if (listen_sock[j] != -1)
			FD_SET(listen_sock[j], read_fds);
		for (i = 0; i < conf->server[j].client.size(); ++i)
		{
			if (conf->server[j].client[i].socket != -1)
				FD_SET(conf->server[j].client[i].socket, read_fds);
			if (conf->server[j].client[i].pipe_cgi_out[0] != -1)
				FD_SET(conf->server[j].client[i].pipe_cgi_out[0], read_fds);
		}
	}

	FD_ZERO(write_fds);
	for (j = 0; j < conf->server.size(); ++j)
	{
		for (i = 0; i < conf->server[j].client.size(); ++i)
		{
			if (conf->server[j].client[i].socket != -1)
				FD_SET(conf->server[j].client[i].socket, write_fds);
			if (conf->server[j].client[i].pipe_cgi_in[1] != -1)
				FD_SET(conf->server[j].client[i].pipe_cgi_in[1], write_fds);
		}
	}

	(void)except_fds;
/*	FD_ZERO(except_fds);
	for (j = 0; j < conf->server.size(); ++j)
	{
		if (listen_sock[j] != -1)
			FD_SET(listen_sock[j], except_fds);
		for (i = 0; i < conf->server[j].client.size(); ++i)
		{
			if (conf->server[j].client[i].socket != -1)
				FD_SET(conf->server[j].client[i].socket, except_fds);
		}
	}*/
	return;
}

// include atoi lib
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
	int new_socket;
	int valread;
	struct sockaddr_in address;
	Config conf(check_config(argc, argv));
	int listen_sock[conf.server.size()];
	signal(SIGINT, quit_sig);

	if (argc >= 2 && !strcmp(argv[2], "--confdebug"))
		std::cout << conf << std::endl;

	for (size_t i = 0; i < conf.server.size(); i++)
	{
		if ((listen_sock[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			perror("In socket");
			std::cout << std::endl
					  << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv" << RESET << std::endl;

			exit(EXIT_FAILURE);
		}
		fcntl(listen_sock[i], F_SETFL, O_NONBLOCK);

		int reuse = 1;
		if (setsockopt(listen_sock[i], SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0)
		{
			perror("set sockopt");
			return -1;
		}

		address = SocketAssign(atoi(conf.server[i].port[0].c_str()), &listen_sock[i]); // uniquement le premier port (port 0)
	}
	int addrlen = sizeof(address);
	int high_sock;
	fd_set read_fds;
	fd_set write_fds;
	//fd_set except_fds;
	std::cout << "                     ";
	for (size_t j = 0; j < conf.server.size(); j++)
		std::cout << RED << "⊛" << WHITE << conf.server[j].port[0] << "  ";
	std::cout << RESET << std::endl
			  << std::endl;

	while (1)
	{
		build_fd_set(&listen_sock[0], &conf, &read_fds, &write_fds, NULL);

		high_sock = -1;
		for (size_t i = 0; i < conf.server.size(); ++i)
		{
			if (listen_sock[i] > high_sock)
				high_sock = listen_sock[i];
		}
		for (size_t j = 0; j < conf.server.size(); j++)
		{
			for (size_t i = 0; i < conf.server[j].client.size(); i++)
			{
				if (high_sock < conf.server[j].client[i].socket)
					high_sock = conf.server[j].client[i].socket;
				if (high_sock < conf.server[j].client[i].pipe_cgi_in[1])
					high_sock = conf.server[j].client[i].pipe_cgi_in[1];
				if (high_sock < conf.server[j].client[i].pipe_cgi_out[0])
					high_sock = conf.server[j].client[i].pipe_cgi_out[0];
			}
		}

		int activity = select(high_sock + 1, &read_fds, &write_fds, NULL, NULL);
		if (exit_status == true)
			return (0);
		if (activity < 0)
		{
			perror("select error");
			std::cout << std::endl
					  << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv" << RESET << std::endl;

			exit(EXIT_FAILURE);
		}

		for (size_t j = 0; j < conf.server.size(); j++)
		{
			if (FD_ISSET(listen_sock[j], &read_fds))
			{
				if ((new_socket = accept(listen_sock[j], (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
				{
					perror("In accept");
					std::cout << std::endl
							  << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv" << RESET << std::endl;
					exit(EXIT_FAILURE);
				}
				std::cout << GREEN << "[⊛ CONNECT]    => " << RESET << inet_ntoa(address.sin_addr) << WHITE << ":" << RESET << ntohs(address.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << GREEN << conf.server[j].port[0] << RESET << std::endl;
				fcntl(new_socket, F_SETFL, O_NONBLOCK);
				if (conf.server[j].client.size() < CO_MAX) // A mettre avec FD_ISSET ?
				{
					conf.server[j].client.push_back(Client(new_socket, address, &conf.server[j]));
				}
				else
				{
					close(new_socket);
					std::cout << RED << "[⊛ DISCONNECT] => " << RESET << inet_ntoa(address.sin_addr) << WHITE << ":" << RESET << ntohs(address.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << RED << conf.server[j].port[0] << RESET << std::endl;
				}
			}
		}

		Client*	client;

		char data[11];
		for (size_t j = 0; j < conf.server.size(); j++)
		{
			for (size_t i = 0; i < conf.server[j].client.size(); i++)
			{
				client = &conf.server[j].client[i];
				if (client->request->ready() == false && FD_ISSET(client->socket, &read_fds)) // read client
				{
					if ((valread = read(client->socket, data, 10)) <= 0)
					{
						std::cout << RED << "[⊛ DISCONNECT] => " << RESET << inet_ntoa(client->sockaddr.sin_addr) << WHITE << ":" << RESET << ntohs(client->sockaddr.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << RED << conf.server[j].port[0] << RESET << std::endl;
						close(client->socket);
						conf.server[j].client.erase(conf.server[j].client.begin() + i);
					}
					else
					{
						data[valread] = '\0';
					    client->request->add(data);
					    if (client->request->ready())
					    {
  							if (is_cgi(client->request) == true)
							{
								treat_cgi(&conf.server[j], client);
							}	        
						}
					}
				}
				else if (client->request->ready() == true && client->request->get_method() == "POST"
					&& client->pipe_cgi_in[1] != -1 && FD_ISSET(client->pipe_cgi_in[1], &write_fds)) //write cgi
				{
         			write(client->pipe_cgi_in[1], client->request->get_body().c_str(), client->request->get_body().length());
        			close(client->pipe_cgi_in[1]);
					client->pipe_cgi_in[1] = -1;
					client->pipe_cgi_in[0] = -1;
				}
				else if (client->request->ready() == true && is_cgi(client->request) && client->pipe_cgi_out[0] != -1
					&& FD_ISSET(client->pipe_cgi_out[0], &read_fds)) // read cgi
				{
					if ((valread = read(client->pipe_cgi_out[0], data, 10)) < 0)
			        {
						close(client->pipe_cgi_out[0]);
 			        	std::cout << RED << "[⊛ DISCONNECT] => " << RESET << inet_ntoa(client->sockaddr.sin_addr) << WHITE << ":" << RESET << ntohs(client->sockaddr.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << RED << conf.server[j].port[0] << RESET << std::endl;
						close(client->socket);
						conf.server[j].client.erase(conf.server[j].client.begin() + i);
   				    }
					else if (valread == 0)
					{
    					close(client->pipe_cgi_out[0]);
						client->pipe_cgi_out[1] = -1;
						client->pipe_cgi_out[0] = -1;
     		   			data[valread] = '\0';
						client->response->cgi_response += data;
					}
					else
					{
						data[valread] = '\0';
						client->response->cgi_response += data;
					}
				}
				else if (client->request->ready() == true && client->pipe_cgi_out[0] == -1 && FD_ISSET(client->socket, &write_fds)) // write client
				{
					client->response->makeResponse();
					//std::cout << client->response->get_response() << std::endl;
					write(client->socket, client->response->get_response().c_str(), client->response->get_response().length());
					if (client->response->getstat() == 400)
					{
						std::cout << RED << "[⊛ DISCONNECT] => " << RESET << inet_ntoa(client->sockaddr.sin_addr) << WHITE << ":" << RESET << ntohs(client->sockaddr.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << RED << conf.server[j].port[0] << RESET << std::endl;
						close(client->socket);
						conf.server[j].client.erase(conf.server[j].client.begin() + i);
					}
					else
					{
						output_log(client->response->getstat(), client->response->get_pathfile());
						client->response->clear();
						client->request->clear();
					}
					//if (LOG == 1)
					//	output(client_data, response.get_response(), request[j][i].get_request());
				}
			}
		}
	}
	return 0;
}

void quit_sig(int sig)
{
	if (SIGINT == sig)
		exit_status = true;
	std::cout << std::endl
			  << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv" << RESET << std::endl;
}