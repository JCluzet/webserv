// Server side C program to demonstrate HTTP Server programming
#include "server.hpp"

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
    launch_browser(port);
    return (address);
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    long valread;
    struct server_data server;
    struct sockaddr_in address;
    
    Config conf(check_config(argc, argv)); // --> get config from config file

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address = SocketAssign(atoi(conf.serv[0].port.c_str()), &server_fd);
    int addrlen = sizeof(address);

    while (1)
    {
        std::cout << std::endl << BLUE << "[⊛] => " << WHITE << "Waiting for connections on port " << GREEN << conf.serv[0].port << RESET  << "..." << std::endl << RESET;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        char client_data[30000] = {0};
        valread = read(new_socket, client_data, 30000);
        (void)valread; // --> ????


        response_sender(&server, client_data, &conf);
        write(new_socket, server.response.c_str(), server.response.length());
        close(new_socket);

        output(client_data, server.response);
    }
    return 0;
}