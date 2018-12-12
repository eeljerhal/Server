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
#include "json.hpp"
#include "checkArgs.h"
#include <iostream>    // For cerr and cout
#include <cstdlib>     // For atoi()
#include <fstream>

const uint32_t RCVBUFSIZE = 32;    // Size of receive buffer

// TCP client handling function
void HandleTCPClient(TCPSocket *sock) {
	std::cout << "Handling client ";
	try {
		std::cout << sock->getForeignAddress() << ":";
	} catch (SocketException e) {
		std::cerr << "Unable to get foreign address" << std::endl;
	}
	try {
		std::cout << sock->getForeignPort();
	} catch (SocketException e) {
		std::cerr << "Unable to get foreign port" << std::endl;
	}
	std::cout << std::endl;

	char buffer[RCVBUFSIZE + 1];    // Buffer para el echo string + \0
	uint32_t bytesReceived = 0;              // Bytes leídos en cada recv()
	uint8_t opcion = 1;
	std::string headerRequest = "";

	while (opcion){
		bytesReceived = sock->recv(buffer, RCVBUFSIZE); //se lee request
		
		if (bytesReceived < 0) {
			std::cerr << "No se puede leer." << std::endl;
		}

		buffer[bytesReceived] = '\0';        // termina el linea!
		headerRequest = headerRequest + buffer + "\n";	//Se concatena cada linea en headerRequest
		
		if (bytesReceived < 32) {	//Cuando se está llegando al final del request
			opcion = 0;	//Salir del while
		}
		
	}

	std::string pagina;
	pagina = headerRequest.substr (4, headerRequest.substr(4).find(" ")); //se recupera la página

	std::string headerResponse;
	std::ifstream archivo;

	if(pagina == "/"){
		archivo.open("../www-data/index.html");
		headerResponse= "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"; //header response
	}else if(pagina == "/pagina1.html"){
		archivo.open("../www-data/pagina1.html");
		headerResponse= "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"; //header response
	}else if(pagina == "/pagina2.html"){
		archivo.open("../www-data/pagina2.html");
		headerResponse= "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"; //header response
	}else if(pagina == "/pagina3.html"){
		archivo.open("../www-data/pagina3.html");
		headerResponse= "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"; //header response
	}else{
		archivo.open("../www-error/404.html");
		headerResponse= "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n"; //header response
	}

	sock->send(headerResponse.c_str(), headerResponse.length());	//Se envía respuesta al cliente
	//se lee archivo html
	std::string linea;
	std::string html = "";
	if (archivo.is_open()){
		while(getline(archivo,linea)){
			html = html + linea + "\n";
		}
		archivo.close();
	}else{
		std::cout << "No se pudo abrir html\n"; 
	}
	//se envía el html
	sock->send(html.c_str(), html.length());
	delete sock;
}

int main(int argc, char *argv[]) {
	
	checkArgs* argumentos = new checkArgs(argc, argv);
	
	uint16_t echoServPort;   
	echoServPort  = argumentos->getArgs().PORT;

	try {
		TCPServerSocket servSock(echoServPort);     // Server Socket object
		for (;;) {   // Run forever
			HandleTCPClient(servSock.accept());       // Wait for a client to connect
		}
	} catch (SocketException &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	// NOT REACHED

	return EXIT_SUCCESS;
}
