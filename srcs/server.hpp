/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 17:08:33 by jcluzet           #+#    #+#             */
/*   Updated: 2022/05/19 00:15:32 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define CO_MAX 20
#define BUFFER_SIZE 2048
#define LOG 1

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define WHITE   "\033[1;37m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define RESET   "\033[0m"
#define DEFAULT_PORT "80"
#define DEFAULT_IP "0.0.0.0"
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <cstring>
#include <stdlib.h>

#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Client.hpp"

struct server_data {
	std::string header;
	int status_code;
	std::string filetosearch;
	std::string filecontent;
	std::string body; 
	std::string response;
	std::string host;
	std::string server_name;
	std::string port;
	bool valid;
};

#ifdef __APPLE__ 
#define MAC 1
#endif
#ifdef __linux__ 
#define MAC 0
#endif

class Config;

std::string chunked(std::string str);
void                        output_debug(std::string request, std::string response);
void                        treat_cgi(Server* server, Client* client);
void	                    quit_sig(int sig);
bool                        is_cgi(Request* request);
bool	                    indexGenerator(std::string* codeHTML, std::string path, std::string defaultFolder = "www");
int                         run_server(Config conf);
int                         build_fd_set(int *listen_sock, Config* conf, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds);
std::string                 findInHeader(std::string header, std::string s);
std::string                 getHour();
void                        cgi_exec(std::vector<std::string> cmd, std::vector<std::string> env, Client* client);
void                        output_log(int ans, std::string filetosearch);
std::string                 cut_aftercomma(std::string s);
std::string                 getDate();
std::string                 getContentType(std::string client_data);
std::string                 getFile(std::string client_data);
std::string                 get_status(int ans);
std::string                 getHeader(std::string client_data, std::string file_content, int ans);
std::string                 set_default_page(std::string filetosearch, std::string client_data);
std::string                 data_sender(std::string client_data);
std::vector<std::string>    cgi_env(std::string cmd, std::string cgi_str, Client* client, Server* server);
//utils
size_t ft_atoi(std::string str);
bool            readinFile(std::string filename, std::string *fileContent);
bool            is_number(char c);
bool            is_space(const char c);
bool            is_blanck(const char c);
void            pass_space(const std::string s, std::string::size_type *i);
void            pass_blanck(const std::string s, std::string::size_type *i, std::string::size_type *line_i);
void            pass_not_blanck(const std::string s, std::string::size_type *i);
bool            s_a_have_b(const std::string a, const std::string::size_type i, const std::string b);
bool            error_msg(const std::string msg);
bool            is_directory(const std::string path);
bool            is_file(const std::string path);
bool			fileExist(const std::string s);
std::string     intToStr(int n);
std::string     sizetToStr(size_t n);
std::string     intToStr(int n);
