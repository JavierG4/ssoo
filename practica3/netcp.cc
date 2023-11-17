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

// Modo de compilación : g++-13 netcp.cc -std=c++23

std::expected<int, std::error_code> open_file(std::string file) {
  int fd = open(file.c_str() ,O_RDONLY, 00007);
  if (fd == -1) {
    std::error_code error(errno, std::system_category());
    return std::unexpected(error); // Aquí está la corrección
  }
  return fd; 
}

std::error_code send_to(int fd, std::vector<uint8_t>& message, sockaddr_in& address) {
  int bytes_sent = sendto(fd, message.data(), message.size(), 0, reinterpret_cast<const sockaddr*>(&address), sizeof(address));
  if (bytes_sent < 0) {
// Error al enviar el mensaje.
    return std::error_code(errno, std::system_category());
  }
  return std::error_code(0, std::system_category());
}

std::error_code read_file(int fd, std::vector<uint8_t>& buffer) {
  ssize_t bytes_read = read(fd, buffer.data(), buffer.size());
  if (bytes_read < 0) {
    return std::error_code(errno, std::system_category());
  }
  buffer.resize(bytes_read);
  return std::error_code(0, std::system_category());  
}

std::optional<sockaddr_in> make_ip_address( const std::optional<std::string> ip_address, uint16_t port=0) { 
  sockaddr_in local_address{};
  local_address.sin_family = AF_INET; 
  if ( ip_address == std::nullopt || ip_address.value().empty() ) {
    local_address.sin_addr.s_addr = htonl(INADDR_ANY); 
  }
  else {
    struct in_addr direccionip;
    int error = inet_aton(ip_address.value().c_str(), &direccionip);
    if (error < 0 ) {
      return std::nullopt;
    }
    local_address.sin_addr = direccionip;
  }
  local_address.sin_port = htons(port);
  return local_address;
}

using make_socket_result = std::expected<int, std::error_code>;
make_socket_result make_socket(std::optional<sockaddr_in> address = std::nullopt) {
  int fd_s = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_s < 0) {
    std::cerr << "Error socket al crear el socket por :" << std::endl;
    std::cerr << std::error_code(errno, std::system_category()).message() << std::endl;
  }
  return fd_s;
}
int main(int argc, char* argv[]) {
  if(argc < 2) {   // comprobar si escribieron parametros de entrada
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << " " << std::endl;
    std::cout << "Netcp: falta un archivo como argumento " << std::endl;
    std::cout << "Modo de empleo: netcp archivo.txt (archivo a enviar) " << std::endl;
    std::cout << "También puedes hacer un --help" << std::endl;
    std::cout << " " << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    return 0;
  }
  std::string help_1 = argv[1];
  if(help_1 == "--help") {  // opcion para mostrar una breve ayuda sobre el comando
    std::cout << "---------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << " " << std::endl;
    std::cout << "Para ejecutar este programa necesitas hacer un ./a.out o un ./netcp y además pasar un archivo de texto" << std::endl;
    std::cout << "por linea de comando del tipo ./netcp texto.txt" << std::endl;
    std::cout << "Luego para recibir el mensaje en otra terminal haces netcat -lu 8080" << std::endl;
    std::cout << " " << std::endl;
    std::cout << "---------------------------------------------------------------------------------------------------------" << std::endl;
    return 0;
  }
  std::string filename = argv[1];
  std::expected<int, std::error_code> result = open_file(filename); 
  if (!result) {
    std::cerr << "Error al abrir el archivo por el siguiente motivo" << std::endl;
    std::cerr << result.error().message() << std::endl;
    int close_error = close(*result);
    if (close_error != 0 ) {
      std::cerr << "Ha habido un error al hacer el close" << std::endl;
    }
    return EXIT_FAILURE;
  }
  int fd = *result; 
  //std::cout << fd << std::endl;
  std::vector<uint8_t> buffer(1024);
  std::error_code error = read_file(fd, buffer);
  if ( error ) {
    std::cerr << "Ha habido un error a la hora de hacer la función read_file y este es el error" << std::endl;
    std::cerr << error.message() << std::endl;
    int close_error = close(fd);
    if (close_error != 0 ) {
      std::cerr << "error al cerrar el socket" << std::endl;
    }
    return EXIT_FAILURE;
  }
// Creo el socket con la funcion make_socket
  std::expected<int, std::error_code> result_socket = make_socket();
  if (!result_socket) {
    std::cerr << "Ha habido un error en la creación del socket" << std::endl;
    //std::cerr << result_socket.error().message() << std::endl;
    int close_result = close(*result_socket);
    //std::cout << close_result << std::endl;
    if (close_result != 0) {
      std::cerr << "error al cerrar el socket" << std::endl;
    }
    return EXIT_FAILURE;
  }
  std::string address_make = "127.0.0.1";
  auto address = make_ip_address(address_make, 8080);
  if(address.has_value()) {
    std::error_code error_sendto = send_to(*result_socket,buffer,address.value());
    if (error_sendto ) {
      std::cerr << "Ha habido un error a la hora de hacer la función send_to que envia el mensaje y este es el error" << std::endl;
      std::cerr << error.message() << std::endl;
      int close_error = close(*result_socket);
      if (close_error != 0 ) {
        std::cerr << "error al cerrar el socket" << std::endl;
      }
      return EXIT_FAILURE;
    }
    else {
      int close_error = close(*result_socket);
      if (close_error != 0 ) {
        std::cerr << "error al cerrar el socket" << std::endl;
        return EXIT_FAILURE;
      }
      else {
        std::cout << "Correcto" << std::endl;
        return EXIT_SUCCESS;
      }
    }
  }
  else {
    std::cout << "Hubo un error al crear la ip con el Make_ip_address" << std::endl;
    int close_error = close(*result_socket);
    if (close_error != 0 ) {
      std::cerr << "Error al cerrar el socket" << std::endl;
    }
    return EXIT_FAILURE;
  }
}
