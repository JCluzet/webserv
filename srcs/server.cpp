// Server side C program to demonstrate HTTP Server programming
#include "server.hpp"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void launch_browser(int port)
{
    std::string test, o;
    std::cout << std::endl;
    std::cout << BLUE << "[⊛] => " << WHITE << "Want to open page on browser ? (y/n)";
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

void output(std::string client_data, std::string server_data)
{
    (void)server_data;
    (void)client_data;
        std::cout << WHITE << "\nRequest: \n" << RESET << client_data << std::endl;
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
    if (listen(*server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    // open browser at launch
    return (address);
}

void	build_fd_set(int listen_sock, int connection_list_sock[9], fd_set* read_fds, fd_set* write_fds, fd_set* except_fds)
{
	int	i;
	FD_ZERO(read_fds);
	FD_SET(listen_sock, read_fds);
	for (i = 0; i < 9; ++i)
	{
		if (connection_list_sock[i] != -1)
			FD_SET(connection_list_sock[i], read_fds);
	}

	FD_ZERO(write_fds);
	for (i = 0; i < 9; ++i)
	{
		if (connection_list_sock[i] != -1)
			FD_SET(connection_list_sock[i], write_fds);
	}

	FD_ZERO(except_fds);
	FD_SET(listen_sock, except_fds);
	for (i = 0; i < 9; ++i)
	{
		if (connection_list_sock[i] != -1)
			FD_SET(connection_list_sock[i], except_fds);
	}

	return ;
}

// include atoi lib
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    int listen_sock, new_socket;
    long valread;
    struct server_data server;
    struct sockaddr_in address;
    
    Config conf(check_config(argc, argv)); // --> get config from config file

    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
	fcntl(listen_sock, F_SETFL, O_NONBLOCK);

	int reuse = 1;
	if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0)
	{
		perror("setsockopt");
		return -1;
	}

    address = SocketAssign(atoi(conf.serv[0].port.c_str()), &listen_sock);
    int addrlen = sizeof(address);

	int	high_sock;
	fd_set	read_fds;
	fd_set	write_fds;
	fd_set	except_fds;
	int	connection_list_sock[9];
	for (int i = 0; i < 9; i++)
		connection_list_sock[i] = -1;

	std::cout << std::endl << BLUE << "[⊛] => " << WHITE << "Waiting for connections on port " << GREEN << conf.serv[0].port << RESET  << "..." << std::endl << RESET;
    launch_browser(atoi(conf.serv[0].port.c_str()));
	while (1)
	{
		build_fd_set(listen_sock, connection_list_sock, &read_fds, &write_fds, &except_fds);

		high_sock = listen_sock;
		for (int i = 0; i < 9; ++i)
		{
			if (connection_list_sock[i] > high_sock)
				high_sock = connection_list_sock[i];
		}

		int	activity = select(high_sock + 1, &read_fds, NULL, NULL, NULL);
	//	int	activity = select(high_sock + 1, &read_fds, &write_fds, &except_fds, NULL);

		if (activity < 0)
		{
			perror("select error");
			exit(EXIT_FAILURE);
		}

		if (FD_ISSET(listen_sock, &read_fds))
		{
			if ((new_socket = accept(listen_sock, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			{
				perror("In accept");
				exit(EXIT_FAILURE);
			}
	        std::cout << std::endl << GREEN << "[⊛ NEW USER] => " << WHITE << "PORT Used: " << RESET << conf.serv[0].port << WHITE << " socket fd: " << RESET << new_socket << std::endl << std::endl << RESET;

			// std::cout << std::endl << GREEN << "New connection , socket fd is : " << new_socket << ", ip is : " << inet_ntoa(address.sin_addr) << ", port is : " << ntohs(address.sin_port) << std::endl << std::endl << RESET;

			fcntl(new_socket, F_SETFL, O_NONBLOCK);
			for (int i = 0; i < 9; i++)
			{
				if (connection_list_sock[i] == -1)
				{
					connection_list_sock[i] = new_socket;
					break ;
				}
			}
		}

		int	client_socket;
		for (int i = 0; i < 9; i++)
		{
			client_socket = connection_list_sock[i];
			if (FD_ISSET(client_socket, &read_fds))
			{
				char client_data[30001];
				if ((valread = read(client_socket, client_data, 30000)) <= 0)
				{
					getpeername(client_socket, (struct sockaddr*)&address , (socklen_t*)&addrlen);
	            std::cout << std::endl << RED << "[⊛ DISCONNECT] => " << WHITE << "User with port " << RESET << conf.serv[0].port << WHITE << " is now disconnected." << RESET  << std::endl;
					
                    
                    // std::cout << std::endl << RED << "Somebody disconnected, ip : " <<
						// inet_ntoa(address.sin_addr) << ", port : " << ntohs(address.sin_port) <<
							// std::endl << std::endl << RESET;
					close(client_socket);
					connection_list_sock[i] = -1;
				}
				else
				{
					client_data[valread] = '\0';
					response_sender(&server, client_data, &conf);
					write(client_socket, server.response.c_str(), server.response.length());
					// output(client_data, server.response);
				}
			}
		}
	}
	return 0;
}
