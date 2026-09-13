#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace std;

enum Equipo
{
    BLANCO,
    NEGRO
};

struct Ficha
{
    bool activa;
    Equipo equipo;
};

struct Movimiento
{
    int filaOrigen;
    int columnaOrigen;

    int filaDestino;
    int columnaDestino;

    bool captura;
};

struct Nodo
{
    Ficha tablero[8][8];

    Movimiento movimiento;

    int valor;

    vector<Nodo*> hijos;
};

struct Arbol
{
    Nodo* raiz;
};

Equipo oponente(Equipo equipo)
{
    if (equipo == NEGRO)
        return BLANCO;
    else
        return NEGRO;
}
int evaluar(Ficha tablero[8][8])
{
    int negras = 0;
    int blancas = 0;

    for (int f = 0; f < 8; f++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (tablero[f][c].activa)
            {
                if (tablero[f][c].equipo == NEGRO)
                    negras++;
                else
                    blancas++;
            }
        }
    }

    return negras - blancas;
}

int minimax(Nodo* nodo, bool maximizando)
{
    // Si no tiene hijos, es una hoja
    if (nodo->hijos.empty()) {
        return nodo->valor;
    }

    // MAX
    if (maximizando) {
        int mejor = -9999;

        for (int i = 0; i < nodo->hijos.size(); i++)
        {
            int valor = minimax(
                nodo->hijos[i],
                false
            );

            if (valor > mejor)
                mejor = valor;
        }
        nodo->valor = mejor;

        return mejor;
    }

    // MIN
    else {
        int mejor = 9999;

        for (int i = 0; i < nodo->hijos.size(); i++) {
            int valor = minimax(nodo->hijos[i], true);

            if (valor < mejor)
                mejor = valor;
        }

        nodo->valor = mejor;

        return mejor;
    }
}
Movimiento mejorMovimiento(Nodo* raiz)
{
    int mejorValor = -9999;
    Movimiento mejor;

    for (int i = 0; i < raiz->hijos.size(); i++) {
        /*
        //Para ver cual de los hijos tiene la mejor jugada
        cout << "Hijo " << i + 1 << " valor: " << raiz->hijos[i]->valor;

        if (raiz->hijos[i]->movimiento.captura) {
            cout << " CAPTURA";
        }
        cout << endl;
        */

        if (raiz->hijos[i]->valor > mejorValor) {
            mejorValor = raiz->hijos[i]->valor;
            mejor = raiz->hijos[i]->movimiento;
        }
    }

    return mejor;
}
void destruirNodo(Nodo* nodo)
{
    if (nodo == nullptr)
        return;

    for (int i = 0; i < nodo->hijos.size(); i++) {
        destruirNodo(nodo->hijos[i]);
    }

    delete nodo;
}
void destruirArbol(Arbol* arbol)
{
    if (arbol == nullptr)
        return;

    destruirNodo(arbol->raiz);

    delete arbol;
}

void inicializar(Ficha tablero[8][8])
{
    // Primero dejamos todas las casillas vacías
    for (int f = 0; f < 8; f++)
    {
        for (int c = 0; c < 8; c++)
        {
            tablero[f][c].activa = false;
        }
    }

    // Fichas negras
    for (int f = 0; f < 3; f++)
    {
        for (int c = 0; c < 8; c++)
        {
            if ((f + c) % 2 == 1)
            {
                tablero[f][c].activa = true;
                tablero[f][c].equipo = NEGRO;
            }
        }
    }

    // Fichas blancas
    for (int f = 5; f < 8; f++)
    {
        for (int c = 0; c < 8; c++)
        {
            if ((f + c) % 2 == 1)
            {
                tablero[f][c].activa = true;
                tablero[f][c].equipo = BLANCO;
            }
        }
    }
}

