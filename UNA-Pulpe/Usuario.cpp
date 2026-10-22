#include "Usuario.h"

Usuario::Usuario() {
	this->id = "";
	this->usuario = "";
	this->contrasena = "";
	this->rol = "";
 }

Usuario::Usuario(std::string id, std::string usuario, std::string contrasena, std::string rol) {
	this->id = id;
	this->usuario = usuario;
	this->contrasena = contrasena;
	this->rol = rol;
}

//Getters

std::string Usuario::getId() {
	return this->id;
}

std::string Usuario::getUsuario() {
	return this->usuario;
}

std::string Usuario::getContrasena() {
	return this->contrasena;
}

std::string Usuario::getRol() {
	return this->rol;
}

bool Usuario::esAdministrador() {
	return this->rol == "Administrador" || this->rol == "administrador" || this->rol == "Admin";
}

bool Usuario::esCajero() {
	return this->contrasena == "Cajero" || this->rol == "cajero";
}