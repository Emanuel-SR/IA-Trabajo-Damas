#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>


/*
Curso: Inteligencia Artificial
Docente: Juan Carlos Gutierrez Caceres

Integrantes:

-Julio Eduardo Pino Charun
-Rodrigo Emanuel Santiago Salas Ramos
-Gianella Ariana Rosas Lipa

*/
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

Equipo oponente(Equipo equipo)
{
    if (equipo == NEGRO)
        return BLANCO;
    else
        return NEGRO;
}

class Tablero
{
private:
    Ficha matriz[8][8];
public:
    Tablero() {
        inicializar();
    }
    void inicializar() {
        // primero dejamos todas las casillas vacías
        for (int f = 0; f < 8; f++)
        {
            for (int c = 0; c < 8; c++)
            {
                matriz[f][c].activa = false;
            }
        }

        // Fichas negras
        for (int f = 0; f < 3; f++)
        {
            for (int c = 0; c < 8; c++)
            {
                if ((f + c) % 2 == 1)
                {
                    matriz[f][c].activa = true;
                    matriz[f][c].equipo = NEGRO;
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
                    matriz[f][c].activa = true;
                    matriz[f][c].equipo = BLANCO;
                }
            }
        }
    }

    vector<Movimiento> generarMovimientos(Equipo equipo)
    {
        vector<Movimiento> movimientos;

        int direccion;

        if (equipo == NEGRO)
            direccion = 1;
        else
            direccion = -1;

        for (int f = 0; f < 8; f++) {
            for (int c = 0; c < 8; c++) {

                if (!matriz[f][c].activa)
                    continue;

                if (matriz[f][c].equipo != equipo)
                    continue;

                // revisamos los dos movimientos
                for (int dc = -1; dc <= 1; dc += 2) {
                    int nf = f + direccion;
                    int nc = c + dc;

                    if (nf >= 0 && nf < 8 && nc >= 0 && nc < 8) {
                        if (!matriz[nf][nc].activa) {
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
                        if (matriz[mf][mc].activa && matriz[mf][mc].equipo != equipo) {
                            // Y la casilla de destino está vacía
                            if (!matriz[nf][nc].activa)
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

    void aplicarMovimiento(Movimiento m)
    {
        Ficha ficha = matriz[m.filaOrigen][m.columnaOrigen];

        // Vaciar la posición de origen
        matriz[m.filaOrigen][m.columnaOrigen].activa = false;

        // Si es una captura, eliminar la ficha del medio
        if (m.captura)
        {
            int filaCapturada = (m.filaOrigen + m.filaDestino) / 2;

            int columnaCapturada = (m.columnaOrigen + m.columnaDestino) / 2;

            matriz[filaCapturada][columnaCapturada].activa = false;
        }

        // Colocar la ficha en la nueva posición
        matriz[m.filaDestino][m.columnaDestino] = ficha;
    }

    int evaluar()
    {
        int negras = 0;
        int blancas = 0;

        for (int f = 0; f < 8; f++)
        {
            for (int c = 0; c < 8; c++)
            {
                if (matriz[f][c].activa)
                {
                    if (matriz[f][c].equipo == NEGRO)
                        negras++;
                    else
                        blancas++;
                }
            }
        }

        return negras - blancas;
    }

    Ficha& getFicha(int f, int c)
    {
        return matriz[f][c];
    }
};

struct Nodo
{
    Tablero tablero;

    Movimiento movimiento;

    int valor;

    vector<Nodo*> hijos;

    ~Nodo()
    {
        for (Nodo* hijo : hijos)
        {
            delete hijo;
        }
    }
};

class IA
{
private:
    Nodo* construirArbol(Tablero estadoActual, int profundidad, Equipo turno)
    {
        Nodo* nodo = new Nodo();
        nodo->tablero = estadoActual;
        // Si llegamos al límite de búsqueda
        if (profundidad == 0)
        {
            nodo->valor = nodo->tablero.evaluar();
            return nodo;
        }

        // Generar todos los movimientos posibles
        vector<Movimiento> movimientos = nodo->tablero.generarMovimientos(turno);

        // Si no hay movimientos, evaluamos la posición
        if (movimientos.empty())
        {
            nodo->valor = nodo->tablero.evaluar();
            return nodo;
        }

        // Crear un hijo por cada movimiento
        for (int i = 0; i < movimientos.size(); i++)
        {
            Tablero tableroHijo = nodo->tablero;

            tableroHijo.aplicarMovimiento(movimientos[i]);

            Nodo* hijo = construirArbol(tableroHijo, profundidad - 1, oponente(turno));
            hijo->movimiento = movimientos[i];
            nodo->hijos.push_back(hijo);
        }

        return nodo;
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

    int minimaxAlphaBeta(Nodo* nodo, int profundidad, int alpha, int beta, bool maximizando, Equipo turno)
    {
        if (profundidad == 0)
        {
            nodo->valor = nodo->tablero.evaluar();
            return nodo->valor;
        }

        vector<Movimiento> movimientos = nodo->tablero.generarMovimientos(turno);

        if (movimientos.empty())
        {
            nodo->valor = nodo->tablero.evaluar();
            return nodo->valor;
        }

        if (maximizando)
        {
            int mejorValor = -99999;

            for (const auto& mov : movimientos)
            {
                Nodo* hijo = new Nodo();
                hijo->tablero = nodo->tablero;
                hijo->tablero.aplicarMovimiento(mov);
                hijo->movimiento = mov;

                nodo->hijos.push_back(hijo);

                int valor = minimaxAlphaBeta(hijo, profundidad - 1, alpha, beta, false, oponente(turno));

                if (valor > mejorValor) mejorValor = valor;
                if (mejorValor > alpha) alpha = mejorValor;

                if (beta <= alpha) {
                    break;
                }
            }
            nodo->valor = mejorValor;
            return mejorValor;
        }
        else
        {
            int mejorValor = 99999;

            for (const auto& mov : movimientos)
            {
                Nodo* hijo = new Nodo();
                hijo->tablero = nodo->tablero;
                hijo->tablero.aplicarMovimiento(mov);
                hijo->movimiento = mov;

                nodo->hijos.push_back(hijo);

                int valor = minimaxAlphaBeta(hijo, profundidad - 1, alpha, beta, true, oponente(turno));

                if (valor < mejorValor) mejorValor = valor;
                if (mejorValor < beta) beta = mejorValor;

                if (beta <= alpha) {
                    break;
                }
            }
            nodo->valor = mejorValor;
            return mejorValor;
        }
    }
public:
    Movimiento obtenerMejorMovimiento(const Tablero& tableroActual, int profundidad)
    {
        Nodo* raiz = construirArbol(tableroActual, profundidad, NEGRO);
        minimax(raiz, true);

        int mejorValor = -9999;
        Movimiento mejor = raiz->hijos[0]->movimiento;

        for (Nodo* hijo : raiz->hijos)
        {
            if (hijo->valor > mejorValor)
            {
                mejorValor = hijo->valor;
                mejor = hijo->movimiento;
            }
        }

        delete raiz;
        return mejor;
    }

    Movimiento obtenerMejorMovimientoAlphaBeta(const Tablero& tableroActual, int profundidad, Equipo equipo)
    {
        Nodo* raiz = new Nodo();
        raiz->tablero = tableroActual;

        bool esMaximizador = (equipo == NEGRO);

        minimaxAlphaBeta(raiz, profundidad, -99999, 99999, esMaximizador, equipo);

        int mejorValor = esMaximizador ? -99999 : 99999;
        Movimiento mejor = raiz->hijos[0]->movimiento; // Fallback

        for (Nodo* hijo : raiz->hijos)
        {
            if (esMaximizador) {
                if (hijo->valor > mejorValor) {
                    mejorValor = hijo->valor;
                    mejor = hijo->movimiento;
                }
            }
            else {
                if (hijo->valor < mejorValor) {
                    mejorValor = hijo->valor;
                    mejor = hijo->movimiento;
                }
            }
        }

        delete raiz;
        return mejor;
    }
};

class Juego
{
private:
    sf::RenderWindow window;
    Tablero tablero;
    IA ia;
    Equipo turnoActual;
    int profundidadIA;

    bool fichaSeleccionada;
    int filaOrigenClick, colOrigenClick;

    void procesarEventos()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (turnoActual == BLANCO && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

                int c = static_cast<int>(worldPos.x) / 80;
                int f = static_cast<int>(worldPos.y) / 80;

                manejarClickJugador(f, c);
            }
        }
    }

    void manejarClickJugador(int f, int c)
    {
        if (f < 0 || f >= 8 || c < 0 || c >= 8) return;

        if (!fichaSeleccionada)
        {
            if (tablero.getFicha(f, c).activa && tablero.getFicha(f, c).equipo == BLANCO)
            {
                filaOrigenClick = f;
                colOrigenClick = c;
                fichaSeleccionada = true;
            }
        }
        else
        {
            ejecutarMovimientoSiEsValido(f, c);
            fichaSeleccionada = false;
        }
    }

    void ejecutarMovimientoSiEsValido(int f, int c)
    {
        vector<Movimiento> movs = tablero.generarMovimientos(BLANCO);
        for (const auto& mov : movs)
        {
            if (mov.filaOrigen == filaOrigenClick && mov.columnaOrigen == colOrigenClick &&
                mov.filaDestino == f && mov.columnaDestino == c)
            {

                tablero.aplicarMovimiento(mov);
                turnoActual = NEGRO;
                break;
            }
        }
    }

    void actualizarLOGICA()
    {
        vector<Movimiento> posibles = tablero.generarMovimientos(turnoActual);
        if (posibles.empty())
        {
            int val = tablero.evaluar();
            if (val == 0) {
                cout << "Empate" << endl;
            }
            else if (val > 0) {
                cout << "Gana la Maquina!" << endl;
            }
            else {
                cout << "Gana el Jugador!" << endl;
            }
            sf::sleep(sf::seconds(3));
            window.close();
            return;
        }

        if (turnoActual == NEGRO)
        {
           //Movimiento mejor = ia.obtenerMejorMovimiento(tablero, profundidadIA);
            Movimiento mejor = ia.obtenerMejorMovimientoAlphaBeta(tablero, profundidadIA, turnoActual);
            tablero.aplicarMovimiento(mejor);
            turnoActual = BLANCO;
        }
    }

    void renderizar()
    {
        window.clear();
        for (int f = 0; f < 8; f++)
        {
            for (int c = 0; c < 8; c++)
            {
                sf::RectangleShape casilla(sf::Vector2f(80.f, 80.f));
                casilla.setPosition(c * 80.f, f * 80.f);
                casilla.setFillColor((f + c) % 2 == 0 ? sf::Color(240, 217, 181) : sf::Color(181, 136, 99));

                if (fichaSeleccionada && f == filaOrigenClick && c == colOrigenClick)
                    casilla.setFillColor(sf::Color(130, 151, 105));

                window.draw(casilla);

                Ficha fichaActual = tablero.getFicha(f, c);
                if (fichaActual.activa)
                {
                    sf::CircleShape ficha(32.f);
                    ficha.setPosition(c * 80.f + 8.f, f * 80.f + 8.f);
                    ficha.setOutlineThickness(2.f);
                    ficha.setOutlineColor(sf::Color(50, 50, 50));
                    ficha.setFillColor(fichaActual.equipo == NEGRO ? sf::Color(30, 30, 30) : sf::Color(240, 240, 240));
                    window.draw(ficha);
                }
            }
        }
        window.display();
    }

public:
    Juego(int prof, Equipo empieza) : profundidadIA(prof), turnoActual(empieza)
    {
        window.create(sf::VideoMode(640, 640), "Juego de Damas");
        window.setFramerateLimit(60);
        fichaSeleccionada = false;
    }

    void ejecutar()
    {
        while (window.isOpen())
        {
            procesarEventos();
            if (window.isOpen())
            {
                actualizarLOGICA();
                renderizar();
            }
        }
    }
};

int main()
{
    int profundidad;
    do {
        cout << "Ingrese profundidad de la IA (mayor a 0): ";
        cin >> profundidad;
    } while (profundidad < 1);

    int opcionInicio;
    cout << "\nQuien empieza?\n1. Maquina\n2. Jugador\nOpcion: ";
    cin >> opcionInicio;

    Equipo inicia = (opcionInicio == 1) ? NEGRO : BLANCO;

    Juego juego(profundidad, inicia);
    juego.ejecutar();


    return 0;
}
