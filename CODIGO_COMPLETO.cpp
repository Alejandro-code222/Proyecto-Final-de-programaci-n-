PROYECTO HEATHCLIFF - CODIGO FUENTE COMPLETO
Cada seccion corresponde a un archivo dentro de src/.


==============================================================================
ARCHIVO: src/administracion.cpp
==============================================================================

#include "administracion.h"
#include "modelos.h"
#include "validaciones.h"
#include <iostream>

using namespace std;

void mostrarMenuAdministracion() {
    int opcion;
    do {
        cout << "\n===== ADMINISTRACION DEL SISTEMA =====\n"
             << "1. Ver estado general de los registros\n"
             << "2. Ver reglas principales del sistema\n"
             << "0. Volver\n";
        opcion = leerEntero("Opcion: ");

        if (opcion == 1) {
            cout << "Productos: " << productos.size() << "\n";
            cout << "Proveedores: " << proveedores.size() << "\n";
            cout << "Clientes: " << clientes.size() << "\n";
            cout << "Compras: " << compras.size() << "\n";
            cout << "Ventas registradas: " << ventas.size() << "\n";
            cout << "Movimientos de inventario: " << movimientos.size() << "\n";
            cout << "Movimientos financieros: " << finanzas.size() << "\n";
        } else if (opcion == 2) {
            cout << "- Los identificadores deben ser positivos y unicos en su modulo.\n";
            cout << "- Los precios e importes no pueden ser negativos.\n";
            cout << "- Las cantidades de entradas y salidas deben ser mayores que cero.\n";
            cout << "- No se permite vender mas unidades de las disponibles.\n";
            cout << "- Una compra requiere dinero suficiente en caja.\n";
            cout << "- Los datos se conservan solamente mientras el programa esta abierto.\n";
        } else if (opcion != 0) {
            cout << "Opcion incorrecta.\n";
        }
    } while (opcion != 0);
}


==============================================================================
ARCHIVO: src/administracion.h
==============================================================================

#ifndef ADMINISTRACION_H
#define ADMINISTRACION_H

void mostrarMenuAdministracion();

#endif


==============================================================================
ARCHIVO: src/compras.cpp
==============================================================================

#include "compras.h"
#include "modelos.h"
#include "productos.h"
#include "inventario.h"
#include "finanzas.h"
#include "validaciones.h"
#include <iostream>
#include <iomanip>

using namespace std;

static int indiceProveedorPorId(int id) {
    for (size_t i = 0; i < proveedores.size(); i++)
        if (proveedores[i].id == id) return static_cast<int>(i);
    return -1;
}

static int siguienteIdCompra() {
    int mayor = 0;
    for (size_t i = 0; i < compras.size(); i++)
        if (compras[i].id > mayor) mayor = compras[i].id;
    return mayor + 1;
}

static void registrarProveedor() {
    Proveedor p;
    p.id = leerEntero("ID del proveedor: ");
    if (p.id <= 0 || indiceProveedorPorId(p.id) != -1) {
        cout << "El ID debe ser positivo y no estar repetido.\n";
        return;
    }
    p.nombre = leerTexto("Nombre del proveedor: ");
    p.contacto = leerTexto("Telefono, correo u otro contacto: ");
    proveedores.push_back(p);
    cout << "Proveedor registrado correctamente.\n";
}

static void listarProveedores() {
    if (proveedores.empty()) { cout << "No hay proveedores registrados.\n"; return; }
    for (size_t i = 0; i < proveedores.size(); i++) {
        cout << "ID: " << proveedores[i].id << " | " << proveedores[i].nombre
             << " | Contacto: " << proveedores[i].contacto << "\n";
    }
}

static void buscarProveedor() {
    if (proveedores.empty()) { cout << "No hay proveedores para buscar.\n"; return; }
    cout << "1. Buscar por identificador\n2. Buscar por nombre\n";
    int opcion = leerEntero("Opcion: ");
    bool encontrado = false;

    if (opcion == 1) {
        int id = leerEntero("ID del proveedor: ");
        int indice = indiceProveedorPorId(id);
        if (indice != -1) {
            cout << "Proveedor: " << proveedores[indice].nombre
                 << " | Contacto: " << proveedores[indice].contacto << "\n";
            encontrado = true;
        }
    } else if (opcion == 2) {
        string nombre = leerTexto("Nombre o parte del nombre: ");
        for (size_t i = 0; i < proveedores.size(); i++) {
            if (proveedores[i].nombre.find(nombre) != string::npos) {
                cout << "ID: " << proveedores[i].id << " | " << proveedores[i].nombre
                     << " | Contacto: " << proveedores[i].contacto << "\n";
                encontrado = true;
            }
        }
    } else {
        cout << "Opcion incorrecta.\n";
        return;
    }
    if (!encontrado) cout << "Proveedor no encontrado.\n";
}

static void registrarCompra() {
    if (proveedores.empty()) {
        cout << "Primero debe registrar al menos un proveedor.\n";
        return;
    }
    if (productos.empty()) {
        cout << "Primero debe registrar productos en el catalogo.\n";
        return;
    }

    listarProveedores();
    int idProveedor = leerEntero("ID del proveedor de esta compra: ");
    if (indiceProveedorPorId(idProveedor) == -1) {
        cout << "Proveedor no encontrado.\n";
        return;
    }

    Compra compra;
    compra.id = siguienteIdCompra();
    compra.idProveedor = idProveedor;
    compra.total = 0;

    cout << "Introduzca los productos. Escriba ID 0 para terminar.\n";
    while (true) {
        int idProducto = leerEntero("ID del producto (0 para terminar): ");
        if (idProducto == 0) break;
        int indice = indiceProductoPorId(idProducto);
        if (indice == -1) { cout << "Producto no encontrado.\n"; continue; }

        int cantidad = leerEntero("Cantidad comprada: ");
        if (cantidad <= 0) { cout << "La cantidad debe ser mayor que cero.\n"; continue; }

        bool agregado = false;
        for (size_t i = 0; i < compra.productos.size(); i++) {
            if (compra.productos[i].idProducto == idProducto) {
                compra.productos[i].cantidad += cantidad;
                compra.total += cantidad * productos[indice].precioCompra;
                agregado = true;
                break;
            }
        }
        if (!agregado) {
            Detalle d;
            d.idProducto = idProducto;
            d.cantidad = cantidad;
            d.precio = productos[indice].precioCompra;
            d.costoUnitario = productos[indice].precioCompra;
            compra.productos.push_back(d);
            compra.total += cantidad * d.precio;
        }
    }

    if (compra.productos.empty()) {
        cout << "Compra cancelada: no se agregaron productos.\n";
        return;
    }

    cout << fixed << setprecision(2)
         << "Total de la compra: " << compra.total << "\n"
         << "Dinero disponible en caja: " << calcularCaja() << "\n";
    if (compra.total > calcularCaja()) {
        cout << "No hay dinero suficiente en caja. Registre un ingreso o capital inicial en Finanzas.\n";
        return;
    }
    if (!confirmarOperacion("Confirma la compra y la entrada al inventario")) {
        cout << "Compra cancelada.\n";
        return;
    }

    compras.push_back(compra);
    for (size_t i = 0; i < compra.productos.size(); i++) {
        int indice = indiceProductoPorId(compra.productos[i].idProducto);
        if (indice != -1) {
            productos[indice].cantidad += compra.productos[i].cantidad;
            registrarMovimientoInventario(compra.productos[i].idProducto,
                                         compra.productos[i].cantidad,
                                         "Entrada", "Compra #" + to_string(compra.id));
        }
    }
    agregarMovimientoFinanciero("Gasto", "Compra #" + to_string(compra.id),
                                "Compra de mercancia", compra.total);
    cout << "Compra registrada. El inventario y la caja fueron actualizados.\n";
}

