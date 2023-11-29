#include "tools.h"

std::error_code netcp_send_file(const std::string& filename) {
  std::expected<int, std::error_code> result = open_file(filename); // Abro el archivo 
  if (!result) {
    std::cerr << "Error al abrir el archivo por el siguiente motivo" << std::endl;
    std::cerr << result.error().message() << std::endl;
    return std::error_code(errno, std::system_category());
  }
  else { 
    int fd = *result; 
  //std::cout << fd << std::endl;
// Creo el socket con la funcion make_socket
    std::expected<int, std::error_code> result_socket = make_socket(std::nullopt);
    if (!result_socket) {
      std::cerr << "Ha habido un error en la creación del socket" << std::endl;
    //std::cerr << result_socket.error().message() << std::endl;
      int close_result = close(*result_socket);
    //std::cout << close_result << std::endl;
      if (close_result != 0) {
        std::cerr << "error al cerrar el socket" << std::endl;
      }
      return std::error_code(errno, std::system_category());
    }
    std::string address_make = "127.0.0.1";
    auto address = make_ip_address(address_make, 8080);
    int contador = 0;
    if(address.has_value()) {
      while(true) {
        std::vector<uint8_t> buffer(1024);
        read_file(fd, buffer); // Bytes > 0 sino error
        //std::cout << error.value() << std::endl;
        std::error_code error_sendto = send_to(*result_socket,buffer,address.value());
        if( error_sendto.value() == 0) {
          contador++;
        }
        if ( error_sendto.value() == 1) {
          break;
        }
      }
      
      if ( contador > 1 ) {
        int close_error = close(*result_socket);
        if (close_error != 0 ) {
          std::cerr << "Error al cerrar el socket" << std::endl;
        }        
        return std::error_code(0, std::system_category());
      }
      else {
        int close_error = close(*result_socket);
        if (close_error != 0 ) {
          std::cerr << "Error al cerrar el socket" << std::endl;
        }
        return std::error_code(errno, std::system_category());
      }
    }
    else {
      std::cout << "Hubo un error al crear la ip con el Make_ip_address" << std::endl;
      int close_error = close(*result_socket);
      if (close_error != 0 ) {
        std::cerr << "Error al cerrar el socket" << std::endl;
      }
      return std::error_code(errno, std::system_category());
    }
  }
}

std::error_code netcp_receive_file(const std::string& filename) {
  int fd = open(filename.c_str(), O_WRONLY | O_CREAT, 0666); 
    //std::cout << "hola3" << std::endl;
    std::expected<int, std::error_code> result_socket = make_socket(std::nullopt);
    if (!result_socket) {
      std::cerr << "Ha habido un error en la creación del socket" << std::endl;
    //std::cerr << result_socket.error().message() << std::endl;
      int close_result = close(*result_socket);
    //std::cout << close_result << std::endl;
      if (close_result != 0) {
        std::cerr << "error al cerrar el socket" << std::endl;
      }
      return std::error_code(errno, std::system_category());
    }
    std::string address_make = "127.0.0.1";
    auto address = make_ip_address(address_make, 8080);    
    bind(*result_socket,reinterpret_cast<const sockaddr*>(&address.value()),sizeof(address));
    while (true) {
      std::vector<uint8_t> buffer(1024);
      std::error_code error = receive_from(*result_socket, buffer ,address.value()); // Bytes > 0 sino error
      //std::cout << "hola" << std::endl;
      std::cout << error.value() << std::endl;
      if ( error.value() != 0 ) {
        std::cout << " break" << std::endl;
        break;
      }
      else {
        write_file(fd,buffer);
      }
    }
    return std::error_code(0, std::system_category());
}

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
  //std::cout << "sent " << bytes_sent << std::endl;
  if (bytes_sent < 0) {
// Error al enviar el mensaje.
    return std::error_code(errno, std::system_category());
  }
  if (bytes_sent == 0) {
    return std::error_code(1, std::system_category());
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

std::error_code receive_from(int fd,std::vector<uint8_t>& message, sockaddr_in& address) {
  socklen_t src_len = sizeof(address);
  int bytes_read = recvfrom(fd, message.data(), message.size(), 0,reinterpret_cast<sockaddr*>(&address), &src_len);
  std::cout << "Receive " << bytes_read << std::endl;
  //std::cout << "hola" << std::endl;
  if (bytes_read <= 0) {
    return std::error_code(errno, std::system_category());
  }
  message.resize(bytes_read);
  //std::cout << std::format("El sistema '{}'' envió el mensaje '{}'\n", ip_address_to_string(address),message.text.c_str());
  return std::error_code(0, std::system_category());
}


std::error_code write_file(int fd, const std::vector<uint8_t>& buffer) {
  int bytes_received = write(fd, buffer.data(), buffer.size());
  std::cout << "Write " << bytes_received << std::endl;
  if (bytes_received < 0) {
// Error al escribir el mensaje
    return std::error_code(errno, std::system_category());
  }
  return std::error_code(0, std::system_category());
}



/*        if (error_sendto ) {
          std::cerr << "Ha habido un error a la hora de hacer la función send_to que envia el mensaje y este es el error" << std::endl;
          std::cerr << error.message() << std::endl;
          int close_error = close(*result_socket);
          if (close_error != 0 ) {
            std::cerr << "error al cerrar el socket" << std::endl;
          }
          return std::error_code(errno, std::system_category());
        }
        else {
          int close_error = close(*result_socket);
          if (close_error != 0 ) {
            std::cerr << "error al cerrar el socket" << std::endl;
            return std::error_code(errno, std::system_category());
          }
          else {
            std::cout << "Correcto" << std::endl;
            return std::error_code(0, std::system_category());
          }
        }*/