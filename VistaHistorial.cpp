#include "VistaHistorial.h"
#include "GestorJSON.h"

#include <QDate>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPageLayout>
#include <QPageSize>
#include <QPrinter>
#include <QSaveFile>
#include <QShowEvent>
#include <QStandardPaths>
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QTextDocument>
#include <QVBoxLayout>
#include <algorithm>
#include <memory>

namespace {
const char* kEstiloBotonPrimario =
    "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
    "border: none; border-radius: 8px; padding: 10px 16px; font-size: 13px; }"
    "QPushButton:hover { background-color: #059669; }"
    "QPushButton:disabled { background-color: #d1d5db; color: #f9fafb; }";

const char* kEstiloBotonSecundario =
    "QPushButton { background-color: #f3f4f6; color: #374151; font-weight: bold; "
    "border: 1px solid #e5e7eb; border-radius: 8px; padding: 10px 16px; font-size: 13px; }"
    "QPushButton:hover { background-color: #e5e7eb; }";

QString moneda(double valor) {
    return QString("₡ %1").arg(valor, 0, 'f', 2);
}
}

VistaHistorial::VistaHistorial(QWidget* parent)
    : QWidget(parent), indiceSeleccionado(-1) {
    configurarInterfaz();
    actualizar();
}

void VistaHistorial::actualizar() {
    cargarVentas();
    aplicarFiltros();
}

void VistaHistorial::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizar();
}

