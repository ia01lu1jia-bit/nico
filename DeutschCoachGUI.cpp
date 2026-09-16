#define UNICODE
#define _UNICODE

#include <windows.h>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <random>
#include <ctime>
#include <cwctype>
#include <sstream>

using namespace std;

// ============================================================
// DeutschCoach GUI v0.3
// Windows nativo - Embarcadero Dev-C++ 6.3 / MinGW
// Requiere enlazar con: -lgdi32 -mwindows
// ============================================================

struct Palabra {
    wstring singular;
    wstring espanol;
    wstring articulo;
    wstring plural;
    int correctas;
    int incorrectas;
};

struct Verbo {
    wstring infinitivo;
    wstring espanol;
    bool regular;
    wstring formas[3][6]; // [tiempo][pronombre]
    int correctas;
    int incorrectas;
};

struct Estadisticas {
    int total;
    int correctas;
    int incorrectas;
    int racha;
    int mejorRacha;
    int porModulo[5][2]; // [modulo][correctas/incorrectas]
};

enum Pantalla {
    PANTALLA_INICIO,
    PANTALLA_PRACTICA_OPCIONES,
    PANTALLA_PRACTICA_TEXTO,
    PANTALLA_CONFIG_VERBOS,
    PANTALLA_PROGRESO
};

enum ModoPractica {
    MODO_REPASO = 0,
    MODO_VOCABULARIO = 1,
    MODO_ARTICULOS = 2,
    MODO_PLURALES = 3,
    MODO_VERBOS = 4
};

enum TiempoVerbal {
    TIEMPO_PRESENTE = 0,
    TIEMPO_PRETERITO = 1,
    TIEMPO_PERFECTO = 2
};

enum FiltroVerbo {
    FILTRO_REGULARES,
    FILTRO_IRREGULARES,
    FILTRO_AMBOS
};

const wchar_t CLASE_VENTANA[] = L"DeutschCoachVentana";
const char* ARCHIVO_PROGRESO = "deutschcoach_progreso.txt";

const int ID_NAV_INICIO = 1001;
const int ID_NAV_ESTUDIAR = 1002;
const int ID_NAV_VERBOS = 1003;
const int ID_NAV_PROGRESO = 1004;

const int ID_HOME_REPASO = 1101;
const int ID_HOME_VOCAB = 1102;
const int ID_HOME_ARTICULOS = 1103;
const int ID_HOME_PLURALES = 1104;
const int ID_HOME_VERBOS = 1105;

const int ID_RESPUESTA_1 = 1201;
const int ID_RESPUESTA_2 = 1202;
const int ID_RESPUESTA_3 = 1203;
const int ID_SIGUIENTE = 1204;

const int ID_EDIT_RESPUESTA = 1301;
const int ID_COMPROBAR = 1302;
const int ID_CHAR_AE = 1303;
const int ID_CHAR_OE = 1304;
const int ID_CHAR_UE = 1305;
const int ID_CHAR_SS = 1306;

const int ID_TIEMPO_PRESENTE = 1401;
const int ID_TIEMPO_PRETERITO = 1402;
const int ID_TIEMPO_PERFECTO = 1403;
const int ID_FILTRO_REGULARES = 1411;
const int ID_FILTRO_IRREGULARES = 1412;
const int ID_FILTRO_AMBOS = 1413;
const int ID_INICIAR_VERBOS = 1420;

// Paleta
const COLORREF COLOR_FONDO = RGB(246, 248, 252);
const COLORREF COLOR_SIDEBAR = RGB(31, 41, 55);
const COLORREF COLOR_BLANCO = RGB(255, 255, 255);
const COLORREF COLOR_TEXTO = RGB(33, 40, 52);
const COLORREF COLOR_TEXTO_SUAVE = RGB(106, 115, 132);
const COLORREF COLOR_VERDE = RGB(74, 186, 92);
const COLORREF COLOR_VERDE_OSCURO = RGB(50, 143, 67);
const COLORREF COLOR_VERDE_CLARO = RGB(232, 248, 235);
const COLORREF COLOR_AZUL = RGB(73, 120, 231);
const COLORREF COLOR_AZUL_CLARO = RGB(235, 241, 255);
const COLORREF COLOR_VIOLETA = RGB(139, 92, 246);
const COLORREF COLOR_VIOLETA_CLARO = RGB(244, 239, 255);
const COLORREF COLOR_NARANJA = RGB(243, 156, 52);
const COLORREF COLOR_NARANJA_CLARO = RGB(255, 246, 233);
const COLORREF COLOR_ROJO = RGB(220, 78, 78);
const COLORREF COLOR_ROJO_CLARO = RGB(255, 238, 238);
const COLORREF COLOR_BORDE = RGB(220, 225, 234);
const COLORREF COLOR_GRIS = RGB(235, 239, 245);
const COLORREF COLOR_SIDEBAR_TXT = RGB(226, 232, 240);
const COLORREF COLOR_SIDEBAR_SEL = RGB(55, 65, 81);

HWND ventanaPrincipal = NULL;

HWND btnNavInicio = NULL;
HWND btnNavEstudiar = NULL;
HWND btnNavVerbos = NULL;
HWND btnNavProgreso = NULL;

HWND btnHomeRepaso = NULL;
HWND btnHomeVocab = NULL;
HWND btnHomeArticulos = NULL;
HWND btnHomePlurales = NULL;
HWND btnHomeVerbos = NULL;

HWND btnRespuesta[3] = {NULL, NULL, NULL};
HWND btnSiguiente = NULL;

HWND editRespuesta = NULL;
HWND btnComprobar = NULL;
HWND btnCharAE = NULL;
HWND btnCharOE = NULL;
HWND btnCharUE = NULL;
HWND btnCharSS = NULL;

HWND btnTiempoPresente = NULL;
HWND btnTiempoPreterito = NULL;
HWND btnTiempoPerfecto = NULL;
HWND btnFiltroRegulares = NULL;
HWND btnFiltroIrregulares = NULL;
HWND btnFiltroAmbos = NULL;
HWND btnIniciarVerbos = NULL;

HFONT fuenteLogo = NULL;
HFONT fuenteTitulo = NULL;
HFONT fuenteSubtitulo = NULL;
HFONT fuenteNormal = NULL;
HFONT fuentePequena = NULL;
HFONT fuenteBoton = NULL;
HFONT fuenteGrande = NULL;

Pantalla pantallaActual = PANTALLA_INICIO;
ModoPractica modoActual = MODO_REPASO;
TiempoVerbal tiempoSeleccionado = TIEMPO_PRESENTE;
FiltroVerbo filtroSeleccionado = FILTRO_AMBOS;
Estadisticas stats = {0, 0, 0, 0, 0, {{0}}};

