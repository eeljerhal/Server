/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "YASL.h"      // For Socket, ServerSocket, and SocketException
#include "json.hpp"		// Librería para la manipulación de archivos json
#include <iostream>    // For cerr and cout
#include <cstdlib>     // For atoi()
#include <fstream>

using namespace nlohmann;	//namespace para la utilización de json

const uint32_t RCVBUFSIZE = 32;    // Tamaño del buffer

// Manejo de la conexion de clientes TCP
void HandleTCPClient(TCPSocket *sock, std::string root_dir, std::string notFoundFile) {
	
	char buffer[RCVBUFSIZE + 1];    // Buffer para el echo string + \0
	uint32_t bytesReceived = 0;              // Bytes leídos en cada recv()
	uint8_t opcion = 1;	//variable para el control del bucle
	std::string HTTPRequest = ""; //almacena la requestLine y el header del request
	std::string HTTPResponse;	//almacena el status line y el header del response + contenido
	std::ifstream archivoHtml;	//archivo HTML

	std::cout << "Se recibe al cliente: ";

	/*Loop para la recuperación de la petición del cliente*/
	while (opcion){
		bytesReceived = sock->recv(buffer, RCVBUFSIZE); //se lee request
		if (bytesReceived < 0) {
			std::cerr << "No se puede leer." << std::endl;
		}else{
			buffer[bytesReceived] = '\0';        // termina la parte del mensaje
			HTTPRequest = HTTPRequest + buffer + "\n";	//Se concatena cada parte
			
			if (bytesReceived < 32) {	//Cuando se está llegando al final del request
				opcion = 0;	//Salir del loop
			}
		}
	}

	/*SE MUESTRAN LOS DATOS DEL CLIENTE*/
	try {
		std::cout << sock->getForeignAddress() << ":";
	} catch (SocketException e) {
		std::cerr << "No se pudo obtener la ip del cliente." << std::endl;
	}
	try {
		std::cout << sock->getForeignPort();
	} catch (SocketException e) {
		std::cerr << "No se pudo obtener el puerto del cliente" << std::endl;
	}

	/*SE OBTIENE EL CONTENIDO QUE SE PIDE EN EL REQUEST*/
	std::string pagina;
	//SI EL FORMATO DEL REQUEST ES CORRECTO
	if(HTTPRequest.length() > 4){
		pagina = HTTPRequest.substr(4, HTTPRequest.substr(4).find(" ")); //se recupera la página requerida

		//SE MUESTRA PETICION POR CONSOLA
		std::cout << " Con una peticion a: " << pagina << std::endl;
		
		/*SE COMPRUEBA SI ES QUE EXISTE DICHO CONTENIDO EN EL SERVIDOR*/
		if(pagina == "/"){
			root_dir = root_dir + "/index.html";
			archivoHtml.open(root_dir);
			HTTPResponse= "HTTP/1.1 200 OK\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n"; //status line + header response

		}else if((pagina == "/pagina1.html") || (pagina == "/pagina2.html") || (pagina == "/pagina3.html")){
			root_dir = root_dir + pagina;
			archivoHtml.open(root_dir);
			HTTPResponse= "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"; //status line + header response

		}else{
			notFoundFile = notFoundFile + "/404.html";
			archivoHtml.open(notFoundFile);
			HTTPResponse= "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n"; //status line + header response

		}
	}else{
		//FORMATO INCORRECTO EN EL REQUEST
		std::cout << " Error al leer Request.\n";
		notFoundFile = notFoundFile + "/400.html";
		archivoHtml.open(notFoundFile);
		HTTPResponse= "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n"; //status line + header response
	}
		
	/*SE LEE Y ENVIA EL CONTENIDO AL CLIENTE*/
	//se lee archivo html
	std::string linea;
	std::string html = "";
	if (archivoHtml.is_open()){
		while(getline(archivoHtml,linea)){
			html = html + linea + "\n";
		}
		archivoHtml.close();
	}else{
		std::cout << "No se pudo abrir html\n"; 
	}

	HTTPResponse = HTTPResponse + html;	//SE EMPAQUETA LA RESPUESTA HTTP
	
	sock->send(HTTPResponse.c_str(), HTTPResponse.length()); //se envía http response
	delete sock;	//finalmente se cierra el socket
}

int main(int argc, char *argv[]) {

	uint16_t serverPort;	//almacena el puerto del servidor
	std::string ipAddress;	//la ip del servidor
	std::string root_dir;	//la ruta al contenido de las páginas
	std::string notFoundFile;	//la ruta al contenido de la pagina en respuesta al error 404

	/* MANEJO DE ARCHIVO JSON*/
	std::ifstream jsonFile("../bin/config.json");	//se abre el archivo de config.json
	if (jsonFile.is_open()){
		json configJson;	//se crea la variable configJson de tipo json
		jsonFile >> configJson;	//se traspasa el archivo a la variable

		/*RECUPERACION DE DATOS DESDE EL JSON*/
		ipAddress = configJson.at("ip");
		serverPort = configJson.at("puerto");
		root_dir = configJson.at("root_dir");
		notFoundFile = configJson.at("notFoundFile");

	}else{
		std::cout << "Error al abrir archivo json\n"; 
	}

	/*CONEXION TPC */
	try {
		TCPServerSocket servSock(ipAddress, serverPort);     // Se crea el socket server con la ip, puerto
		for (;;) {   // se ejecuta por siempre
			HandleTCPClient(servSock.accept(), root_dir, notFoundFile);       // Espera por la conexión de clientes
		}
	} catch (SocketException &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	// NOT REACHED

	return EXIT_SUCCESS;
}
