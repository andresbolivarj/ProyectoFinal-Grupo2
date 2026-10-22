#pragma once
#include <string>

class Usuario {
private:
	std::string id;
	std::string usuario;
	std::string contrasena;
	std::string rol;

public:
	Usuario();
		Usuario(std::string id, std::string usuario, std::string contrasena, std::string rol);
	std::string getId();
	std::string getUsuario();
	std::string getContrasena();
	std::string getRol();


	bool esAdministrador();
	bool esCajero();
};