vector<Palabra> palabras = {
    {L"Haus", L"casa", L"das", L"Häuser", 0, 0},
    {L"Buch", L"libro", L"das", L"Bücher", 0, 0},
    {L"Tisch", L"mesa", L"der", L"Tische", 0, 0},
    {L"Schule", L"escuela", L"die", L"Schulen", 0, 0},
    {L"Auto", L"auto", L"das", L"Autos", 0, 0},
    {L"Bahnhof", L"estación", L"der", L"Bahnhöfe", 0, 0},
    {L"Apfel", L"manzana", L"der", L"Äpfel", 0, 0},
    {L"Kind", L"niño/a", L"das", L"Kinder", 0, 0},
    {L"Freund", L"amigo", L"der", L"Freunde", 0, 0},
    {L"Stadt", L"ciudad", L"die", L"Städte", 0, 0},
    {L"Tag", L"día", L"der", L"Tage", 0, 0},
    {L"Nacht", L"noche", L"die", L"Nächte", 0, 0}
};

vector<Verbo> verbos = {
    {
        L"lernen", L"aprender", true,
        {
            {L"lerne", L"lernst", L"lernt", L"lernen", L"lernt", L"lernen"},
            {L"lernte", L"lerntest", L"lernte", L"lernten", L"lerntet", L"lernten"},
            {L"habe gelernt", L"hast gelernt", L"hat gelernt", L"haben gelernt", L"habt gelernt", L"haben gelernt"}
        },
        0, 0
    },
    {
        L"machen", L"hacer", true,
        {
            {L"mache", L"machst", L"macht", L"machen", L"macht", L"machen"},
            {L"machte", L"machtest", L"machte", L"machten", L"machtet", L"machten"},
            {L"habe gemacht", L"hast gemacht", L"hat gemacht", L"haben gemacht", L"habt gemacht", L"haben gemacht"}
        },
        0, 0
    },
    {
        L"spielen", L"jugar", true,
        {
            {L"spiele", L"spielst", L"spielt", L"spielen", L"spielt", L"spielen"},
            {L"spielte", L"spieltest", L"spielte", L"spielten", L"spieltet", L"spielten"},
            {L"habe gespielt", L"hast gespielt", L"hat gespielt", L"haben gespielt", L"habt gespielt", L"haben gespielt"}
        },
        0, 0
    },
    {
        L"wohnen", L"vivir / residir", true,
        {
            {L"wohne", L"wohnst", L"wohnt", L"wohnen", L"wohnt", L"wohnen"},
            {L"wohnte", L"wohntest", L"wohnte", L"wohnten", L"wohntet", L"wohnten"},
            {L"habe gewohnt", L"hast gewohnt", L"hat gewohnt", L"haben gewohnt", L"habt gewohnt", L"haben gewohnt"}
        },
        0, 0
    },
    {
        L"sein", L"ser / estar", false,
        {
            {L"bin", L"bist", L"ist", L"sind", L"seid", L"sind"},
            {L"war", L"warst", L"war", L"waren", L"wart", L"waren"},
            {L"bin gewesen", L"bist gewesen", L"ist gewesen", L"sind gewesen", L"seid gewesen", L"sind gewesen"}
        },
        0, 0
    },
    {
        L"haben", L"tener", false,
        {
            {L"habe", L"hast", L"hat", L"haben", L"habt", L"haben"},
            {L"hatte", L"hattest", L"hatte", L"hatten", L"hattet", L"hatten"},
            {L"habe gehabt", L"hast gehabt", L"hat gehabt", L"haben gehabt", L"habt gehabt", L"haben gehabt"}
        },
        0, 0
    },
    {
        L"gehen", L"ir", false,
        {
            {L"gehe", L"gehst", L"geht", L"gehen", L"geht", L"gehen"},
            {L"ging", L"gingst", L"ging", L"gingen", L"gingt", L"gingen"},
            {L"bin gegangen", L"bist gegangen", L"ist gegangen", L"sind gegangen", L"seid gegangen", L"sind gegangen"}
        },
        0, 0
    },
    {
        L"kommen", L"venir", false,
        {
            {L"komme", L"kommst", L"kommt", L"kommen", L"kommt", L"kommen"},
            {L"kam", L"kamst", L"kam", L"kamen", L"kamt", L"kamen"},
            {L"bin gekommen", L"bist gekommen", L"ist gekommen", L"sind gekommen", L"seid gekommen", L"sind gekommen"}
        },
        0, 0
    },
    {
        L"sehen", L"ver", false,
        {
            {L"sehe", L"siehst", L"sieht", L"sehen", L"seht", L"sehen"},
            {L"sah", L"sahst", L"sah", L"sahen", L"saht", L"sahen"},
            {L"habe gesehen", L"hast gesehen", L"hat gesehen", L"haben gesehen", L"habt gesehen", L"haben gesehen"}
        },
        0, 0
    },
    {
        L"sprechen", L"hablar", false,
        {
            {L"spreche", L"sprichst", L"spricht", L"sprechen", L"sprecht", L"sprechen"},
            {L"sprach", L"sprachst", L"sprach", L"sprachen", L"spracht", L"sprachen"},
            {L"habe gesprochen", L"hast gesprochen", L"hat gesprochen", L"haben gesprochen", L"habt gesprochen", L"haben gesprochen"}
        },
        0, 0
    }
};

const wchar_t* PRONOMBRES[6] = {
    L"ich", L"du", L"er / sie / es", L"wir", L"ihr", L"sie / Sie"
};

mt19937 generador(static_cast<unsigned int>(time(NULL)));

int indicePalabraActual = 0;
int indiceVerboActual = 0;
int indicePronombreActual = 0;
bool preguntaEsArticulo = true;
bool respondida = false;
int botonSeleccionado = 0;
wstring respuestaCorrecta;
wstring mensajeFeedback;
int preguntaSesion = 0;
int correctasSesion = 0;
const int PREGUNTAS_POR_SESION = 10;

wstring Normalizar(const wstring& texto) {
    wstring salida;
    bool espacioPendiente = false;

    for (size_t i = 0; i < texto.size(); ++i) {
        wchar_t c = texto[i];
        if (iswspace(c)) {
            if (!salida.empty()) espacioPendiente = true;
        } else {
            if (espacioPendiente && !salida.empty()) salida += L' ';
            salida += (wchar_t)towlower(c);
            espacioPendiente = false;
        }
    }
    return salida;
}

void GuardarProgreso() {
    ofstream archivo(ARCHIVO_PROGRESO);
    if (!archivo) return;

    archivo << "DEUTSCHCOACH_V3\n";
    archivo << stats.total << ' '
            << stats.correctas << ' '
            << stats.incorrectas << ' '
            << stats.racha << ' '
            << stats.mejorRacha << '\n';

    for (int m = 0; m < 5; ++m) {
        archivo << stats.porModulo[m][0] << ' ' << stats.porModulo[m][1] << '\n';
    }

    archivo << palabras.size() << '\n';
    for (size_t i = 0; i < palabras.size(); ++i) {
        archivo << palabras[i].correctas << ' ' << palabras[i].incorrectas << '\n';
    }

    archivo << verbos.size() << '\n';
    for (size_t i = 0; i < verbos.size(); ++i) {
        archivo << verbos[i].correctas << ' ' << verbos[i].incorrectas << '\n';
    }
}