void VistaHistorial::configurarInterfaz() {
    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(15, 15, 15, 15);
    layoutPrincipal->setSpacing(15);
    setStyleSheet("background-color: #f8f9fa;");

    // Encabezado
    QHBoxLayout* layoutHeader = new QHBoxLayout();
    QVBoxLayout* layoutTitulos = new QVBoxLayout();
    layoutTitulos->setSpacing(2);

    QLabel* lblTitulo = new QLabel("Historial de Ventas", this);
    lblTitulo->setStyleSheet("font-size: 20px; font-weight: bold; color: #111827;");
    QLabel* lblSubtitulo = new QLabel("Consulte las ventas registradas y regenere sus comprobantes", this);
    lblSubtitulo->setStyleSheet("font-size: 12px; color: #6b7280;");

    layoutTitulos->addWidget(lblTitulo);
    layoutTitulos->addWidget(lblSubtitulo);

    btnActualizar = new QPushButton("Actualizar", this);
    btnActualizar->setStyleSheet(kEstiloBotonSecundario);
    connect(btnActualizar, &QPushButton::clicked, this, &VistaHistorial::actualizar);

    layoutHeader->addLayout(layoutTitulos);
    layoutHeader->addStretch();
    layoutHeader->addWidget(btnActualizar);
    layoutPrincipal->addLayout(layoutHeader);

    // Tarjetas resumen
    QHBoxLayout* layoutResumen = new QHBoxLayout();
    layoutResumen->setSpacing(15);

    auto crearTarjeta = [this](const QString& titulo, QLabel*& valor) -> QFrame* {
        QFrame* tarjeta = new QFrame(this);
        tarjeta->setStyleSheet(
            "QFrame { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 12px; }"
        );
        QVBoxLayout* layoutTarjeta = new QVBoxLayout(tarjeta);
        layoutTarjeta->setContentsMargins(16, 12, 16, 12);
        layoutTarjeta->setSpacing(4);

        QLabel* lblNombre = new QLabel(titulo, tarjeta);
        lblNombre->setStyleSheet(
            "color: #6b7280; font-size: 11px; font-weight: bold; border: none; background: transparent;"
        );

        valor = new QLabel("0", tarjeta);
        valor->setStyleSheet(
            "color: #111827; font-size: 20px; font-weight: bold; border: none; background: transparent;"
        );

        layoutTarjeta->addWidget(lblNombre);
        layoutTarjeta->addWidget(valor);
        return tarjeta;
    };

    layoutResumen->addWidget(crearTarjeta("Ventas encontradas", lblTotalVentas));
    layoutResumen->addWidget(crearTarjeta("Ingresos", lblIngresos));
    layoutResumen->addWidget(crearTarjeta("Ticket promedio", lblPromedio));
    layoutPrincipal->addLayout(layoutResumen);

    // Filtros
    QFrame* panelFiltros = new QFrame(this);
    panelFiltros->setStyleSheet(
        "QFrame { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 12px; }"
    );
    QHBoxLayout* layoutFiltros = new QHBoxLayout(panelFiltros);
    layoutFiltros->setContentsMargins(16, 12, 16, 12);
    layoutFiltros->setSpacing(12);

    txtBuscar = new QLineEdit(panelFiltros);
    txtBuscar->setPlaceholderText("Buscar por folio o cajero...");
    txtBuscar->setStyleSheet(
        "QLineEdit { background-color: #ffffff; border: 1px solid #d1d5db; border-radius: 8px; "
        "padding: 8px 12px; font-size: 13px; color: #111827; }"
        "QLineEdit:focus { border: 1px solid #10b981; }"
    );

    chkFiltrarFecha = new QCheckBox("Filtrar por fecha", panelFiltros);
    chkFiltrarFecha->setStyleSheet("QCheckBox { color: #374151; font-size: 12px; font-weight: bold; }");

    fechaDesde = new QDateEdit(QDate::currentDate().addDays(-30), panelFiltros);
    fechaDesde->setCalendarPopup(true);
    fechaDesde->setDisplayFormat("dd/MM/yyyy");
    fechaDesde->setEnabled(false);

    fechaHasta = new QDateEdit(QDate::currentDate(), panelFiltros);
    fechaHasta->setCalendarPopup(true);
    fechaHasta->setDisplayFormat("dd/MM/yyyy");
    fechaHasta->setEnabled(false);

    QString estiloFecha =
        "QDateEdit { background-color: #ffffff; border: 1px solid #d1d5db; border-radius: 8px; "
        "padding: 7px 10px; font-size: 12px; color: #111827; }"
        "QDateEdit:focus { border: 1px solid #10b981; }"
        "QDateEdit:disabled { background-color: #f3f4f6; color: #9ca3af; }";
    fechaDesde->setStyleSheet(estiloFecha);
    fechaHasta->setStyleSheet(estiloFecha);

    QLabel* lblDesde = new QLabel("Desde", panelFiltros);
    lblDesde->setStyleSheet("color: #6b7280; font-size: 12px;");
    QLabel* lblHasta = new QLabel("Hasta", panelFiltros);
    lblHasta->setStyleSheet("color: #6b7280; font-size: 12px;");

    layoutFiltros->addWidget(txtBuscar, 1);
    layoutFiltros->addWidget(chkFiltrarFecha);
    layoutFiltros->addWidget(lblDesde);
    layoutFiltros->addWidget(fechaDesde);
    layoutFiltros->addWidget(lblHasta);
    layoutFiltros->addWidget(fechaHasta);
    layoutPrincipal->addWidget(panelFiltros);

    connect(txtBuscar, &QLineEdit::textChanged, this, [this]() { aplicarFiltros(); });
    connect(chkFiltrarFecha, &QCheckBox::toggled, this, [this](bool activo) {
        fechaDesde->setEnabled(activo);
        fechaHasta->setEnabled(activo);
        aplicarFiltros();
    });
    connect(fechaDesde, &QDateEdit::dateChanged, this, [this]() { aplicarFiltros(); });
    connect(fechaHasta, &QDateEdit::dateChanged, this, [this]() { aplicarFiltros(); });

    // Contenido: tabla de ventas + detalle
    QHBoxLayout* layoutContenido = new QHBoxLayout();
    layoutContenido->setSpacing(15);

    QFrame* panelTabla = new QFrame(this);
    panelTabla->setStyleSheet(
        "QFrame { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 12px; }"
    );
    QVBoxLayout* layoutTabla = new QVBoxLayout(panelTabla);
    layoutTabla->setContentsMargins(16, 14, 16, 16);
    layoutTabla->setSpacing(10);

    QLabel* lblTituloTabla = new QLabel("Ventas registradas", panelTabla);
    lblTituloTabla->setStyleSheet("font-size: 14px; font-weight: bold; color: #111827; border: none;");
    layoutTabla->addWidget(lblTituloTabla);

    tablaVentas = new QTableWidget(panelTabla);
    tablaVentas->setColumnCount(5);
    tablaVentas->setHorizontalHeaderLabels({"Folio", "Fecha y hora", "Cajero", "Artículos", "Total"});
    tablaVentas->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaVentas->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaVentas->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaVentas->setAlternatingRowColors(true);
    tablaVentas->verticalHeader()->setVisible(false);
    tablaVentas->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaVentas->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tablaVentas->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    tablaVentas->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; gridline-color: #f3f4f6; }"
        "QHeaderView::section { background-color: #f9fafb; color: #6b7280; font-weight: bold; "
        "border: none; padding: 8px; }"
        "QTableWidget::item { padding: 6px; color: #111827; }"
        "QTableWidget::item:selected { background-color: #d1fae5; color: #111827; }"
    );
    layoutTabla->addWidget(tablaVentas);
    layoutContenido->addWidget(panelTabla, 3);

    QFrame* panelDetalle = new QFrame(this);
    panelDetalle->setStyleSheet(
        "QFrame { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 12px; }"
    );
    QVBoxLayout* layoutDetalle = new QVBoxLayout(panelDetalle);
    layoutDetalle->setContentsMargins(16, 14, 16, 16);
    layoutDetalle->setSpacing(10);

    lblDetalleTitulo = new QLabel("Seleccione una venta", panelDetalle);
    lblDetalleTitulo->setStyleSheet("font-size: 14px; font-weight: bold; color: #111827; border: none;");
    layoutDetalle->addWidget(lblDetalleTitulo);

    tablaDetalle = new QTableWidget(panelDetalle);
    tablaDetalle->setColumnCount(4);
    tablaDetalle->setHorizontalHeaderLabels({"Producto", "Cant.", "P. Unit.", "Subtotal"});
    tablaDetalle->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaDetalle->setSelectionMode(QAbstractItemView::NoSelection);
    tablaDetalle->verticalHeader()->setVisible(false);
    tablaDetalle->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tablaDetalle->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tablaDetalle->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tablaDetalle->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tablaDetalle->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: none; gridline-color: #f3f4f6; }"
        "QHeaderView::section { background-color: #f9fafb; color: #6b7280; font-weight: bold; "
        "border: none; padding: 8px; }"
        "QTableWidget::item { padding: 6px; color: #111827; }"
    );
    layoutDetalle->addWidget(tablaDetalle, 1);

    lblResumenDetalle = new QLabel(panelDetalle);
    lblResumenDetalle->setWordWrap(true);
    lblResumenDetalle->setTextFormat(Qt::RichText);
    lblResumenDetalle->setStyleSheet(
        "color: #374151; font-size: 12px; border: none; background: transparent;"
    );
    layoutDetalle->addWidget(lblResumenDetalle);

    btnRegenerar = new QPushButton("Regenerar ticket", panelDetalle);
    btnRegenerar->setStyleSheet(kEstiloBotonPrimario);
    btnRegenerar->setEnabled(false);
    connect(btnRegenerar, &QPushButton::clicked, this, &VistaHistorial::regenerarTicket);
    layoutDetalle->addWidget(btnRegenerar);

    layoutContenido->addWidget(panelDetalle, 2);
    layoutPrincipal->addLayout(layoutContenido, 1);

    connect(tablaVentas, &QTableWidget::itemSelectionChanged, this, [this]() {
        const int indice = indiceVentaDeFila(tablaVentas->currentRow());
        if (indice >= 0) {
            indiceSeleccionado = indice;
            mostrarDetalle(indice);
        } else {
            indiceSeleccionado = -1;
            limpiarDetalle();
        }
    });
    connect(tablaVentas, &QTableWidget::cellDoubleClicked, this, [this](int, int) {
        if (indiceSeleccionado >= 0) {
            regenerarTicket();
        }
    });

    limpiarDetalle();
}