static void mostrarHistorialCompras() {
    if (compras.empty()) { cout << "No hay compras registradas.\n"; return; }
    cout << fixed << setprecision(2);
    for (size_t i = 0; i < compras.size(); i++) {
        int indiceProveedor = indiceProveedorPorId(compras[i].idProveedor);
        string nombreProveedor = indiceProveedor == -1 ? "Proveedor no disponible" : proveedores[indiceProveedor].nombre;
        cout << "\nCompra #" << compras[i].id << " | Proveedor: " << nombreProveedor
             << " | Total: " << compras[i].total << "\n";
        for (size_t j = 0; j < compras[i].productos.size(); j++) {
            int indiceProducto = indiceProductoPorId(compras[i].productos[j].idProducto);
            string nombreProducto = indiceProducto == -1 ? "Producto eliminado" : productos[indiceProducto].nombre;
            cout << "  " << nombreProducto << " | Cantidad: "
                 << compras[i].productos[j].cantidad << " | Precio unitario: "
                 << compras[i].productos[j].precio << "\n";
        }
    }
}

void mostrarMenuCompras() {
    int opcion;
    do {
        cout << "\n===== COMPRAS Y PROVEEDORES =====\n"
             << "1. Registrar proveedor\n2. Listar proveedores\n3. Buscar proveedor\n"
             << "4. Registrar compra\n5. Historial de compras\n0. Volver\n";
        opcion = leerEntero("Opcion: ");
        switch (opcion) {
            case 1: registrarProveedor(); break;
            case 2: listarProveedores(); break;
            case 3: buscarProveedor(); break;
            case 4: registrarCompra(); break;
            case 5: mostrarHistorialCompras(); break;
            case 0: break;
            default: cout << "Opcion incorrecta.\n";
        }
    } while (opcion != 0);
}


==============================================================================
ARCHIVO: src/compras.h
==============================================================================

#ifndef COMPRAS_H
#define COMPRAS_H

void mostrarMenuCompras();

#endif


==============================================================================
ARCHIVO: src/finanzas.cpp
==============================================================================

#include "finanzas.h"
#include "modelos.h"
#include "validaciones.h"
#include <iostream>
#include <iomanip>

using namespace std;

void agregarMovimientoFinanciero(const string& tipo,
                                 const string& concepto,
                                 const string& categoria,
                                 double importe) {
    MovimientoDinero m;
    m.tipo = tipo;
    m.concepto = concepto;
    m.categoria = categoria;
    m.importe = importe;
    finanzas.push_back(m);
}

double calcularTotalIngresos() {
    double total = 0;
    for (size_t i = 0; i < finanzas.size(); i++)
        if (finanzas[i].tipo == "Ingreso") total += finanzas[i].importe;
    return total;
}

double calcularTotalGastos() {
    double total = 0;
    for (size_t i = 0; i < finanzas.size(); i++)
        if (finanzas[i].tipo == "Gasto") total += finanzas[i].importe;
    return total;
}

double calcularCaja() {
    return calcularTotalIngresos() - calcularTotalGastos();
}

static void registrarIngresoManual() {
    string concepto = leerTexto("Concepto del ingreso: ");
    double importe = leerDouble("Importe: ");
    if (importe <= 0 || !validarPrecio(importe)) {
        cout << "El importe debe ser mayor que cero.\n";
        return;
    }
    agregarMovimientoFinanciero("Ingreso", concepto, "Ingreso adicional", importe);
    cout << "Ingreso registrado. Caja actual: " << fixed << setprecision(2) << calcularCaja() << "\n";
}

static void registrarGastoManual() {
    string concepto = leerTexto("Concepto del gasto: ");
    string categoria = leerTexto("Categoria del gasto: ");
    double importe = leerDouble("Importe: ");
    if (importe <= 0 || !validarPrecio(importe)) {
        cout << "El importe debe ser mayor que cero.\n";
        return;
    }
    if (importe > calcularCaja()) {
        cout << "No hay dinero suficiente en caja para registrar ese gasto.\n";
        return;
    }
    agregarMovimientoFinanciero("Gasto", concepto, categoria, importe);
    cout << "Gasto registrado. Caja actual: " << fixed << setprecision(2) << calcularCaja() << "\n";
}

static void mostrarBalance() {
    cout << fixed << setprecision(2);
    cout << "Total de ingresos: " << calcularTotalIngresos() << "\n";
    cout << "Total de gastos:   " << calcularTotalGastos() << "\n";
    cout << "Dinero en caja:    " << calcularCaja() << "\n";
}

static void mostrarMovimientosDinero() {
    if (finanzas.empty()) { cout << "No hay movimientos financieros.\n"; return; }
    cout << fixed << setprecision(2);
    for (size_t i = 0; i < finanzas.size(); i++) {
        cout << i + 1 << ". " << finanzas[i].tipo
             << " | " << finanzas[i].concepto
             << " | Categoria: " << finanzas[i].categoria
             << " | Importe: " << finanzas[i].importe << "\n";
    }
}

