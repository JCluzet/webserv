// Server side C program to demonstrate HTTP Server programming
#include "server.hpp"

sockaddr_in SocketAssign(int port, int *server_fd)
{
    struct sockaddr_in address;

    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    // bind of the socket to assign the port

    if (bind(*server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(*server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    // open browser at launch

    std::string test, o;
    std::cout << MAGENTA << "[⊛] => " << WHITE << "Want to open page on browser ? (y/n)";
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
    return(address);
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    long valread;
    std::string response;
    struct sockaddr_in address;
    // (void)argc;
    std::string tmp;
    std::cout << RED << "   _      __    __   ____            \n  | | /| / /__ / /  / __/__ _____  __\n  | |/ |/ / -_) _ \\_\\ \\/ -_) __/ |/ /\n  |__/|__/\\__/_.__/___/\\__/_/  |___/ \n " << BLUE << "\n⎯⎯  jcluzet  ⎯  alebross ⎯  amanchon  ⎯⎯\n\n" << RESET;
    if (argc == 1)
    {
        tmp = "config/default.conf";
        std::cout << MAGENTA << "[⊛] => " << YELLOW << "Using default config file: " << RESET << tmp << std::endl;
    }
    else
        tmp = argv[1];
    // check if argv is a valid file
    if (access(tmp.c_str(), F_OK) == -1)
    {
        std::cout << "Config file not found" << std::endl;
        exit(EXIT_FAILURE);
    }

    Config conf(tmp);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address = SocketAssign(atoi(conf.serv[0].port.c_str()), &server_fd);
    int addrlen = sizeof(address);
    std::string filename;
    std::string filecontent;
    while (1)
    {
        // printf("\n+++++++ Waiting for new connection on %d port ++++++++\n\n", conf.serv[0].port );
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        char client_data[30000] = {0};
        valread = read(new_socket, client_data, 30000);
        // printf("CLIENT:\n%s\n", client_data);
        std::string file;
        (void)valread; // --> ????

        response = response_sender(client_data, conf);
        write(new_socket, response.c_str(), response.length());
        close(new_socket);
        if (strncmp(response.c_str(), "HTTP/1.1 200 OK", 15) == 0)
        {
            //  << WHITE << findInHeader(client_data, "Referer") << findInHeader(client_data, "File") << RESET << std::endl;
        }
 

        // std::cout << "\n\nOUR RESPONSE: " << std::endl
        //           << response << std::endl;
    }
    return 0;
}