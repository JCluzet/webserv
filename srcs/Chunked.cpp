/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chunked.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/16 20:07:13 by jcluzet           #+#    #+#             */
/*   Updated: 2022/05/25 20:59:31 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int chunked(std::string *strrec, int chunked_len)
{
    unsigned int len;
    std::string str = *strrec;
    std::string str1;

    *strrec = "";
    if (chunked_len == -1)
    {
        if (str == "0\r\n" || str == "0\r\n\r\n")
            return (0);
        if (str.find("\r\n") == std::string::npos)
            return (-2);
        str1 = str.substr(0, str.find("\r\n"));
        if ((len = strtol(str1.c_str(), NULL, 16)) == 0)
            return (-2);
        else
            return (len);
    }
    if (chunked_len == 0 && str == "\r\n")
        return (-1);
    if (static_cast<signed int>(str.length()) != chunked_len + 2 || str[str.length() - 2] != '\r' || str[str.length() - 1] != '\n')
        return (-2);
    *strrec = str.substr(0, chunked_len);
    return (-1);
}