void mostrarMenuFinanzas() {
    int opcion;
    do {
        cout << "\n===== FINANZAS Y CAJA =====\n"
             << "1. Registrar ingreso (incluye capital inicial)\n"
             << "2. Registrar gasto\n3. Consultar balance y caja\n"
             << "4. Historial de movimientos\n0. Volver\n";
        opcion = leerEntero("Opcion: ");
        switch (opcion) {
            case 1: registrarIngresoManual(); break;
            case 2: registrarGastoManual(); break;
            case 3: mostrarBalance(); break;
            case 4: mostrarMovimientosDinero(); break;
            case 0: break;
            default: cout << "Opcion incorrecta.\n";
        }
    } while (opcion != 0);
}


==============================================================================
ARCHIVO: src/finanzas.h
==============================================================================

#ifndef FINANZAS_H
#define FINANZAS_H

#include <string>

void mostrarMenuFinanzas();
void agregarMovimientoFinanciero(const std::string& tipo,
                                 const std::string& concepto,
                                 const std::string& categoria,
                                 double importe);
double calcularTotalIngresos();
double calcularTotalGastos();
double calcularCaja();

#endif


==============================================================================
ARCHIVO: src/inventario.cpp
==============================================================================

#include "inventario.h"
#include "productos.h"
#include "modelos.h"
#include "validaciones.h"
#include <iostream>
#include <iomanip>

using namespace std;

void registrarMovimientoInventario(int idProducto, int cantidad,
                                   const string& tipo, const string& motivo) {
    Movimiento m;
    m.idProducto = idProducto;
    m.cantidad = cantidad;
    m.tipo = tipo;
    m.motivo = motivo;
    movimientos.push_back(m);
}

static void consultarExistencias() {
    if (productos.empty()) { cout << "No hay productos en el inventario.\n"; return; }
    cout << "\nID       PRODUCTO                       EXISTENCIAS\n";
    for (size_t i = 0; i < productos.size(); i++) {
        cout << left << setw(9) << productos[i].id
             << setw(32) << productos[i].nombre
             << productos[i].cantidad << "\n";
    }
}

static void registrarEntrada() {
    int id = leerEntero("ID del producto: ");
    int indice = indiceProductoPorId(id);
    if (indice == -1) { cout << "Producto no encontrado.\n"; return; }
    int cantidad = leerEntero("Cantidad que entra: ");
    if (cantidad <= 0) { cout << "La cantidad debe ser mayor que cero.\n"; return; }
    string motivo = leerTexto("Motivo de la entrada: ");
    productos[indice].cantidad += cantidad;
    registrarMovimientoInventario(id, cantidad, "Entrada", motivo);
    cout << "Entrada registrada. Existencias actuales: " << productos[indice].cantidad << "\n";
}

static void registrarSalida() {
    int id = leerEntero("ID del producto: ");
    int indice = indiceProductoPorId(id);
    if (indice == -1) { cout << "Producto no encontrado.\n"; return; }
    int cantidad = leerEntero("Cantidad que sale: ");
    if (cantidad <= 0 || cantidad > productos[indice].cantidad) {
        cout << "Cantidad incorrecta o superior a las existencias disponibles.\n";
        return;
    }
    string motivo = leerTexto("Motivo de la salida: ");
    productos[indice].cantidad -= cantidad;
    registrarMovimientoInventario(id, cantidad, "Salida", motivo);
    cout << "Salida registrada. Existencias actuales: " << productos[indice].cantidad << "\n";
}

static void mostrarAlertas() {
    if (productos.empty()) { cout << "No hay productos registrados para comprobar alertas.\n"; return; }
    bool hayAlertas = false;
    for (size_t i = 0; i < productos.size(); i++) {
        if (productos[i].cantidad <= productos[i].minimo) {
            cout << "ALERTA: " << productos[i].nombre << " (ID " << productos[i].id
                 << ") tiene " << productos[i].cantidad << " unidades; minimo: "
                 << productos[i].minimo << ".\n";
            hayAlertas = true;
        }
    }
    if (!hayAlertas) cout << "No hay productos por debajo o en el minimo establecido.\n";
}

static void calcularValorInventario() {
    double total = 0;
    for (size_t i = 0; i < productos.size(); i++)
        total += productos[i].cantidad * productos[i].precioCompra;
    cout << fixed << setprecision(2)
         << "Valor del inventario a precio de adquisicion: " << total << "\n";
}

static void mostrarMayorYMenorStock() {
    if (productos.empty()) { cout << "No hay productos registrados.\n"; return; }
    int mayor = productos[0].cantidad;
    int menor = productos[0].cantidad;
    for (size_t i = 1; i < productos.size(); i++) {
        if (productos[i].cantidad > mayor) mayor = productos[i].cantidad;
        if (productos[i].cantidad < menor) menor = productos[i].cantidad;
    }
    cout << "Mayor existencia (" << mayor << "): ";
    for (size_t i = 0; i < productos.size(); i++)
        if (productos[i].cantidad == mayor) cout << productos[i].nombre << " | ";
    cout << "\nMenor existencia (" << menor << "): ";
    for (size_t i = 0; i < productos.size(); i++)
        if (productos[i].cantidad == menor) cout << productos[i].nombre << " | ";
    cout << "\n";
}

static void mostrarHistorialMovimientos() {
    if (movimientos.empty()) { cout << "Todavia no hay movimientos de inventario.\n"; return; }
    for (size_t i = 0; i < movimientos.size(); i++) {
        cout << i + 1 << ". Producto ID " << movimientos[i].idProducto
             << " | " << movimientos[i].tipo << " | Cantidad: "
             << movimientos[i].cantidad << " | Motivo: "
             << movimientos[i].motivo << "\n";
    }
}

void mostrarMenuInventario() {
    int opcion;
    do {
        cout << "\n===== GESTION DE INVENTARIO =====\n"
             << "1. Consultar existencias\n2. Registrar entrada\n3. Registrar salida\n"
             << "4. Alertas de bajo inventario\n5. Valor del inventario\n"
             << "6. Productos con mayor y menor existencia\n7. Historial de movimientos\n0. Volver\n";
        opcion = leerEntero("Opcion: ");
        switch (opcion) {
            case 1: consultarExistencias(); break;
            case 2: registrarEntrada(); break;
            case 3: registrarSalida(); break;
            case 4: mostrarAlertas(); break;
            case 5: calcularValorInventario(); break;
            case 6: mostrarMayorYMenorStock(); break;
            case 7: mostrarHistorialMovimientos(); break;
            case 0: break;
            default: cout << "Opcion incorrecta.\n";
        }
    } while (opcion != 0);
}