void mostrarTablero(Ficha tablero[8][8])
{
    for (int f = 0; f < 8; f++) {
        for (int c = 0; c < 8; c++) {
            if (!tablero[f][c].activa) {
                cout << ". ";
            }
            else if (tablero[f][c].equipo == NEGRO) {
                cout << "N ";
            }
            else {
                cout << "B ";
            }
        }

        cout << endl;
    }
}

vector<Movimiento> generar(Ficha tablero[8][8], Equipo equipo)
{
    vector<Movimiento> movimientos;

    int direccion;

    if (equipo == NEGRO)
        direccion = 1;
    else
        direccion = -1;

    for (int f = 0; f < 8; f++) {
        for (int c = 0; c < 8; c++) {

            if (!tablero[f][c].activa)
                continue;

            if (tablero[f][c].equipo != equipo)
                continue;

            // Revisamos los dos movimientos
            for (int dc = -1; dc <= 1; dc += 2) {
                int nf = f + direccion;
                int nc = c + dc;

                if (nf >= 0 && nf < 8 && nc >= 0 && nc < 8) {
                    if (!tablero[nf][nc].activa) {
                        Movimiento m;

                        m.filaOrigen = f;
                        m.columnaOrigen = c;

                        m.filaDestino = nf;
                        m.columnaDestino = nc;

                        m.captura = false;

                        movimientos.push_back(m);
                    }
                }

                //Captura
                int mf = f + direccion;
                int mc = c + dc;

                nf = f + 2 * direccion;
                nc = c + 2 * dc;

                if (mf >= 0 && mf < 8 &&
                    mc >= 0 && mc < 8 &&
                    nf >= 0 && nf < 8 &&
                    nc >= 0 && nc < 8)
                {
                    // Hay una ficha enemiga en medio
                    if (tablero[mf][mc].activa && tablero[mf][mc].equipo != equipo) {
                        // Y la casilla de destino está vacía
                        if (!tablero[nf][nc].activa)
                        {
                            Movimiento m;

                            m.filaOrigen = f;
                            m.columnaOrigen = c;

                            m.filaDestino = nf;
                            m.columnaDestino = nc;

                            m.captura = true;

                            movimientos.push_back(m);
                        }
                    }
                }
            }
        }
    }

    return movimientos;
}

void mostrarMovimientos(vector<Movimiento> movimientos)
{
    cout << "Movimientos posibles: " << movimientos.size() << endl;

    for (int i = 0; i < movimientos.size(); i++) {
        cout << i + 1 << ". ";

        cout << "(" << movimientos[i].filaOrigen << "," << movimientos[i].columnaOrigen << ") -> ";
        cout << "(" << movimientos[i].filaDestino << "," << movimientos[i].columnaDestino << ")";

        if (movimientos[i].captura)
            cout << " CAPTURA";

        cout << endl;
    }
}
void aplicarMovimiento(Ficha tablero[8][8], Movimiento m)
{
    Ficha ficha = tablero[m.filaOrigen][m.columnaOrigen];

    // Vaciar la posición de origen
    tablero[m.filaOrigen][m.columnaOrigen].activa = false;

    // Si es una captura, eliminar la ficha del medio
    if (m.captura)
    {
        int filaCapturada = (m.filaOrigen + m.filaDestino) / 2;

        int columnaCapturada = (m.columnaOrigen + m.columnaDestino) / 2;

        tablero[filaCapturada][columnaCapturada].activa = false;
    }

    // Colocar la ficha en la nueva posición
    tablero[m.filaDestino][m.columnaDestino] = ficha;
}
void hacerMovimientoJugador(Ficha tablero[8][8])
{
    vector<Movimiento> movimientos =
        generar(tablero, BLANCO);

    if (movimientos.empty()) {
        cout << "El jugador no tiene movimientos." << endl;
        return;
    }

    mostrarMovimientos(movimientos);

    int opcion;

    cout << "Elija un movimiento: ";
    cin >> opcion;

    while (opcion < 1 || opcion > movimientos.size())
    {
        cout << "Opcion invalida. Elija nuevamente: ";
        cin >> opcion;
    }

    aplicarMovimiento(tablero, movimientos[opcion - 1]);

    cout << "Movimiento del jugador realizado." << endl;

}

