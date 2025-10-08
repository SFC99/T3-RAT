//Juan Angel Rodríguez Bulnes
#ifndef COMM_H
#define COMM_H

#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class Comm {
private:
    SOCKET socket_fd;
    std::vector<unsigned char> llave;
    
public:
    Comm();
    ~Comm();
    
    static std::vector<unsigned char> generarLlave();
    static void imprimirLlave(const std::vector<unsigned char>& llave);
    
    bool iniciarServidor(int puerto);
    bool conectarCliente(const std::string& ip, int puerto);
    
    bool enviarLlave(const std::vector<unsigned char>& llave);
    std::vector<unsigned char> recibirLlave();
    
    bool enviarMensaje(const std::string& mensaje);
    std::string recibirMensaje();
    
    void cerrarConexion();
    
    void setLlave(const std::vector<unsigned char>& nuevaLlave);
    std::vector<unsigned char> getLlave() const;
};

#endif
