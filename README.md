# Proyecto Final: La Pulpe 🛒
Aplicación de escritorio desarrollada en **C++** utilizando **Qt Widgets** para la administración integral de una pequeña pulpería.

## 👥 Integrantes del Equipo
* Integrante 1 - [Moisés Mora] (https://github.com/mmora5852-crypto) - Módulo: Productos e inventario.
* Integrante 2 - [Alison Gonzales] (https://github.com/alisongonzalezv) - Módulo: Ventas y cobro.
* Integrante 3 - [Andrés Bolívar] (https://github.com/andresbolivarj) - Módulo: Comprobantes e historial.
* Integrante 4 - [Adriel Villalobos] (https://github.com/adrielvillalobostoruno-coder) - Módulo: Usuarios y permisos.

---

## 🏗️ Enfoque Técnico y Decisiones Arquitectónicas
* **Principio de Responsabilidad Única (SRP):** Se separó estrictamente la interfaz gráfica (ventanas de Qt) de la lógica de negocio y del subsistema de persistencia en archivos.
* **Principio Abierto-Cerrado (OCP):** El módulo de generación de comprobantes utiliza una estructura polimórfica que permite añadir nuevos formatos (como TXT o PDF) mediante la extensión de clases sin alterar el código existente.
* **Persistencia:** Almacenamiento local basado en archivos (formato JSON) para conservar productos, ventas y usuarios.

---

## ⚙️ Requisitos Previos
Para compilar y ejecutar este proyecto en su computadora, debe tener instalado:
* **Visual Studio** (con soporte para C++).
* **Qt Widgets** (versión compatible con el desarrollo del curso).

---

## 🚀 Pasos para Compilar y Ejecutar

1. **Clonar el repositorio:**
   
   git clone https://github.com/tu-usuario/tu-repositorio.git
   

2. **Abrir el proyecto:**
   Abra Visual Studio y seleccione la opción para abrir un proyecto o solución, o abra el archivo de proyecto de Qt (.pro o configuración de CMake según su entorno).

3. **Compilar:**
   * Configure la compilación en modo Debug o Release.
   * Haga clic en Compilar (Build Solution) para generar los ejecutables y verificar que no existan errores de sintaxis.

4. **Ejecutar:**
   Presione el botón de Iniciar depuración (Start Debugging / F5) en Visual Studio para lanzar la aplicación de escritorio.
