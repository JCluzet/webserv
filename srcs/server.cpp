// Server side C program to demonstrate HTTP Server programming
#include "server.hpp"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CO_MAX 20

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
    std::cout << RED << "   _      __    __   ____            \n  | | /| / /__ / /  / __/__ _____  __\n  | |/ |/ / -_) _ \\_\\ \\/ -_) __/ |/ /\n  |__/|__/\\__/_.__/___/\\__/_/  |___/ \n " << BLUE << "\n⎯⎯  jcluzet  ⎯  alebross ⎯  amanchon  ⎯⎯\n\n" << RESET;
}

Config check_config(int argc, char const *argv[])
{
    std::string tmp;
    std::cout << RED << "   _      __    __   ____            \n  | | /| / /__ / /  / __/__ _____  __\n  | |/ |/ / -_) _ \\_\\ \\/ -_) __/ |/ /\n  |__/|__/\\__/_.__/___/\\__/_/  |___/ \n " << BLUE << "\n⎯⎯  jcluzet  ⎯  alebross ⎯  amanchon  ⎯⎯\n\n" << RESET;
    if (argc == 1 || strcmp("--debug", argv[1]) == 0)
    {
        tmp = "config/default.conf";
        std::cout << BLUE << "[⊛] => " << YELLOW << "Using default config file: " << RESET << tmp << std::endl;
    }
    else
        tmp = argv[1];
    if (access(tmp.c_str(), F_OK) == -1)
    {
        std::cout << RED << "[⊛] => " << WHITE << "Config file " << RESET << tmp << WHITE << " not found" << std::endl;
        exit(EXIT_FAILURE);
    }

    Config conf(tmp);
    if (conf.nb_servers == 0)
    {
        std::cout << RED << "[⊛] => " << RESET << tmp << WHITE << " Configuration ERROR" << std::endl;
        exit(EXIT_FAILURE);
    }
    return conf;
}

void output(std::string client_data, std::string server_data, std::string request_cast)
{
    (void)server_data;
    (void)client_data;
        std::cout << WHITE << "\nRequest: \n" << RESET << client_data << std::endl;
        std::cout << WHITE << "\nRequest CAST: \n" << RESET << request_cast << std::endl;
		
        std::cout << WHITE << "\nOUR RESPONSE: " << RESET << std::endl << server_data << std::endl;
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
        exit(EXIT_FAILURE);
    }

    // open browser at launch
    return (address);
}

