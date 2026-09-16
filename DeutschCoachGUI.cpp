#define UNICODE
#define _UNICODE

#include <windows.h>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <random>
#include <ctime>

using namespace std;

// ============================================================
// DeutschCoach GUI v0.2
// Aplicacion grafica nativa para Windows, compatible con
// Embarcadero Dev-C++ 6.3 / MinGW.
// No usa consola ni librerias externas.
// ============================================================

struct Palabra {
    wstring aleman;
    wstring espanol;
    wstring articulo;
    int correctas;
    int incorrectas;
};

struct Estadisticas {
    int total;
    int correctas;
    int incorrectas;
    int racha;
    int mejorRacha;
};

enum Pantalla {
    PANTALLA_INICIO,
    PANTALLA_PRACTICA,
    PANTALLA_PROGRESO
};

enum ModoPractica {
    MODO_ARTICULOS,
    MODO_VOCABULARIO,
    MODO_REPASO
};

const wchar_t CLASE_VENTANA[] = L"DeutschCoachVentana";
const char* ARCHIVO_PROGRESO = "deutschcoach_progreso.txt";

// IDs de controles
const int ID_NAV_INICIO = 1001;
const int ID_NAV_PRACTICA = 1002;
const int ID_NAV_PROGRESO = 1003;

const int ID_HOME_REPASO = 1101;
const int ID_HOME_ARTICULOS = 1102;
const int ID_HOME_VOCAB = 1103;

const int ID_RESPUESTA_1 = 1201;
const int ID_RESPUESTA_2 = 1202;
const int ID_RESPUESTA_3 = 1203;
const int ID_SIGUIENTE = 1204;

// Paleta visual
const COLORREF COLOR_FONDO = RGB(247, 249, 252);
const COLORREF COLOR_BLANCO = RGB(255, 255, 255);
const COLORREF COLOR_TEXTO = RGB(39, 48, 67);
const COLORREF COLOR_TEXTO_SUAVE = RGB(108, 117, 136);
const COLORREF COLOR_VERDE = RGB(76, 175, 80);
const COLORREF COLOR_VERDE_OSCURO = RGB(57, 145, 61);
const COLORREF COLOR_VERDE_CLARO = RGB(232, 246, 233);
const COLORREF COLOR_AZUL = RGB(68, 125, 219);
const COLORREF COLOR_AZUL_CLARO = RGB(235, 241, 252);
const COLORREF COLOR_ROJO = RGB(217, 83, 79);
const COLORREF COLOR_ROJO_CLARO = RGB(252, 236, 235);
const COLORREF COLOR_BORDE = RGB(220, 225, 233);
const COLORREF COLOR_GRIS = RGB(235, 238, 243);

HWND ventanaPrincipal = NULL;
HWND btnNavInicio = NULL;
HWND btnNavPractica = NULL;
HWND btnNavProgreso = NULL;
HWND btnHomeRepaso = NULL;
HWND btnHomeArticulos = NULL;
HWND btnHomeVocab = NULL;
HWND btnRespuesta[3] = {NULL, NULL, NULL};
HWND btnSiguiente = NULL;

HFONT fuenteTitulo = NULL;
HFONT fuenteSubtitulo = NULL;
HFONT fuenteNormal = NULL;
HFONT fuenteBoton = NULL;
HFONT fuenteGrande = NULL;

Pantalla pantallaActual = PANTALLA_INICIO;
ModoPractica modoActual = MODO_REPASO;
Estadisticas stats = {0, 0, 0, 0, 0};

vector<Palabra> palabras = {
    {L"Haus", L"casa", L"das", 0, 0},
    {L"Buch", L"libro", L"das", 0, 0},
    {L"Tisch", L"mesa", L"der", 0, 0},
    {L"Schule", L"escuela", L"die", 0, 0},
    {L"Auto", L"auto", L"das", 0, 0},
    {L"Wasser", L"agua", L"das", 0, 0},
    {L"Brot", L"pan", L"das", 0, 0},
    {L"Freund", L"amigo", L"der", 0, 0},
    {L"Freundin", L"amiga", L"die", 0, 0},
    {L"Stadt", L"ciudad", L"die", 0, 0},
    {L"Tag", L"día", L"der", 0, 0},
    {L"Nacht", L"noche", L"die", 0, 0},
    {L"Bahnhof", L"estación", L"der", 0, 0},
    {L"Apfel", L"manzana", L"der", 0, 0},
    {L"Milch", L"leche", L"die", 0, 0},
    {L"Kaffee", L"café", L"der", 0, 0},
    {L"Familie", L"familia", L"die", 0, 0},
    {L"Kind", L"niño/a", L"das", 0, 0}
};