==============================================================================
ARCHIVO: src/inventario.h
==============================================================================

#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <string>

void mostrarMenuInventario();
void registrarMovimientoInventario(int idProducto, int cantidad,
                                   const std::string& tipo,
                                   const std::string& motivo);

#endif


==============================================================================
ARCHIVO: src/main.cpp
==============================================================================

#include "menu.h"
#include "modelos.h"
#include <iostream>

using namespace std;

// Definiciones unicas de los datos compartidos por todos los modulos.
vector<Producto> productos;
vector<Proveedor> proveedores;
vector<Cliente> clientes;
vector<Compra> compras;
vector<Venta> ventas;
vector<Movimiento> movimientos;
vector<MovimientoDinero> finanzas;

int main() {
    cout << "==============================================\n";
    cout << " CUMBRES BORRASCOSAS - LIBRO MAYOR DE HEATHCLIFF\n";
    cout << " Sistema basico de gestion empresarial en C++\n";
    cout << "==============================================\n";

    mostrarMenu();
    return 0;
}


==============================================================================
ARCHIVO: src/menu.cpp
==============================================================================

#include "menu.h"
#include "productos.h"
#include "inventario.h"
#include "compras.h"
#include "ventas.h"
#include "finanzas.h"
#include "reportes.h"
#include "administracion.h"
#include "validaciones.h"
#include <iostream>

using namespace std;

void mostrarMenu() {
    int opcion;
    do {
        cout << "\n========================================\n"
             << "       SISTEMA DE GESTION HEATHCLIFF\n"
             << "========================================\n"
             << "1. Gestion de productos\n"
             << "2. Gestion de inventario\n"
             << "3. Compras y proveedores\n"
             << "4. Ventas y clientes\n"
             << "5. Finanzas y caja\n"
             << "6. Reportes y estadisticas\n"
             << "7. Administracion y validaciones\n"
             << "0. Salir\n";
        opcion = leerEntero("Seleccione una opcion: ");

        switch (opcion) {
            case 1: mostrarMenuProductos(); break;
            case 2: mostrarMenuInventario(); break;
            case 3: mostrarMenuCompras(); break;
            case 4: mostrarMenuVentas(); break;
            case 5: mostrarMenuFinanzas(); break;
            case 6: mostrarMenuReportes(); break;
            case 7: mostrarMenuAdministracion(); break;
            case 0:
                if (!confirmarOperacion("Desea cerrar el programa")) opcion = -1;
                else cout << "Programa finalizado. Los datos en memoria se perderan.\n";
                break;
            default: cout << "Opcion incorrecta. Seleccione una opcion del menu.\n";
        }
    } while (opcion != 0);
}


==============================================================================
ARCHIVO: src/menu.h
==============================================================================

#ifndef MENU_H
#define MENU_H

void mostrarMenu();

#endif


==============================================================================
ARCHIVO: src/modelos.h
==============================================================================

#ifndef MODELOS_H
#define MODELOS_H

#include <string>
#include <vector>

using namespace std;

struct Producto {
    int id;
    string nombre;
    string categoria;
    double precioCompra;
    double precioVenta;
    int cantidad;
    int minimo;
};

struct Proveedor {
    int id;
    string nombre;
    string contacto;
};

struct Cliente {
    int id;
    string nombre;
    string contacto;
};

struct Detalle {
    int idProducto;
    int cantidad;
    double precio;        // Precio de compra o de venta, segun la operacion.
    double costoUnitario; // Costo de adquisicion al momento de la venta.
};

struct Compra {
    int id;
    int idProveedor;
    vector<Detalle> productos;
    double total;
};

struct Venta {
    int id;
    int idCliente; // 0 significa venta sin cliente registrado.
    vector<Detalle> productos;
    double total;
    bool anulada;
};

struct Movimiento {
    int idProducto;
    int cantidad;
    string tipo;
    string motivo;
};

struct MovimientoDinero {
    string concepto;
    string categoria;
    double importe;
    string tipo; // "Ingreso" o "Gasto".
};

// Los vectores se definen una sola vez en main.cpp.
extern vector<Producto> productos;
extern vector<Proveedor> proveedores;
extern vector<Cliente> clientes;
extern vector<Compra> compras;
extern vector<Venta> ventas;
extern vector<Movimiento> movimientos;
extern vector<MovimientoDinero> finanzas;

#endif


==============================================================================
ARCHIVO: src/productos.cpp
==============================================================================

#include "productos.h"
#include "validaciones.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

static string minusculas(string texto) {
    for (size_t i = 0; i < texto.size(); i++) {
        texto[i] = static_cast<char>(tolower(static_cast<unsigned char>(texto[i])));
    }
    return texto;
}

int indiceProductoPorId(int id) {
    for (size_t i = 0; i < productos.size(); i++) {
        if (productos[i].id == id) return static_cast<int>(i);
    }
    return -1;
}

static void registrarProducto() {
    Producto p;
    p.id = leerEntero("Identificador: ");

    if (p.id <= 0 || indiceProductoPorId(p.id) != -1) {
        cout << "El identificador debe ser positivo y no estar repetido.\n";
        return;
    }

    p.nombre = leerTexto("Nombre: ");
    p.categoria = leerTexto("Categoria: ");
    p.precioCompra = leerDouble("Precio de compra: ");
    p.precioVenta = leerDouble("Precio de venta: ");
    p.cantidad = leerEntero("Cantidad inicial: ");
    p.minimo = leerEntero("Existencia minima para alerta: ");

    if (!validarPrecio(p.precioCompra) || !validarPrecio(p.precioVenta) ||
        !validarCantidad(p.cantidad) || !validarCantidad(p.minimo)) {
        cout << "No se registro: precios y cantidades no pueden ser negativos.\n";
        return;
    }

    productos.push_back(p);
    cout << "Producto registrado correctamente.\n";
}

