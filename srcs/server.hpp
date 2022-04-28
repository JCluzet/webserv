/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 17:08:33 by jcluzet           #+#    #+#             */
/*   Updated: 2022/04/28 18:49:19 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// #include "utils.hpp"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define  WHITE "\033[1;37m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"
#include "config.hpp"
#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>
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

struct server_data {
    std::string header;
    int status_code;
    std::string filetosearch;
    std::string filecontent;
    std::string body; 
    std::string response; // ✅
    std::string host;
    std::string server_name;
    std::string port;
    // std::string default_folder;
    // std::string default_page;
    // std::string page404;
    // std::string max_body_size;
    bool valid;
};

#ifdef __APPLE__ 
    #define MAC 1
#endif
#ifdef __linux__ 
    #define MAC 0
#endif

// PARSING

bool readinFile(std::string filename, std::string *fileContent);
bool	indexGenerator(std::string* codeHTML, std::string path, std::string defaultFolder = "www");
bool is_directory(std::string path);
std::string findInHeader(std::string header, std::string s);
// void response_sender(server_data *server, std::string client_data, Config *conf);
void response_sender(server_data *server, std::string client_data, s_server *conf);

int readFile(std::string filename, std::string *fileContent);
std::string cut_aftercomma(std::string s);
std::string getContentType(std::string client_data);
std::string getFile(std::string client_data);
std::string get_status(int ans);
std::string getHeader(std::string client_data, std::string file_content, int ans);
std::string set_default_page(std::string filetosearch, std::string client_data);
std::string data_sender(std::string client_data);

std::string sizetToStr(size_t n);
std::string intToStr(int n);