void VistaHistorial::cargarVentas() {
    GestorJSON gestor;
    ventas = gestor.cargarVentas();

    std::sort(ventas.begin(), ventas.end(), [](const DatosVentaTicket& a, const DatosVentaTicket& b) {
        return a.fechaHora > b.fechaHora;
    });
}

bool VistaHistorial::ventaCumpleFiltros(const DatosVentaTicket& venta) const {
    const QString texto = txtBuscar->text().trimmed();
    if (!texto.isEmpty()) {
        const bool coincide = venta.idVenta.contains(texto, Qt::CaseInsensitive) ||
                              venta.cajero.contains(texto, Qt::CaseInsensitive);
        if (!coincide) {
            return false;
        }
    }

    if (chkFiltrarFecha->isChecked()) {
        QDate desde = fechaDesde->date();
        QDate hasta = fechaHasta->date();
        if (desde > hasta) {
            std::swap(desde, hasta);
        }

        const QDate fecha = venta.fechaHora.date();
        if (fecha < desde || fecha > hasta) {
            return false;
        }
    }

    return true;
}

void VistaHistorial::aplicarFiltros() {
    std::vector<int> indices;
    indices.reserve(ventas.size());

    for (int i = 0; i < static_cast<int>(ventas.size()); ++i) {
        if (ventaCumpleFiltros(ventas[i])) {
            indices.push_back(i);
        }
    }

    renderizarTabla(indices);
    actualizarResumen(indices);
    indiceSeleccionado = -1;
    limpiarDetalle();
}

