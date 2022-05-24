#include "server.hpp"

bool exit_status = false;

void	checkHost(Client* client, std::string ip, std::string port, std::vector<std::string> server_name)
{
	if (client->request->get_header("Host") != ip + ":" + port)
	{
		for (size_t k = 0; k < server_name.size(); k++)
		{
			if (server_name[k] == client->request->get_header("Host")
				|| server_name[k] + ":" + port == client->request->get_header("Host"))
				break ;
			else if (k + 1 == server_name.size())
			{
				if (ip == "0.0.0.0" && client->request->get_header("Host").find(":") != std::string::npos
					&& client->request->get_header("Host").find(":") == client->request->get_header("Host").find_last_of(":")
					&& client->request->get_header("Host").substr(client->request->get_header("Host").find(":") + 1, client->request->get_header("Host").length()) == port)
					break ;
				client->response->setStatus(400);				
				client->fd_file = client->response->openFile();
				return ;
			}
		}
	}
	return ;
}

void	launch_treatment(Client* client, Server* conf_o)
{
	Server*		conf_local;
	std::string	location;
	std::string	jquery = "";
	std::string	tmp;

	location = apply_location(client->request->get_path(), conf_o, &conf_local);
	if (is_directory(conf_local->root + client->request->get_path()) == false && client->request->get_path()[client->request->get_path().length() - 1] == '/')
		client->request->set_path(client->request->get_path().substr(0, client->request->get_path().length() - 1));
	client->response->setConf(conf_local);
	if (conf_o->root != conf_local->root)
	{
		client->request->set_path(client->request->get_path().erase(1, location.length()));
		if (client->request->get_path().compare(0, 2, "//"))
			client->request->set_path(client->request->get_path().erase(1, 1));	
	}
	if ((client->request->get_method() == "POST" && !conf_local->methods[1]) || (client->request->get_method() == "GET" && !conf_local->methods[0]) || (client->request->get_method() == "DELETE" && !conf_local->methods[2])) // check error 405 Method not allowed
	{
		client->response->setStatus(405);
		client->fd_file = client->response->openFile();
		return ;
	}
    for (size_t i = 0; i < conf_local->cgi.size(); i++)
    {
        if (client->request->get_method() == "GET" && client->request->get_path().find("." + conf_local->cgi[i].first + "?") != std::string::npos)
		{
            jquery = client->request->get_path().substr(client->request->get_path().find_last_of("." + conf_local->cgi[i].first + "?"), std::string::npos);
			tmp = client->request->get_path().substr(0, client->request->get_path().find_last_of("." + conf_local->cgi[i].first + "?"));
			client->request->set_path(url_decode(tmp) + jquery);
			break ;
		}
        if (client->request->get_method() == "POST" && client->request->get_path().find("." + conf_local->cgi[i].first) != std::string::npos)
		{
			tmp = client->request->get_path().substr(0, client->request->get_path().find_last_of("." + conf_local->cgi[i].first));
			client->request->set_path(url_decode(tmp) + jquery);
			break ;
		}
		
    }
	checkHost(client, conf_o->ip, conf_o->port, conf_o->server_name);
	if (is_cgi(client->request, conf_local) == true)
	{
   		if(ft_atoi(conf_local->client_max_body_size.c_str()) < client->request->get_body().length())
		{
			client->response->setStatus(413);
			client->fd_file = client->response->openFile();
			return ;
		}
		treat_cgi(conf_local, client);
	}
	else
		client->fd_file = client->response->treatRequest();
	return ;
}

int build_fd_set(int *listen_sock, Config *conf, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds)
{
	size_t i;
	size_t j;
	int high_sock = -1;
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
			if (high_sock < conf->server[j].client[i].fd_file)
				high_sock = conf->server[j].client[i].fd_file;
		}
	}
	return (high_sock);
}

