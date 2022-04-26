/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 17:08:33 by jcluzet           #+#    #+#             */
/*   Updated: 2022/04/26 20:54:45 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// #include "utils.hpp"
#include "config.hpp"
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

#ifdef __APPLE__ 
    #define MAC 1
#endif
#ifdef __linux__ 
    #define MAC 0
#endif

// PARSING

bool readinFile(std::string filename, std::string *fileContent);
bool is_directory(std::string path);
std::string findInHeader(std::string header, std::string s);
int readFile(std::string filename, std::string *fileContent);
std::string response_sender(std::string client_data, Config conf);
std::string cut_aftercomma(std::string s);
std::string getContentType(std::string client_data);
std::string getFile(std::string client_data);
std::string get_status(int ans);
std::string getHeader(std::string client_data, std::string file_content, int ans);
std::string set_default_page(std::string filetosearch, std::string client_data);
std::string data_sender(std::string client_data);
