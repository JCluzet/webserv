/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 17:08:33 by jcluzet           #+#    #+#             */
/*   Updated: 2022/05/03 02:55:17 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define LOG 1

// #include "utils.hpp"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define  WHITE "\033[1;37m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"
#include "Config.hpp"
#include "Request.hpp"
#include <signal.h>
#include "Response.hpp"
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
#include <cstring>
#include "Client.hpp"

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


void	quit_sig(int sig);
bool	indexGenerator(std::string* codeHTML, std::string path, std::string defaultFolder = "www");
std::string getHour();
std::string findInHeader(std::string header, std::string s);
// void response_sender(server_data *server, std::string client_data, Config *conf);
// void response_sender(server_data *server, std::string client_data, s_server *conf);
//Response response_sender(std::string data, Client* client, Server *conf);
bool        is_cgi(Request* request);
void    treat_cgi(Server* server, Client* client);

void output_log(int ans, std::string filetosearch);
std::string cut_aftercomma(std::string s);
std::string getDate();
std::string getContentType(std::string client_data);
std::string getFile(std::string client_data);
std::string get_status(int ans);
std::string getHeader(std::string client_data, std::string file_content, int ans);
std::string set_default_page(std::string filetosearch, std::string client_data);
std::string data_sender(std::string client_data);
std::vector<std::string>  cgi_env(std::string cmd, std::string cgi_str, Client* client, Server* server);
// std::string    cgi_exec(char** cmd, char **env, Request* request);
void    cgi_exec(std::vector<std::string> cmd, std::vector<std::string> env, Client* client);
//utils
bool            readinFile(std::string filename, std::string *fileContent);
bool            is_number(char c);
bool            is_space(const char c);
bool            is_blanck(const char c);
std::string     intToStr(int n);
void            pass_space(const std::string s, std::string::size_type *i);
void            pass_blanck(const std::string s, std::string::size_type *i, std::string::size_type *line_i);
void            pass_not_blanck(const std::string s, std::string::size_type *i);
bool            s_a_have_b(const std::string a, const std::string::size_type i, const std::string b);
bool            error_msg(const std::string msg);
bool            is_directory(const std::string path);
bool            is_file(const std::string path);
std::string sizetToStr(size_t n);
std::string intToStr(int n);