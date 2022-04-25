// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
// #include <string>

#define PORT 9898

bool    readFile(const char * filename, std::string *fileContent) {
    std::string     s;
    std::ifstream   ifs(filename);

    if (!ifs) {
        std::cerr << "Can't open " << filename << "." << std::endl;
        return (1);
    }
    getline(ifs, s);
    if (s == "") {
        std::cerr << "Empty file." << std::endl;
        ifs.close();
        return (1);
    }
    *fileContent += s;
    while(getline(ifs, s))
    {
        *fileContent += "\n";
        *fileContent += s;
    }
    ifs.close();
    return (0);
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Only this line has been changed. Everything is same.
    // std::string hello = "HTTP/1.1 200 OK\nDate: Fri, 16 Mar 2018 17:36:27 GMT\nserver: myServerContent-Type: text/html;charset=UFT-8\nContent-Length: 1846\n\n<?xml>\n<!DOCTYPE html>\n<html>\n<body>\n\n<h1>My First Heading</h1>\n<p>My first paragraph.</p>\n\n</body>\n</html>\n";
    std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";//110\n\n<!DOCTYPE html>\n<html>\n<body>\n\n<h1>My First Heading</h1>\n<p>My first paragraph.</p>\n\n</body>\n</html>\n";
    std::string filecontent;
    std::string filename = "test.html";
    if (readFile(filename.c_str(), &filecontent))
        return (1);
    hello += std::to_string(filecontent.length() + hello.length() + 1);
    hello += "\n";
    hello += filecontent;
    // std::cout << hello << std::endl;
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf("%s\n",buffer );
        write(new_socket , hello.c_str() , hello.length());
        printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
    return 0;
}