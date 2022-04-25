// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <string>

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

std::string getFile(std::string client_data)
{
  std::string file;
  for (int i = 4; i < client_data.length(); i++)
  {
    if (client_data[i] == ' ')
      break;
    file += client_data[i];
  }
  return (file);
}

std::string getHeader(std::string client_data)
{
  std::string response = "HTTP/1.1 200 OK";
  response += "\nContent-Type: ";
  response += "text/html"; // -->> need to detect the content type
  response += "\nContent-Length: ";
  return(response);
}

std::string data_sender(std::string client_data)
{
  std::string response;
  std::string filecontent;
  response = getHeader(client_data);
  std::string filetosearch = "root" + getFile(client_data);
  std::cout << "filetosearch>>" << filetosearch << "<<" << std::endl;
  readFile(filetosearch.c_str(), &filecontent);
  response += std::to_string(filecontent.length());              // fill the content-lenght header part
  response += "\n\n";
  response += filecontent;
  return(response);
}

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
    std::string filetosearch;   
    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";
 
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
        printf("\n+++++++ Waiting for new connection on %d port ++++++++\n\n", port);
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        // response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        
        char client_data[30000] = {0};
        valread = read( new_socket , client_data, 30000);
        printf(">>%s<<\n",client_data);
        std::string file;

        response = data_sender(client_data);
        // response += std::to_string(filecontent.length());
        std::cout << "<<< " << response << std::endl;
        write(new_socket , response.c_str() , response.length());
        // printf("------->   message sent : \n%s <-------", response.c_str());
        close(new_socket);
    }
    return 0;
}