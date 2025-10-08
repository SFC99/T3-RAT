# Proyecto: Mini-RAT en C++ 🐱‍👤
 
## Descripción general 📃
Cliente/servidor minimalista para control remoto cifrado en Windows. El cliente se conecta a un servidor, negocia AES-128 y ejecuta comandos remotos.


## Integrantes 🤝
- **Juan Angel Rodríguez Bulnes** — Módulo Comm, Modulo Persist
- **Angel David Morales Palomo** — Módulo Crypto,
- **Sofía Martínez Cisneros** — Módulo Exec, Main
---

## Requisitos del sistema ✔

- Windows 10/11 x64
- Visual Studio Code 2019+
- OpenSSL (o Cripto++)
- Winsock2
---

## Compilación ▶
1. Para iniciar el SERVIDOR:
   ./tcp_aes servidor [puerto]
   Ejemplo: ./tcp_aes servidor 8888

2. Para iniciar el CLIENTE (en otra terminal):
   ./tcp_aes cliente [ip] [puerto]
   Ejemplo: ./tcp_aes cliente 127.0.0.1 8888

El cliente generará una llave AES-128 aleatoria que la compartirá con el servidor, y ambos la imprimirán.
Luego se podrán enviar comandos cifrados desde el cliente que se ejecutarán en el servidor.
Para salir, escribe 'exit' o 'salir' en el cliente.