static void listarProductos() {
    if (productos.empty()) {
        cout << "No hay productos registrados.\n";
        return;
    }

    cout << "\nID  Nombre                 Categoria             Compra     Venta     Stock\n";
    cout << "----------------------------------------------------------------------------\n";
    for (size_t i = 0; i < productos.size(); i++) {
        cout << left << setw(4) << productos[i].id
             << setw(23) << productos[i].nombre.substr(0, 21)
             << setw(22) << productos[i].categoria.substr(0, 20)
             << right << fixed << setprecision(2)
             << setw(10) << productos[i].precioCompra
             << setw(10) << productos[i].precioVenta
             << setw(8) << productos[i].cantidad << "\n";
    }
}

static void buscarProducto() {
    if (productos.empty()) {
        cout << "No hay productos para buscar.\n";
        return;
    }

    cout << "1. Buscar por identificador\n2. Buscar por nombre\n";
    int opcion = leerEntero("Opcion: ");

    if (opcion == 1) {
        int id = leerEntero("ID del producto: ");
        int indice = indiceProductoPorId(id);
        if (indice == -1) {
            cout << "No existe un producto con ese ID.\n";
        } else {
            const Producto& p = productos[indice];
            cout << "ID: " << p.id << " | Nombre: " << p.nombre
                 << " | Categoria: " << p.categoria
                 << " | Compra: " << p.precioCompra
                 << " | Venta: " << p.precioVenta
                 << " | Stock: " << p.cantidad << "\n";
        }
    } else if (opcion == 2) {
        string nombre = minusculas(leerTexto("Nombre o parte del nombre: "));
        bool encontrado = false;
        for (size_t i = 0; i < productos.size(); i++) {
            if (minusculas(productos[i].nombre).find(nombre) != string::npos) {
                cout << "ID: " << productos[i].id << " | " << productos[i].nombre
                     << " | Stock: " << productos[i].cantidad << "\n";
                encontrado = true;
            }
        }
        if (!encontrado) cout << "No se encontraron coincidencias.\n";
    } else {
        cout << "Opcion incorrecta.\n";
    }
}

static void modificarProducto() {
    int id = leerEntero("ID del producto que desea modificar: ");
    int indice = indiceProductoPorId(id);
    if (indice == -1) {
        cout << "Producto no encontrado.\n";
        return;
    }

    Producto& p = productos[indice];
    cout << "1. Nombre\n2. Categoria\n3. Precio de compra\n4. Precio de venta\n";
    int opcion = leerEntero("Que desea modificar: ");

    if (opcion == 1) {
        p.nombre = leerTexto("Nuevo nombre: ");
    } else if (opcion == 2) {
        p.categoria = leerTexto("Nueva categoria: ");
    } else if (opcion == 3) {
        double precio = leerDouble("Nuevo precio de compra: ");
        if (!validarPrecio(precio)) { cout << "Precio no valido.\n"; return; }
        p.precioCompra = precio;
    } else if (opcion == 4) {
        double precio = leerDouble("Nuevo precio de venta: ");
        if (!validarPrecio(precio)) { cout << "Precio no valido.\n"; return; }
        p.precioVenta = precio;
    } else {
        cout << "Opcion incorrecta.\n";
        return;
    }
    cout << "Producto modificado correctamente.\n";
}

static bool productoTieneHistorial(int id) {
    for (size_t i = 0; i < compras.size(); i++) {
        for (size_t j = 0; j < compras[i].productos.size(); j++)
            if (compras[i].productos[j].idProducto == id) return true;
    }
    for (size_t i = 0; i < ventas.size(); i++) {
        for (size_t j = 0; j < ventas[i].productos.size(); j++)
            if (ventas[i].productos[j].idProducto == id) return true;
    }
    return false;
}

static void eliminarProducto() {
    int id = leerEntero("ID del producto que desea eliminar: ");
    int indice = indiceProductoPorId(id);
    if (indice == -1) {
        cout << "Producto no encontrado.\n";
        return;
    }
    if (productoTieneHistorial(id)) {
        cout << "No se puede eliminar: el producto aparece en compras o ventas historicas.\n";
        cout << "Puede modificarlo o mantenerlo en el catalogo para conservar el historial.\n";
        return;
    }
    if (!confirmarOperacion("Confirma que desea eliminar el producto")) {
        cout << "Operacion cancelada.\n";
        return;
    }
    productos.erase(productos.begin() + indice);
    cout << "Producto eliminado.\n";
}

static void filtrarProductos() {
    if (productos.empty()) {
        cout << "No hay productos registrados.\n";
        return;
    }
    cout << "1. Filtrar por categoria\n2. Filtrar por precio maximo\n";
    int opcion = leerEntero("Opcion: ");
    bool encontrado = false;

    if (opcion == 1) {
        string categoria = minusculas(leerTexto("Categoria: "));
        for (size_t i = 0; i < productos.size(); i++) {
            if (minusculas(productos[i].categoria) == categoria) {
                cout << productos[i].id << " - " << productos[i].nombre
                     << " - " << productos[i].precioVenta << "\n";
                encontrado = true;
            }
        }
    } else if (opcion == 2) {
        double maximo = leerDouble("Precio maximo: ");
        if (!validarPrecio(maximo)) { cout << "Precio no valido.\n"; return; }
        for (size_t i = 0; i < productos.size(); i++) {
            if (productos[i].precioVenta <= maximo) {
                cout << productos[i].id << " - " << productos[i].nombre
                     << " - " << productos[i].precioVenta << "\n";
                encontrado = true;
            }
        }
    } else {
        cout << "Opcion incorrecta.\n";
        return;
    }
    if (!encontrado) cout << "No hay productos que cumplan el criterio.\n";
}

void mostrarMenuProductos() {
    int opcion;
    do {
        cout << "\n===== GESTION DE PRODUCTOS =====\n"
             << "1. Registrar producto\n2. Listar productos\n3. Buscar producto\n"
             << "4. Modificar producto\n5. Eliminar producto\n6. Filtrar productos\n0. Volver\n";
        opcion = leerEntero("Opcion: ");
        switch (opcion) {
            case 1: registrarProducto(); break;
            case 2: listarProductos(); break;
            case 3: buscarProducto(); break;
            case 4: modificarProducto(); break;
            case 5: eliminarProducto(); break;
            case 6: filtrarProductos(); break;
            case 0: break;
            default: cout << "Opcion incorrecta.\n";
        }
    } while (opcion != 0);
}


==============================================================================
ARCHIVO: src/productos.h
==============================================================================

#ifndef PRODUCTOS_H
#define PRODUCTOS_H

#include "modelos.h"

