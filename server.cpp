// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <iostream>

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

std::string data_sender(std::string file)
{
  std::string data = "HTTP/1.1 200 OK\n";
  data += "Content-Type: text/html\n"; // --> check the content-type
  data += "Content-Length: 12\n\n"; 
  return(data);
}

// #define PORT 8585
int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    if(argc != 2)
    {
      std::cout << "port number needed" << std::endl;
      return(0);
    }
    
    // Only this line has been changed. Everything is same.
    // std::string hello;
    std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";
    // std::string hello = "HTTP/1.1 200 OK\nDate: Fri, 16 Mar 2018 17:36:27 GMT\nserver: myServerContent-Type: text/html;charset=UFT-8\nContent-Length: 1846\n\n<?xml>\n<!DOCTYPE html>\n<html>\n<body>\n\n<h1>My First Heading</h1>\n<p>My first paragraph.</p>\n\n</body>\n</html>\n";
 
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    int port = atoi(argv[1]);
    address.sin_port = htons( port );
    
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
    std::string test;
    printf ("Want to open page on port %d ?\n (y/n)", port);
    while(1)
    {
      std::cin >> test;
      if (test == "y")
      {
        std::cout << "Opening page on port " << port << std::endl;
        system(("open http://localhost:" + std::to_string(port)).c_str());     // --> mac
        // system(("xdg-open http://localhost:" + std::to_string(port)).c_str()); --> linux
        break;
      }
      if (test == "n")
      {
        break;
      }
    }
    std::string filename;
    std::string filecontent;
    while(1)
    {
        filename="root";
        printf("\n+++++++ Waiting for new connection on %d port ++++++++\n\n", port);
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf(">>%s<<\n",buffer);
        std::string file;
        // file contain the word after GET /

        for (int i = 4; i < valread; i++)
        {
            if (buffer[i] == ' ')
                break;
            file += buffer[i];
        }
        printf("file>>%s<<\n",file.c_str());
        filename += file;
        printf("filename>>%s<<\n",filename.c_str());
        readFile(filename.c_str(), &filecontent);
          // return (1);
        hello += std::to_string(filecontent.length());
        hello += "\n\n";
        hello += filecontent;
        // hello = data_sender(file);
        // send_parse(new_socket, buffer); -- > get 
        write(new_socket , hello.c_str() , hello.length());
        printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
    return 0;
}