void VistaHistorial::renderizarTabla(const std::vector<int>& indices) {
    tablaVentas->setSortingEnabled(false);
    tablaVentas->clearContents();
    tablaVentas->setRowCount(static_cast<int>(indices.size()));

    for (int fila = 0; fila < static_cast<int>(indices.size()); ++fila) {
        const DatosVentaTicket& venta = ventas[indices[fila]];

        QTableWidgetItem* itemFolio = new QTableWidgetItem(venta.idVenta);
        itemFolio->setData(Qt::UserRole, indices[fila]);
        tablaVentas->setItem(fila, 0, itemFolio);

        tablaVentas->setItem(fila, 1, new QTableWidgetItem(
            venta.fechaHora.toString("dd/MM/yyyy hh:mm")));
        tablaVentas->setItem(fila, 2, new QTableWidgetItem(venta.cajero));

        int articulos = 0;
        for (const auto& item : venta.items) {
            articulos += item.cantidad;
        }

        QTableWidgetItem* itemArticulos = new QTableWidgetItem(QString::number(articulos));
        itemArticulos->setTextAlignment(Qt::AlignCenter);
        tablaVentas->setItem(fila, 3, itemArticulos);

        QTableWidgetItem* itemTotal = new QTableWidgetItem(moneda(venta.total));
        itemTotal->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tablaVentas->setItem(fila, 4, itemTotal);
    }

    tablaVentas->clearSelection();
}

void VistaHistorial::actualizarResumen(const std::vector<int>& indices) {
    double total = 0.0;
    for (const int indice : indices) {
        total += ventas[indice].total;
    }

    const int cantidad = static_cast<int>(indices.size());
    const double promedio = cantidad > 0 ? total / cantidad : 0.0;

    lblTotalVentas->setText(QString::number(cantidad));
    lblIngresos->setText(moneda(total));
    lblPromedio->setText(moneda(promedio));
}