void copiar(Ficha origen[8][8], Ficha destino[8][8])
{
    for (int f = 0; f < 8; f++)
    {
        for (int c = 0; c < 8; c++)
        {
            destino[f][c] = origen[f][c];
        }
    }
}

Nodo* construirArbol(Nodo* nodo, int profundidad, Equipo turno)
{
    // Si llegamos al límite de búsqueda
    if (profundidad == 0)
    {
        nodo->valor = evaluar(nodo->tablero);
        return nodo;
    }

    // Generar todos los movimientos posibles
    vector<Movimiento> movimientos = generar(nodo->tablero, turno);

    // Si no hay movimientos, evaluamos la posición
    if (movimientos.empty())
    {
        nodo->valor = evaluar(nodo->tablero);
        return nodo;
    }

    // Crear un hijo por cada movimiento
    for (int i = 0; i < movimientos.size(); i++)
    {
        Nodo* hijo = new Nodo;

        // Copiamos el tablero del padre
        copiar(nodo->tablero, hijo->tablero);

        // Guardamos el movimiento que produjo este tablero
        hijo->movimiento = movimientos[i];

        // Todavía no sabemos su valor
        hijo->valor = 0;

        // Aplicamos el movimiento al tablero del hijo
        aplicarMovimiento(hijo->tablero, movimientos[i]);

        // Continuamos con el siguiente nivel
        construirArbol(hijo, profundidad - 1, oponente(turno));

        // Agregamos el hijo al nodo actual
        nodo->hijos.push_back(hijo);
    }

    return nodo;
}
int contarNodos(Nodo* nodo)
{
    if (nodo == nullptr)
        return 0;

    int cantidad = 1;

    for (int i = 0; i < nodo->hijos.size(); i++)
    {
        cantidad += contarNodos(nodo->hijos[i]);
    }

    return cantidad;
}

void hacerMovimientoMaquina(
    Ficha tablero[8][8],
    int profundidad)
{
    cout << endl;
    cout << "Turno de la maquina..." << endl;

    // Crear un arbol nuevo
    Arbol* arbol = new Arbol;

    arbol->raiz = new Nodo;

    // La raiz representa el tablero actual
    copiar(tablero, arbol->raiz->tablero);

    arbol->raiz->valor = 0;

    // Construir el arbol
    construirArbol(arbol->raiz, profundidad, NEGRO);

    cout << "Arbol construido." << endl;

    cout << "Cantidad de nodos: " << contarNodos(arbol->raiz) << endl;

    // Aplicar Minimax
    int valor = minimax(arbol->raiz, true);

    cout << "Valor Minimax: " << valor << endl;

    // Obtener mejor movimiento
    Movimiento mejor = mejorMovimiento(arbol->raiz);

    cout << "Movimiento de la maquina: (" << mejor.filaOrigen << "," << mejor.columnaOrigen << ") -> (" << mejor.filaDestino << "," << mejor.columnaDestino << ")";

    if (mejor.captura)
        cout << " CAPTURA";

    cout << endl;

    // Aplicar el movimiento al tablero real
    aplicarMovimiento(tablero, mejor);

    // Destruir el arbol anterior
    destruirArbol(arbol);

    cout << "Arbol destruido." << endl;
}