void mostrarMenuProductos();
int indiceProductoPorId(int id);

#endif


==============================================================================
ARCHIVO: src/reportes.cpp
==============================================================================

#include "reportes.h"
#include "modelos.h"
#include "finanzas.h"
#include "validaciones.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <utility>

using namespace std;

static int ventasActivas() {
    int cantidad = 0;
    for (size_t i = 0; i < ventas.size(); i++)
        if (!ventas[i].anulada) cantidad++;
    return cantidad;
}

static double totalVentasActivas() {
    double total = 0;
    for (size_t i = 0; i < ventas.size(); i++)
        if (!ventas[i].anulada) total += ventas[i].total;
    return total;
}

static void mostrarResumenGeneral() {
    int unidades = 0;
    for (size_t i = 0; i < productos.size(); i++) unidades += productos[i].cantidad;

    cout << fixed << setprecision(2);
    cout << "\n===== RESUMEN GENERAL =====\n";
    cout << "Productos registrados: " << productos.size() << "\n";
    cout << "Unidades en inventario: " << unidades << "\n";
    cout << "Compras registradas: " << compras.size() << "\n";
    cout << "Ventas activas: " << ventasActivas() << "\n";
    cout << "Ingresos acumulados: " << calcularTotalIngresos() << "\n";
    cout << "Gastos acumulados: " << calcularTotalGastos() << "\n";
    cout << "Dinero en caja: " << calcularCaja() << "\n";
}

static void calcularPrecioPromedio() {
    if (productos.empty()) { cout << "No hay productos para calcular el promedio.\n"; return; }
    double suma = 0;
    for (size_t i = 0; i < productos.size(); i++) suma += productos[i].precioVenta;
    cout << fixed << setprecision(2)
         << "Precio de venta promedio: " << suma / productos.size() << "\n";
}

static void calcularPromedioVentas() {
    int cantidad = ventasActivas();
    if (cantidad == 0) { cout << "No hay ventas activas para calcular el promedio.\n"; return; }
    cout << fixed << setprecision(2)
         << "Importe promedio por venta: " << totalVentasActivas() / cantidad << "\n";
}

static vector<pair<int, int> > calcularUnidadesPorProducto() {
    vector<pair<int, int> > resultado;
    for (size_t i = 0; i < productos.size(); i++)
        resultado.push_back(make_pair(productos[i].id, 0));

    for (size_t i = 0; i < ventas.size(); i++) {
        if (ventas[i].anulada) continue;
        for (size_t j = 0; j < ventas[i].productos.size(); j++) {
            for (size_t k = 0; k < resultado.size(); k++) {
                if (resultado[k].first == ventas[i].productos[j].idProducto) {
                    resultado[k].second += ventas[i].productos[j].cantidad;
                    break;
                }
            }
        }
    }
    return resultado;
}

static string nombreProductoPorId(int id) {
    for (size_t i = 0; i < productos.size(); i++)
        if (productos[i].id == id) return productos[i].nombre;
    return "Producto no disponible";
}

static void mostrarProductoMasVendido() {
    vector<pair<int, int> > datos = calcularUnidadesPorProducto();
    if (datos.empty()) { cout << "No hay productos registrados.\n"; return; }
    int mayor = 0;
    for (size_t i = 0; i < datos.size(); i++)
        if (datos[i].second > mayor) mayor = datos[i].second;
    if (mayor == 0) { cout << "Todavia no se han vendido productos.\n"; return; }
    cout << "Producto(s) mas vendido(s), con " << mayor << " unidades: ";
    for (size_t i = 0; i < datos.size(); i++)
        if (datos[i].second == mayor) cout << nombreProductoPorId(datos[i].first) << " | ";
    cout << "\n";
}

static void mostrarVentasPorProducto() {
    vector<pair<int, int> > datos = calcularUnidadesPorProducto();
    if (datos.empty()) { cout << "No hay productos registrados.\n"; return; }
    cout << fixed << setprecision(2);
    for (size_t i = 0; i < datos.size(); i++) {
        double importe = 0;
        for (size_t j = 0; j < ventas.size(); j++) {
            if (ventas[j].anulada) continue;
            for (size_t k = 0; k < ventas[j].productos.size(); k++) {
                if (ventas[j].productos[k].idProducto == datos[i].first)
                    importe += ventas[j].productos[k].cantidad * ventas[j].productos[k].precio;
            }
        }
        cout << nombreProductoPorId(datos[i].first) << " | Unidades vendidas: "
             << datos[i].second << " | Importe generado: " << importe << "\n";
    }
}

static void mostrarRanking() {
    vector<pair<int, int> > datos = calcularUnidadesPorProducto();
    if (datos.empty()) { cout << "No hay productos para crear un ranking.\n"; return; }
    sort(datos.begin(), datos.end(),
         [](const pair<int, int>& a, const pair<int, int>& b) {
             return a.second > b.second;
         });
    cout << "Ranking por unidades vendidas:\n";
    for (size_t i = 0; i < datos.size(); i++) {
        cout << i + 1 << ". " << nombreProductoPorId(datos[i].first)
             << " - " << datos[i].second << " unidades\n";
    }
}

static void mostrarEstadisticasInventario() {
    int agotados = 0;
    int bajos = 0;
    int disponibles = 0;
    for (size_t i = 0; i < productos.size(); i++) {
        if (productos[i].cantidad == 0) agotados++;
        else if (productos[i].cantidad <= productos[i].minimo) bajos++;
        else disponibles++;
    }
    cout << "Productos agotados: " << agotados << "\n";
    cout << "Productos con existencias bajas: " << bajos << "\n";
    cout << "Productos disponibles por encima del minimo: " << disponibles << "\n";
}

static void calcularRentabilidad() {
    double ingresosVentas = 0;
    double costoProductos = 0;
    for (size_t i = 0; i < ventas.size(); i++) {
        if (ventas[i].anulada) continue;
        for (size_t j = 0; j < ventas[i].productos.size(); j++) {
            ingresosVentas += ventas[i].productos[j].precio * ventas[i].productos[j].cantidad;
            costoProductos += ventas[i].productos[j].costoUnitario * ventas[i].productos[j].cantidad;
        }
    }
    cout << fixed << setprecision(2);
    cout << "Ingresos por ventas activas: " << ingresosVentas << "\n";
    cout << "Costo estimado de los productos vendidos: " << costoProductos << "\n";
    cout << "Rentabilidad bruta estimada: " << ingresosVentas - costoProductos << "\n";
    cout << "Criterio: precio de venta menos costo de adquisicion guardado al registrar cada venta.\n";
}