void VistaHistorial::mostrarDetalle(int indiceVenta) {
    if (indiceVenta < 0 || indiceVenta >= static_cast<int>(ventas.size())) {
        limpiarDetalle();
        return;
    }

    const DatosVentaTicket& venta = ventas[indiceVenta];
    lblDetalleTitulo->setText(QString("Ticket %1").arg(venta.idVenta));

    tablaDetalle->setRowCount(static_cast<int>(venta.items.size()));
    for (int fila = 0; fila < static_cast<int>(venta.items.size()); ++fila) {
        const ItemTicket& item = venta.items[fila];

        tablaDetalle->setItem(fila, 0, new QTableWidgetItem(item.nombreProducto));

        QTableWidgetItem* itemCantidad = new QTableWidgetItem(QString::number(item.cantidad));
        itemCantidad->setTextAlignment(Qt::AlignCenter);
        tablaDetalle->setItem(fila, 1, itemCantidad);

        QTableWidgetItem* itemPrecio = new QTableWidgetItem(moneda(item.precioUnitario));
        itemPrecio->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tablaDetalle->setItem(fila, 2, itemPrecio);

        QTableWidgetItem* itemSubtotal = new QTableWidgetItem(moneda(item.subtotal));
        itemSubtotal->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tablaDetalle->setItem(fila, 3, itemSubtotal);
    }

    lblResumenDetalle->setText(QString(
        "<b>Cajero:</b> %1<br>"
        "<b>Fecha:</b> %2<br>"
        "<b>Subtotal:</b> %3 &nbsp;&nbsp; <b>IVA:</b> %4<br>"
        "<b>Total:</b> <span style='color:#047857;'>%5</span><br>"
        "<b>Paga con:</b> %6 &nbsp;&nbsp; <b>Cambio:</b> %7"
    ).arg(venta.cajero,
          venta.fechaHora.toString("dd/MM/yyyy hh:mm"),
          moneda(venta.subtotal),
          moneda(venta.iva),
          moneda(venta.total),
          moneda(venta.pagaCon),
          moneda(venta.cambio)));

    btnRegenerar->setEnabled(true);
}

void VistaHistorial::limpiarDetalle() {
    lblDetalleTitulo->setText("Seleccione una venta");
    tablaDetalle->setRowCount(0);
    lblResumenDetalle->setText(
        "Elija una venta de la lista para ver su detalle y regenerar el comprobante.");
    btnRegenerar->setEnabled(false);
}

int VistaHistorial::indiceVentaDeFila(int fila) const {
    if (fila < 0 || fila >= tablaVentas->rowCount()) {
        return -1;
    }

    QTableWidgetItem* item = tablaVentas->item(fila, 0);
    if (!item || !item->data(Qt::UserRole).isValid()) {
        return -1;
    }

    return item->data(Qt::UserRole).toInt();
}

void VistaHistorial::regenerarTicket() {
    if (indiceSeleccionado < 0 || indiceSeleccionado >= static_cast<int>(ventas.size())) {
        QMessageBox::information(this, "Historial", "Seleccione una venta primero.");
        return;
    }

    const DatosVentaTicket datos = ventas[indiceSeleccionado];

    GeneradorTicketHTML generador;
    const QString contenidoHTML = generador.generarContenido(datos);

    QDialog dlgTicket(this);
    dlgTicket.setWindowTitle(QString("Ticket %1 - UNA Pulpe").arg(datos.idVenta));
    dlgTicket.resize(380, 580);
    dlgTicket.setStyleSheet("background-color: #ffffff;");

    QVBoxLayout* layoutDlg = new QVBoxLayout(&dlgTicket);
    layoutDlg->setContentsMargins(15, 15, 15, 15);
    layoutDlg->setSpacing(10);

    QTextBrowser* visor = new QTextBrowser(&dlgTicket);
    visor->setHtml(contenidoHTML);
    visor->setStyleSheet(
        "QTextBrowser { border: 1px solid #e5e7eb; border-radius: 8px; "
        "background-color: #ffffff; color: #111827; padding: 10px; }"
    );
    layoutDlg->addWidget(visor);

    QHBoxLayout* layoutBotones = new QHBoxLayout();
    layoutBotones->setSpacing(8);

    QPushButton* btnPDF = new QPushButton("PDF", &dlgTicket);
    QPushButton* btnTXT = new QPushButton("TXT", &dlgTicket);
    QPushButton* btnHTML = new QPushButton("HTML", &dlgTicket);
    QPushButton* btnCerrar = new QPushButton("Cerrar", &dlgTicket);

    btnPDF->setStyleSheet(kEstiloBotonPrimario);
    btnTXT->setStyleSheet(kEstiloBotonSecundario);
    btnHTML->setStyleSheet(kEstiloBotonSecundario);
    btnCerrar->setStyleSheet(kEstiloBotonSecundario);

    connect(btnPDF, &QPushButton::clicked, &dlgTicket, [this, &dlgTicket, datos]() {
        guardarTicketComo(datos, "PDF", &dlgTicket);
    });
    connect(btnTXT, &QPushButton::clicked, &dlgTicket, [this, &dlgTicket, datos]() {
        guardarTicketComo(datos, "TXT", &dlgTicket);
    });
    connect(btnHTML, &QPushButton::clicked, &dlgTicket, [this, &dlgTicket, datos]() {
        guardarTicketComo(datos, "HTML", &dlgTicket);
    });
    connect(btnCerrar, &QPushButton::clicked, &dlgTicket, &QDialog::accept);

    layoutBotones->addWidget(btnPDF);
    layoutBotones->addWidget(btnTXT);
    layoutBotones->addWidget(btnHTML);
    layoutBotones->addStretch();
    layoutBotones->addWidget(btnCerrar);
    layoutDlg->addLayout(layoutBotones);

    dlgTicket.exec();
}