int main()
{
    int profundidad;
    cout << "Ingrese profundidad: ";
    cin >> profundidad;

    while (profundidad < 1)
    {
        cout << "La profundidad debe ser mayor que 0: ";
        cin >> profundidad;
    }

    int opcionInicio;
    cout << endl << "Quien empieza?" << endl;
    cout << "1. Maquina" << endl;
    cout << "2. Jugador" << endl;
    cout << "Opcion: ";
    cin >> opcionInicio;

    Equipo turno = (opcionInicio == 1) ? NEGRO : BLANCO;
    Ficha tablero[8][8];
    inicializar(tablero);
    sf::RenderWindow window(sf::VideoMode(640, 640), "Juego de Damas - Grupo D");
    window.setFramerateLimit(60);

    bool fichaSeleccionada = false;
    int filaOrigenClick = -1, colOrigenClick = -1;

    // --- GAME LOOP PRINCIPAL ---
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (turno == BLANCO && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                int c = event.mouseButton.x / 80;
                int f = event.mouseButton.y / 80;

                if (f >= 0 && f < 8 && c >= 0 && c < 8)
                {
                    if (!fichaSeleccionada)
                    {
                        if (tablero[f][c].activa && tablero[f][c].equipo == BLANCO)
                        {
                            filaOrigenClick = f;
                            colOrigenClick = c;
                            fichaSeleccionada = true;
                        }
                    }
                    else
                    {
                        vector<Movimiento> movimientosValidos = generar(tablero, BLANCO);
                        bool movimientoEjecutado = false;

                        for (size_t i = 0; i < movimientosValidos.size(); i++)
                        {
                            if (movimientosValidos[i].filaOrigen == filaOrigenClick &&
                                movimientosValidos[i].columnaOrigen == colOrigenClick &&
                                movimientosValidos[i].filaDestino == f &&
                                movimientosValidos[i].columnaDestino == c)
                            {
                                aplicarMovimiento(tablero, movimientosValidos[i]);
                                turno = NEGRO;
                                movimientoEjecutado = true;
                                break;
                            }
                        }
                        fichaSeleccionada = false;
                        filaOrigenClick = -1;
                        colOrigenClick = -1;
                    }
                }
            }
        }

        // --- LÓGICA DE TURNOS ---
        // Verificaciones de fin de juego y ejecucion de la IA
        vector<Movimiento> movimientosDisponibles = generar(tablero, turno);
        if (movimientosDisponibles.empty())
        {
            if (turno == NEGRO)
                cout << "La maquina no tiene movimientos. Gana el jugador!" << endl;
            else
                cout << "El jugador no tiene movimientos. Gana la maquina!" << endl;
            sf::sleep(sf::seconds(3));
            window.close();
            break;
        }

        // Si es el turno de la IA, ejecuta su analisis Minimax sin bloquear la ventana
        if (turno == NEGRO)
        {
            hacerMovimientoMaquina(tablero, profundidad);
            turno = BLANCO;
        }

        // --- RENDERIZADO GRÁFICO (DIBUJO) ---
        window.clear();
        for (int f = 0; f < 8; f++)
        {
            for (int c = 0; c < 8; c++)
            {
                sf::RectangleShape casilla(sf::Vector2f(80.f, 80.f));
                casilla.setPosition(c * 80.f, f * 80.f);

                if ((f + c) % 2 == 0)
                    casilla.setFillColor(sf::Color(240, 217, 181));
                else
                    casilla.setFillColor(sf::Color(181, 136, 99));

                if (fichaSeleccionada && f == filaOrigenClick && c == colOrigenClick)
                    casilla.setFillColor(sf::Color(130, 151, 105));

                window.draw(casilla);
            }
        }

        for (int f = 0; f < 8; f++)
        {
            for (int c = 0; c < 8; c++)
            {
                if (tablero[f][c].activa)
                {
                    sf::CircleShape ficha(32.f);
                    ficha.setPosition(c * 80.f + 8.f, f * 80.f + 8.f);
                    ficha.setOutlineThickness(2.f);
                    ficha.setOutlineColor(sf::Color(50, 50, 50));

                    if (tablero[f][c].equipo == NEGRO)
                        ficha.setFillColor(sf::Color(30, 30, 30));
                    else
                        ficha.setFillColor(sf::Color(240, 240, 240));

                    window.draw(ficha);
                }
            }
        }

        window.display();
    }

    return 0;
}