void mostrarMenuReportes() {
    int opcion;
    do {
        cout << "\n===== REPORTES Y ESTADISTICAS =====\n"
             << "1. Resumen general\n2. Precio promedio de productos\n"
             << "3. Importe promedio de ventas\n4. Producto mas vendido\n"
             << "5. Ventas por producto\n6. Ranking de productos\n"
             << "7. Estadisticas de inventario\n8. Rentabilidad estimada\n0. Volver\n";
        opcion = leerEntero("Opcion: ");
        switch (opcion) {
            case 1: mostrarResumenGeneral(); break;
            case 2: calcularPrecioPromedio(); break;
            case 3: calcularPromedioVentas(); break;
            case 4: mostrarProductoMasVendido(); break;
            case 5: mostrarVentasPorProducto(); break;
            case 6: mostrarRanking(); break;
            case 7: mostrarEstadisticasInventario(); break;
            case 8: calcularRentabilidad(); break;
            case 0: break;
            default: cout << "Opcion incorrecta.\n";
        }
    } while (opcion != 0);
}


==============================================================================
ARCHIVO: src/reportes.h
==============================================================================

#ifndef REPORTES_H
#define REPORTES_H

void mostrarMenuReportes();

#endif


==============================================================================
ARCHIVO: src/validaciones.cpp
==============================================================================

#include "validaciones.h"
#include <iostream>
#include <sstream>
#include <cctype>

using namespace std;

int leerEntero(const string& mensaje) {
    string linea;
    int valor;

    while (true) {
        cout << mensaje;
        getline(cin, linea);
        stringstream entrada(linea);
        char resto;

        if (entrada >> valor && !(entrada >> resto)) {
            return valor;
        }
        cout << "Entrada invalida. Escriba un numero entero.\n";
    }
}

double leerDouble(const string& mensaje) {
    string linea;
    double valor;

    while (true) {
        cout << mensaje;
        getline(cin, linea);
        stringstream entrada(linea);
        char resto;

        if (entrada >> valor && !(entrada >> resto)) {
            return valor;
        }
        cout << "Entrada invalida. Escriba un numero.\n";
    }
}

string leerTexto(const string& mensaje) {
    string texto;
    do {
        cout << mensaje;
        getline(cin, texto);
        if (texto.empty()) {
            cout << "El texto no puede quedar vacio.\n";
        }
    } while (texto.empty());
    return texto;
}

bool validarCantidad(int cantidad) {
    return cantidad >= 0;
}

bool validarPrecio(double precio) {
    return precio >= 0;
}

bool validarTexto(const string& texto) {
    return !texto.empty();
}

bool confirmarOperacion(const string& mensaje) {
    string respuesta;
    cout << mensaje << " (s/n): ";
    getline(cin, respuesta);
    return respuesta == "s" || respuesta == "S" ||
           respuesta == "si" || respuesta == "SI" ||
           respuesta == "Si";
}


==============================================================================
ARCHIVO: src/validaciones.h
==============================================================================

#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <string>
using namespace std;

int leerEntero(const string& mensaje);
double leerDouble(const string& mensaje);
string leerTexto(const string& mensaje);
bool validarCantidad(int cantidad);
bool validarPrecio(double precio);
bool validarTexto(const string& texto);
bool confirmarOperacion(const string& mensaje);

#endif


==============================================================================
ARCHIVO: src/ventas.cpp
==============================================================================

#include "ventas.h"
#include "modelos.h"
#include "productos.h"
#include "inventario.h"
#include "finanzas.h"
#include "validaciones.h"
#include <iostream>
#include <iomanip>

using namespace std;

static int indiceClientePorId(int id) {
    for (size_t i = 0; i < clientes.size(); i++)
        if (clientes[i].id == id) return static_cast<int>(i);
    return -1;
}

static int indiceVentaPorId(int id) {
    for (size_t i = 0; i < ventas.size(); i++)
        if (ventas[i].id == id) return static_cast<int>(i);
    return -1;
}

static int siguienteIdVenta() {
    int mayor = 0;
    for (size_t i = 0; i < ventas.size(); i++)
        if (ventas[i].id > mayor) mayor = ventas[i].id;
    return mayor + 1;
}

static void registrarCliente() {
    Cliente c;
    c.id = leerEntero("ID del cliente: ");
    if (c.id <= 0 || indiceClientePorId(c.id) != -1) {
        cout << "El ID debe ser positivo y no estar repetido.\n";
        return;
    }
    c.nombre = leerTexto("Nombre del cliente: ");
    cout << "Contacto (puede dejarse vacio): ";
    getline(cin, c.contacto);
    clientes.push_back(c);
    cout << "Cliente registrado correctamente.\n";
}

static void listarClientes() {
    if (clientes.empty()) { cout << "No hay clientes registrados.\n"; return; }
    for (size_t i = 0; i < clientes.size(); i++) {
        cout << "ID: " << clientes[i].id << " | " << clientes[i].nombre
             << " | Contacto: " << clientes[i].contacto << "\n";
    }
}

static void buscarCliente() {
    if (clientes.empty()) { cout << "No hay clientes para buscar.\n"; return; }
    int id = leerEntero("ID del cliente: ");
    int indice = indiceClientePorId(id);
    if (indice == -1) cout << "Cliente no encontrado.\n";
    else cout << "Cliente: " << clientes[indice].nombre
              << " | Contacto: " << clientes[indice].contacto << "\n";
}

