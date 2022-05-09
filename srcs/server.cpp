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
	if (conf.server.empty())
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

sockaddr_in ListenSocketAssign(int port, int *listen_sock)
{
	struct sockaddr_in address;

	if ((*listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("In socket");
		std::cout << std::endl
				  << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv" << RESET << std::endl;
			exit(EXIT_FAILURE);
	}
	fcntl(*listen_sock, F_SETFL, O_NONBLOCK);
		int reuse = 1;
	if (setsockopt(*listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0)
	{
		perror("set sockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	if (bind(*listen_sock, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		while (bind(*listen_sock, (struct sockaddr *)&address, sizeof(address)) < 0)
		{
			std::cout << RED << "[⊛] => " << WHITE << "PORT " << port << " Already in use." << RESET << std::endl;
			port++;
			address.sin_port = htons(port);
		}
		std::cout << GREEN << "[⊛] => " << WHITE << "We have change the port number to " << GREEN << port << RESET << std::endl;
	}
	if (listen(*listen_sock, 3) < 0)
	{
		perror("In listen");
		std::cout << std::endl
				  << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv" << RESET << std::endl;

		exit(EXIT_FAILURE);
	}
	return (address);
}

int build_fd_set(int *listen_sock, Config* conf, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds)
{
	size_t i;
	size_t j;
	int	high_sock = -1;
	(void)except_fds;

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
			if (conf->server[j].client[i].fd_file != -1)
				FD_SET(conf->server[j].client[i].fd_file, read_fds);	
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

	for (size_t i = 0; i < conf->server.size(); ++i)
	{
		if (listen_sock[i] > high_sock)
			high_sock = listen_sock[i];
	}
	for (size_t j = 0; j < conf->server.size(); j++)
	{
		for (size_t i = 0; i < conf->server[j].client.size(); i++)
		{
			if (high_sock < conf->server[j].client[i].socket)
				high_sock = conf->server[j].client[i].socket;
			if (high_sock < conf->server[j].client[i].pipe_cgi_in[1])
				high_sock = conf->server[j].client[i].pipe_cgi_in[1];
			if (high_sock < conf->server[j].client[i].pipe_cgi_out[0])
				high_sock = conf->server[j].client[i].pipe_cgi_out[0];
			if (high_sock <conf->server[j].client[i].fd_file)
				high_sock = conf->server[j].client[i].fd_file;
		}
	}
	return (high_sock);
}

// include atoi lib
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void	WriteResponse(Config* conf, Client* client, size_t j, size_t i)
{
	int	valwrite;
	
	if (client->response->writing == false)
	{
		client->response->makeResponse();
		client->response->transfer = client->response->get_response();
		client->response->writing = true;
	}
	if (client->response->transfer.length() < BUFFER_SIZE * 10)
	{
		valwrite = write(client->socket, client->response->transfer.c_str(), client->response->transfer.length());
		client->response->writing = false;
		//std::cout << client->response->transfer << std::endl;
	}
	else
	{
		valwrite = write(client->socket, client->response->transfer.c_str(), BUFFER_SIZE * 10);
		client->response->transfer = client->response->transfer.erase(0, BUFFER_SIZE * 10);
	}
	if (valwrite < 0)
	{
		client->response->setStatus(500);
	}
	else if (valwrite == 0)
	{
	} // ???
	//std::cout << valwrite << " " << client->response->get_response().length();
	if (client->response->getstat() == 400 || client->response->getstat() == 500)
	{
		std::cout << RED << "[⊛ DISCONNECT] => " << RESET << inet_ntoa(client->sockaddr.sin_addr) << WHITE << ":" << RESET << ntohs(client->sockaddr.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << RED << conf->server[j].port << RESET << std::endl;
		close(client->socket);
		conf->server[j].client.erase(conf->server[j].client.begin() + i);
		i--;
	}
	else if (client->response->writing == false)
	{
		output_log(client->response->getstat(), client->response->get_pathfile());
		client->response->clear();
		client->request->clear();
	}
	//if (LOG == 1)
	//	output(client_data, response.get_response(), request[j][i].get_request());
	return ;
}

void	ReadFile(Client *client)
{
	int valread;
	char data[BUFFER_SIZE + 1];

	if ((valread = read(client->fd_file, data, BUFFER_SIZE)) < 0)
    {
		close(client->fd_file); // error 500
		client->fd_file = -1;
    }
	else if (valread == 0)
	{
		close(client->fd_file);
		client->fd_file = -1;
	}
	else
	{
		data[valread] = '\0';
		client->response->transfer += data;
	}
	return ;
}

void	ReadCGI(Client* client)
{
	int valread;
	char data[BUFFER_SIZE + 1];

	if ((valread = read(client->pipe_cgi_out[0], data, BUFFER_SIZE)) < 0)
    {
		close(client->pipe_cgi_out[0]); // error 500
		client->pipe_cgi_out[1] = -1;
		client->pipe_cgi_out[0] = -1;
    }
	else if (valread == 0)
	{
		close(client->pipe_cgi_out[0]);
		client->pipe_cgi_out[1] = -1;
		client->pipe_cgi_out[0] = -1;
	}
	else
	{
		data[valread] = '\0';
		client->response->transfer += data;
	}
	return ;
}

void	WriteCGI(Client* client)
{
	int valwrite;

	valwrite = write(client->pipe_cgi_in[1], client->request->get_body().c_str(), client->request->get_body().length());
	if (valwrite < 0)
	{
		close(client->pipe_cgi_in[1]); // error 500
		client->pipe_cgi_in[1] = -1;
		client->pipe_cgi_in[0] = -1;
	}
	else if (valwrite == 0 || valwrite == static_cast<int>(client->request->get_body().length()))
	{
		close(client->pipe_cgi_in[1]);
		client->pipe_cgi_in[1] = -1;
		client->pipe_cgi_in[0] = -1;
	}
	return ;
}

void	ReadRequest(Config* conf, Client* client, size_t j, size_t i)
{
	int valread;
	char data[BUFFER_SIZE + 1];

	if ((valread = read(client->socket, data, BUFFER_SIZE)) < 0)
	{
		client->response->setStatus(500);
		// error 500, envoyer rep avant
	}
	else if (valread == 0)
	{
		client->response->setStatus(500);
	}
	else
	{
		data[valread] = '\0';
	    client->request->add(data);
	    if (client->request->ready())
	    {
			//std::cout << client->request->get_request() << " !" << std::endl;
			if (is_cgi(client->request) == true)
			{
				treat_cgi(&conf->server[j], client);
			}
			else
				client->fd_file = client->response->treatRequest();
		}
	}
	return ;
}

void	NewClients(int* listen_sock, Config* conf, fd_set* read_fds)
{
	int new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	for (size_t j = 0; j < conf->server.size(); j++)
	{
		if (conf->server[j].client.size() < CO_MAX && FD_ISSET(listen_sock[j], read_fds))
		{
			if ((new_socket = accept(listen_sock[j], (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			{
				// pas grave (?)
				return ;
			}
			fcntl(new_socket, F_SETFL, O_NONBLOCK);
			conf->server[j].client.push_back(Client(new_socket, address, &conf->server[j]));
			std::cout << GREEN << "[⊛ CONNECT]    => " << RESET << inet_ntoa(address.sin_addr) << WHITE << ":" << RESET << ntohs(address.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << GREEN << conf->server[j].port << RESET << std::endl;
		}
	}
	return ;
}

int main(int argc, char const *argv[])
{
	int		high_sock;
	fd_set	read_fds;
	fd_set	write_fds;
	Client*	client;
	Config	conf(check_config(argc, argv));
	int		listen_sock[conf.server.size()];

	signal(SIGINT, quit_sig);

	if (argc >= 2 && !strcmp(argv[2], "--confdebug"))
		std::cout << conf << std::endl;

	for (size_t i = 0; i < conf.server.size(); i++)
		ListenSocketAssign(atoi(conf.server[i].port.c_str()), &listen_sock[i]);

	std::cout << "                     ";
	for (size_t j = 0; j < conf.server.size(); j++)
		std::cout << RED << "⊛" << WHITE << conf.server[j].port << "  ";
	std::cout << RESET << std::endl
			  << std::endl;

	while (1)
	{
		high_sock = build_fd_set(&listen_sock[0], &conf, &read_fds, &write_fds, NULL);

		int activity = select(high_sock + 1, &read_fds, &write_fds, NULL, NULL);
		if (exit_status == true) // ??
			return (0);
		if (activity < 0)
		{
			perror("select error");
			std::cout << std::endl
					  << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv" << RESET << std::endl;
			exit(EXIT_FAILURE);
		}

		NewClients(&listen_sock[0], &conf, &read_fds);

		for (size_t j = 0; j < conf.server.size(); j++)
		{
			for (size_t i = 0; i < conf.server[j].client.size(); i++)
			{
				client = &conf.server[j].client[i];
				if (client->request->ready() == false && FD_ISSET(client->socket, &read_fds)) // read client
				{
					ReadRequest(&conf, client, j, i);
				}
				else if (client->request->ready() == true && client->request->get_method() == "POST"
					&& client->pipe_cgi_in[1] != -1 && FD_ISSET(client->pipe_cgi_in[1], &write_fds)) //write cgi
				{
					WriteCGI(client);
				}
				else if (client->request->ready() == true && is_cgi(client->request) && client->pipe_cgi_in[1] == -1 && client->pipe_cgi_out[0] != -1
					&& FD_ISSET(client->pipe_cgi_out[0], &read_fds)) // read cgi
				{
					ReadCGI(client);
				}
				else if (client->request->ready() == true && is_cgi(client->request) == false //&& client->request->get_method() == "GET"
					&& client->fd_file != -1 && FD_ISSET(client->fd_file, &read_fds))
				{
					ReadFile(client);
				}
				else if (client->request->ready() == true && client->pipe_cgi_out[0] == -1
					&& client->fd_file == -1 && FD_ISSET(client->socket, &write_fds)) // write client
				{
					WriteResponse(&conf, client, j, i);
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