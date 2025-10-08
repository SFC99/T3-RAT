#ifndef PERSIST_H
#define PERSIST_H

#include <string>
#include <windows.h>

class Persist {
public:
    static bool establecerPersistencia(const std::string& nombreEjecutable, const std::string& rutaEjecutable);
    static bool eliminarPersistencia(const std::string& nombreEjecutable);
    static bool verificarPersistencia(const std::string& nombreEjecutable);
    static std::string obtenerRutaAPPDATA();
    static bool copiarEjecutable(const std::string& rutaActual, const std::string& nombreDestino);
};

#endif