mt19937 generador(static_cast<unsigned int>(time(NULL)));
int indicePalabraActual = 0;
bool preguntaEsArticulo = true;
bool respondida = false;
int botonSeleccionado = 0;
wstring respuestaCorrecta;
wstring mensajeFeedback;
int preguntaSesion = 0;
int correctasSesion = 0;
const int PREGUNTAS_POR_SESION = 10;

void GuardarProgreso() {
    ofstream archivo(ARCHIVO_PROGRESO);
    if (!archivo) return;

    archivo << stats.total << ' '
            << stats.correctas << ' '
            << stats.incorrectas << ' '
            << stats.racha << ' '
            << stats.mejorRacha << '\n';

    archivo << palabras.size() << '\n';
    for (size_t i = 0; i < palabras.size(); ++i) {
        archivo << palabras[i].correctas << ' ' << palabras[i].incorrectas << '\n';
    }
}

void CargarProgreso() {
    ifstream archivo(ARCHIVO_PROGRESO);
    if (!archivo) return;

    archivo >> stats.total
            >> stats.correctas
            >> stats.incorrectas
            >> stats.racha
            >> stats.mejorRacha;

    size_t cantidadGuardada = 0;
    archivo >> cantidadGuardada;

    size_t limite = min(cantidadGuardada, palabras.size());
    for (size_t i = 0; i < limite; ++i) {
        archivo >> palabras[i].correctas >> palabras[i].incorrectas;
    }
}

int PalabrasDominadas() {
    int dominadas = 0;
    for (size_t i = 0; i < palabras.size(); ++i) {
        int total = palabras[i].correctas + palabras[i].incorrectas;
        if (total >= 3) {
            double precision = (100.0 * palabras[i].correctas) / total;
            if (precision >= 75.0) dominadas++;
        }
    }
    return dominadas;
}

double PrecisionGlobal() {
    if (stats.total == 0) return 0.0;
    return (100.0 * stats.correctas) / stats.total;
}

void DibujarTexto(HDC hdc, const wstring& texto, RECT rc, HFONT fuente,
                   COLORREF color, UINT formato) {
    HFONT anterior = (HFONT)SelectObject(hdc, fuente);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    DrawTextW(hdc, texto.c_str(), -1, &rc, formato);
    SelectObject(hdc, anterior);
}

void DibujarRectRedondeado(HDC hdc, RECT rc, COLORREF fondo,
                           COLORREF borde, int radio = 18, int anchoBorde = 1) {
    HBRUSH brocha = CreateSolidBrush(fondo);
    HPEN lapiz = CreatePen(PS_SOLID, anchoBorde, borde);
    HBRUSH brochaAnterior = (HBRUSH)SelectObject(hdc, brocha);
    HPEN lapizAnterior = (HPEN)SelectObject(hdc, lapiz);

    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, radio, radio);

    SelectObject(hdc, brochaAnterior);
    SelectObject(hdc, lapizAnterior);
    DeleteObject(brocha);
    DeleteObject(lapiz);
}

void CrearFuentes() {
    fuenteTitulo = CreateFontW(-34, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               DEFAULT_PITCH, L"Segoe UI");

    fuenteSubtitulo = CreateFontW(-22, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH, L"Segoe UI");

    fuenteNormal = CreateFontW(-18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               DEFAULT_PITCH, L"Segoe UI");

    fuenteBoton = CreateFontW(-18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH, L"Segoe UI");

    fuenteGrande = CreateFontW(-52, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               DEFAULT_PITCH, L"Segoe UI");
}