bool ListenSocketAssign(int port, int *listen_sock, std::string ip)
{
	struct sockaddr_in address;

	if ((*listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv... ERROR !" << std::endl << std::endl;
		perror("Socket");
		std::cout << RESET;
		return 1;
	}
	fcntl(*listen_sock, F_SETFL, O_NONBLOCK);
	int reuse = 1;
	if (setsockopt(*listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0)
	{
		std::cout << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv... ERROR !" << std::endl << std::endl;
		perror("SetSockOpt");
		std::cout << RESET;
		return 1;
	}

	if (ip == "0.0.0.0")
		address.sin_addr.s_addr = INADDR_ANY;
	else if (ip == "127.0.0.1")
		address.sin_addr.s_addr = INADDR_LOOPBACK;
	else
		address.sin_addr.s_addr = inet_addr(ip.c_str()); // fonctionne aussi avec "0.0.0.0" et "127.0.0.1"	
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	if (bind(*listen_sock, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cout << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv... ERROR !" << std::endl << std::endl;
		perror("Bind");
		std::cout << RESET;
		return 1;
	}
	if (listen(*listen_sock, MAX_QUEUED_CONNEXIONS) < 0)
	{
		std::cout << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv... ERROR !" << std::endl << std::endl;
		perror("Listen");
		std::cout << RESET;
		return 1;
	}
	return 0;
}

void WriteResponse(Config *conf, Client *client, size_t j, size_t i)
{
	int valwrite;

	if (client->response->writing == false)
	{
		client->response->makeResponse();
		if (client->request->get_header("Accept").find(client->response->c_type()) == std::string::npos && (client->response->getstat() == 0 || client->response->getstat() == 200) // error 406 not acceptable
		 && client->request->get_header("Accept") != "" && client->request->get_header("Accept").find("*/*") == std::string::npos)
		{
			client->response->clear();
			client->response->setStatus(406);
			client->fd_file = client->response->openFile();
			client->response->transfer = "";
			return;
		}
		client->response->transfer = client->response->get_response();
		client->response->writing = true;
	}

	valwrite = write(client->socket, client->response->transfer.c_str(), client->response->transfer.length());
	if (valwrite < 0)
	{
		if(CONNEXION_LOG == 1)
			std::cout << RED << "[⊛ DISCONNECT] => " << RESET << inet_ntoa(client->sockaddr.sin_addr) << WHITE << ":" << RESET << ntohs(client->sockaddr.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << RED << conf->server[j].port << RESET << std::endl;
		close(client->socket);
		conf->server[j].client.erase(conf->server[j].client.begin() + i);
		i--;
		return;
	}
	else if (valwrite == 0 || valwrite == static_cast<int>(client->response->transfer.length()))
	{
		client->response->writing = false;
	}
	if (client->response->writing == false && (client->response->getstat() == 400 || client->response->getstat() == 500 || client->request->get_header("Connection") == "close"))
	{
		if(CONNEXION_LOG == 1)
			std::cout << RED << "[⊛ DISCONNECT] => " << RESET << inet_ntoa(client->sockaddr.sin_addr) << WHITE << ":" << RESET << ntohs(client->sockaddr.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << RED << conf->server[j].port << RESET << std::endl;
		close(client->socket);
		conf->server[j].client.erase(conf->server[j].client.begin() + i);
		i--;
	}
	else if (client->response->writing == false)
	{
		if(LOG == 1)
			output_log(client->response->getstat(), client->response->get_fpath());
		if (conf->get_debug() == true)
			output_debug(client->request->get_request(), client->response->get_response());
		client->response->clear();
		client->request->clear();
	}
	return;
}

void ReadFile(Client *client)
{
	int valread;
	char data[BUFFER_SIZE + 1];

	if ((valread = read(client->fd_file, data, BUFFER_SIZE)) < 0)
	{
		client->fd_file = -1;
		client->response->setStatus(500);
		client->fd_file = client->response->openFile();
		client->response->transfer = "";
	}
	else if (valread == 0)
	{
		close(client->fd_file);
		client->fd_file = -1;
	}
	else
	{
		data[valread] = '\0';
		client->response->transfer += std::string(data, valread);
	}
	return;
}

void ReadCGI(Client *client)
{
	int valread;
	char data[BUFFER_SIZE + 1];

	if ((valread = read(client->pipe_cgi_out[0], data, BUFFER_SIZE)) < 0)
	{
		client->response->setStatus(500);
		client->fd_file = client->response->openFile();
		close(client->pipe_cgi_out[0]);
		client->pipe_cgi_out[1] = -1;
		client->pipe_cgi_out[0] = -1;
		client->response->transfer = "";
	}
	else if (valread == 0)
	{
		close(client->pipe_cgi_out[0]);
		client->pipe_cgi_out[1] = -1;
		client->pipe_cgi_out[0] = -1;
        if (client->response->transfer.length() > 10 && client->response->transfer.find("Status: ") != std::string::npos
			&& client->response->transfer.find("Status: ") < client->response->transfer.find("\r\n\r\n"))
        {
			int tmp_status = atoi(client->response->transfer.substr(client->response->transfer.find("Status: ") + 8, 3).c_str());
			if (tmp_status <= 0)
				tmp_status = 200;
            client->response->setStatus(tmp_status);
			if (tmp_status != 200 && tmp_status != 201)
			{
				if (client->response->transfer.find("\r\n\r\n") != std::string::npos)
					client->response->transfer = "CGI: " + client->response->transfer.substr(client->response->transfer.find("\r\n\r\n") + 4, std::string::npos);
				else 
					client->response->transfer = "";
		    	client->fd_file = client->response->openFile();
			}
        }
		else
			client->response->setStatus(200);
		if (client->response->transfer.length() > 0 && LOG == 1)
        	std::cout << GREEN << "[⊛ CGI]        => " << WHITE << client->response->transfer.substr(0, 20) + "....." << RESET << std::endl;
		else if (LOG == 1)
        	std::cout << GREEN << "[⊛ CGI]        => " << RED << "NOT VALID CGI-BIN" << RESET << std::endl;
	}
	else
	{
		data[valread] = '\0';
		client->response->transfer += std::string(data, valread);
	}
	return;
}

void WriteCGI(Client *client)
{
	int valwrite;

	valwrite = write(client->pipe_cgi_in[1], client->request->get_body().c_str(), client->request->get_body().length());
	if (valwrite < 0)
	{
		client->response->setStatus(500);
		client->fd_file = client->response->openFile();
		close(client->pipe_cgi_in[1]);
		client->pipe_cgi_in[1] = -1;
		client->pipe_cgi_in[0] = -1;
		close(client->pipe_cgi_out[0]);
		client->pipe_cgi_out[1] = -1;
		client->pipe_cgi_out[0] = -1;
		client->response->transfer = "";
	}
	else if (valwrite == 0 || valwrite == static_cast<int>(client->request->get_body().length()))
	{
		close(client->pipe_cgi_in[1]);
		client->pipe_cgi_in[1] = -1;
		client->pipe_cgi_in[0] = -1;

	}
	return;
}

void ReadRequest(Config *conf, Client *client, size_t j, size_t i)
{
	int valread;
	char data[BUFFER_SIZE + 1];

	if ((valread = read(client->socket, data, BUFFER_SIZE)) < 0)
	{
		if(CONNEXION_LOG == 1)
			std::cout << RED << "[⊛ DISCONNECT] => " << RESET << inet_ntoa(client->sockaddr.sin_addr) << WHITE << ":" << RESET << ntohs(client->sockaddr.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << RED << conf->server[j].port << RESET << std::endl;
		close(client->socket);
		conf->server[j].client.erase(conf->server[j].client.begin() + i);
		i--;
		return;
	}
	else if (valread == 0)
	{
		if(CONNEXION_LOG == 1)
			std::cout << RED << "[⊛ DISCONNECT] => " << RESET << inet_ntoa(client->sockaddr.sin_addr) << WHITE << ":" << RESET << ntohs(client->sockaddr.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << RED << conf->server[j].port << RESET << std::endl;
		close(client->socket);
		conf->server[j].client.erase(conf->server[j].client.begin() + i);
		i--;
		return;
	}
	else
	{
		data[valread] = '\0';
		client->request->add(std::string(data, valread));
		if (client->request->ready())
		{
			launch_treatment(client, &conf->server[j]);
		}
	}
	return;
}

void NewClients(int *listen_sock, Config *conf, fd_set *read_fds)
{
	int new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	for (size_t j = 0; j < conf->server.size(); j++)
	{
		if (conf->server[j].client.size() < MAX_CONNEXIONS && FD_ISSET(listen_sock[j], read_fds))
		{
			if ((new_socket = accept(listen_sock[j], (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
				return;
			fcntl(new_socket, F_SETFL, O_NONBLOCK);
			conf->server[j].client.push_back(Client(new_socket, address));
			if(CONNEXION_LOG == 1)
				std::cout << GREEN << "[⊛ CONNECT]    => " << RESET << inet_ntoa(address.sin_addr) << WHITE << ":" << RESET << ntohs(address.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << GREEN << conf->server[j].port << RESET << std::endl;
		}
	}
	return;
}

int run_server(Config conf)
{
	Client *client;
	int high_sock;
	fd_set read_fds;
	fd_set write_fds;
	int listen_sock[conf.server.size()];
	for (size_t i = 0; i < conf.server.size(); i++)
		if (ListenSocketAssign(atoi(conf.server[i].port.c_str()), &listen_sock[i], conf.server[i].ip))
			return (-1);

	while (1)
	{
		high_sock = build_fd_set(&listen_sock[0], &conf, &read_fds, &write_fds, NULL);

		int activity = select(high_sock + 1, &read_fds, &write_fds, NULL, NULL);
		if (exit_status == true)
			return (0);
		if (activity < 0)
		{
			std::cout << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv... ERROR !" << std::endl << std::endl;
			perror("Select");
			std::cout << RESET;
			return 1;
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
					&& client->pipe_cgi_in[1] != -1 && FD_ISSET(client->pipe_cgi_in[1], &write_fds)) // write cgi
				{
					WriteCGI(client);

				}
				else if (client->request->ready() == true && is_cgi(client->request, client->response->get_conf())
					&& client->pipe_cgi_in[1] == -1 && client->pipe_cgi_out[0] != -1 && FD_ISSET(client->pipe_cgi_out[0], &read_fds)) // read cgi
				{
					ReadCGI(client);
				}
				else if (client->request->ready() == true && client->fd_file != -1 && FD_ISSET(client->fd_file, &read_fds)) // read file
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
	std::cout << std::endl << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv" << RESET << std::endl;
}