static void registrarVenta() {
    if (productos.empty()) { cout << "No hay productos registrados.\n"; return; }

    listarClientes();
    int idCliente = leerEntero("ID del cliente (0 si es una venta sin cliente): ");
    if (idCliente != 0 && indiceClientePorId(idCliente) == -1) {
        cout << "Cliente no encontrado.\n";
        return;
    }

    Venta venta;
    venta.id = siguienteIdVenta();
    venta.idCliente = idCliente;
    venta.total = 0;
    venta.anulada = false;

    cout << "Introduzca los productos. Escriba ID 0 para terminar.\n";
    while (true) {
        int idProducto = leerEntero("ID del producto (0 para terminar): ");
        if (idProducto == 0) break;
        int indice = indiceProductoPorId(idProducto);
        if (indice == -1) { cout << "Producto no encontrado.\n"; continue; }

        int cantidad = leerEntero("Cantidad que desea vender: ");
        if (cantidad <= 0) { cout << "La cantidad debe ser mayor que cero.\n"; continue; }

        int cantidadYaAgregada = 0;
        int posicion = -1;
        for (size_t i = 0; i < venta.productos.size(); i++) {
            if (venta.productos[i].idProducto == idProducto) {
                cantidadYaAgregada = venta.productos[i].cantidad;
                posicion = static_cast<int>(i);
                break;
            }
        }
        if (cantidad + cantidadYaAgregada > productos[indice].cantidad) {
            cout << "No hay existencias suficientes. Disponibles: "
                 << productos[indice].cantidad - cantidadYaAgregada << "\n";
            continue;
        }

        if (posicion == -1) {
            Detalle d;
            d.idProducto = idProducto;
            d.cantidad = cantidad;
            d.precio = productos[indice].precioVenta;
            d.costoUnitario = productos[indice].precioCompra;
            venta.productos.push_back(d);
        } else {
            venta.productos[posicion].cantidad += cantidad;
        }
        venta.total += cantidad * productos[indice].precioVenta;
    }

    if (venta.productos.empty()) {
        cout << "Venta cancelada: no se agregaron productos.\n";
        return;
    }

    cout << fixed << setprecision(2) << "\nResumen de venta #" << venta.id << "\n";
    for (size_t i = 0; i < venta.productos.size(); i++) {
        int indice = indiceProductoPorId(venta.productos[i].idProducto);
        cout << productos[indice].nombre << " | Unidades: " << venta.productos[i].cantidad
             << " | Subtotal: " << venta.productos[i].cantidad * venta.productos[i].precio << "\n";
    }
    cout << "TOTAL: " << venta.total << "\n";

    if (!confirmarOperacion("Confirma la venta")) {
        cout << "Venta cancelada. No se modifico el inventario.\n";
        return;
    }

    ventas.push_back(venta);
    for (size_t i = 0; i < venta.productos.size(); i++) {
        int indice = indiceProductoPorId(venta.productos[i].idProducto);
        if (indice != -1) {
            productos[indice].cantidad -= venta.productos[i].cantidad;
            registrarMovimientoInventario(venta.productos[i].idProducto,
                                         venta.productos[i].cantidad,
                                         "Salida", "Venta #" + to_string(venta.id));
        }
    }
    agregarMovimientoFinanciero("Ingreso", "Venta #" + to_string(venta.id),
                                "Venta", venta.total);
    cout << "Venta registrada. Inventario y caja actualizados.\n";
}

static void mostrarHistorialVentas() {
    if (ventas.empty()) { cout << "No hay ventas registradas.\n"; return; }
    cout << fixed << setprecision(2);
    for (size_t i = 0; i < ventas.size(); i++) {
        cout << "\nVenta #" << ventas[i].id << " | Total: " << ventas[i].total
             << " | Estado: " << (ventas[i].anulada ? "ANULADA" : "ACTIVA") << "\n";
        if (ventas[i].idCliente == 0) cout << "Cliente: No registrado\n";
        else {
            int ic = indiceClientePorId(ventas[i].idCliente);
            cout << "Cliente: " << (ic == -1 ? "No disponible" : clientes[ic].nombre) << "\n";
        }
        for (size_t j = 0; j < ventas[i].productos.size(); j++) {
            int ip = indiceProductoPorId(ventas[i].productos[j].idProducto);
            string nombre = ip == -1 ? "Producto eliminado" : productos[ip].nombre;
            cout << "  " << nombre << " | Unidades: " << ventas[i].productos[j].cantidad
                 << " | Precio unitario: " << ventas[i].productos[j].precio << "\n";
        }
    }
}

static void anularVenta() {
    int id = leerEntero("Numero de la venta que desea anular: ");
    int indiceVenta = indiceVentaPorId(id);
    if (indiceVenta == -1) { cout << "Venta no encontrada.\n"; return; }
    if (ventas[indiceVenta].anulada) { cout << "Esa venta ya fue anulada.\n"; return; }

    if (ventas[indiceVenta].total > calcularCaja()) {
        cout << "No hay dinero suficiente en caja para devolver el importe de esta venta.\n";
        return;
    }
    for (size_t i = 0; i < ventas[indiceVenta].productos.size(); i++) {
        if (indiceProductoPorId(ventas[indiceVenta].productos[i].idProducto) == -1) {
            cout << "No se puede anular: falta un producto del historial en el catalogo.\n";
            return;
        }
    }
    if (!confirmarOperacion("Confirma la anulacion y la devolucion del dinero")) {
        cout << "Operacion cancelada.\n";
        return;
    }

    for (size_t i = 0; i < ventas[indiceVenta].productos.size(); i++) {
        const Detalle& d = ventas[indiceVenta].productos[i];
        int ip = indiceProductoPorId(d.idProducto);
        productos[ip].cantidad += d.cantidad;
        registrarMovimientoInventario(d.idProducto, d.cantidad,
                                     "Entrada", "Anulacion de venta #" + to_string(id));
    }
    agregarMovimientoFinanciero("Gasto", "Devolucion de venta #" + to_string(id),
                                "Anulacion de venta", ventas[indiceVenta].total);
    ventas[indiceVenta].anulada = true;
    cout << "Venta anulada. Se restauraron las existencias y se registro la devolucion.\n";
}

void mostrarMenuVentas() {
    int opcion;
    do {
        cout << "\n===== VENTAS Y CLIENTES =====\n"
             << "1. Registrar cliente\n2. Listar clientes\n3. Buscar cliente\n"
             << "4. Registrar venta\n5. Historial de ventas\n6. Anular venta\n0. Volver\n";
        opcion = leerEntero("Opcion: ");
        switch (opcion) {
            case 1: registrarCliente(); break;
            case 2: listarClientes(); break;
            case 3: buscarCliente(); break;
            case 4: registrarVenta(); break;
            case 5: mostrarHistorialVentas(); break;
            case 6: anularVenta(); break;
            case 0: break;
            default: cout << "Opcion incorrecta.\n";
        }
    } while (opcion != 0);
}


==============================================================================
ARCHIVO: src/ventas.h
==============================================================================

#ifndef VENTAS_H
#define VENTAS_H

void mostrarMenuVentas();

#endif

