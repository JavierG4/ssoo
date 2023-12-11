#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> //htonl
#include<fstream>
#include <fcntl.h>
//#include <system_error>
//#include "netinet/ip.h"
#include <vector>
#include <optional>
#include <expected>
#include <signal.h>
#include <thread>
#include <chrono>
#include <atomic>
// Modo de compilación : g++-13 netcp.cc tools.cc -std=c++23

std::error_code netcp_send_file(const std::string&);
std::error_code netcp_receive_file(const std::string&);
std::expected<int, std::error_code> open_file(std::string);
std::error_code send_to(int, std::vector<uint8_t>&, sockaddr_in&);
std::error_code read_file(int, std::vector<uint8_t>&);
std::optional<sockaddr_in> make_ip_address( const std::optional<std::string>, uint16_t);
using make_socket_result = std::expected<int, std::error_code>;
make_socket_result make_socket(std::optional<sockaddr_in>);
std::error_code receive_from(int,std::vector<uint8_t>&,sockaddr_in&);
std::error_code write_file(int, const std::vector<uint8_t>&);
void segv_signal_handler(int);
void term_signal_handler(int);
std::string getenv(const std::string&);
