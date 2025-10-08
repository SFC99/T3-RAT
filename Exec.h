#ifndef EXEC_H
#define EXEC_H

#include <string>

class Exec {
public:
    static std::string capturarComando();
    static std::string ejecutarComando(const std::string& comando);
};

#endif
