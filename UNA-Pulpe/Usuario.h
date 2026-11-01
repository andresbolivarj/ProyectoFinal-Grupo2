#ifndef USUARIO.H //Guardia de inclusion: Evita que el archivo se lea más de una vez.
#define USUARIO.H

#include <QString.h>

class Usuario {
protected: //se utiliza este tipo de modificador para que solo se puedan acceder las clases derivadas (administrador y cajero)
	QString id;
	QString nombre;
	QString nombreUsuario;
	QString hashContrasena;
	bool activo;

public:
	Usuario(const QString& id, const QString& nombre, const QString& nombreUsuario,
		const QString& hashContrasena, bool activo = true)
		: id(id), nombre(nombre), nombreUsuario(nombreUsuario),
		hashContrasena(hashContrasena), activo(activo) {} //lista de inicializacion, lo que esta afuera es el atributo y lo que esta por dentro del parentesis es el valor que entra por argumento.

	
	Usuario(const QString& id, const QString& clave)
		: id(usuario), nombre(usuario), nombreUsuario(usuario),
		hashContrasena(clave), activo(true) { } //esta parte sirve para crear un usuario o cuenta en el programa de forma rapida con solo dos datos (usuario y la clave)

	virtual ~Usuario() = default; //Destructor: evita qque se quede basura en la memoria al borrar un usuario y al poner el = default se le dice al compilador que use la destruccion para mantener el comportamiento virtual.

	QString getId() const { return id; }
	QString getNombre() const { return nombre; }
	QString getNombreUsuario() const { return nombreUsuario; }  //Metodos get: sirven para leer datos del usuario desde afuera sin modificar nada.
	QString getHashContrasena() const { return hashContrasena; }
	bool isActivo() const { return activo:}

	void setClave(const QString& nuevaClave) { hashContrasena = nuevaClave; } //Metodo set: permite cambiar o actualizar la clave del usuario.

	virtual QString rol() const = 0; //Obliga a las clases (administrador y cajero) a decir su rol y hace que esta clase sea abstracta.
};

class Administrador : public Usuario {   //Clase administrador que hereda de usuario y reutiliza el constructor comple de la clase padre.
public:
	Administrador(const QString& id, const QString& nombre, const QString& nombreUsuario,
		const QString& hashContrasena, bool activo = true)    //se declara la clase hija heredando de la clase padre y pasando por parametros el constructor de la clase base para reutilizar su codigo.
		: Usuario(id, nombre, nombreUsuario, hashContrasena, activo) {}

	Administrador(const QString& usuario, const QString& clave)
		: Usuario(usuario, clave) { }    //constructor simplificado, la pasa el usuario y la clave al constructor rapida de usuario.

	QString rol() const override { return "Administrador"; }  //sobreescribe el metodo virtual de la clase base para devolver el rol especifico.
};

class Cajero : public Usuario {  //clase cajero, hereda de usuario y reutiliza sus constructores para definir su rol como cajero.
public:
	Cajero(const QString& id, const QString& nombre, const QString& nombreUsuario,
		const QString& hashContrasena, bool activo = true)
		: Usuario(id, nombre, nombreUsuario, hashContrasena, activo) { }

	Cajero(const QString& usuario, const QString& clave)
		: Usuario(usuario, clave) {}

	QString rol() const override { return "Cajero"; }
};

#endif //cierra el bloque de proteccion iniciado al principio.
//Listo