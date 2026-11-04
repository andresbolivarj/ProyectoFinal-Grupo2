#ifndef VISTAHISTORIAL_H
#define VISTAHISTORIAL_H

#include <QWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <vector>
#include "IGeneradorTicket.h"

class QShowEvent;

class VistaHistorial : public QWidget {
    Q_OBJECT

private:
    QLineEdit* txtBuscar;
    QCheckBox* chkFiltrarFecha;
    QDateEdit* fechaDesde;
    QDateEdit* fechaHasta;

    QTableWidget* tablaVentas;
    QTableWidget* tablaDetalle;
    QLabel* lblDetalleTitulo;
    QLabel* lblResumenDetalle;

    QLabel* lblTotalVentas;
    QLabel* lblIngresos;
    QLabel* lblPromedio;

    QPushButton* btnActualizar;
    QPushButton* btnRegenerar;

    std::vector<DatosVentaTicket> ventas;
    int indiceSeleccionado;

public:
    explicit VistaHistorial(QWidget* parent = nullptr);
    void actualizar();

protected:
    void showEvent(QShowEvent* event) override;

private:
    void configurarInterfaz();
    void cargarVentas();
    void aplicarFiltros();
    void renderizarTabla(const std::vector<int>& indices);
    void actualizarResumen(const std::vector<int>& indices);
    void mostrarDetalle(int indiceVenta);
    void limpiarDetalle();
    int indiceVentaDeFila(int fila) const;
    bool ventaCumpleFiltros(const DatosVentaTicket& venta) const;
    void regenerarTicket();
    bool guardarTicketComo(const DatosVentaTicket& datos, const QString& formato, QWidget* parent);
};

#endif // VISTAHISTORIAL_H