void	build_fd_set(int *listen_sock, int nb_servers, int **connection_list_sock, fd_set* read_fds, fd_set* write_fds, fd_set* except_fds)
{
	size_t	i;
	int		j;
	FD_ZERO(read_fds);
	for (j = 0; j < nb_servers; ++j)
	{
		if (listen_sock[j] != -1)
			FD_SET(listen_sock[j], read_fds);
		for (i = 0; i < CO_MAX; ++i)
		{
			if (connection_list_sock[j][i] != -1)
				FD_SET(connection_list_sock[j][i], read_fds);
		}
	}


	FD_ZERO(write_fds);
	for (j = 0; j < nb_servers; ++j)
	{
		for (i = 0; i < CO_MAX; ++i)
		{
			if (connection_list_sock[j][i] != -1)
				FD_SET(connection_list_sock[j][i], write_fds);
		}
	}

	FD_ZERO(except_fds);
	for (j = 0; j < nb_servers; ++j)
	{
		if (listen_sock[j] != -1)
			FD_SET(listen_sock[j], except_fds);
		for (i = 0; i < CO_MAX; ++i)
		{
			if (connection_list_sock[j][i] != -1)
				FD_SET(connection_list_sock[j][i], except_fds);
		}
	}
	return ;
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
	int	listen_sock[conf.nb_servers];

	for (size_t i = 0; i < conf.nb_servers; i++)
	{
    	if ((listen_sock[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   		{
   	    	perror("In socket");
   	    	exit(EXIT_FAILURE);
   		}
		fcntl(listen_sock[i], F_SETFL, O_NONBLOCK);

		int reuse = 1;
		if (setsockopt(listen_sock[i], SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0)
		{
			perror("setsockopt");
			return -1;
		}

    	address = SocketAssign(atoi(conf.serv[i].port.c_str()), &listen_sock[i]);
	}
	int addrlen = sizeof(address);
	int	high_sock;
	fd_set	read_fds;
	fd_set	write_fds;
	fd_set	except_fds;
	int	**connection_list_sock = new int*[conf.nb_servers];
	for(size_t i = 0; i < conf.nb_servers; ++i)
  	  connection_list_sock[i] = new int[CO_MAX];
	for (size_t j = 0; j < conf.nb_servers; j++)
	{
		for (size_t i = 0; i < CO_MAX; i++)
			connection_list_sock[j][i] = -1;
	}
	Request	**request = new Request*[conf.nb_servers];
	for(size_t i = 0; i < conf.nb_servers; ++i)
  	  request[i] = new Request[CO_MAX];
	for (size_t j = 0; j < conf.nb_servers; j++)
	{
		for (size_t i = 0; i < CO_MAX; i++)
			request[j][i] = Request();
	}

	for (size_t i = 0; i < conf.nb_servers; i++)
		std::cout << BLUE << "[⊛] => " << WHITE << "Waiting for connections on port " << GREEN << conf.serv[i].port << RESET  << "..." << std::endl << RESET;
	launch_browser(atoi(conf.serv[0].port.c_str()));
	while (1)
	{
		build_fd_set(&listen_sock[0], conf.nb_servers, connection_list_sock, &read_fds, &write_fds, &except_fds);

		high_sock = -1;
		for (size_t i = 0; i < conf.nb_servers; ++i)
		{
			if (listen_sock[i] > high_sock)
				high_sock = listen_sock[i];
		}
		for (size_t j = 0; j < conf.nb_servers; j++)
		{
			for (size_t i = 0; i < CO_MAX; i++)
			{
				if (high_sock < connection_list_sock[j][i])
					high_sock = connection_list_sock[j][i];
			}
		}

		int	activity = select(high_sock + 1, &read_fds, &write_fds, &except_fds, NULL);

		if (activity < 0)
		{
			perror("select error");
			for(size_t k = 0; k < conf.nb_servers; ++k)
  				delete [] connection_list_sock[k];
			delete [] connection_list_sock;
			for(size_t k = 0; k < conf.nb_servers; ++k)
				delete [] request[k];
			delete [] request;
			exit(EXIT_FAILURE);
		}
		for (size_t j = 0; j < conf.nb_servers; j++)
		{
			if (connection_list_sock[j][CO_MAX - 1] == -1 && FD_ISSET(listen_sock[j], &read_fds))
			{
				if ((new_socket = accept(listen_sock[j], (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
				{
					perror("In accept");
					for(size_t k = 0; k < conf.nb_servers; ++k)
  						delete [] connection_list_sock[k];
					delete [] connection_list_sock;
					for(size_t k = 0; k < conf.nb_servers; ++k)
  						delete [] request[k];
					delete [] request;
					exit(EXIT_FAILURE);
				}
		        std::cout << GREEN << "[⊛ CONNECT]   => " << RESET << inet_ntoa(address.sin_addr) << WHITE  << ":" << RESET << ntohs(address.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << GREEN << conf.serv[j].port << RESET << std::endl;

				fcntl(new_socket, F_SETFL, O_NONBLOCK);
				for (size_t i = 0; i < CO_MAX; i++)
				{
					if (connection_list_sock[j][i] == -1)
					{
						connection_list_sock[j][i] = new_socket;
						break ;
					}
					if (i == CO_MAX - 1)
						close(new_socket);
				}
			}
		}

		int	client_socket;
		Response	response;
		for (size_t j = 0; j < conf.nb_servers; j++)
		{
			response = Response();
			for (int i = 0; i < CO_MAX; i++)
			{
				client_socket = connection_list_sock[j][i];
				if (FD_ISSET(client_socket, &read_fds))
				{
					char client_data[30001];
					if ((valread = read(client_socket, client_data, 30000)) <= 0)
					{
						getpeername(client_socket, (struct sockaddr*)&address , (socklen_t*)&addrlen);
		                std::cout << RED << "[⊛ DISCONNECT] => " << RESET << inet_ntoa(address.sin_addr) << WHITE  << ":" << RESET << ntohs(address.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << RED << conf.serv[j].port  << RESET << std::endl;

						close(client_socket);
						connection_list_sock[j][i] = -1;
					}
					else
					{
						client_data[valread] = '\0';
						response = response_sender(client_data, &request[j][i], &conf.serv[j]);
						if (request[j][i].ready() == true)
						{
							 size_t tmp = write(client_socket, response.get_response().c_str(), response.get_response().length());
							if (tmp < response.get_response().length())
							{
								std::cout << "ERROR: " << errno << std::endl;
								std::cout << "ERROR: " << strerror(errno) << std::endl;
							}
							
							if (response.getstat() == 400)
							{
								getpeername(client_socket, (struct sockaddr*)&address , (socklen_t*)&addrlen);
								close(client_socket);
				                std::cout << RED << "[⊛ DISCONNECT] => " << RESET << inet_ntoa(address.sin_addr) << WHITE  << ":" << RESET << ntohs(address.sin_port) << RED << "    ⊛ " << WHITE << "PORT: " << RED << conf.serv[j].port << RESET << std::endl;

								connection_list_sock[j][i] = -1;
							}
							if (LOG == 1)
								output(client_data, response.get_response(), request[j][i].get_request());
							request[j][i].clear();
						}
					}
				}
			}
		}
	}
	return 0;
}