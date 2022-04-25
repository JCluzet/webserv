// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <string>
#include <sys/stat.h>
#include <fstream>


bool    readFile(std::string filename, std::string *fileContent) {
    std::string     s;
    std::ifstream   ifs(filename);

    if (!ifs) {
        std::cerr << "Not Found " << filename << "." << std::endl;
        *fileContent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>";
        return (1);
    }
    // if filename is a folder, set it to index.html ( detect by using is_directory() )
    // if (is_directory(filename)) {
    //     filename += "index.html";
    // }
    getline(ifs, s);
    if (s == "") {
        std::cerr << "Empty file." << std::endl;
        ifs.close();
        *fileContent = "\n";
        *fileContent = "<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>Empty file.</p>\n</body>\n\n</html>";
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

std::string cut_aftercomma(std::string s)
{
    int i = 0;
    while (s[i] != ',')
        i++;
    return (s.substr(0, i));
}

std::string getContentType(std::string client_data)
{
    std::string content_type = "";
    for (unsigned int i = 0; i < client_data.length() - 7; i++)
    {
        if (client_data[i] == 'A' && client_data[i + 1] == 'c' && client_data[i + 2] == 'c' && client_data[i + 3] == 'e' && client_data[i + 4] == 'p' && client_data[i + 5] == 't' && client_data[i + 6] == ':')
        {
            i += 8;
            while (client_data[i] != ',')
            {
                content_type += client_data[i];
                i++;
            }
            break;
        }
    }
    std::cout << "Content-Type:" << content_type << std::endl;
    return (content_type);
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
  response += getContentType(client_data);
  // response += "text/html"; // -->> need to detect the content type
  response += "\nContent-Length: ";
  return(response);
}

#include <sys/types.h>

std::string set_default_page(std::string filetosearch, std::string client_data)
{
    if(filetosearch[filetosearch.length() - 1] == '/')
    {
      filetosearch += "index.html";                                             // <-- replace here the default page in config file
      std::cout << "setting DEFAULT to index.html" << std::endl;
    }
    return (filetosearch);
}


std::string data_sender(std::string client_data)
{
  std::string response;
  std::string filecontent;
  response = getHeader(client_data);
  std::string filetosearch = "root" + getFile(client_data);                       // <-- replace "root" here with config file
  filetosearch = set_default_page(filetosearch, client_data);
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
        
        char client_data[30000] = {0};
        valread = read( new_socket , client_data, 30000);
        printf("RECEIVING:\n%s\n",client_data);
        std::string file;

        response = data_sender(client_data);
        std::cout << "\n\nOUR RESPONSE: " << std::endl << response << std::endl;
        write(new_socket , response.c_str() , response.length());
        close(new_socket);
    }
    return 0;
}