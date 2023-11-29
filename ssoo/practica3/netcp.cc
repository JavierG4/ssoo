#include "tools.h"

// Modo de compilación : g++-13 netcp.cc tools.cc -std=c++23

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
  std::string flag_receive = argv[1];
  std::string receive = "-l";
  std::string filename = argv[1];
  if (flag_receive == receive) {
    std::string filename2 = argv[2];
    netcp_receive_file(filename2);
    //std::cout << " e";
  }
  else {
    std::error_code error = netcp_send_file(filename);
    if (error) {
      std::cerr << "Ha habido un error a la hora de hacer la función read_file y este es el error" << std::endl;
    }
  }
}
