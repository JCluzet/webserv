/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chunked.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/16 20:07:13 by jcluzet           #+#    #+#             */
/*   Updated: 2022/05/16 20:53:35 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

// send a chunk of data ( must begin by hexadecimal number, then a \r\n, then data, then a \r\n)
// return data sent without hexadecimal number and \r\n
// return empty string if error

#include <iostream>

std::string chunked(std::string str)
{
    std::string _chunked;
    unsigned int i = 0;
    unsigned int len = strtol(str.c_str(), NULL, 16);
    while (i < str.length())
    {
        if (str[i] == '\r' && str[i + 1] == '\n')
        {
            i += 2;
            break;
        }
        i++;
    }
    unsigned int tmp = i;
    while (i < str.length() && (len > (i - tmp)))
    {
        if (str[i] == '\r' && str[i + 1] == '\n')
        {
            i += 2;
            if (str[i] == '\r' && str[i + 1] == '\n')
            {
                i += 2;
                _chunked += "\r\n";
            }
            break;
        }
        _chunked += str[i];
        i++;
    }
    if (i != str.length())
    {
        try
        {
            _chunked += chunked(str.substr(i, str.length()));
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    return (_chunked);
}

// int main()
// {
//     std::string str = "26\r\n";
//     str += "Voici les données du premier morceau\r\n\r\n";
//     str += "1C\r\n";
//     str += "et voici un second morceau\r\n\r\n";
//     std::cout << chunked(str);
//     return (0);
// }