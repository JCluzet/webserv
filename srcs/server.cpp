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

    std::string test;
    printf("Want to open page on port %d ?\n (y/n)", port);
    while (1)
    {
        std::cin >> test;
        if (test == "y")
        {
            std::cout << "Opening page on port " << port << std::endl;
            if (MAC == 1)
                system(("open http://localhost:" + std::to_string(port)).c_str()); // --> mac
            else
                system(("xdg-open http://localhost:" + std::to_string(port)).c_str());
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
    if (argc == 1)
    {
        tmp = "config/default.conf";
        std::cout << "Using default config file: " << tmp << std::endl;
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

    address = SocketAssign(conf.serv[0].port, &server_fd);
    int addrlen = sizeof(address);
    std::string filename;
    std::string filecontent;
    while (1)
    {
        printf("\n+++++++ Waiting for new connection on %d port ++++++++\n\n", conf.serv[0].port );
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        char client_data[30000] = {0};
        valread = read(new_socket, client_data, 30000);
        printf("RECEIVING:\n%s\n", client_data);
        std::string file;
        (void)valread; // --> ????

        response = response_sender(client_data, conf);
        write(new_socket, response.c_str(), response.length());
        close(new_socket);
        std::cout << "\n\nOUR RESPONSE: " << std::endl
                  << response << std::endl;
    }
    return 0;
}