void CargarProgreso() {
    ifstream archivo(ARCHIVO_PROGRESO);
    if (!archivo) return;

    string cabecera;
    getline(archivo, cabecera);

    if (cabecera != "DEUTSCHCOACH_V3") {
        return;
    }

    archivo >> stats.total
            >> stats.correctas
            >> stats.incorrectas
            >> stats.racha
            >> stats.mejorRacha;

    for (int m = 0; m < 5; ++m) {
        archivo >> stats.porModulo[m][0] >> stats.porModulo[m][1];
    }

    size_t cantidadPalabras = 0;
    archivo >> cantidadPalabras;
    for (size_t i = 0; i < min(cantidadPalabras, palabras.size()); ++i) {
        archivo >> palabras[i].correctas >> palabras[i].incorrectas;
    }

    size_t cantidadVerbos = 0;
    archivo >> cantidadVerbos;
    for (size_t i = 0; i < min(cantidadVerbos, verbos.size()); ++i) {
        archivo >> verbos[i].correctas >> verbos[i].incorrectas;
    }
}

double PrecisionGlobal() {
    if (stats.total == 0) return 0.0;
    return (100.0 * stats.correctas) / stats.total;
}

int PalabrasDominadas() {
    int dominadas = 0;
    for (size_t i = 0; i < palabras.size(); ++i) {
        int total = palabras[i].correctas + palabras[i].incorrectas;
        if (total >= 3 && palabras[i].correctas * 100 / total >= 75) dominadas++;
    }
    return dominadas;
}

