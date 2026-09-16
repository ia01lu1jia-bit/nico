#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cctype>
#include <limits>

using namespace std;

struct Palabra {
    string aleman;
    string espanol;
    string articulo;
};

struct Estadisticas {
    int correctas = 0;
    int incorrectas = 0;

    int total() const {
        return correctas + incorrectas;
    }

    double precision() const {
        if (total() == 0) return 0.0;
        return (100.0 * correctas) / total();
    }
};

string normalizar(string texto) {
    while (!texto.empty() && isspace(static_cast<unsigned char>(texto.front()))) {
        texto.erase(texto.begin());
    }

    while (!texto.empty() && isspace(static_cast<unsigned char>(texto.back()))) {
        texto.pop_back();
    }

    transform(texto.begin(), texto.end(), texto.begin(), [](unsigned char c) {
        return static_cast<char>(tolower(c));
    });

    return texto;
}

void mostrarTitulo() {
    cout << "========================================\n";
    cout << "            DEUTSCHCOACH v0.1           \n";
    cout << "========================================\n";
    cout << "Entrenador personal de aleman en C++\n\n";
}

void registrarRespuesta(bool correcta, Estadisticas& stats) {
    if (correcta) {
        cout << "Correcto!\n\n";
        stats.correctas++;
    } else {
        stats.incorrectas++;
    }
}

vector<int> crearOrdenAleatorio(int cantidad) {
    vector<int> indices(cantidad);
    for (int i = 0; i < cantidad; i++) {
        indices[i] = i;
    }

    random_device rd;
    mt19937 generador(rd());
    shuffle(indices.begin(), indices.end(), generador);
    return indices;
}

void practicarAlemanAEspanol(const vector<Palabra>& palabras, Estadisticas& stats) {
    vector<int> orden = crearOrdenAleatorio(static_cast<int>(palabras.size()));
    int cantidad = min(10, static_cast<int>(palabras.size()));

    cout << "\nMODO: Aleman -> Espanol\n";
    cout << "Escribe la traduccion. Son " << cantidad << " preguntas.\n\n";

    for (int i = 0; i < cantidad; i++) {
        const Palabra& p = palabras[orden[i]];
        string respuesta;

        cout << "Pregunta " << (i + 1) << "/" << cantidad << ": " << p.aleman << " = ";
        getline(cin, respuesta);

        if (normalizar(respuesta) == normalizar(p.espanol)) {
            registrarRespuesta(true, stats);
        } else {
            cout << "Incorrecto. Respuesta: " << p.espanol << "\n\n";
            registrarRespuesta(false, stats);
        }
    }
}

void practicarEspanolAAleman(const vector<Palabra>& palabras, Estadisticas& stats) {
    vector<int> orden = crearOrdenAleatorio(static_cast<int>(palabras.size()));
    int cantidad = min(10, static_cast<int>(palabras.size()));

    cout << "\nMODO: Espanol -> Aleman\n";
    cout << "Escribe la palabra alemana SIN el articulo.\n\n";

    for (int i = 0; i < cantidad; i++) {
        const Palabra& p = palabras[orden[i]];
        string respuesta;

        cout << "Pregunta " << (i + 1) << "/" << cantidad << ": " << p.espanol << " = ";
        getline(cin, respuesta);

        if (normalizar(respuesta) == normalizar(p.aleman)) {
            registrarRespuesta(true, stats);
        } else {
            cout << "Incorrecto. Respuesta: " << p.aleman << "\n\n";
            registrarRespuesta(false, stats);
        }
    }
}

void practicarArticulos(const vector<Palabra>& palabras, Estadisticas& stats) {
    vector<int> orden = crearOrdenAleatorio(static_cast<int>(palabras.size()));
    int cantidad = min(10, static_cast<int>(palabras.size()));

    cout << "\nMODO: der / die / das\n";
    cout << "Escribe el articulo correcto.\n\n";

    for (int i = 0; i < cantidad; i++) {
        const Palabra& p = palabras[orden[i]];
        string respuesta;

        cout << "Pregunta " << (i + 1) << "/" << cantidad << ": ___ " << p.aleman << " = ";
        getline(cin, respuesta);

        if (normalizar(respuesta) == normalizar(p.articulo)) {
            registrarRespuesta(true, stats);
        } else {
            cout << "Incorrecto. Respuesta: " << p.articulo << " " << p.aleman << "\n\n";
            registrarRespuesta(false, stats);
        }
    }
}

void mostrarEstadisticas(const Estadisticas& stats) {
    cout << "\n========== ESTADISTICAS ==========" << "\n";
    cout << "Respuestas totales: " << stats.total() << "\n";
    cout << "Correctas:          " << stats.correctas << "\n";
    cout << "Incorrectas:        " << stats.incorrectas << "\n";
    cout << "Precision:           " << stats.precision() << "%\n";
    cout << "==================================\n\n";
}

int leerOpcion() {
    string entrada;
    getline(cin, entrada);

    if (entrada.size() == 1 && entrada[0] >= '0' && entrada[0] <= '9') {
        return entrada[0] - '0';
    }

    return -1;
}

int main() {
    vector<Palabra> palabras = {
        {"Haus", "casa", "das"},
        {"Buch", "libro", "das"},
        {"Tisch", "mesa", "der"},
        {"Schule", "escuela", "die"},
        {"Auto", "auto", "das"},
        {"Wasser", "agua", "das"},
        {"Brot", "pan", "das"},
        {"Freund", "amigo", "der"},
        {"Stadt", "ciudad", "die"},
        {"Zeit", "tiempo", "die"},
        {"Arbeit", "trabajo", "die"},
        {"Geld", "dinero", "das"},
        {"Tag", "dia", "der"},
        {"Nacht", "noche", "die"},
        {"Bahnhof", "estacion", "der"},
        {"Rechnung", "factura", "die"}
    };

    Estadisticas stats;
    int opcion = -1;

    while (opcion != 0) {
        mostrarTitulo();
        cout << "1. Practicar Aleman -> Espanol\n";
        cout << "2. Practicar Espanol -> Aleman\n";
        cout << "3. Practicar articulos (der/die/das)\n";
        cout << "4. Ver estadisticas de esta sesion\n";
        cout << "0. Salir\n\n";
        cout << "Opcion: ";

        opcion = leerOpcion();

        switch (opcion) {
            case 1:
                practicarAlemanAEspanol(palabras, stats);
                break;
            case 2:
                practicarEspanolAAleman(palabras, stats);
                break;
            case 3:
                practicarArticulos(palabras, stats);
                break;
            case 4:
                mostrarEstadisticas(stats);
                break;
            case 0:
                cout << "Auf Wiedersehen!\n";
                break;
            default:
                cout << "Opcion no valida.\n\n";
                break;
        }

        if (opcion != 0) {
            cout << "Presiona ENTER para volver al menu...";
            string pausa;
            getline(cin, pausa);
            cout << "\n";
        }
    }

    return 0;
}
