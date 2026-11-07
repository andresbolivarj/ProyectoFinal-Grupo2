#ifndef VISTAINVENTARIO_H
#define VISTAINVENTARIO_H

#include <QWidget> //ventana 
#include <QTableWidget>//tabla
#include <QLineEdit>//escribir texto
#include <QPushButton>//boton
#include <QComboBox>//lista desplegable
#include <QLabel>//mostrar texto
#include <QVBoxLayout>//organizar verticalmente
#include <QHBoxLayout>//organizar horizontalmente
#include <QHeaderView>//configurar encabezado
#include <QMessageBox>//mostrar mensaje
#include <QDialog>//ventana de dialogo
#include <QDoubleSpinBox>//numeros decimales
#include <QSpinBox>//numeros enteros
#include <vector>//vector
#include "VistaVentas.h"
#include "GestorJSON.h"

class VistaInventario : public QWidget { //VistaInventario hereda de QWidget
    Q_OBJECT //permite utilizar señales y ranuras

private:
    //elementos visuales
    QLineEdit* txtBuscar;
    QComboBox* comboCategoriaFiltro;
    QTableWidget* tablaProductos;
    //botones
    QPushButton* btnAgregar;
    QPushButton* btnEditar;
    QPushButton* btnEliminar;

    std::vector<ProductoItem> listaProductos;

public:
    //constructor
    explicit VistaInventario(QWidget* parent = nullptr); //parent: representa un componente padre y nullptr significa que no tiene este componnente
    void actualizar(); // Permite recargar la tabla externamente

signals:  //comunicar eventos entre diferentes componentes
    void productosActualizados();

protected:
    void showEvent(QShowEvent* event) override; // Se ejecuta automáticamente al abrir la vista

private:
    void configurarInterfaz();
    void cargarDatos();
    void renderizarTabla();
    void actualizarCategoriasFiltro();

    void abrirFormularioProducto(ProductoItem* productoExistente = nullptr);
    bool guardarProductoEnJSON(const ProductoItem& prod, bool esEdicion);
    void eliminarProductoSeleccionado();
    void filtrarTabla();
};

#endif // VISTAINVENTARIO_H
