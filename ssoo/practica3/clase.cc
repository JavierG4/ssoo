#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> //htonl

int main(int argc, char** argv) {
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
      // Error al crear el socket
      std::cerr << "Error socket" << std::endl;
    }
  std::cout << fd << " e " << std::endl;
  std::string message_text = "Prueba";
  sockaddr_in remote_address{}; 
  remote_address.sin_family = AF_INET; 
  remote_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
  remote_address.sin_port = htons(8080);
  int bytes_sent = sendto(fd, message_text.data(), message_text.size(), 0, reinterpret_cast<const sockaddr*>(&remote_address), sizeof(remote_address));
  if (bytes_sent < 0) {
// Error al enviar el mensaje.
  }
  //Estaria bien si se comprueba si se cierra bien el close
  close(fd);
  std::cout << "fd " << fd << std::endl; 
}