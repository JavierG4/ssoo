#include "tools.h"

std::atomic<bool> quit_request = false;
int signal_flag = 15;

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
      close(fd);
      int close_result = close(*result_socket);
    //std::cout << close_result << std::endl;
      if (close_result != 0) {
        std::cerr << "error al cerrar el socket" << std::endl;
      }
      return std::error_code(errno, std::system_category());
    }
    std::string address_make;
    uint16_t f_port;
    if (getenv ("NETCP_IP") == NULL) {
      address_make = "127.0.0.1";
    } 
    else {
      address_make = getenv ("NETCP_IP");
    }
    if (getenv ("NETCP_PORT") == NULL) {
      f_port = 8080;
    }   
    else {
      f_port = static_cast<uint16_t>(std::stoi(getenv("NETCP_PORT")));
    }
    std::cout << address_make << " " << f_port << std::endl;
    auto address = make_ip_address(address_make, f_port);
    if(address.has_value()) {
      while(quit_request == false) {
        std::vector<uint8_t> buffer(1024);
        std::error_code error_read = read_file(fd, buffer); // Bytes > 0 sino error
        if ( error_read ) {
          std::cout << " Error en el read " << error_read.message() <<std::endl;
          close(fd);
          close(*result_socket);
          return std::error_code(errno, std::system_category());
        }
        //std::cout << buffer.size() << std::endl;
        // Comrpobar error
        //std::cout << error.value() << std::endl;
        std::error_code error_sendto = send_to(*result_socket,buffer,address.value());
        // Tamaño buffer comprobar error
        if (error_sendto) {
          std::cout << "Error en el sendto" << error_sendto.message() << std::endl;
          close(fd);
          close(*result_socket);
          return std::error_code(errno, std::system_category());
        }
        if ( buffer.size() == 0) {
          close(fd);
          close(*result_socket);    
          return std::error_code(0, std::system_category());
        }
        std::this_thread::sleep_for(std::chrono::microseconds(2));
      }
      close(fd);
      close(*result_socket);
      std::cout << "Finished with Signal " << signal_flag << std::endl;
      return std::error_code(errno, std::system_category());
    }
    else {
      std::cout << "Hubo un error al crear la ip con el Make_ip_address" << std::endl;
      close(fd);
      int close_error = close(*result_socket);
      if (close_error != 0 ) {
        std::cerr << "Error al cerrar el socket" << std::endl;
      }
      return std::error_code(errno, std::system_category());
    }
  }
}

std::error_code netcp_receive_file(const std::string& filename) {
  int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666); 
  //std::cout << "hola3" << std::endl;
  std::expected<int, std::error_code> result_socket = make_socket(std::nullopt);
  if (!result_socket) {
    std::cerr << "Ha habido un error en la creación del socket" << std::endl;
    //std::cerr << result_socket.error().message() << std::endl;
    int close_result = close(*result_socket);
    close(fd);
    //std::cout << close_result << std::endl;
    if (close_result != 0) {
      std::cerr << "error al cerrar el socket" << std::endl;
    }
    return std::error_code(errno, std::system_category());
  }
  std::string address_make;
  uint16_t f_port;  
  if (getenv ("NETCP_IP") == NULL) {
    address_make = "127.0.0.1";
  } 
  else {
    address_make = getenv ("NETCP_IP");
  }
  if (getenv ("NETCP_PORT") == NULL) {
    f_port = 8080;
  }   
  else {
    f_port = static_cast<uint16_t>(std::stoi(getenv("NETCP_PORT")));
  }
  std::cout << address_make << " " << f_port << std::endl;
  auto address = make_ip_address(address_make, f_port);    
  bind(*result_socket,reinterpret_cast<const sockaddr*>(&address.value()),sizeof(address));
  while (quit_request == false) {
    std::vector<uint8_t> buffer(1024);
    std::error_code error = receive_from(*result_socket, buffer ,address.value()); // Bytes > 0 sino error
    std::cout << buffer.size() << std::endl;
      //std::cout << "hola" << std::endl;
      //std::cout << error.value() << std::endl;
    if ( error ) {
      std::cout << "Error en el receive " << error.message() << std::endl;
      close(fd);
      close(*result_socket);
      std::cout << "Finished with Signal " << signal_flag << std::endl;
      return std::error_code(errno, std::system_category());
    }
    if ( buffer.empty() ) {
      std::cout << "Final de recibir" << std::endl;
      close(fd);
      close(*result_socket);
      return std::error_code(0, std::system_category());
    }
    std::error_code error_write = write_file(fd,buffer);
    if ( error_write ) {
      std::cout << "error en el write " << error_write.message() << std::endl;
      close(fd);
      close(*result_socket);
      return std::error_code(errno, std::system_category());      
    }
  }
  close(fd);
  close(*result_socket);
  std::cout << "Finished with Signal " << signal_flag << std::endl;
  return std::error_code(errno, std::system_category());
}

std::expected<int, std::error_code> open_file(std::string file) { // Flag int 0 1
  int fd = open(file.c_str() ,O_RDONLY);
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
  //std::cout << "Receive " << bytes_read << std::endl;
  //std::cout << "hola" << std::endl;
  if (bytes_read < 0) {
    return std::error_code(errno, std::system_category());
  }
  message.resize(bytes_read);
  //std::cout << std::format("El sistema '{}'' envió el mensaje '{}'\n", ip_address_to_string(address),message.text.c_str());
  return std::error_code(0, std::system_category());
}


std::error_code write_file(int fd, const std::vector<uint8_t>& buffer) {
  int bytes_received = write(fd, buffer.data(), buffer.size());
  //std::cout << "Write " << bytes_received << std::endl;
  if (bytes_received < 0) {
// Error al escribir el mensaje
    return std::error_code(errno, std::system_category());
  }
  return std::error_code(0, std::system_category());
}
void term_signal_handler(int signum) {
  const char *message;
  if (signum == SIGTERM) {
    message = "Señal SIGTERM recibida.\n";
    signal_flag = SIGTERM;
  }
  else if (signum == SIGINT) {
    message = "Señal SIGINT recibida.\n";
    signal_flag = SIGINT;
  }
  else if (signum == SIGQUIT) {
    message = "Señal SIGQUIT recibida.\n";
    signal_flag = SIGQUIT;
  }
  else if(signum == SIGHUP) {
    message = "Señal SIGUP recibida .\n";
    signal_flag = SIGHUP;
  }
  else {
    message = "Señal desconocida recibida.\n";
  }
  quit_request = true;
  write(STDOUT_FILENO, message, strlen(message));
}

void segv_signal_handler(int signum) {
    const char* message = "¡Algo ha ido mal! Señal SIGSEGV recibida.\n";
    write( STDOUT_FILENO, message, strlen(message));
    quit_request = true;
    // No podemos ignorar el problema porque si salimos de aquí sin hacer nada se reintentará la instrucción que causo
    // el error, devolviéndonos aquí de nuevo, indefinidamente.
    //
    // Lo interesante es que esta señal nos da la oportunidad de hacer cosas críticas antes de terminar.
    exit(1);
}

std::string getenv(const std::string& name) {
  char* value = getenv(name.c_str());
  if (value) {
    return std::string(value);
  }
  else {
    return std::string();
  }
}