bool VistaHistorial::guardarTicketComo(const DatosVentaTicket& datos, const QString& formato,
                                       QWidget* parent) {
    const QString formatoMayus = formato.toUpper();

    QString sufijo;
    QString filtro;
    if (formatoMayus == "PDF") {
        sufijo = ".pdf";
        filtro = "Documento PDF (*.pdf)";
    } else if (formatoMayus == "TXT") {
        sufijo = ".txt";
        filtro = "Archivo de texto (*.txt)";
    } else {
        sufijo = ".html";
        filtro = "Documento HTML (*.html)";
    }

    QString carpeta = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (carpeta.isEmpty()) {
        carpeta = QDir::homePath();
    }

    const QString nombreSugerido = QString("Ticket_%1%2").arg(datos.idVenta, sufijo);
    QString ruta = QFileDialog::getSaveFileName(
        parent,
        QString("Guardar ticket %1").arg(datos.idVenta),
        QDir(carpeta).filePath(nombreSugerido),
        filtro
    );

    if (ruta.isEmpty()) {
        return false;
    }

    if (!ruta.endsWith(sufijo, Qt::CaseInsensitive)) {
        ruta += sufijo;
    }

    if (formatoMayus == "PDF") {
        GeneradorTicketHTML generador;
        QString contenido = generador.generarContenido(datos);
        contenido.replace("font-size: 12px", "font-size: 11pt");
        contenido.replace("font-size: 11px", "font-size: 9pt");
        contenido.replace("font-size: 14px", "font-size: 13pt");

        QPrinter impresora(QPrinter::HighResolution);
        impresora.setOutputFormat(QPrinter::PdfFormat);
        impresora.setOutputFileName(ruta);

        const qreal altoTicketMm = qBound<qreal>(120.0, 95.0 + datos.items.size() * 8.0, 300.0);
        impresora.setPageSize(QPageSize(QSizeF(80.0, altoTicketMm), QPageSize::Millimeter,
                                        "Ticket", QPageSize::ExactMatch));
        impresora.setPageMargins(QMarginsF(4, 5, 4, 5), QPageLayout::Millimeter);

        QTextDocument documento;
        documento.setDefaultFont(QFont("Arial", 11));
        documento.setHtml(contenido);
        documento.setPageSize(impresora.pageLayout().paintRect(QPageLayout::Point).size());
        documento.print(&impresora);
    } else {
        QString contenido;
        if (formatoMayus == "TXT") {
            GeneradorTicketTXT generador;
            contenido = generador.generarContenido(datos);
        } else {
            GeneradorTicketHTML generador;
            contenido = generador.generarContenido(datos);
        }

        QSaveFile archivo(ruta);
        if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(parent, "Error", "No se pudo crear el archivo del ticket.");
            return false;
        }

        const QByteArray bytes = contenido.toUtf8();
        if (archivo.write(bytes) != bytes.size() || !archivo.commit()) {
            QMessageBox::critical(parent, "Error", "No se pudo guardar el archivo del ticket.");
            return false;
        }
    }

    const QFileInfo archivoGuardado(ruta);
    if (!archivoGuardado.exists() || archivoGuardado.size() == 0) {
        QMessageBox::critical(parent, "Error", "No se pudo generar el archivo del ticket.");
        return false;
    }

    QMessageBox::information(
        parent,
        "Ticket guardado",
        QString("El ticket se guardó en:\n%1").arg(ruta)
    );
    return true;
}