int VerbosPracticados() {
    int cantidad = 0;
    for (size_t i = 0; i < verbos.size(); ++i) {
        if (verbos[i].correctas + verbos[i].incorrectas > 0) cantidad++;
    }
    return cantidad;
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
    fuenteLogo = CreateFontW(-25, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    fuenteTitulo = CreateFontW(-34, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    fuenteSubtitulo = CreateFontW(-22, 0, 0, 0, 600, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                  CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    fuenteNormal = CreateFontW(-18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    fuentePequena = CreateFontW(-15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    fuenteBoton = CreateFontW(-17, 0, 0, 0, 600, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

    fuenteGrande = CreateFontW(-46, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
}

HWND CrearBoton(HWND padre, int id, const wchar_t* texto) {
    return CreateWindowExW(
        0, L"BUTTON", texto,
        WS_CHILD | WS_TABSTOP | BS_OWNERDRAW,
        0, 0, 100, 40,
        padre, (HMENU)(INT_PTR)id, GetModuleHandleW(NULL), NULL
    );
}

HWND CrearEdit(HWND padre) {
    HWND control = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL,
        0, 0, 100, 40,
        padre, (HMENU)(INT_PTR)ID_EDIT_RESPUESTA,
        GetModuleHandleW(NULL), NULL
    );
    SendMessageW(control, WM_SETFONT, (WPARAM)fuenteSubtitulo, TRUE);
    return control;
}

void MostrarControl(HWND control, bool mostrar) {
    if (!control) return;
    ShowWindow(control, mostrar ? SW_SHOW : SW_HIDE);
}

bool EsPantallaPractica() {
    return pantallaActual == PANTALLA_PRACTICA_OPCIONES ||
           pantallaActual == PANTALLA_PRACTICA_TEXTO;
}

void ActualizarControles() {
    bool inicio = pantallaActual == PANTALLA_INICIO;
    bool opciones = pantallaActual == PANTALLA_PRACTICA_OPCIONES;
    bool texto = pantallaActual == PANTALLA_PRACTICA_TEXTO;
    bool configVerbos = pantallaActual == PANTALLA_CONFIG_VERBOS;

    MostrarControl(btnNavInicio, true);
    MostrarControl(btnNavEstudiar, true);
    MostrarControl(btnNavVerbos, true);
    MostrarControl(btnNavProgreso, true);

    MostrarControl(btnHomeRepaso, inicio);
    MostrarControl(btnHomeVocab, inicio);
    MostrarControl(btnHomeArticulos, inicio);
    MostrarControl(btnHomePlurales, inicio);
    MostrarControl(btnHomeVerbos, inicio);

    for (int i = 0; i < 3; ++i) MostrarControl(btnRespuesta[i], opciones);
    MostrarControl(btnSiguiente, (opciones || texto) && respondida);

    MostrarControl(editRespuesta, texto);
    MostrarControl(btnComprobar, texto && !respondida);
    MostrarControl(btnCharAE, texto);
    MostrarControl(btnCharOE, texto);
    MostrarControl(btnCharUE, texto);
    MostrarControl(btnCharSS, texto);

    MostrarControl(btnTiempoPresente, configVerbos);
    MostrarControl(btnTiempoPreterito, configVerbos);
    MostrarControl(btnTiempoPerfecto, configVerbos);
    MostrarControl(btnFiltroRegulares, configVerbos);
    MostrarControl(btnFiltroIrregulares, configVerbos);
    MostrarControl(btnFiltroAmbos, configVerbos);
    MostrarControl(btnIniciarVerbos, configVerbos);

    if (texto && !respondida) {
        EnableWindow(editRespuesta, TRUE);
        SetFocus(editRespuesta);
    }

    InvalidateRect(ventanaPrincipal, NULL, TRUE);
}

void DistribuirControles(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);

    const int sidebar = 220;
    const int margen = 28;
    int anchoContenido = rc.right - sidebar;
    int x = sidebar + margen;

    MoveWindow(btnNavInicio, 22, 105, 176, 46, TRUE);
    MoveWindow(btnNavEstudiar, 22, 160, 176, 46, TRUE);
    MoveWindow(btnNavVerbos, 22, 215, 176, 46, TRUE);
    MoveWindow(btnNavProgreso, 22, 270, 176, 46, TRUE);

    int cardGap = 18;
    int cardW = (anchoContenido - margen * 2 - cardGap) / 2;
    if (cardW < 240) cardW = 240;
    int y1 = 315;
    int y2 = 398;
    int y3 = 481;

    MoveWindow(btnHomeRepaso, x, y1, cardW, 64, TRUE);
    MoveWindow(btnHomeVocab, x + cardW + cardGap, y1, cardW, 64, TRUE);
    MoveWindow(btnHomeArticulos, x, y2, cardW, 64, TRUE);
    MoveWindow(btnHomePlurales, x + cardW + cardGap, y2, cardW, 64, TRUE);
    MoveWindow(btnHomeVerbos, x, y3, cardW * 2 + cardGap, 64, TRUE);

    int centroX = sidebar + anchoContenido / 2;
    int anchoResp = 180;
    int gap = 18;
    int total = anchoResp * 3 + gap * 2;
    int inicioX = centroX - total / 2;

    for (int i = 0; i < 3; ++i) {
        MoveWindow(btnRespuesta[i], inicioX + i * (anchoResp + gap), 415, anchoResp, 60, TRUE);
    }

    MoveWindow(editRespuesta, centroX - 250, 405, 500, 48, TRUE);
    MoveWindow(btnCharAE, centroX - 118, 466, 52, 42, TRUE);
    MoveWindow(btnCharOE, centroX - 59, 466, 52, 42, TRUE);
    MoveWindow(btnCharUE, centroX, 466, 52, 42, TRUE);
    MoveWindow(btnCharSS, centroX + 59, 466, 52, 42, TRUE);
    MoveWindow(btnComprobar, centroX - 115, 525, 230, 54, TRUE);

    MoveWindow(btnSiguiente, centroX - 115, 525, 230, 54, TRUE);

    int cfgW = min(260, (anchoContenido - margen * 2 - 36) / 3);
    int cfgStart = x;
    MoveWindow(btnTiempoPresente, cfgStart, 240, cfgW, 54, TRUE);
    MoveWindow(btnTiempoPreterito, cfgStart + cfgW + 18, 240, cfgW, 54, TRUE);
    MoveWindow(btnTiempoPerfecto, cfgStart + (cfgW + 18) * 2, 240, cfgW, 54, TRUE);

    MoveWindow(btnFiltroRegulares, cfgStart, 365, cfgW, 54, TRUE);
    MoveWindow(btnFiltroIrregulares, cfgStart + cfgW + 18, 365, cfgW, 54, TRUE);
    MoveWindow(btnFiltroAmbos, cfgStart + (cfgW + 18) * 2, 365, cfgW, 54, TRUE);

    MoveWindow(btnIniciarVerbos, centroX - 150, 505, 300, 58, TRUE);
}

int ElegirPalabraAleatoria() {
    uniform_int_distribution<int> dist(0, (int)palabras.size() - 1);
    int elegido = dist(generador);
    if (palabras.size() > 1 && elegido == indicePalabraActual) {
        elegido = (elegido + 1) % (int)palabras.size();
    }
    return elegido;
}

int ElegirPalabraDebil() {
    vector<pair<int, int> > ranking;

    for (size_t i = 0; i < palabras.size(); ++i) {
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

vector<int> VerbosFiltrados() {
    vector<int> indices;
    for (size_t i = 0; i < verbos.size(); ++i) {
        if (filtroSeleccionado == FILTRO_AMBOS ||
            (filtroSeleccionado == FILTRO_REGULARES && verbos[i].regular) ||
            (filtroSeleccionado == FILTRO_IRREGULARES && !verbos[i].regular)) {
            indices.push_back((int)i);
        }
    }
    return indices;
}

void PrepararPreguntaOpciones() {
    respondida = false;
    botonSeleccionado = 0;
    mensajeFeedback = L"";

    for (int i = 0; i < 3; ++i) EnableWindow(btnRespuesta[i], TRUE);

    if (modoActual == MODO_REPASO) {
        indicePalabraActual = ElegirPalabraDebil();
        uniform_int_distribution<int> tipo(0, 1);
        preguntaEsArticulo = (tipo(generador) == 0);
    } else {
        indicePalabraActual = ElegirPalabraAleatoria();
        preguntaEsArticulo = (modoActual == MODO_ARTICULOS);
    }

    if (preguntaEsArticulo) {
        SetWindowTextW(btnRespuesta[0], L"der");
        SetWindowTextW(btnRespuesta[1], L"die");
        SetWindowTextW(btnRespuesta[2], L"das");
        respuestaCorrecta = palabras[indicePalabraActual].articulo;
    } else {
        vector<wstring> opciones = OpcionesVocabulario(indicePalabraActual);
        for (int i = 0; i < 3; ++i) {
            SetWindowTextW(btnRespuesta[i], opciones[i].c_str());
        }
        respuestaCorrecta = palabras[indicePalabraActual].espanol;
    }
}

void PrepararPreguntaTexto() {
    respondida = false;
    mensajeFeedback = L"";
    SetWindowTextW(editRespuesta, L"");
    EnableWindow(editRespuesta, TRUE);

    if (modoActual == MODO_PLURALES) {
        indicePalabraActual = ElegirPalabraAleatoria();
        respuestaCorrecta = palabras[indicePalabraActual].plural;
    } else if (modoActual == MODO_VERBOS) {
        vector<int> candidatos = VerbosFiltrados();
        if (candidatos.empty()) return;

        uniform_int_distribution<int> verboDist(0, (int)candidatos.size() - 1);
        uniform_int_distribution<int> pronDist(0, 5);

        indiceVerboActual = candidatos[verboDist(generador)];
        indicePronombreActual = pronDist(generador);
        respuestaCorrecta = verbos[indiceVerboActual].formas[(int)tiempoSeleccionado][indicePronombreActual];
    }
}

void IniciarPractica(ModoPractica modo) {
    modoActual = modo;
    preguntaSesion = 1;
    correctasSesion = 0;

    if (modo == MODO_PLURALES || modo == MODO_VERBOS) {
        pantallaActual = PANTALLA_PRACTICA_TEXTO;
        PrepararPreguntaTexto();
    } else {
        pantallaActual = PANTALLA_PRACTICA_OPCIONES;
        PrepararPreguntaOpciones();
    }

    ActualizarControles();
}

void RegistrarResultado(bool correcta) {
    stats.total++;
    if (correcta) {
        stats.correctas++;
        stats.racha++;
        correctasSesion++;
        stats.porModulo[(int)modoActual][0]++;
        if (stats.racha > stats.mejorRacha) stats.mejorRacha = stats.racha;
    } else {
        stats.incorrectas++;
        stats.racha = 0;
        stats.porModulo[(int)modoActual][1]++;
    }

    if (modoActual == MODO_VERBOS) {
        if (correcta) verbos[indiceVerboActual].correctas++;
        else verbos[indiceVerboActual].incorrectas++;
    } else {
        if (correcta) palabras[indicePalabraActual].correctas++;
        else palabras[indicePalabraActual].incorrectas++;
    }

    GuardarProgreso();
}

void RegistrarRespuestaOpcion(int idBoton) {
    if (respondida) return;

    wchar_t texto[256];
    GetWindowTextW(GetDlgItem(ventanaPrincipal, idBoton), texto, 256);
    bool correcta = (wstring(texto) == respuestaCorrecta);

    respondida = true;
    botonSeleccionado = idBoton;
    mensajeFeedback = correcta
        ? L"¡Correcto! Sehr gut."
        : L"Respuesta correcta: " + respuestaCorrecta;

    RegistrarResultado(correcta);

    for (int i = 0; i < 3; ++i) EnableWindow(btnRespuesta[i], FALSE);
    ActualizarControles();
}

void RegistrarRespuestaTexto() {
    if (respondida) return;

    wchar_t buffer[512];
    GetWindowTextW(editRespuesta, buffer, 512);
    wstring respuesta = Normalizar(buffer);
    wstring correctaNormalizada = Normalizar(respuestaCorrecta);

    bool correcta = (respuesta == correctaNormalizada);

    respondida = true;
    mensajeFeedback = correcta
        ? L"¡Correcto! Sehr gut."
        : L"Respuesta correcta: " + respuestaCorrecta;

    RegistrarResultado(correcta);

    EnableWindow(editRespuesta, FALSE);
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
    if (modoActual == MODO_PLURALES || modoActual == MODO_VERBOS) {
        PrepararPreguntaTexto();
    } else {
        PrepararPreguntaOpciones();
    }
    ActualizarControles();
}

void InsertarCaracter(wchar_t c) {
    if (!editRespuesta || !IsWindowVisible(editRespuesta) || respondida) return;

    DWORD inicio = 0, fin = 0;
    SendMessageW(editRespuesta, EM_GETSEL, (WPARAM)&inicio, (LPARAM)&fin);

    wchar_t texto[512];
    GetWindowTextW(editRespuesta, texto, 512);
    wstring actual = texto;

    actual.replace(inicio, fin - inicio, 1, c);
    SetWindowTextW(editRespuesta, actual.c_str());

    int nuevaPos = (int)inicio + 1;
    SendMessageW(editRespuesta, EM_SETSEL, nuevaPos, nuevaPos);
    SetFocus(editRespuesta);
}

wstring NombreTiempo() {
    if (tiempoSeleccionado == TIEMPO_PRESENTE) return L"Präsens";
    if (tiempoSeleccionado == TIEMPO_PRETERITO) return L"Präteritum";
    return L"Perfekt";
}

wstring NombreFiltro() {
    if (filtroSeleccionado == FILTRO_REGULARES) return L"Solo regulares";
    if (filtroSeleccionado == FILTRO_IRREGULARES) return L"Solo irregulares";
    return L"Regulares + irregulares";
}

void DibujarSidebar(HDC hdc, RECT rc) {
    RECT lateral = {0, 0, 220, rc.bottom};
    HBRUSH brocha = CreateSolidBrush(COLOR_SIDEBAR);
    FillRect(hdc, &lateral, brocha);
    DeleteObject(brocha);

    RECT logo = {22, 25, 198, 60};
    DibujarTexto(hdc, L"DEUTSCHCOACH", logo, fuenteLogo, COLOR_BLANCO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT nivel = {22, 63, 198, 87};
    DibujarTexto(hdc, L"Alemán desde cero · A1", nivel, fuentePequena,
                 RGB(156, 163, 175), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT footer = {22, rc.bottom - 70, 195, rc.bottom - 22};
    DibujarTexto(hdc, L"Aprendé con práctica corta,\nprogreso real y repaso.", footer,
                 fuentePequena, RGB(156, 163, 175), DT_LEFT | DT_WORDBREAK);
}

void DibujarInicio(HDC hdc, RECT rc) {
    int x = 252;

    RECT titulo = {x, 42, rc.right - 40, 86};
    DibujarTexto(hdc, L"Guten Tag", titulo, fuenteTitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT sub = {x, 88, rc.right - 40, 118};
    DibujarTexto(hdc, L"Elegí una práctica. Las sesiones son cortas y se guardan automáticamente.",
                 sub, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT hero = {x, 145, rc.right - 42, 280};
    DibujarRectRedondeado(hdc, hero, COLOR_VERDE_CLARO, RGB(205, 236, 211), 24, 1);

    RECT h1 = {hero.left + 24, hero.top + 18, hero.right - 24, hero.top + 50};
    DibujarTexto(hdc, L"Tu avance", h1, fuenteSubtitulo, COLOR_VERDE_OSCURO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    int precision = (int)(PrecisionGlobal() + 0.5);
    wstring resumen = L"Precisión " + to_wstring(precision) + L"%   •   Racha " +
                      to_wstring(stats.racha) + L"   •   " +
                      to_wstring(PalabrasDominadas()) + L" palabras dominadas";
    RECT h2 = {hero.left + 24, hero.top + 55, hero.right - 24, hero.top + 88};
    DibujarTexto(hdc, resumen, h2, fuenteNormal, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT barraFondo = {hero.left + 24, hero.top + 98, hero.right - 24, hero.top + 112};
    DibujarRectRedondeado(hdc, barraFondo, COLOR_BLANCO, COLOR_BLANCO, 10, 1);
    RECT barra = barraFondo;
    barra.right = barra.left + (int)((barraFondo.right - barraFondo.left) * (precision / 100.0));
    if (barra.right > barra.left) DibujarRectRedondeado(hdc, barra, COLOR_VERDE, COLOR_VERDE, 10, 1);

    RECT etiqueta = {x, 287, rc.right - 40, 313};
    DibujarTexto(hdc, L"¿Qué querés practicar hoy?", etiqueta, fuenteSubtitulo,
                 COLOR_TEXTO, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void DibujarCabeceraPractica(HDC hdc, RECT rc, const wstring& titulo) {
    RECT modo = {250, 40, rc.right - 40, 75};
    DibujarTexto(hdc, titulo, modo, fuenteSubtitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    wstring contador = L"Pregunta " + to_wstring(preguntaSesion) +
                       L" de " + to_wstring(PREGUNTAS_POR_SESION);
    RECT cuenta = {250, 78, rc.right - 40, 104};
    DibujarTexto(hdc, contador, cuenta, fuentePequena, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT barraFondo = {250, 116, rc.right - 55, 130};
    DibujarRectRedondeado(hdc, barraFondo, COLOR_GRIS, COLOR_GRIS, 10, 1);
    RECT barra = barraFondo;
    barra.right = barra.left + (int)((barraFondo.right - barraFondo.left) *
                                     (preguntaSesion / (double)PREGUNTAS_POR_SESION));
    DibujarRectRedondeado(hdc, barra, COLOR_VERDE, COLOR_VERDE, 10, 1);
}

void DibujarPracticaOpciones(HDC hdc, RECT rc) {
    wstring titulo;
    if (modoActual == MODO_ARTICULOS) titulo = L"Artículos · der / die / das";
    else if (modoActual == MODO_VOCABULARIO) titulo = L"Vocabulario básico";
    else titulo = L"Repaso inteligente";

    DibujarCabeceraPractica(hdc, rc, titulo);

    RECT tarjeta = {275, 165, rc.right - 75, 365};
    DibujarRectRedondeado(hdc, tarjeta, COLOR_BLANCO, COLOR_BORDE, 24, 1);

    if (preguntaEsArticulo) {
        RECT ins = {tarjeta.left + 20, tarjeta.top + 20, tarjeta.right - 20, tarjeta.top + 52};
        DibujarTexto(hdc, L"Elegí el artículo correcto", ins, fuenteNormal,
                     COLOR_TEXTO_SUAVE, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT palabra = {tarjeta.left + 20, tarjeta.top + 60, tarjeta.right - 20, tarjeta.top + 128};
        DibujarTexto(hdc, palabras[indicePalabraActual].singular, palabra, fuenteGrande,
                     COLOR_TEXTO, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT sig = {tarjeta.left + 20, tarjeta.top + 132, tarjeta.right - 20, tarjeta.bottom - 18};
        DibujarTexto(hdc, L"Significado: " + palabras[indicePalabraActual].espanol, sig,
                     fuenteNormal, COLOR_TEXTO_SUAVE,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else {
        RECT ins = {tarjeta.left + 20, tarjeta.top + 20, tarjeta.right - 20, tarjeta.top + 52};
        DibujarTexto(hdc, L"¿Qué significa esta palabra?", ins, fuenteNormal,
                     COLOR_TEXTO_SUAVE, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT palabra = {tarjeta.left + 20, tarjeta.top + 62, tarjeta.right - 20, tarjeta.top + 136};
        DibujarTexto(hdc, palabras[indicePalabraActual].singular, palabra, fuenteGrande,
                     COLOR_TEXTO, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    if (respondida) {
        COLORREF color = mensajeFeedback.find(L"¡Correcto!") == 0 ? COLOR_VERDE_OSCURO : COLOR_ROJO;
        RECT feed = {250, 482, rc.right - 55, 518};
        DibujarTexto(hdc, mensajeFeedback, feed, fuenteSubtitulo, color,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void DibujarPracticaTexto(HDC hdc, RECT rc) {
    wstring titulo = (modoActual == MODO_PLURALES)
        ? L"Plurales · respuesta escrita"
        : L"Verbos · " + NombreTiempo();

    DibujarCabeceraPractica(hdc, rc, titulo);

    RECT tarjeta = {275, 160, rc.right - 75, 365};
    DibujarRectRedondeado(hdc, tarjeta, COLOR_BLANCO, COLOR_BORDE, 24, 1);

    if (modoActual == MODO_PLURALES) {
        RECT ins = {tarjeta.left + 20, tarjeta.top + 20, tarjeta.right - 20, tarjeta.top + 50};
        DibujarTexto(hdc, L"Escribí el plural sin artículo", ins, fuenteNormal,
                     COLOR_TEXTO_SUAVE, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        wstring singular = palabras[indicePalabraActual].articulo + L" " +
                           palabras[indicePalabraActual].singular;
        RECT palabra = {tarjeta.left + 20, tarjeta.top + 58, tarjeta.right - 20, tarjeta.top + 120};
        DibujarTexto(hdc, singular, palabra, fuenteGrande,
                     COLOR_TEXTO, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT sig = {tarjeta.left + 20, tarjeta.top + 125, tarjeta.right - 20, tarjeta.bottom - 18};
        DibujarTexto(hdc, L"Significado: " + palabras[indicePalabraActual].espanol, sig,
                     fuenteNormal, COLOR_TEXTO_SUAVE,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else {
        const Verbo& v = verbos[indiceVerboActual];

        RECT ins = {tarjeta.left + 20, tarjeta.top + 18, tarjeta.right - 20, tarjeta.top + 48};
        DibujarTexto(hdc, L"Escribí solo la forma verbal", ins, fuenteNormal,
                     COLOR_TEXTO_SUAVE, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT verbo = {tarjeta.left + 20, tarjeta.top + 52, tarjeta.right - 20, tarjeta.top + 108};
        DibujarTexto(hdc, v.infinitivo, verbo, fuenteGrande, COLOR_TEXTO,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        wstring detalle = wstring(PRONOMBRES[indicePronombreActual]) +
                          L"   •   " + NombreTiempo() +
                          L"   •   " + (v.regular ? L"regular" : L"irregular");
        RECT d1 = {tarjeta.left + 20, tarjeta.top + 112, tarjeta.right - 20, tarjeta.top + 144};
        DibujarTexto(hdc, detalle, d1, fuenteSubtitulo, COLOR_VIOLETA,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT d2 = {tarjeta.left + 20, tarjeta.top + 148, tarjeta.right - 20, tarjeta.bottom - 14};
        DibujarTexto(hdc, L"Significado: " + v.espanol, d2, fuenteNormal,
                     COLOR_TEXTO_SUAVE, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    RECT ayuda = {280, 370, rc.right - 80, 397};
    DibujarTexto(hdc, L"Podés usar los botones ä ö ü ß si tu teclado no los tiene.",
                 ayuda, fuentePequena, COLOR_TEXTO_SUAVE,
                 DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    if (respondida) {
        COLORREF color = mensajeFeedback.find(L"¡Correcto!") == 0 ? COLOR_VERDE_OSCURO : COLOR_ROJO;
        RECT feed = {250, 470, rc.right - 55, 515};
        DibujarTexto(hdc, mensajeFeedback, feed, fuenteSubtitulo, color,
                     DT_CENTER | DT_WORDBREAK);
    }
}

void DibujarConfigVerbos(HDC hdc, RECT rc) {
    int x = 252;

    RECT titulo = {x, 42, rc.right - 40, 86};
    DibujarTexto(hdc, L"Práctica de verbos", titulo, fuenteTitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT sub = {x, 88, rc.right - 40, 128};
    DibujarTexto(hdc, L"Configurá la sesión antes de empezar. Después respondés escribiendo.",
                 sub, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT t1 = {x, 175, rc.right - 40, 212};
    DibujarTexto(hdc, L"1. Elegí el tiempo verbal", t1, fuenteSubtitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT t2 = {x, 300, rc.right - 40, 337};
    DibujarTexto(hdc, L"2. Elegí qué tipo de verbos", t2, fuenteSubtitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    wstring resumen = L"Sesión: " + NombreTiempo() + L" · " + NombreFiltro() +
                      L" · 10 preguntas";
    RECT box = {x, 448, rc.right - 45, 492};
    DibujarRectRedondeado(hdc, box, COLOR_VIOLETA_CLARO, RGB(221, 209, 255), 16, 1);
    DibujarTexto(hdc, resumen, box, fuenteNormal, COLOR_VIOLETA,
                 DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void DibujarProgreso(HDC hdc, RECT rc) {
    int x = 252;

    RECT titulo = {x, 42, rc.right - 40, 86};
    DibujarTexto(hdc, L"Tu progreso", titulo, fuenteTitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT sub = {x, 88, rc.right - 40, 118};
    DibujarTexto(hdc, L"Todo se guarda automáticamente mientras practicás.",
                 sub, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    int gap = 18;
    int ancho = (rc.right - x - 45 - gap) / 2;

    RECT c1 = {x, 155, x + ancho, 265};
    RECT c2 = {x + ancho + gap, 155, rc.right - 45, 265};
    RECT c3 = {x, 285, x + ancho, 395};
    RECT c4 = {x + ancho + gap, 285, rc.right - 45, 395};

    DibujarRectRedondeado(hdc, c1, COLOR_BLANCO, COLOR_BORDE, 20, 1);
    DibujarRectRedondeado(hdc, c2, COLOR_BLANCO, COLOR_BORDE, 20, 1);
    DibujarRectRedondeado(hdc, c3, COLOR_BLANCO, COLOR_BORDE, 20, 1);
    DibujarRectRedondeado(hdc, c4, COLOR_BLANCO, COLOR_BORDE, 20, 1);

    RECT a = {c1.left + 18, c1.top + 14, c1.right - 18, c1.top + 43};
    DibujarTexto(hdc, L"Respuestas totales", a, fuentePequena, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT av = {c1.left + 18, c1.top + 46, c1.right - 18, c1.bottom - 10};
    DibujarTexto(hdc, to_wstring(stats.total), av, fuenteTitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT b = {c2.left + 18, c2.top + 14, c2.right - 18, c2.top + 43};
    DibujarTexto(hdc, L"Precisión", b, fuentePequena, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT bv = {c2.left + 18, c2.top + 46, c2.right - 18, c2.bottom - 10};
    DibujarTexto(hdc, to_wstring((int)(PrecisionGlobal() + 0.5)) + L"%", bv,
                 fuenteTitulo, COLOR_VERDE_OSCURO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT c = {c3.left + 18, c3.top + 14, c3.right - 18, c3.top + 43};
    DibujarTexto(hdc, L"Palabras dominadas", c, fuentePequena, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT cv = {c3.left + 18, c3.top + 46, c3.right - 18, c3.bottom - 10};
    DibujarTexto(hdc, to_wstring(PalabrasDominadas()), cv, fuenteTitulo, COLOR_AZUL,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT d = {c4.left + 18, c4.top + 14, c4.right - 18, c4.top + 43};
    DibujarTexto(hdc, L"Verbos practicados", d, fuentePequena, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT dv = {c4.left + 18, c4.top + 46, c4.right - 18, c4.bottom - 10};
    DibujarTexto(hdc, to_wstring(VerbosPracticados()), dv, fuenteTitulo, COLOR_VIOLETA,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT resumen = {x, 430, rc.right - 45, 545};
    DibujarRectRedondeado(hdc, resumen, COLOR_BLANCO, COLOR_BORDE, 20, 1);

    wstring r1 = L"Racha actual: " + to_wstring(stats.racha) +
                 L"   •   Mejor racha: " + to_wstring(stats.mejorRacha);
    RECT rr1 = {resumen.left + 22, resumen.top + 18, resumen.right - 22, resumen.top + 52};
    DibujarTexto(hdc, r1, rr1, fuenteSubtitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    int vOk = stats.porModulo[MODO_VERBOS][0];
    int vBad = stats.porModulo[MODO_VERBOS][1];
    int pOk = stats.porModulo[MODO_PLURALES][0];
    int pBad = stats.porModulo[MODO_PLURALES][1];

    wstring r2 = L"Verbos: " + to_wstring(vOk) + L" correctas / " +
                 to_wstring(vBad) + L" errores   •   Plurales: " +
                 to_wstring(pOk) + L" correctas / " + to_wstring(pBad) + L" errores";
    RECT rr2 = {resumen.left + 22, resumen.top + 60, resumen.right - 22, resumen.bottom - 16};
    DibujarTexto(hdc, r2, rr2, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_WORDBREAK);
}

bool BotonSeleccionadoPorEstado(int id) {
    if (id == ID_TIEMPO_PRESENTE) return tiempoSeleccionado == TIEMPO_PRESENTE;
    if (id == ID_TIEMPO_PRETERITO) return tiempoSeleccionado == TIEMPO_PRETERITO;
    if (id == ID_TIEMPO_PERFECTO) return tiempoSeleccionado == TIEMPO_PERFECTO;
    if (id == ID_FILTRO_REGULARES) return filtroSeleccionado == FILTRO_REGULARES;
    if (id == ID_FILTRO_IRREGULARES) return filtroSeleccionado == FILTRO_IRREGULARES;
    if (id == ID_FILTRO_AMBOS) return filtroSeleccionado == FILTRO_AMBOS;
    return false;
}

void DibujarBoton(const DRAWITEMSTRUCT* dis) {
    int id = (int)GetWindowLongPtr(dis->hwndItem, GWLP_ID);

    wchar_t texto[256];
    GetWindowTextW(dis->hwndItem, texto, 256);

    RECT rc = dis->rcItem;
    bool presionado = (dis->itemState & ODS_SELECTED) != 0;
    bool deshabilitado = (dis->itemState & ODS_DISABLED) != 0;

    COLORREF fondo = COLOR_BLANCO;
    COLORREF borde = COLOR_BORDE;
    COLORREF textoColor = COLOR_TEXTO;
    int radio = 18;

    bool navSeleccionado =
        (id == ID_NAV_INICIO && pantallaActual == PANTALLA_INICIO) ||
        (id == ID_NAV_ESTUDIAR && EsPantallaPractica()) ||
        (id == ID_NAV_VERBOS && pantallaActual == PANTALLA_CONFIG_VERBOS) ||
        (id == ID_NAV_PROGRESO && pantallaActual == PANTALLA_PROGRESO);

    if (id >= ID_NAV_INICIO && id <= ID_NAV_PROGRESO) {
        fondo = navSeleccionado ? COLOR_SIDEBAR_SEL : COLOR_SIDEBAR;
        borde = fondo;
        textoColor = navSeleccionado ? COLOR_BLANCO : COLOR_SIDEBAR_TXT;
        radio = 14;
    }

    if (id == ID_HOME_REPASO) {
        fondo = presionado ? COLOR_VERDE_CLARO : COLOR_BLANCO;
        borde = COLOR_VERDE;
        textoColor = COLOR_VERDE_OSCURO;
    } else if (id == ID_HOME_VOCAB) {
        borde = COLOR_AZUL;
        textoColor = COLOR_AZUL;
        fondo = presionado ? COLOR_AZUL_CLARO : COLOR_BLANCO;
    } else if (id == ID_HOME_ARTICULOS) {
        borde = COLOR_NARANJA;
        textoColor = RGB(200, 120, 25);
        fondo = presionado ? COLOR_NARANJA_CLARO : COLOR_BLANCO;
    } else if (id == ID_HOME_PLURALES) {
        borde = COLOR_VIOLETA;
        textoColor = COLOR_VIOLETA;
        fondo = presionado ? COLOR_VIOLETA_CLARO : COLOR_BLANCO;
    } else if (id == ID_HOME_VERBOS) {
        borde = COLOR_VIOLETA;
        textoColor = COLOR_VIOLETA;
        fondo = presionado ? COLOR_VIOLETA_CLARO : COLOR_BLANCO;
    }

    if (id == ID_COMPROBAR || id == ID_SIGUIENTE || id == ID_INICIAR_VERBOS) {
        fondo = presionado ? COLOR_VERDE_OSCURO : COLOR_VERDE;
        borde = fondo;
        textoColor = COLOR_BLANCO;
    }

    if (id == ID_CHAR_AE || id == ID_CHAR_OE || id == ID_CHAR_UE || id == ID_CHAR_SS) {
        fondo = presionado ? COLOR_AZUL_CLARO : COLOR_BLANCO;
        borde = COLOR_AZUL;
        textoColor = COLOR_AZUL;
        radio = 12;
    }

    if ((id >= ID_TIEMPO_PRESENTE && id <= ID_TIEMPO_PERFECTO) ||
        (id >= ID_FILTRO_REGULARES && id <= ID_FILTRO_AMBOS)) {
        bool sel = BotonSeleccionadoPorEstado(id);
        fondo = sel ? COLOR_VIOLETA_CLARO : COLOR_BLANCO;
        borde = sel ? COLOR_VIOLETA : COLOR_BORDE;
        textoColor = sel ? COLOR_VIOLETA : COLOR_TEXTO;
    }

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
    HBRUSH fondoBase = CreateSolidBrush(
        (id >= ID_NAV_INICIO && id <= ID_NAV_PROGRESO) ? COLOR_SIDEBAR : COLOR_FONDO
    );
    FillRect(hdc, &rc, fondoBase);
    DeleteObject(fondoBase);

    RECT caja = rc;
    InflateRect(&caja, -2, -2);
    DibujarRectRedondeado(hdc, caja, fondo, borde, radio, 2);

    DibujarTexto(hdc, texto, caja, fuenteBoton, textoColor,
                 DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK ProcedimientoVentana(HWND hwnd, UINT mensaje, WPARAM wParam, LPARAM lParam) {
    switch (mensaje) {
        case WM_CREATE: {
            ventanaPrincipal = hwnd;
            CrearFuentes();
            CargarProgreso();

            btnNavInicio = CrearBoton(hwnd, ID_NAV_INICIO, L"Inicio");
            btnNavEstudiar = CrearBoton(hwnd, ID_NAV_ESTUDIAR, L"Estudiar");
            btnNavVerbos = CrearBoton(hwnd, ID_NAV_VERBOS, L"Verbos");
            btnNavProgreso = CrearBoton(hwnd, ID_NAV_PROGRESO, L"Progreso");

            btnHomeRepaso = CrearBoton(hwnd, ID_HOME_REPASO, L"Repaso inteligente");
            btnHomeVocab = CrearBoton(hwnd, ID_HOME_VOCAB, L"Vocabulario");
            btnHomeArticulos = CrearBoton(hwnd, ID_HOME_ARTICULOS, L"Artículos");
            btnHomePlurales = CrearBoton(hwnd, ID_HOME_PLURALES, L"Plurales · escribir");
            btnHomeVerbos = CrearBoton(hwnd, ID_HOME_VERBOS, L"Verbos · configurar y escribir");

            btnRespuesta[0] = CrearBoton(hwnd, ID_RESPUESTA_1, L"der");
            btnRespuesta[1] = CrearBoton(hwnd, ID_RESPUESTA_2, L"die");
            btnRespuesta[2] = CrearBoton(hwnd, ID_RESPUESTA_3, L"das");
            btnSiguiente = CrearBoton(hwnd, ID_SIGUIENTE, L"Siguiente");

            editRespuesta = CrearEdit(hwnd);
            btnComprobar = CrearBoton(hwnd, ID_COMPROBAR, L"Comprobar");
            btnCharAE = CrearBoton(hwnd, ID_CHAR_AE, L"ä");
            btnCharOE = CrearBoton(hwnd, ID_CHAR_OE, L"ö");
            btnCharUE = CrearBoton(hwnd, ID_CHAR_UE, L"ü");
            btnCharSS = CrearBoton(hwnd, ID_CHAR_SS, L"ß");

            btnTiempoPresente = CrearBoton(hwnd, ID_TIEMPO_PRESENTE, L"Präsens");
            btnTiempoPreterito = CrearBoton(hwnd, ID_TIEMPO_PRETERITO, L"Präteritum");
            btnTiempoPerfecto = CrearBoton(hwnd, ID_TIEMPO_PERFECTO, L"Perfekt");

            btnFiltroRegulares = CrearBoton(hwnd, ID_FILTRO_REGULARES, L"Regulares");
            btnFiltroIrregulares = CrearBoton(hwnd, ID_FILTRO_IRREGULARES, L"Irregulares");
            btnFiltroAmbos = CrearBoton(hwnd, ID_FILTRO_AMBOS, L"Ambos");
            btnIniciarVerbos = CrearBoton(hwnd, ID_INICIAR_VERBOS, L"Empezar práctica");

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

                case ID_NAV_ESTUDIAR:
                    pantallaActual = PANTALLA_INICIO;
                    ActualizarControles();
                    break;

                case ID_NAV_VERBOS:
                    pantallaActual = PANTALLA_CONFIG_VERBOS;
                    ActualizarControles();
                    break;

                case ID_NAV_PROGRESO:
                    pantallaActual = PANTALLA_PROGRESO;
                    ActualizarControles();
                    break;

                case ID_HOME_REPASO:
                    IniciarPractica(MODO_REPASO);
                    break;

                case ID_HOME_VOCAB:
                    IniciarPractica(MODO_VOCABULARIO);
                    break;

                case ID_HOME_ARTICULOS:
                    IniciarPractica(MODO_ARTICULOS);
                    break;

                case ID_HOME_PLURALES:
                    IniciarPractica(MODO_PLURALES);
                    break;

                case ID_HOME_VERBOS:
                    pantallaActual = PANTALLA_CONFIG_VERBOS;
                    ActualizarControles();
                    break;

                case ID_RESPUESTA_1:
                case ID_RESPUESTA_2:
                case ID_RESPUESTA_3:
                    RegistrarRespuestaOpcion(id);
                    break;

                case ID_COMPROBAR:
                    RegistrarRespuestaTexto();
                    break;

                case ID_SIGUIENTE:
                    SiguientePregunta();
                    break;

                case ID_CHAR_AE:
                    InsertarCaracter(L'ä');
                    break;
                case ID_CHAR_OE:
                    InsertarCaracter(L'ö');
                    break;
                case ID_CHAR_UE:
                    InsertarCaracter(L'ü');
                    break;
                case ID_CHAR_SS:
                    InsertarCaracter(L'ß');
                    break;

                case ID_TIEMPO_PRESENTE:
                    tiempoSeleccionado = TIEMPO_PRESENTE;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case ID_TIEMPO_PRETERITO:
                    tiempoSeleccionado = TIEMPO_PRETERITO;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case ID_TIEMPO_PERFECTO:
                    tiempoSeleccionado = TIEMPO_PERFECTO;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;

                case ID_FILTRO_REGULARES:
                    filtroSeleccionado = FILTRO_REGULARES;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case ID_FILTRO_IRREGULARES:
                    filtroSeleccionado = FILTRO_IRREGULARES;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case ID_FILTRO_AMBOS:
                    filtroSeleccionado = FILTRO_AMBOS;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;

                case ID_INICIAR_VERBOS:
                    IniciarPractica(MODO_VERBOS);
                    break;
            }
            return 0;
        }

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, COLOR_BLANCO);
            SetTextColor(hdc, COLOR_TEXTO);
            static HBRUSH brushEdit = CreateSolidBrush(COLOR_BLANCO);
            return (LRESULT)brushEdit;
        }

        case WM_DRAWITEM:
            DibujarBoton((DRAWITEMSTRUCT*)lParam);
            return TRUE;

        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;
            mmi->ptMinTrackSize.x = 900;
            mmi->ptMinTrackSize.y = 660;
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
            } else if (pantallaActual == PANTALLA_PRACTICA_OPCIONES) {
                DibujarPracticaOpciones(hdc, rc);
            } else if (pantallaActual == PANTALLA_PRACTICA_TEXTO) {
                DibujarPracticaTexto(hdc, rc);
            } else if (pantallaActual == PANTALLA_CONFIG_VERBOS) {
                DibujarConfigVerbos(hdc, rc);
            } else {
                DibujarProgreso(hdc, rc);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            GuardarProgreso();

            if (fuenteLogo) DeleteObject(fuenteLogo);
            if (fuenteTitulo) DeleteObject(fuenteTitulo);
            if (fuenteSubtitulo) DeleteObject(fuenteSubtitulo);
            if (fuenteNormal) DeleteObject(fuenteNormal);
            if (fuentePequena) DeleteObject(fuentePequena);
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
        MessageBoxW(NULL, L"No se pudo registrar la ventana.",
                    L"DeutschCoach", MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowExW(
        0,
        CLASE_VENTANA,
        L"DeutschCoach · Alemán desde cero",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1080, 720,
        NULL, NULL,
        hInstance,
        NULL
    );

    if (!hwnd) {
        MessageBoxW(NULL, L"No se pudo crear la ventana principal.",
                    L"DeutschCoach", MB_ICONERROR);
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