HWND CrearBoton(HWND padre, int id, const wchar_t* texto) {
    return CreateWindowExW(
        0,
        L"BUTTON",
        texto,
        WS_CHILD | WS_TABSTOP | BS_OWNERDRAW,
        0, 0, 100, 40,
        padre,
        (HMENU)(INT_PTR)id,
        GetModuleHandleW(NULL),
        NULL
    );
}

void MostrarControl(HWND control, bool mostrar) {
    ShowWindow(control, mostrar ? SW_SHOW : SW_HIDE);
}

void ActualizarControles() {
    bool inicio = (pantallaActual == PANTALLA_INICIO);
    bool practica = (pantallaActual == PANTALLA_PRACTICA);

    MostrarControl(btnNavInicio, true);
    MostrarControl(btnNavPractica, true);
    MostrarControl(btnNavProgreso, true);

    MostrarControl(btnHomeRepaso, inicio);
    MostrarControl(btnHomeArticulos, inicio);
    MostrarControl(btnHomeVocab, inicio);

    for (int i = 0; i < 3; ++i) {
        MostrarControl(btnRespuesta[i], practica);
    }

    MostrarControl(btnSiguiente, practica && respondida);

    InvalidateRect(ventanaPrincipal, NULL, TRUE);
}

void DistribuirControles(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);

    const int sidebar = 205;
    const int margen = 28;
    int anchoContenido = rc.right - sidebar;

    MoveWindow(btnNavInicio, 20, 86, 165, 48, TRUE);
    MoveWindow(btnNavPractica, 20, 144, 165, 48, TRUE);
    MoveWindow(btnNavProgreso, 20, 202, 165, 48, TRUE);

    int xContenido = sidebar + margen;

    // Inicio
    int yCards = 340;
    int anchoCard = (anchoContenido - margen * 3) / 2;
    if (anchoCard < 220) anchoCard = 220;

    MoveWindow(btnHomeRepaso, xContenido, yCards, anchoCard, 72, TRUE);
    MoveWindow(btnHomeArticulos, xContenido + anchoCard + margen, yCards, anchoCard, 72, TRUE);
    MoveWindow(btnHomeVocab, xContenido, yCards + 92, anchoCard, 72, TRUE);

    // Practica
    int centroX = sidebar + anchoContenido / 2;
    int anchoResp = 180;
    int separacion = 18;
    int totalResp = anchoResp * 3 + separacion * 2;
    int inicioResp = centroX - totalResp / 2;

    for (int i = 0; i < 3; ++i) {
        MoveWindow(btnRespuesta[i], inicioResp + i * (anchoResp + separacion), 390,
                   anchoResp, 62, TRUE);
    }

    MoveWindow(btnSiguiente, centroX - 110, 490, 220, 56, TRUE);
}

int ElegirPalabraAleatoria() {
    uniform_int_distribution<int> dist(0, (int)palabras.size() - 1);
    int elegido = dist(generador);

    if (palabras.size() > 1 && elegido == indicePalabraActual) {
        elegido = (elegido + 1) % palabras.size();
    }
    return elegido;
}

int ElegirPalabraDebil() {
    vector<pair<int, int> > ranking;

    for (size_t i = 0; i < palabras.size(); ++i) {
        // Mayor puntaje = necesita mas repaso.
        int puntaje = palabras[i].incorrectas * 4 - palabras[i].correctas;
        ranking.push_back(make_pair(puntaje, (int)i));
    }

    sort(ranking.begin(), ranking.end(),
         [](const pair<int, int>& a, const pair<int, int>& b) {
             return a.first > b.first;
         });

    int candidatos = min(5, (int)ranking.size());
    uniform_int_distribution<int> dist(0, candidatos - 1);
    return ranking[dist(generador)].second;
}

vector<wstring> OpcionesVocabulario(int indiceCorrecto) {
    vector<wstring> opciones;
    opciones.push_back(palabras[indiceCorrecto].espanol);

    vector<int> indices;
    for (size_t i = 0; i < palabras.size(); ++i) {
        if ((int)i != indiceCorrecto) indices.push_back((int)i);
    }

    shuffle(indices.begin(), indices.end(), generador);
    for (int i = 0; i < 2 && i < (int)indices.size(); ++i) {
        opciones.push_back(palabras[indices[i]].espanol);
    }

    shuffle(opciones.begin(), opciones.end(), generador);
    return opciones;
}

