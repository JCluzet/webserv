#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read

int main() {
  // Create a socket (IPv4, TCP)
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    std::cout << "SERVER: Failed to create socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  printf("SERVER: Socket created with sockfd: %d\n", sockfd);

  std::cout << "SERVER: Enter port number: ";
  int port;
  std::cin >> port;


  // Listen to port 9999 on any address
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(port); // htons is necessary to convert a number to
                                   // network byte order
  if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
    std::cout << "SERVER: Failed to bind to port " << port << "." << "errno : " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "SERVER: Listening on port  " << port << " setting 10 place max in the queue" << "..." << std::endl;

  // Start listening. Hold at most 10 connections in the queue
  if (listen(sockfd, 10) < 0) {
    std::cout << "SERVER: Failed to listen on socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "SERVER: Waiting for incoming connections..." << std::endl;

  // Grab a connection from the queue
  auto addrlen = sizeof(sockaddr);
  int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
  if (connection < 0) {
    std::cout << "SERVER: Failed to grab connection. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "SERVER: Got a connection" << std::endl;

  // Read from the connection
  char buffer[100];
  auto bytesRead = read(connection, buffer, 100);
  std::cout << "CLIENT: " << buffer;

  // Send a message to the connection
  std::string response = "SERVER:Good talking to you\n";
  send(connection, response.c_str(), response.size(), 0);

  // Close the connections
  while (true) {
    close(connection);
    connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
    if (connection < 0) {
      std::cout << "SERVER: Failed to grab connection. errno: " << errno << std::endl;
      exit(EXIT_FAILURE);
    }
    std::cout << "SERVER: Got a connection" << std::endl;

    // Read from the connection
    char buffer[100];
    auto bytesRead = read(connection, buffer, 100);
    std::cout << "CLIENT: " << buffer;

    // Send a message to the connection
    std::string response = "SERVER:Good talking to you\n";
    send(connection, response.c_str(), response.size(), 0);
  }
  close(connection);
  close(sockfd);
}