/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 23:10:04 by jcluzet           #+#    #+#             */
/*   Updated: 2022/04/22 23:30:01 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// include socket library
#include <sys/socket.h>

#include <iostream>

int main(void)
{
    // Create a socket                                     --> A Socket can be seeen as a file descriptor that can be used for communication.
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cout << "Error creating socket" << std::endl;
        return -1;
    }
    std::cout << "Server socket: " << server_socket << std::endl;

    // Use of bind                                         --> Bind is to assign an IP address and port to the socket. (like assigning a phone number to the line that you requested from the phone company)
    int bind_result;
    int bind(int sockfd, const sockaddr *addr, socklen_t addrlen);
    if ((bind_result = bind(server_socket, const sockaddr *addr, socklen_t addrlen)) == -1)
    {
        std::cout << "Error creating socket" << std::endl;
        return -1;
    }



    
    
    return (0);
}