void PrepararPregunta() {
    respondida = false;
    botonSeleccionado = 0;
    mensajeFeedback = L"";

    for (int i = 0; i < 3; ++i) {
        EnableWindow(btnRespuesta[i], TRUE);
    }

    if (modoActual == MODO_REPASO) {
        indicePalabraActual = ElegirPalabraDebil();
        uniform_int_distribution<int> tipo(0, 1);
        preguntaEsArticulo = (tipo(generador) == 0);
    } else {
        indicePalabraActual = ElegirPalabraAleatoria();
        preguntaEsArticulo = (modoActual == MODO_ARTICULOS);
    }

    if (preguntaEsArticulo) {
        vector<wstring> opciones;
        opciones.push_back(L"der");
        opciones.push_back(L"die");
        opciones.push_back(L"das");
        respuestaCorrecta = palabras[indicePalabraActual].articulo;

        for (int i = 0; i < 3; ++i) {
            SetWindowTextW(btnRespuesta[i], opciones[i].c_str());
        }
    } else {
        vector<wstring> opciones = OpcionesVocabulario(indicePalabraActual);
        respuestaCorrecta = palabras[indicePalabraActual].espanol;

        for (int i = 0; i < 3; ++i) {
            SetWindowTextW(btnRespuesta[i], opciones[i].c_str());
        }
    }

    SetWindowTextW(btnSiguiente, L"Siguiente");
    ActualizarControles();
}

void IniciarPractica(ModoPractica modo) {
    modoActual = modo;
    pantallaActual = PANTALLA_PRACTICA;
    preguntaSesion = 1;
    correctasSesion = 0;
    PrepararPregunta();
}

void RegistrarRespuesta(int idBoton) {
    if (respondida) return;

    wchar_t buffer[256];
    GetWindowTextW(GetDlgItem(ventanaPrincipal, idBoton), buffer, 256);
    wstring respuestaUsuario = buffer;

    bool correcta = (respuestaUsuario == respuestaCorrecta);
    respondida = true;
    botonSeleccionado = idBoton;

    stats.total++;

    if (correcta) {
        stats.correctas++;
        stats.racha++;
        correctasSesion++;
        palabras[indicePalabraActual].correctas++;
        mensajeFeedback = L"¡Correcto!";

        if (stats.racha > stats.mejorRacha) {
            stats.mejorRacha = stats.racha;
        }
    } else {
        stats.incorrectas++;
        stats.racha = 0;
        palabras[indicePalabraActual].incorrectas++;
        mensajeFeedback = L"La respuesta correcta es: " + respuestaCorrecta;
    }

    for (int i = 0; i < 3; ++i) {
        EnableWindow(btnRespuesta[i], FALSE);
    }

    if (preguntaSesion >= PREGUNTAS_POR_SESION) {
        SetWindowTextW(btnSiguiente, L"Ver resultado");
    }

    GuardarProgreso();
    ActualizarControles();
}

void SiguientePregunta() {
    if (!respondida) return;

    if (preguntaSesion >= PREGUNTAS_POR_SESION) {
        pantallaActual = PANTALLA_PROGRESO;
        ActualizarControles();
        return;
    }

    preguntaSesion++;
    PrepararPregunta();
}

