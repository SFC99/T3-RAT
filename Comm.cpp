//Juan Angel Rodríguez Bulnes
#include "Comm.h"
#include <iostream>
#include <cstring>
#include <openssl/rand.h>
#include <iomanip>
#include <stdexcept>

struct WinsockInitializer {
    WinsockInitializer() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("Error al inicializar Winsock");
        }
    }
    
    ~WinsockInitializer() {
        WSACleanup();
    }
};

static WinsockInitializer winsockInit;

Comm::Comm() : socket_fd(INVALID_SOCKET) {}

Comm::~Comm() {
    cerrarConexion();
}

std::vector<unsigned char> Comm::generarLlave() {
    std::vector<unsigned char> llave(16);
    RAND_bytes(llave.data(), 16);
    return llave;
}

void Comm::imprimirLlave(const std::vector<unsigned char>& llave) {
    std::cout << "Llave AES-128 generada: ";
    for (unsigned char byte : llave) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << std::dec << std::endl;
}

bool Comm::iniciarServidor(int puerto) {
    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd == INVALID_SOCKET) {
        std::cerr << "Error al crear socket: " << WSAGetLastError() << std::endl;
        return false;
    }
    
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        std::cerr << "Error setsockopt: " << WSAGetLastError() << std::endl;
        closesocket(socket_fd);
        return false;
    }
    
    sockaddr_in direccion;
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(puerto);
    
    if (bind(socket_fd, (sockaddr*)&direccion, sizeof(direccion)) == SOCKET_ERROR) {
        std::cerr << "Error al hacer bind: " << WSAGetLastError() << std::endl;
        closesocket(socket_fd);
        return false;
    }
    
    if (listen(socket_fd, 1) == SOCKET_ERROR) {
        std::cerr << "Error al escuchar: " << WSAGetLastError() << std::endl;
        closesocket(socket_fd);
        return false;
    }
    
    std::cout << "Servidor escuchando en puerto " << puerto << std::endl;
    
    sockaddr_in clienteDir;
    int clienteLen = sizeof(clienteDir);
    SOCKET cliente_fd = accept(socket_fd, (sockaddr*)&clienteDir, &clienteLen);
    
    if (cliente_fd == INVALID_SOCKET) {
        std::cerr << "Error al aceptar conexión: " << WSAGetLastError() << std::endl;
        closesocket(socket_fd);
        return false;
    }
    
    closesocket(socket_fd);
    socket_fd = cliente_fd;
    
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clienteDir.sin_addr), clientIP, INET_ADDRSTRLEN);
    std::cout << "Cliente conectado desde " << clientIP << std::endl;
    return true;
}

bool Comm::conectarCliente(const std::string& ip, int puerto) {
    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd == INVALID_SOCKET) {
        std::cerr << "Error al crear socket: " << WSAGetLastError() << std::endl;
        return false;
    }
    
    sockaddr_in direccion;
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    
    if (inet_pton(AF_INET, ip.c_str(), &direccion.sin_addr) <= 0) {
        std::cerr << "Dirección IP inválida: " << ip << std::endl;
        closesocket(socket_fd);
        return false;
    }
    
    if (connect(socket_fd, (sockaddr*)&direccion, sizeof(direccion)) == SOCKET_ERROR) {
        std::cerr << "Error al conectar al servidor: " << WSAGetLastError() << std::endl;
        closesocket(socket_fd);
        return false;
    }
    
    std::cout << "Conectado al servidor " << ip << ":" << puerto << std::endl;
    return true;
}

bool Comm::enviarLlave(const std::vector<unsigned char>& llave) {
    if (send(socket_fd, (char*)llave.data(), llave.size(), 0) == SOCKET_ERROR) {
        std::cerr << "Error al enviar llave: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

std::vector<unsigned char> Comm::recibirLlave() {
    std::vector<unsigned char> llave(16);
    int recibido = recv(socket_fd, (char*)llave.data(), 16, 0);
    if (recibido <= 0) {
        std::cerr << "Error al recibir llave: " << WSAGetLastError() << std::endl;
        return std::vector<unsigned char>();
    }
    return llave;
}

bool Comm::enviarMensaje(const std::string& mensaje) {
    uint32_t longitud = htonl(mensaje.length());
    if (send(socket_fd, (char*)&longitud, sizeof(longitud), 0) == SOCKET_ERROR) {
        return false;
    }
    
    if (send(socket_fd, mensaje.c_str(), mensaje.length(), 0) == SOCKET_ERROR) {
        return false;
    }
    return true;
}

std::string Comm::recibirMensaje() {
    uint32_t longitud;
    int recibido = recv(socket_fd, (char*)&longitud, sizeof(longitud), 0);
    if (recibido <= 0) {
        return "";
    }
    
    longitud = ntohl(longitud);
    std::vector<char> buffer(longitud);
    recibido = recv(socket_fd, buffer.data(), longitud, 0);
    if (recibido <= 0) {
        return "";
    }
    
    return std::string(buffer.begin(), buffer.end());
}

void Comm::cerrarConexion() {
    if (socket_fd != INVALID_SOCKET) {
        closesocket(socket_fd);
        socket_fd = INVALID_SOCKET;
    }
}

void Comm::setLlave(const std::vector<unsigned char>& nuevaLlave) {
    llave = nuevaLlave;
}

std::vector<unsigned char> Comm::getLlave() const {
    return llave;
}