void DibujarSidebar(HDC hdc, RECT rc) {
    RECT sidebar = {0, 0, 205, rc.bottom};
    HBRUSH b = CreateSolidBrush(COLOR_BLANCO);
    FillRect(hdc, &sidebar, b);
    DeleteObject(b);

    HPEN pen = CreatePen(PS_SOLID, 1, COLOR_BORDE);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, 204, 0, NULL);
    LineTo(hdc, 204, rc.bottom);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    RECT logo = {24, 24, 185, 65};
    DibujarTexto(hdc, L"DeutschCoach", logo, fuenteSubtitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT nivel = {25, 260, 180, 290};
    DibujarTexto(hdc, L"Nivel: A1 · Inicio", nivel, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void DibujarInicio(HDC hdc, RECT rc) {
    int x = 242;

    RECT titulo = {x, 54, rc.right - 30, 105};
    DibujarTexto(hdc, L"Guten Tag! 👋", titulo, fuenteTitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT subtitulo = {x, 104, rc.right - 30, 142};
    DibujarTexto(hdc, L"Empecemos desde cero y avancemos paso a paso.", subtitulo,
                 fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT tarjeta = {x, 168, rc.right - 38, 305};
    DibujarRectRedondeado(hdc, tarjeta, COLOR_BLANCO, COLOR_BORDE, 22, 1);

    RECT txt1 = {x + 24, 186, x + 300, 218};
    DibujarTexto(hdc, L"Tu progreso", txt1, fuenteSubtitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    int dominadas = PalabrasDominadas();
    wstring progreso = to_wstring(dominadas) + L" de " + to_wstring(palabras.size()) + L" palabras dominadas";
    RECT txt2 = {x + 24, 222, x + 380, 252};
    DibujarTexto(hdc, progreso, txt2, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT barraFondo = {x + 24, 266, rc.right - 70, 282};
    DibujarRectRedondeado(hdc, barraFondo, COLOR_GRIS, COLOR_GRIS, 12, 1);

    double proporcion = palabras.empty() ? 0.0 : ((double)dominadas / palabras.size());
    RECT barra = barraFondo;
    barra.right = barra.left + (int)((barraFondo.right - barraFondo.left) * proporcion);
    if (barra.right > barra.left) {
        DibujarRectRedondeado(hdc, barra, COLOR_VERDE, COLOR_VERDE, 12, 1);
    }

    RECT etiqueta = {x, 310, rc.right - 30, 338};
    DibujarTexto(hdc, L"¿Qué querés practicar hoy?", etiqueta, fuenteSubtitulo,
                 COLOR_TEXTO, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT ayuda = {x, 525, rc.right - 40, 570};
    DibujarTexto(hdc,
                 L"Consejo: el Repaso inteligente prioriza las palabras en las que más te equivocás.",
                 ayuda, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_WORDBREAK);
}

void DibujarPractica(HDC hdc, RECT rc) {
    int xCentro = 205 + (rc.right - 205) / 2;

    wstring nombreModo;
    if (modoActual == MODO_ARTICULOS) nombreModo = L"Artículos · der / die / das";
    else if (modoActual == MODO_VOCABULARIO) nombreModo = L"Vocabulario básico";
    else nombreModo = L"Repaso inteligente";

    RECT modo = {240, 48, rc.right - 30, 82};
    DibujarTexto(hdc, nombreModo, modo, fuenteSubtitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    wstring contador = L"Pregunta " + to_wstring(preguntaSesion) + L" de " + to_wstring(PREGUNTAS_POR_SESION);
    RECT cuenta = {240, 86, rc.right - 40, 116};
    DibujarTexto(hdc, contador, cuenta, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT barraFondo = {240, 126, rc.right - 55, 140};
    DibujarRectRedondeado(hdc, barraFondo, COLOR_GRIS, COLOR_GRIS, 10, 1);
    RECT barra = barraFondo;
    barra.right = barra.left + (int)((barraFondo.right - barraFondo.left) * (preguntaSesion / 10.0));
    DibujarRectRedondeado(hdc, barra, COLOR_VERDE, COLOR_VERDE, 10, 1);

    RECT tarjeta = {260, 176, rc.right - 70, 350};
    DibujarRectRedondeado(hdc, tarjeta, COLOR_BLANCO, COLOR_BORDE, 24, 1);

    if (preguntaEsArticulo) {
        RECT instruccion = {tarjeta.left + 20, tarjeta.top + 18, tarjeta.right - 20, tarjeta.top + 52};
        DibujarTexto(hdc, L"Elegí el artículo correcto", instruccion, fuenteNormal,
                     COLOR_TEXTO_SUAVE, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT palabra = {tarjeta.left + 20, tarjeta.top + 58, tarjeta.right - 20, tarjeta.top + 120};
        DibujarTexto(hdc, palabras[indicePalabraActual].aleman, palabra, fuenteGrande,
                     COLOR_TEXTO, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        wstring ayuda = L"Significado: " + palabras[indicePalabraActual].espanol;
        RECT significado = {tarjeta.left + 20, tarjeta.top + 120, tarjeta.right - 20, tarjeta.top + 154};
        DibujarTexto(hdc, ayuda, significado, fuenteNormal, COLOR_TEXTO_SUAVE,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else {
        RECT instruccion = {tarjeta.left + 20, tarjeta.top + 18, tarjeta.right - 20, tarjeta.top + 52};
        DibujarTexto(hdc, L"¿Qué significa esta palabra?", instruccion, fuenteNormal,
                     COLOR_TEXTO_SUAVE, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT palabra = {tarjeta.left + 20, tarjeta.top + 62, tarjeta.right - 20, tarjeta.top + 130};
        DibujarTexto(hdc, palabras[indicePalabraActual].aleman, palabra, fuenteGrande,
                     COLOR_TEXTO, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    if (respondida) {
        COLORREF color = (mensajeFeedback == L"¡Correcto!") ? COLOR_VERDE_OSCURO : COLOR_ROJO;
        RECT feed = {235, 455, rc.right - 45, 488};
        DibujarTexto(hdc, mensajeFeedback, feed, fuenteSubtitulo, color,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    (void)xCentro;
}

void DibujarProgreso(HDC hdc, RECT rc) {
    int x = 242;

    RECT titulo = {x, 54, rc.right - 40, 105};
    DibujarTexto(hdc, L"Tu progreso", titulo, fuenteTitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT subtitulo = {x, 104, rc.right - 40, 138};
    DibujarTexto(hdc, L"El progreso se guarda automáticamente al responder.", subtitulo,
                 fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    int ancho = (rc.right - x - 70) / 2;

    RECT card1 = {x, 175, x + ancho, 300};
    RECT card2 = {x + ancho + 25, 175, rc.right - 45, 300};
    RECT card3 = {x, 325, x + ancho, 450};
    RECT card4 = {x + ancho + 25, 325, rc.right - 45, 450};

    DibujarRectRedondeado(hdc, card1, COLOR_BLANCO, COLOR_BORDE, 20, 1);
    DibujarRectRedondeado(hdc, card2, COLOR_BLANCO, COLOR_BORDE, 20, 1);
    DibujarRectRedondeado(hdc, card3, COLOR_BLANCO, COLOR_BORDE, 20, 1);
    DibujarRectRedondeado(hdc, card4, COLOR_BLANCO, COLOR_BORDE, 20, 1);

    RECT a1 = {card1.left + 20, card1.top + 18, card1.right - 20, card1.top + 50};
    DibujarTexto(hdc, L"Respuestas", a1, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT a2 = {card1.left + 20, card1.top + 52, card1.right - 20, card1.bottom - 12};
    DibujarTexto(hdc, to_wstring(stats.total), a2, fuenteTitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT b1 = {card2.left + 20, card2.top + 18, card2.right - 20, card2.top + 50};
    DibujarTexto(hdc, L"Precisión", b1, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    int precisionRedondeada = (int)(PrecisionGlobal() + 0.5);
    RECT b2 = {card2.left + 20, card2.top + 52, card2.right - 20, card2.bottom - 12};
    DibujarTexto(hdc, to_wstring(precisionRedondeada) + L"%", b2, fuenteTitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT c1 = {card3.left + 20, card3.top + 18, card3.right - 20, card3.top + 50};
    DibujarTexto(hdc, L"Palabras dominadas", c1, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT c2 = {card3.left + 20, card3.top + 52, card3.right - 20, card3.bottom - 12};
    DibujarTexto(hdc, to_wstring(PalabrasDominadas()), c2, fuenteTitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT d1 = {card4.left + 20, card4.top + 18, card4.right - 20, card4.top + 50};
    DibujarTexto(hdc, L"Mejor racha", d1, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT d2 = {card4.left + 20, card4.top + 52, card4.right - 20, card4.bottom - 12};
    DibujarTexto(hdc, to_wstring(stats.mejorRacha), d2, fuenteTitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    if (preguntaSesion >= PREGUNTAS_POR_SESION && stats.total > 0) {
        wstring sesion = L"Última sesión: " + to_wstring(correctasSesion) + L" / " +
                          to_wstring(PREGUNTAS_POR_SESION) + L" correctas";
        RECT res = {x, 480, rc.right - 45, 525};
        DibujarTexto(hdc, sesion, res, fuenteSubtitulo, COLOR_VERDE_OSCURO,
                     DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
}

void DibujarBoton(const DRAWITEMSTRUCT* dis) {
    HWND hwndBoton = dis->hwndItem;
    int id = (int)GetWindowLongPtr(hwndBoton, GWLP_ID);

    wchar_t texto[256];
    GetWindowTextW(hwndBoton, texto, 256);

    RECT rc = dis->rcItem;
    bool presionado = (dis->itemState & ODS_SELECTED) != 0;
    bool deshabilitado = (dis->itemState & ODS_DISABLED) != 0;

    COLORREF fondo = COLOR_BLANCO;
    COLORREF borde = COLOR_BORDE;
    COLORREF textoColor = COLOR_TEXTO;

    // Navegacion
    bool navSeleccionado =
        (id == ID_NAV_INICIO && pantallaActual == PANTALLA_INICIO) ||
        (id == ID_NAV_PRACTICA && pantallaActual == PANTALLA_PRACTICA) ||
        (id == ID_NAV_PROGRESO && pantallaActual == PANTALLA_PROGRESO);

    if (id >= ID_NAV_INICIO && id <= ID_NAV_PROGRESO) {
        fondo = navSeleccionado ? COLOR_VERDE_CLARO : COLOR_BLANCO;
        borde = fondo;
        textoColor = navSeleccionado ? COLOR_VERDE_OSCURO : COLOR_TEXTO_SUAVE;
    }

    // Botones principales
    if (id == ID_HOME_REPASO || id == ID_SIGUIENTE) {
        fondo = presionado ? COLOR_VERDE_OSCURO : COLOR_VERDE;
        borde = fondo;
        textoColor = COLOR_BLANCO;
    }

    if (id == ID_HOME_ARTICULOS || id == ID_HOME_VOCAB) {
        fondo = presionado ? COLOR_AZUL_CLARO : COLOR_BLANCO;
        borde = COLOR_AZUL;
        textoColor = COLOR_AZUL;
    }

    // Respuestas
    if (id >= ID_RESPUESTA_1 && id <= ID_RESPUESTA_3) {
        fondo = presionado ? COLOR_AZUL_CLARO : COLOR_BLANCO;
        borde = COLOR_BORDE;

        if (respondida) {
            wstring txt = texto;
            if (txt == respuestaCorrecta) {
                fondo = COLOR_VERDE_CLARO;
                borde = COLOR_VERDE;
                textoColor = COLOR_VERDE_OSCURO;
            } else if (id == botonSeleccionado) {
                fondo = COLOR_ROJO_CLARO;
                borde = COLOR_ROJO;
                textoColor = COLOR_ROJO;
            }
        }
    }

    if (deshabilitado && !respondida) {
        fondo = COLOR_GRIS;
        borde = COLOR_BORDE;
        textoColor = COLOR_TEXTO_SUAVE;
    }

    HDC hdc = dis->hDC;
    HBRUSH brocha = CreateSolidBrush(COLOR_FONDO);
    FillRect(hdc, &rc, brocha);
    DeleteObject(brocha);

    RECT caja = rc;
    InflateRect(&caja, -2, -2);
    DibujarRectRedondeado(hdc, caja, fondo, borde, 18, 2);

    DibujarTexto(hdc, texto, caja, fuenteBoton, textoColor,
                 DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    if (dis->itemState & ODS_FOCUS) {
        RECT foco = caja;
        InflateRect(&foco, -5, -5);
        DrawFocusRect(hdc, &foco);
    }
}

LRESULT CALLBACK ProcedimientoVentana(HWND hwnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
    switch (mensaje) {
        case WM_CREATE: {
            ventanaPrincipal = hwnd;
            CrearFuentes();
            CargarProgreso();

            btnNavInicio = CrearBoton(hwnd, ID_NAV_INICIO, L"Inicio");
            btnNavPractica = CrearBoton(hwnd, ID_NAV_PRACTICA, L"Practicar");
            btnNavProgreso = CrearBoton(hwnd, ID_NAV_PROGRESO, L"Progreso");

            btnHomeRepaso = CrearBoton(hwnd, ID_HOME_REPASO, L"Repaso inteligente");
            btnHomeArticulos = CrearBoton(hwnd, ID_HOME_ARTICULOS, L"Artículos");
            btnHomeVocab = CrearBoton(hwnd, ID_HOME_VOCAB, L"Vocabulario");

            btnRespuesta[0] = CrearBoton(hwnd, ID_RESPUESTA_1, L"der");
            btnRespuesta[1] = CrearBoton(hwnd, ID_RESPUESTA_2, L"die");
            btnRespuesta[2] = CrearBoton(hwnd, ID_RESPUESTA_3, L"das");
            btnSiguiente = CrearBoton(hwnd, ID_SIGUIENTE, L"Siguiente");

            ActualizarControles();
            return 0;
        }

        case WM_SIZE:
            DistribuirControles(hwnd);
            return 0;

        case WM_COMMAND: {
            int id = LOWORD(wParam);

            switch (id) {
                case ID_NAV_INICIO:
                    pantallaActual = PANTALLA_INICIO;
                    ActualizarControles();
                    break;

                case ID_NAV_PRACTICA:
                    IniciarPractica(MODO_REPASO);
                    break;

                case ID_NAV_PROGRESO:
                    pantallaActual = PANTALLA_PROGRESO;
                    ActualizarControles();
                    break;

                case ID_HOME_REPASO:
                    IniciarPractica(MODO_REPASO);
                    break;

                case ID_HOME_ARTICULOS:
                    IniciarPractica(MODO_ARTICULOS);
                    break;

                case ID_HOME_VOCAB:
                    IniciarPractica(MODO_VOCABULARIO);
                    break;

                case ID_RESPUESTA_1:
                case ID_RESPUESTA_2:
                case ID_RESPUESTA_3:
                    RegistrarRespuesta(id);
                    break;

                case ID_SIGUIENTE:
                    SiguientePregunta();
                    break;
            }
            return 0;
        }

        case WM_DRAWITEM:
            DibujarBoton((DRAWITEMSTRUCT*)lParam);
            return TRUE;

        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;
            mmi->ptMinTrackSize.x = 850;
            mmi->ptMinTrackSize.y = 620;
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            HBRUSH fondo = CreateSolidBrush(COLOR_FONDO);
            FillRect(hdc, &rc, fondo);
            DeleteObject(fondo);

            DibujarSidebar(hdc, rc);

            if (pantallaActual == PANTALLA_INICIO) {
                DibujarInicio(hdc, rc);
            } else if (pantallaActual == PANTALLA_PRACTICA) {
                DibujarPractica(hdc, rc);
            } else {
                DibujarProgreso(hdc, rc);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            GuardarProgreso();

            if (fuenteTitulo) DeleteObject(fuenteTitulo);
            if (fuenteSubtitulo) DeleteObject(fuenteSubtitulo);
            if (fuenteNormal) DeleteObject(fuenteNormal);
            if (fuenteBoton) DeleteObject(fuenteBoton);
            if (fuenteGrande) DeleteObject(fuenteGrande);

            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, mensaje, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSEXW wc;
    ZeroMemory(&wc, sizeof(wc));

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ProcedimientoVentana;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.lpszClassName = CLASE_VENTANA;

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"No se pudo registrar la ventana.", L"DeutschCoach", MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowExW(
        0,
        CLASE_VENTANA,
        L"DeutschCoach · Alemán desde cero",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        980, 680,
        NULL, NULL,
        hInstance,
        NULL
    );

    if (!hwnd) {
        MessageBoxW(NULL, L"No se pudo crear la ventana principal.", L"DeutschCoach", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}
