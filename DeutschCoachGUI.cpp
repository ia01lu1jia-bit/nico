#define NOMINMAX
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
// DeutschCoach GUI v0.5 - Netzwerk neu A1.1
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
    PANTALLA_PROGRESO,
    PANTALLA_DOMINADAS
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
const int ID_HOME_ESPANOL = 1501;
const int ID_DOMINADAS = 1502;
const int ID_LISTA_DOMINADAS = 1503;

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
HWND btnHomeEspanol = NULL;
HWND btnDominadas = NULL;
HWND listaDominadas = NULL;
bool soloEspanol = false;
bool preguntaEspanol = false;

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

// Netzwerk neu A1.1: indice alfabetico y revision OCR del libro (179 paginas).
// Se excluyen nombres personales, ciudades y marcas.
// Referencias de casos especiales:
// https://gfds.de/unterschied-zwischen-moechten-und-moegen/
// https://www.duden.de/rechtschreibung/downloaden
// Traducciones y conjugaciones añadidas para la práctica.
// Plural vacío: no se pregunta; artículo vacío: uso sin artículo.
vector<Palabra> palabras = {
    {L"Haus", L"casa", L"das", L"Häuser", 0, 0},
    {L"Buch", L"libro", L"das", L"Bücher", 0, 0},
    {L"Tisch", L"mesa", L"der", L"Tische", 0, 0},
    {L"Schule", L"escuela", L"die", L"Schulen", 0, 0},
    {L"Auto", L"auto", L"das", L"Autos", 0, 0},
    {L"Bahnhof", L"estación de tren", L"der", L"Bahnhöfe", 0, 0},
    {L"Apfel", L"manzana", L"der", L"Äpfel", 0, 0},
    {L"Kind", L"niño/a", L"das", L"Kinder", 0, 0},
    {L"Freund", L"amigo", L"der", L"Freunde", 0, 0},
    {L"Stadt", L"ciudad", L"die", L"Städte", 0, 0},
    {L"Tag", L"día", L"der", L"Tage", 0, 0},
    {L"Nacht", L"noche", L"die", L"Nächte", 0, 0},
    {L"Abbildung", L"ilustración / figura", L"die", L"Abbildungen", 0, 0},
    {L"Abend", L"tarde / noche", L"der", L"Abende", 0, 0},
    {L"Abendessen", L"cena", L"das", L"Abendessen", 0, 0},
    {L"Abschnitt", L"sección", L"der", L"Abschnitte", 0, 0},
    {L"Achtung", L"atención", L"die", L"", 0, 0},
    {L"Adjektiv", L"adjetivo", L"das", L"Adjektive", 0, 0},
    {L"Adresse", L"dirección", L"die", L"Adressen", 0, 0},
    {L"Akkusativ", L"acusativo", L"der", L"Akkusative", 0, 0},
    {L"Aktivität", L"actividad", L"die", L"Aktivitäten", 0, 0},
    {L"Akustik", L"acústica", L"die", L"", 0, 0},
    {L"Akzent", L"acento", L"der", L"Akzente", 0, 0},
    {L"Album", L"álbum", L"das", L"Alben", 0, 0},
    {L"Algerien", L"Argelia", L"", L"", 0, 0},
    {L"Alltag", L"vida cotidiana", L"der", L"", 0, 0},
    {L"Alltagsgespräch", L"conversación cotidiana", L"das", L"Alltagsgespräche", 0, 0},
    {L"Alphabet", L"alfabeto", L"das", L"Alphabete", 0, 0},
    {L"Alter", L"edad", L"das", L"", 0, 0},
    {L"Altstadt", L"casco antiguo", L"die", L"Altstädte", 0, 0},
    {L"Anfang", L"principio", L"der", L"Anfänge", 0, 0},
    {L"Angabe", L"dato", L"die", L"Angaben", 0, 0},
    {L"Angebot", L"oferta", L"das", L"Angebote", 0, 0},
    {L"Anhang", L"anexo", L"der", L"Anhänge", 0, 0},
    {L"Anmeldung", L"inscripción", L"die", L"Anmeldungen", 0, 0},
    {L"Anrede", L"tratamiento / saludo inicial", L"die", L"Anreden", 0, 0},
    {L"Anregung", L"sugerencia", L"die", L"Anregungen", 0, 0},
    {L"Ansage", L"anuncio oral", L"die", L"Ansagen", 0, 0},
    {L"Anschaulichkeit", L"claridad visual", L"die", L"", 0, 0},
    {L"Antwort", L"respuesta", L"die", L"Antworten", 0, 0},
    {L"Antwortbogen", L"hoja de respuestas", L"der", L"Antwortbögen", 0, 0},
    {L"Anzeige", L"anuncio", L"die", L"Anzeigen", 0, 0},
    {L"Apfelsaft", L"jugo de manzana", L"der", L"Apfelsäfte", 0, 0},
    {L"Apfelsaftschorle", L"jugo de manzana con soda", L"die", L"Apfelsaftschorlen", 0, 0},
    {L"Apfelschorle", L"jugo de manzana con agua con gas", L"die", L"Apfelschorlen", 0, 0},
    {L"App", L"aplicación", L"die", L"Apps", 0, 0},
    {L"Appetit", L"apetito", L"der", L"", 0, 0},
    {L"April", L"abril", L"der", L"", 0, 0},
    {L"Arabisch", L"árabe (idioma)", L"", L"", 0, 0},
    {L"Arbeit", L"trabajo", L"die", L"Arbeiten", 0, 0},
    {L"Arbeitsanweisung", L"instrucción de trabajo", L"die", L"Arbeitsanweisungen", 0, 0},
    {L"Arbeitstag", L"día de trabajo", L"der", L"Arbeitstage", 0, 0},
    {L"Arbeitszeit", L"horario de trabajo", L"die", L"Arbeitszeiten", 0, 0},
    {L"Architekt", L"arquitecto", L"der", L"Architekten", 0, 0},
    {L"Architektin", L"arquitecta", L"die", L"Architektinnen", 0, 0},
    {L"Arena", L"estadio / recinto", L"die", L"Arenen", 0, 0},
    {L"Arm", L"brazo", L"der", L"Arme", 0, 0},
    {L"Artikel", L"artículo", L"der", L"Artikel", 0, 0},
    {L"Artikelbild", L"imagen para aprender artículos", L"das", L"Artikelbilder", 0, 0},
    {L"Arzt", L"médico", L"der", L"Ärzte", 0, 0},
    {L"Arztpraxis", L"consultorio médico", L"die", L"Arztpraxen", 0, 0},
    {L"Assoziation", L"asociación de ideas", L"die", L"Assoziationen", 0, 0},
    {L"Atmosphäre", L"ambiente", L"die", L"", 0, 0},
    {L"Audio", L"audio", L"das", L"Audios", 0, 0},
    {L"Aufforderung", L"petición / indicación", L"die", L"Aufforderungen", 0, 0},
    {L"Aufgabe", L"ejercicio / tarea", L"die", L"Aufgaben", 0, 0},
    {L"Auflage", L"edición", L"die", L"Auflagen", 0, 0},
    {L"August", L"agosto", L"der", L"", 0, 0},
    {L"Ausdruck", L"expresión", L"der", L"Ausdrücke", 0, 0},
    {L"Ausflug", L"excursión", L"der", L"Ausflüge", 0, 0},
    {L"Aussage", L"afirmación", L"die", L"Aussagen", 0, 0},
    {L"Aussagesatz", L"oración enunciativa", L"der", L"Aussagesätze", 0, 0},
    {L"Aussicht", L"vista / perspectiva", L"die", L"Aussichten", 0, 0},
    {L"Aussprache", L"pronunciación", L"die", L"", 0, 0},
    {L"Ausstellung", L"exposición", L"die", L"Ausstellungen", 0, 0},
    {L"Austausch", L"intercambio", L"der", L"", 0, 0},
    {L"Australien", L"Australia", L"", L"", 0, 0},
    {L"Autobahn", L"autopista", L"die", L"Autobahnen", 0, 0},
    {L"Autor", L"autor", L"der", L"Autoren", 0, 0},
    {L"Baby", L"bebé", L"das", L"Babys", 0, 0},
    {L"Backware", L"producto de panadería", L"die", L"Backwaren", 0, 0},
    {L"Bahn", L"tren / ferrocarril", L"die", L"Bahnen", 0, 0},
    {L"Ball", L"pelota", L"der", L"Bälle", 0, 0},
    {L"Banane", L"banana", L"die", L"Bananen", 0, 0},
    {L"Band", L"banda musical", L"die", L"Bands", 0, 0},
    {L"Bank", L"banco para sentarse", L"die", L"Bänke", 0, 0},
    {L"Bar", L"bar", L"die", L"Bars", 0, 0},
    {L"Basketball", L"básquet", L"der", L"", 0, 0},
    {L"Baumeister", L"maestro de obras", L"der", L"Baumeister", 0, 0},
    {L"Bauzeit", L"duración de la construcción", L"die", L"", 0, 0},
    {L"Beamer", L"proyector", L"der", L"Beamer", 0, 0},
    {L"Becher", L"vaso / recipiente", L"der", L"Becher", 0, 0},
    {L"Bedeutung", L"significado", L"die", L"Bedeutungen", 0, 0},
    {L"Befinden", L"estado de salud", L"das", L"", 0, 0},
    {L"Beginn", L"comienzo", L"der", L"", 0, 0},
    {L"Beisl", L"taberna austríaca", L"das", L"Beisl/Beisln", 0, 0},
    {L"Beispiel", L"ejemplo", L"das", L"Beispiele", 0, 0},
    {L"Beitrag", L"aporte / publicación", L"der", L"Beiträge", 0, 0},
    {L"Beiz", L"taberna suiza", L"die", L"Beizen", 0, 0},
    {L"Beratung", L"asesoramiento", L"die", L"Beratungen", 0, 0},
    {L"Berg", L"montaña", L"der", L"Berge", 0, 0},
    {L"Bericht", L"informe", L"der", L"Berichte", 0, 0},
    {L"Beruf", L"profesión", L"der", L"Berufe", 0, 0},
    {L"Berufswahl", L"elección de profesión", L"die", L"", 0, 0},
    {L"Beschreibung", L"descripción", L"die", L"Beschreibungen", 0, 0},
    {L"Besprechung", L"reunión de trabajo", L"die", L"Besprechungen", 0, 0},
    {L"Bestellung", L"pedido", L"die", L"Bestellungen", 0, 0},
    {L"Besuch", L"visita", L"der", L"Besuche", 0, 0},
    {L"Besucher", L"visitante varón", L"der", L"Besucher", 0, 0},
    {L"Besucherin", L"visitante mujer", L"die", L"Besucherinnen", 0, 0},
    {L"Betreff", L"asunto de un mensaje", L"der", L"Betreffe", 0, 0},
    {L"Biathletin", L"biatleta mujer", L"die", L"Biathletinnen", 0, 0},
    {L"Bibliothek", L"biblioteca", L"die", L"Bibliotheken", 0, 0},
    {L"Bier", L"cerveza", L"das", L"Biere", 0, 0},
    {L"Biergarten", L"cervecería al aire libre", L"der", L"Biergärten", 0, 0},
    {L"Bild", L"imagen", L"das", L"Bilder", 0, 0},
    {L"Bilderkennung", L"reconocimiento de imágenes", L"die", L"", 0, 0},
    {L"Bildgeschichte", L"historia en imágenes", L"die", L"Bildgeschichten", 0, 0},
    {L"Bindung", L"encuadernación / vínculo", L"die", L"Bindungen", 0, 0},
    {L"Binnendifferenzierung", L"enseñanza diferenciada", L"die", L"", 0, 0},
    {L"Birne", L"pera", L"die", L"Birnen", 0, 0},
    {L"Bitte", L"petición", L"die", L"Bitten", 0, 0},
    {L"Blatt", L"hoja de papel", L"das", L"Blätter", 0, 0},
    {L"Block", L"bloc de notas", L"der", L"Blöcke", 0, 0},
    {L"Blume", L"flor", L"die", L"Blumen", 0, 0},
    {L"Brasilien", L"Brasil", L"", L"", 0, 0},
    {L"Bratkartoffel", L"papa salteada", L"die", L"Bratkartoffeln", 0, 0},
    {L"Brief", L"carta", L"der", L"Briefe", 0, 0},
    {L"Brot", L"pan", L"das", L"Brote", 0, 0},
    {L"Bruder", L"hermano", L"der", L"Brüder", 0, 0},
    {L"Brötchen", L"panecillo", L"das", L"Brötchen", 0, 0},
    {L"Brücke", L"puente", L"die", L"Brücken", 0, 0},
    {L"Buchseite", L"página de libro", L"die", L"Buchseiten", 0, 0},
    {L"Buchstabe", L"letra del alfabeto", L"der", L"Buchstaben", 0, 0},
    {L"Buchstaben-Rap", L"rap del alfabeto", L"der", L"Buchstaben-Raps", 0, 0},
    {L"Bucht", L"bahía", L"die", L"Buchten", 0, 0},
    {L"Bulgarisch", L"búlgaro (idioma)", L"", L"", 0, 0},
    {L"Bus", L"colectivo", L"der", L"Busse", 0, 0},
    {L"Butter", L"manteca", L"die", L"", 0, 0},
    {L"Butterbrot", L"pan con manteca", L"das", L"Butterbrote", 0, 0},
    {L"Bäcker", L"panadero", L"der", L"Bäcker", 0, 0},
    {L"Bäckerei", L"panadería", L"die", L"Bäckereien", 0, 0},
    {L"Bühne", L"escenario", L"die", L"Bühnen", 0, 0},
    {L"Büro", L"oficina", L"das", L"Büros", 0, 0},
    {L"Café", L"cafetería", L"das", L"Cafés", 0, 0},
    {L"Cent", L"centavo", L"der", L"Cents", 0, 0},
    {L"Champignon", L"champiñón", L"der", L"Champignons", 0, 0},
    {L"Chat", L"chat", L"der", L"Chats", 0, 0},
    {L"Checkliste", L"lista de control", L"die", L"Checklisten", 0, 0},
    {L"Cheerleader", L"animador deportivo", L"der", L"Cheerleader", 0, 0},
    {L"Chef", L"jefe", L"der", L"Chefs", 0, 0},
    {L"Chefin", L"jefa", L"die", L"Chefinnen", 0, 0},
    {L"China", L"China", L"", L"", 0, 0},
    {L"Chor", L"coro", L"der", L"Chöre", 0, 0},
    {L"Club", L"club", L"der", L"Clubs", 0, 0},
    {L"Code", L"código", L"der", L"Codes", 0, 0},
    {L"Cola", L"gaseosa de cola", L"die/das", L"Colas", 0, 0},
    {L"Comedy", L"comedia / humor", L"die", L"Comedys", 0, 0},
    {L"Computer", L"computadora", L"der", L"Computer", 0, 0},
    {L"Computerspiel", L"videojuego", L"das", L"Computerspiele", 0, 0},
    {L"Currywurst", L"salchicha con curry", L"die", L"Currywürste", 0, 0},
    {L"Dach", L"techo", L"das", L"Dächer", 0, 0},
    {L"Dank", L"agradecimiento", L"der", L"", 0, 0},
    {L"Datum", L"fecha", L"das", L"Daten", 0, 0},
    {L"Datumsangabe", L"indicación de fecha", L"die", L"Datumsangaben", 0, 0},
    {L"Denkblase", L"globo de pensamiento", L"die", L"Denkblasen", 0, 0},
    {L"Dessert", L"postre", L"das", L"Desserts", 0, 0},
    {L"Deutsch", L"alemán (idioma)", L"", L"", 0, 0},
    {L"Deutschland", L"Alemania", L"", L"", 0, 0},
    {L"Dezember", L"diciembre", L"der", L"", 0, 0},
    {L"Dialog", L"diálogo", L"der", L"Dialoge", 0, 0},
    {L"Dienstag", L"martes", L"der", L"Dienstage", 0, 0},
    {L"Dienstleistungsmarke", L"marca de servicios", L"die", L"Dienstleistungsmarken", 0, 0},
    {L"Ding", L"cosa", L"das", L"Dinge", 0, 0},
    {L"Dirigent", L"director de orquesta", L"der", L"Dirigenten", 0, 0},
    {L"Doktor", L"doctor", L"der", L"Doktoren", 0, 0},
    {L"Donnerstag", L"jueves", L"der", L"Donnerstage", 0, 0},
    {L"Dose", L"lata", L"die", L"Dosen", 0, 0},
    {L"Double Feature", L"función doble de cine", L"das", L"Double Features", 0, 0},
    {L"Download", L"descarga", L"der", L"Downloads", 0, 0},
    {L"Druck", L"impresión", L"der", L"Drucke", 0, 0},
    {L"Durchsage", L"aviso por altavoz", L"die", L"Durchsagen", 0, 0},
    {L"Durst", L"sed", L"der", L"", 0, 0},
    {L"Dänemark", L"Dinamarca", L"", L"", 0, 0},
    {L"Döner", L"döner / kebab", L"der", L"Döner", 0, 0},
    {L"E-Mail", L"correo electrónico", L"die", L"E-Mails", 0, 0},
    {L"E-Mail-Adresse", L"dirección de correo electrónico", L"die", L"E-Mail-Adressen", 0, 0},
    {L"Ei", L"huevo", L"das", L"Eier", 0, 0},
    {L"Einkauf", L"compra", L"der", L"Einkäufe", 0, 0},
    {L"Einkaufswagen", L"carrito de compras", L"der", L"Einkaufswagen", 0, 0},
    {L"Einkaufszettel", L"lista de compras", L"der", L"Einkaufszettel", 0, 0},
    {L"Einladung", L"invitación", L"die", L"Einladungen", 0, 0},
    {L"Einladungs-Mail", L"correo de invitación", L"die", L"Einladungs-Mails", 0, 0},
    {L"Eintrag", L"entrada / registro", L"der", L"Einträge", 0, 0},
    {L"Eintritt", L"entrada / acceso", L"der", L"Eintritte", 0, 0},
    {L"Einwilligung", L"consentimiento", L"die", L"Einwilligungen", 0, 0},
    {L"Einwohner", L"habitante", L"der", L"Einwohner", 0, 0},
    {L"Einwohnerzahl", L"cantidad de habitantes", L"die", L"Einwohnerzahlen", 0, 0},
    {L"Eis", L"helado / hielo", L"das", L"", 0, 0},
    {L"Elektriker", L"electricista varón", L"der", L"Elektriker", 0, 0},
    {L"Elektrikerin", L"electricista mujer", L"die", L"Elektrikerinnen", 0, 0},
    {L"Eltern", L"padres", L"die", L"", 0, 0},
    {L"Emmentaler", L"queso emmental", L"der", L"Emmentaler", 0, 0},
    {L"Endung", L"terminación", L"die", L"Endungen", 0, 0},
    {L"England", L"Inglaterra", L"", L"", 0, 0},
    {L"Englisch", L"inglés (idioma)", L"", L"", 0, 0},
    {L"Englisch-Test", L"examen de inglés", L"der", L"Englisch-Tests", 0, 0},
    {L"Entschuldigung", L"disculpa", L"die", L"Entschuldigungen", 0, 0},
    {L"Ereignis", L"acontecimiento", L"das", L"Ereignisse", 0, 0},
    {L"Ergebnis", L"resultado", L"das", L"Ergebnisse", 0, 0},
    {L"Erklärung", L"explicación", L"die", L"Erklärungen", 0, 0},
    {L"Erwachsene", L"persona adulta", L"der/die", L"Erwachsenen", 0, 0},
    {L"Erzieher", L"educador", L"der", L"Erzieher", 0, 0},
    {L"Erzieherin", L"educadora", L"die", L"Erzieherinnen", 0, 0},
    {L"Essen", L"comida", L"das", L"Essen", 0, 0},
    {L"Essgewohnheit", L"hábito alimentario", L"die", L"Essgewohnheiten", 0, 0},
    {L"Essig", L"vinagre", L"der", L"Essige", 0, 0},
    {L"Esszimmer", L"comedor de casa", L"das", L"Esszimmer", 0, 0},
    {L"Euro", L"euro", L"der", L"Euros", 0, 0},
    {L"Europameister", L"campeón europeo", L"der", L"Europameister", 0, 0},
    {L"Event", L"evento", L"das", L"Events", 0, 0},
    {L"Experte", L"experto", L"der", L"Experten", 0, 0},
    {L"Expertin", L"experta", L"die", L"Expertinnen", 0, 0},
    {L"Extra-Programm", L"programa adicional", L"das", L"Extra-Programme", 0, 0},
    {L"Fach", L"materia / compartimento", L"das", L"Fächer", 0, 0},
    {L"Fahrer", L"conductor", L"der", L"Fahrer", 0, 0},
    {L"Fahrkarte", L"boleto de transporte", L"die", L"Fahrkarten", 0, 0},
    {L"Fahrrad", L"bicicleta", L"das", L"Fahrräder", 0, 0},
    {L"Fahrradtour", L"paseo en bicicleta", L"die", L"Fahrradtouren", 0, 0},
    {L"Familie", L"familia", L"die", L"Familien", 0, 0},
    {L"Familienfeier", L"celebración familiar", L"die", L"Familienfeiern", 0, 0},
    {L"Familienfoto", L"foto familiar", L"das", L"Familienfotos", 0, 0},
    {L"Familienname", L"apellido familiar", L"der", L"Familiennamen", 0, 0},
    {L"Fantasie", L"imaginación", L"die", L"", 0, 0},
    {L"Farbe", L"color", L"die", L"Farben", 0, 0},
    {L"Februar", L"febrero", L"der", L"", 0, 0},
    {L"Feld", L"casilla / campo", L"das", L"Felder", 0, 0},
    {L"Fernsehen", L"televisión", L"das", L"", 0, 0},
    {L"Fest", L"fiesta / celebración", L"das", L"Feste", 0, 0},
    {L"Festival", L"festival", L"das", L"Festivals", 0, 0},
    {L"Fett", L"grasa", L"das", L"Fette", 0, 0},
    {L"Film", L"película", L"der", L"Filme", 0, 0},
    {L"Film-Schauspieler", L"actor de cine", L"der", L"Film-Schauspieler", 0, 0},
    {L"Filmfan", L"aficionado al cine", L"der", L"Filmfans", 0, 0},
    {L"Filmnacht", L"noche de cine", L"die", L"Filmnächte", 0, 0},
    {L"Filmpalast", L"gran sala de cine", L"der", L"Filmpaläste", 0, 0},
    {L"Firma", L"empresa", L"die", L"Firmen", 0, 0},
    {L"Fisch", L"pescado / pez", L"der", L"Fische", 0, 0},
    {L"Fischgericht", L"plato de pescado", L"das", L"Fischgerichte", 0, 0},
    {L"Fischsuppe", L"sopa de pescado", L"die", L"Fischsuppen", 0, 0},
    {L"Fitness-Studio", L"gimnasio", L"das", L"Fitness-Studios", 0, 0},
    {L"Flasche", L"botella", L"die", L"Flaschen", 0, 0},
    {L"Fleisch", L"carne", L"das", L"", 0, 0},
    {L"Fleischprodukt", L"producto cárnico", L"das", L"Fleischprodukte", 0, 0},
    {L"Flugzeug", L"avión", L"das", L"Flugzeuge", 0, 0},
    {L"Fluss", L"río", L"der", L"Flüsse", 0, 0},
    {L"Form", L"forma", L"die", L"Formen", 0, 0},
    {L"Formel", L"fórmula", L"die", L"Formeln", 0, 0},
    {L"Formular", L"formulario", L"das", L"Formulare", 0, 0},
    {L"Forum", L"foro", L"das", L"Foren", 0, 0},
    {L"Foto", L"foto", L"das", L"Fotos", 0, 0},
    {L"Fotokurs", L"curso de fotografía", L"der", L"Fotokurse", 0, 0},
    {L"Frage", L"pregunta", L"die", L"Fragen", 0, 0},
    {L"Franken", L"franco suizo", L"der", L"Franken", 0, 0},
    {L"Frankreich", L"Francia", L"", L"", 0, 0},
    {L"Französisch", L"francés (idioma)", L"", L"", 0, 0},
    {L"Frau", L"mujer / señora", L"die", L"Frauen", 0, 0},
    {L"Freitag", L"viernes", L"der", L"Freitage", 0, 0},
    {L"Freitagnachmittag", L"viernes por la tarde", L"der", L"Freitagnachmittage", 0, 0},
    {L"Freizeit", L"tiempo libre", L"die", L"", 0, 0},
    {L"Freizeitaktivität", L"actividad de tiempo libre", L"die", L"Freizeitaktivitäten", 0, 0},
    {L"Fremdsprache", L"lengua extranjera", L"die", L"Fremdsprachen", 0, 0},
    {L"Freundin", L"amiga", L"die", L"Freundinnen", 0, 0},
    {L"Friseur", L"peluquero", L"der", L"Friseure", 0, 0},
    {L"Friseur-Termin", L"turno de peluquería", L"der", L"Friseur-Termine", 0, 0},
    {L"Friseurin", L"peluquera", L"die", L"Friseurinnen", 0, 0},
    {L"Früchtekompott", L"compota de frutas", L"das", L"Früchtekompotte", 0, 0},
    {L"Frühling", L"primavera", L"der", L"", 0, 0},
    {L"Frühstück", L"desayuno", L"das", L"Frühstücke", 0, 0},
    {L"Fußball", L"fútbol", L"der", L"", 0, 0},
    {L"Fußballspiel", L"partido de fútbol", L"das", L"Fußballspiele", 0, 0},
    {L"Gabel", L"tenedor", L"die", L"Gabeln", 0, 0},
    {L"Garten", L"jardín", L"der", L"Gärten", 0, 0},
    {L"Gast", L"invitado / huésped", L"der", L"Gäste", 0, 0},
    {L"Geburtsdatum", L"fecha de nacimiento", L"das", L"Geburtsdaten", 0, 0},
    {L"Geburtsort", L"lugar de nacimiento", L"der", L"Geburtsorte", 0, 0},
    {L"Geburtstag", L"cumpleaños", L"der", L"Geburtstage", 0, 0},
    {L"Geburtstagsfest", L"fiesta de cumpleaños", L"das", L"Geburtstagsfeste", 0, 0},
    {L"Gebäude", L"edificio", L"das", L"Gebäude", 0, 0},
    {L"Geige", L"violín", L"die", L"Geigen", 0, 0},
    {L"Geigenunterricht", L"clases de violín", L"der", L"", 0, 0},
    {L"Geld", L"dinero", L"das", L"Gelder", 0, 0},
    {L"Gemüse", L"verdura", L"das", L"Gemüse", 0, 0},
    {L"Gericht", L"plato de comida", L"das", L"Gerichte", 0, 0},
    {L"Geschenk", L"regalo", L"das", L"Geschenke", 0, 0},
    {L"Geschichte", L"historia", L"die", L"Geschichten", 0, 0},
    {L"Geschlecht", L"género / sexo", L"das", L"Geschlechter", 0, 0},
    {L"Geschnetzelte", L"carne cortada en tiras", L"das", L"", 0, 0},
    {L"Geschwister", L"hermanos y hermanas", L"die", L"", 0, 0},
    {L"Geschäft", L"negocio / tienda", L"das", L"Geschäfte", 0, 0},
    {L"Gespräch", L"conversación", L"das", L"Gespräche", 0, 0},
    {L"Gestaltungskonzept", L"concepto de diseño", L"das", L"Gestaltungskonzepte", 0, 0},
    {L"Getränk", L"bebida", L"das", L"Getränke", 0, 0},
    {L"Getränkekarte", L"carta de bebidas", L"die", L"Getränkekarten", 0, 0},
    {L"Gewohnheit", L"costumbre", L"die", L"Gewohnheiten", 0, 0},
    {L"Girlande", L"guirnalda", L"die", L"Girlanden", 0, 0},
    {L"Glas", L"vaso / vidrio", L"das", L"Gläser", 0, 0},
    {L"Glück", L"suerte / felicidad", L"das", L"", 0, 0},
    {L"Gramm", L"gramo", L"das", L"Gramm", 0, 0},
    {L"Grammatik", L"gramática", L"die", L"Grammatiken", 0, 0},
    {L"Grammatikübersicht", L"resumen gramatical", L"die", L"Grammatikübersichten", 0, 0},
    {L"Griechenland", L"Grecia", L"", L"", 0, 0},
    {L"Griechisch", L"griego (idioma)", L"", L"", 0, 0},
    {L"Grill", L"parrilla", L"der", L"Grills", 0, 0},
    {L"Grillfest", L"fiesta con parrilla", L"das", L"Grillfeste", 0, 0},
    {L"Grillparty", L"fiesta con asado", L"die", L"Grillpartys", 0, 0},
    {L"Großeltern", L"abuelos", L"die", L"", 0, 0},
    {L"Großmutter", L"abuela", L"die", L"Großmütter", 0, 0},
    {L"Großvater", L"abuelo", L"der", L"Großväter", 0, 0},
    {L"Gruppe", L"grupo", L"die", L"Gruppen", 0, 0},
    {L"Gruß", L"saludo", L"der", L"Grüße", 0, 0},
    {L"Gurke", L"pepino", L"die", L"Gurken", 0, 0},
    {L"Gutachten", L"dictamen / informe experto", L"das", L"Gutachten", 0, 0},
    {L"Gästebuch", L"libro de visitas", L"das", L"Gästebücher", 0, 0},
    {L"Hafen", L"puerto", L"der", L"Häfen", 0, 0},
    {L"halbe Stunde", L"media hora", L"die", L"halbe Stunden", 0, 0},
    {L"Halbmarathon", L"media maratón", L"der", L"Halbmarathons", 0, 0},
    {L"Halle", L"pabellón / salón", L"die", L"Hallen", 0, 0},
    {L"Hamster", L"hámster", L"der", L"Hamster", 0, 0},
    {L"Handtuch", L"toalla", L"das", L"Handtücher", 0, 0},
    {L"Handwerker", L"trabajador de oficio", L"der", L"Handwerker", 0, 0},
    {L"Handwerkerin", L"trabajadora de oficio", L"die", L"Handwerkerinnen", 0, 0},
    {L"Handy", L"celular", L"das", L"Handys", 0, 0},
    {L"Handynummer", L"número de celular", L"die", L"Handynummern", 0, 0},
    {L"Hauptbahnhof", L"estación central", L"der", L"Hauptbahnhöfe", 0, 0},
    {L"Hauptstadt", L"capital de país", L"die", L"Hauptstädte", 0, 0},
    {L"Hausaufgabe", L"tarea escolar", L"die", L"Hausaufgaben", 0, 0},
    {L"Hausnummer", L"número de casa", L"die", L"Hausnummern", 0, 0},
    {L"Heft", L"cuaderno", L"das", L"Hefte", 0, 0},
    {L"Hektar", L"hectárea", L"der", L"Hektar", 0, 0},
    {L"Herbst", L"otoño", L"der", L"", 0, 0},
    {L"Herr", L"señor", L"der", L"Herren", 0, 0},
    {L"Herstellung", L"fabricación", L"die", L"", 0, 0},
    {L"Hobby", L"pasatiempo", L"das", L"Hobbys", 0, 0},
    {L"Homepage", L"página web", L"die", L"Homepages", 0, 0},
    {L"Hotel", L"hotel", L"das", L"Hotels", 0, 0},
    {L"Hund", L"perro", L"der", L"Hunde", 0, 0},
    {L"Hunger", L"hambre", L"der", L"", 0, 0},
    {L"Hähnchen", L"pollo", L"das", L"Hähnchen", 0, 0},
    {L"Höflichkeit", L"cortesía", L"die", L"", 0, 0},
    {L"Hör-Seh-Verstehen", L"comprensión audiovisual", L"das", L"", 0, 0},
    {L"Hördatei", L"archivo de audio", L"die", L"Hördateien", 0, 0},
    {L"Idee", L"idea", L"die", L"Ideen", 0, 0},
    {L"Illustration", L"ilustración", L"die", L"Illustrationen", 0, 0},
    {L"Imperativ", L"imperativo", L"der", L"Imperative", 0, 0},
    {L"Imperativsatz", L"oración imperativa", L"der", L"Imperativsätze", 0, 0},
    {L"Indonesisch", L"indonesio (idioma)", L"", L"", 0, 0},
    {L"Infinitiv", L"infinitivo", L"der", L"Infinitive", 0, 0},
    {L"Informatiker", L"informático", L"der", L"Informatiker", 0, 0},
    {L"Informatikerin", L"informática (profesión)", L"die", L"Informatikerinnen", 0, 0},
    {L"Information", L"información", L"die", L"Informationen", 0, 0},
    {L"Informationstext", L"texto informativo", L"der", L"Informationstexte", 0, 0},
    {L"Ingenieur", L"ingeniero", L"der", L"Ingenieure", 0, 0},
    {L"Ingenieurin", L"ingeniera", L"die", L"Ingenieurinnen", 0, 0},
    {L"Intensivtrainer", L"cuaderno de práctica intensiva", L"der", L"Intensivtrainer", 0, 0},
    {L"Interview", L"entrevista", L"das", L"Interviews", 0, 0},
    {L"Irak", L"Irak", L"der", L"", 0, 0},
    {L"Iran", L"Irán", L"der", L"", 0, 0},
    {L"Irland", L"Irlanda", L"", L"", 0, 0},
    {L"Italien", L"Italia", L"", L"", 0, 0},
    {L"Italienisch", L"italiano (idioma)", L"", L"", 0, 0},
    {L"Ja-/Nein-Frage", L"pregunta de sí o no", L"die", L"Ja-/Nein-Fragen", 0, 0},
    {L"Jahr", L"año", L"das", L"Jahre", 0, 0},
    {L"Jahreszeit", L"estación del año", L"die", L"Jahreszeiten", 0, 0},
    {L"Januar", L"enero", L"der", L"", 0, 0},
    {L"Japan", L"Japón", L"", L"", 0, 0},
    {L"Japanisch", L"japonés (idioma)", L"", L"", 0, 0},
    {L"Jemen", L"Yemen", L"der", L"", 0, 0},
    {L"Joghurt", L"yogur", L"der/das", L"Joghurts", 0, 0},
    {L"Journalist", L"periodista varón", L"der", L"Journalisten", 0, 0},
    {L"Journalistin", L"periodista mujer", L"die", L"Journalistinnen", 0, 0},
    {L"Juli", L"julio", L"der", L"", 0, 0},
    {L"Junge", L"chico", L"der", L"Jungen", 0, 0},
    {L"Juni", L"junio", L"der", L"", 0, 0},
    {L"Jurist", L"jurista varón", L"der", L"Juristen", 0, 0},
    {L"Juristin", L"jurista mujer", L"die", L"Juristinnen", 0, 0},
    {L"Jänner", L"enero (austríaco)", L"der", L"", 0, 0},
    {L"Kaffee", L"café (bebida)", L"der", L"Kaffees", 0, 0},
    {L"Kaffeehaus", L"café tradicional", L"das", L"Kaffeehäuser", 0, 0},
    {L"Kaiser", L"emperador", L"der", L"Kaiser", 0, 0},
    {L"Kaiserschmarrn", L"postre de panqueque troceado", L"der", L"", 0, 0},
    {L"Kalender", L"calendario", L"der", L"Kalender", 0, 0},
    {L"Kamera", L"cámara", L"die", L"Kameras", 0, 0},
    {L"Kanada", L"Canadá", L"", L"", 0, 0},
    {L"Kantine", L"comedor de empresa", L"die", L"Kantinen", 0, 0},
    {L"Kapitel", L"capítulo", L"das", L"Kapitel", 0, 0},
    {L"Karate", L"karate", L"das", L"", 0, 0},
    {L"Karneval", L"carnaval", L"der", L"Karnevale", 0, 0},
    {L"Karte", L"tarjeta / entrada", L"die", L"Karten", 0, 0},
    {L"Kartoffel", L"papa", L"die", L"Kartoffeln", 0, 0},
    {L"Kartoffelsalat", L"ensalada de papa", L"der", L"Kartoffelsalate", 0, 0},
    {L"Kartoffelsuppe", L"sopa de papa", L"die", L"Kartoffelsuppen", 0, 0},
    {L"Kassenzettel", L"ticket de compra", L"der", L"Kassenzettel", 0, 0},
    {L"Keks", L"galletita", L"der/das", L"Kekse", 0, 0},
    {L"Kellner", L"mozo", L"der", L"Kellner", 0, 0},
    {L"Kellnerin", L"moza", L"die", L"Kellnerinnen", 0, 0},
    {L"Kerze", L"vela", L"die", L"Kerzen", 0, 0},
    {L"Ketchup", L"kétchup", L"der/das", L"Ketchups", 0, 0},
    {L"Kilo", L"kilo", L"das", L"Kilos", 0, 0},
    {L"Kilogramm", L"kilogramo", L"das", L"Kilogramm", 0, 0},
    {L"Kilometer", L"kilómetro", L"der", L"Kilometer", 0, 0},
    {L"Kindergarten", L"jardín de infantes", L"der", L"Kindergärten", 0, 0},
    {L"Kino", L"cine", L"das", L"Kinos", 0, 0},
    {L"Kirche", L"iglesia", L"die", L"Kirchen", 0, 0},
    {L"Klammer", L"paréntesis", L"die", L"Klammern", 0, 0},
    {L"Kleber", L"pegamento", L"der", L"Kleber", 0, 0},
    {L"Kneipe", L"taberna", L"die", L"Kneipen", 0, 0},
    {L"Koch", L"cocinero", L"der", L"Köche", 0, 0},
    {L"Koffer", L"valija", L"der", L"Koffer", 0, 0},
    {L"Kollege", L"compañero de trabajo", L"der", L"Kollegen", 0, 0},
    {L"Kollegin", L"compañera de trabajo", L"die", L"Kolleginnen", 0, 0},
    {L"Kommentar", L"comentario", L"der", L"Kommentare", 0, 0},
    {L"Kompliment", L"cumplido", L"das", L"Komplimente", 0, 0},
    {L"Komponente", L"componente", L"die", L"Komponenten", 0, 0},
    {L"Konjugation", L"conjugación", L"die", L"Konjugationen", 0, 0},
    {L"Konsonant", L"consonante", L"der", L"Konsonanten", 0, 0},
    {L"Kontrolle", L"control", L"die", L"Kontrollen", 0, 0},
    {L"Konzert", L"concierto", L"das", L"Konzerte", 0, 0},
    {L"Konzert-Sommer", L"verano de conciertos", L"der", L"Konzert-Sommer", 0, 0},
    {L"Konzertbeginn", L"inicio del concierto", L"der", L"", 0, 0},
    {L"Konzerthaus", L"sala de conciertos", L"das", L"Konzerthäuser", 0, 0},
    {L"Konzertkarte", L"entrada de concierto", L"die", L"Konzertkarten", 0, 0},
    {L"Kosten", L"costos", L"die", L"", 0, 0},
    {L"Kranke", L"persona enferma", L"der/die", L"Kranken", 0, 0},
    {L"Krankenhaus", L"hospital", L"das", L"Krankenhäuser", 0, 0},
    {L"Krankenpfleger", L"enfermero", L"der", L"Krankenpfleger", 0, 0},
    {L"Krankenpflegerin", L"enfermera", L"die", L"Krankenpflegerinnen", 0, 0},
    {L"Kuchen", L"torta", L"der", L"Kuchen", 0, 0},
    {L"Kultur", L"cultura", L"die", L"Kulturen", 0, 0},
    {L"Kultur-Nacht", L"noche cultural", L"die", L"Kultur-Nächte", 0, 0},
    {L"Kulturprogramm", L"programa cultural", L"das", L"Kulturprogramme", 0, 0},
    {L"Kunde", L"cliente varón", L"der", L"Kunden", 0, 0},
    {L"Kundin", L"clienta", L"die", L"Kundinnen", 0, 0},
    {L"Kunst", L"arte", L"die", L"Künste", 0, 0},
    {L"Kunsthalle", L"sala de arte", L"die", L"Kunsthallen", 0, 0},
    {L"Kurs", L"curso", L"der", L"Kurse", 0, 0},
    {L"Kursbuch", L"libro del curso", L"das", L"Kursbücher", 0, 0},
    {L"Kursplakat", L"afiche del curso", L"das", L"Kursplakate", 0, 0},
    {L"Kursraum", L"aula del curso", L"der", L"Kursräume", 0, 0},
    {L"Kurssprache", L"lenguaje del aula", L"die", L"Kurssprachen", 0, 0},
    {L"Kärtchen", L"tarjetita", L"das", L"Kärtchen", 0, 0},
    {L"Käse", L"queso", L"der", L"", 0, 0},
    {L"Käsefondue", L"fondue de queso", L"das", L"Käsefondues", 0, 0},
    {L"Käsespätzle", L"pasta suaba con queso", L"die", L"", 0, 0},
    {L"Köchin", L"cocinera", L"die", L"Köchinnen", 0, 0},
    {L"König", L"rey", L"der", L"Könige", 0, 0},
    {L"Kühlschrank", L"heladera", L"der", L"Kühlschränke", 0, 0},
    {L"Label", L"sello / etiqueta", L"das", L"Labels", 0, 0},
    {L"Land", L"país", L"das", L"Länder", 0, 0},
    {L"Landeskunde", L"conocimiento del país", L"die", L"", 0, 0},
    {L"Landwirtin", L"agricultora", L"die", L"Landwirtinnen", 0, 0},
    {L"Layoutkonzeption", L"concepción de la maquetación", L"die", L"Layoutkonzeptionen", 0, 0},
    {L"Leben", L"vida", L"das", L"Leben", 0, 0},
    {L"Lebensmittel", L"alimento", L"das", L"Lebensmittel", 0, 0},
    {L"Lehrer", L"profesor", L"der", L"Lehrer", 0, 0},
    {L"Lehrerhandbuch", L"manual del docente", L"das", L"Lehrerhandbücher", 0, 0},
    {L"Lehrerin", L"profesora", L"die", L"Lehrerinnen", 0, 0},
    {L"Lernkarte", L"tarjeta de estudio", L"die", L"Lernkarten", 0, 0},
    {L"Lernwortschatz", L"vocabulario para aprender", L"der", L"Lernwortschätze", 0, 0},
    {L"Lernwortschatzseite", L"página de vocabulario", L"die", L"Lernwortschatzseiten", 0, 0},
    {L"Leute", L"gente", L"die", L"", 0, 0},
    {L"Libanon", L"Líbano", L"der", L"", 0, 0},
    {L"Lieblingskuchen", L"torta favorita", L"der", L"Lieblingskuchen", 0, 0},
    {L"Lied", L"canción", L"das", L"Lieder", 0, 0},
    {L"Limonade", L"gaseosa / limonada", L"die", L"Limonaden", 0, 0},
    {L"Liste", L"lista", L"die", L"Listen", 0, 0},
    {L"Liter", L"litro", L"der", L"Liter", 0, 0},
    {L"Literatur", L"literatura", L"die", L"", 0, 0},
    {L"Lokal", L"local gastronómico", L"das", L"Lokale", 0, 0},
    {L"Lust", L"ganas", L"die", L"", 0, 0},
    {L"Ländername", L"nombre de país", L"der", L"Ländernamen", 0, 0},
    {L"Löffel", L"cuchara", L"der", L"Löffel", 0, 0},
    {L"Lösung", L"solución", L"die", L"Lösungen", 0, 0},
    {L"Lösungswort", L"palabra solución", L"das", L"Lösungswörter", 0, 0},
    {L"Löwe", L"león", L"der", L"Löwen", 0, 0},
    {L"Mahlzeit", L"comida del día", L"die", L"Mahlzeiten", 0, 0},
    {L"Mai", L"mayo", L"der", L"", 0, 0},
    {L"Mail", L"correo", L"die", L"Mails", 0, 0},
    {L"Mail-Adresse", L"dirección de correo", L"die", L"Mail-Adressen", 0, 0},
    {L"Mailbox", L"buzón de voz", L"die", L"Mailboxen", 0, 0},
    {L"Mal", L"vez", L"das", L"Male", 0, 0},
    {L"Mama", L"mamá", L"die", L"Mamas", 0, 0},
    {L"Mann", L"hombre / marido", L"der", L"Männer", 0, 0},
    {L"Marathon", L"maratón", L"der", L"Marathons", 0, 0},
    {L"Marke", L"marca", L"die", L"Marken", 0, 0},
    {L"Markt", L"mercado", L"der", L"Märkte", 0, 0},
    {L"Marktplatz", L"plaza del mercado", L"der", L"Marktplätze", 0, 0},
    {L"Marmelade", L"mermelada", L"die", L"Marmeladen", 0, 0},
    {L"Marokko", L"Marruecos", L"", L"", 0, 0},
    {L"Maß", L"medida", L"das", L"Maße", 0, 0},
    {L"Material", L"material", L"das", L"Materialien", 0, 0},
    {L"Mathe", L"matemática (abreviado)", L"die", L"", 0, 0},
    {L"Mathe-Test", L"examen de matemática", L"der", L"Mathe-Tests", 0, 0},
    {L"Matjes", L"arenque joven", L"der", L"Matjes", 0, 0},
    {L"Mauer", L"muro", L"die", L"Mauern", 0, 0},
    {L"Maurer", L"albañil", L"der", L"Maurer", 0, 0},
    {L"Maus", L"ratón", L"die", L"Mäuse", 0, 0},
    {L"Mayo", L"mayonesa (abreviado)", L"die", L"", 0, 0},
    {L"Mechaniker", L"mecánico", L"der", L"Mechaniker", 0, 0},
    {L"Mechanikerin", L"mecánica (profesión)", L"die", L"Mechanikerinnen", 0, 0},
    {L"Medaille", L"medalla", L"die", L"Medaillen", 0, 0},
    {L"Medikament", L"medicamento", L"das", L"Medikamente", 0, 0},
    {L"Meer", L"mar", L"das", L"Meere", 0, 0},
    {L"Mehl", L"harina", L"das", L"Mehle", 0, 0},
    {L"Mensa", L"comedor universitario", L"die", L"Mensen", 0, 0},
    {L"Mensch", L"persona / ser humano", L"der", L"Menschen", 0, 0},
    {L"Messer", L"cuchillo", L"das", L"Messer", 0, 0},
    {L"Meter", L"metro", L"der", L"Meter", 0, 0},
    {L"Methode", L"método", L"die", L"Methoden", 0, 0},
    {L"Metzgerei", L"carnicería", L"die", L"Metzgereien", 0, 0},
    {L"Mexiko", L"México", L"", L"", 0, 0},
    {L"Milch", L"leche", L"die", L"", 0, 0},
    {L"Milchkaffee", L"café con leche", L"der", L"Milchkaffees", 0, 0},
    {L"Milchprodukt", L"producto lácteo", L"das", L"Milchprodukte", 0, 0},
    {L"Milliarde", L"mil millones", L"die", L"Milliarden", 0, 0},
    {L"Million", L"millón", L"die", L"Millionen", 0, 0},
    {L"Mindmap", L"mapa mental", L"die", L"Mindmaps", 0, 0},
    {L"Minute", L"minuto", L"die", L"Minuten", 0, 0},
    {L"Mitspieler", L"compañero de juego", L"der", L"Mitspieler", 0, 0},
    {L"Mitspielerin", L"compañera de juego", L"die", L"Mitspielerinnen", 0, 0},
    {L"Mittag", L"mediodía", L"der", L"Mittage", 0, 0},
    {L"Mittagessen", L"almuerzo", L"das", L"Mittagessen", 0, 0},
    {L"Mitte", L"centro / mitad", L"die", L"", 0, 0},
    {L"Mitteilung", L"comunicación / aviso", L"die", L"Mitteilungen", 0, 0},
    {L"Mittwoch", L"miércoles", L"der", L"Mittwoche", 0, 0},
    {L"Modalverb", L"verbo modal", L"das", L"Modalverben", 0, 0},
    {L"Modelleisenbahn", L"tren en miniatura", L"die", L"Modelleisenbahnen", 0, 0},
    {L"Moment", L"momento", L"der", L"Momente", 0, 0},
    {L"Monat", L"mes", L"der", L"Monate", 0, 0},
    {L"Montag", L"lunes", L"der", L"Montage", 0, 0},
    {L"Morgen", L"mañana (parte del día)", L"der", L"Morgen", 0, 0},
    {L"Motorrad", L"moto", L"das", L"Motorräder", 0, 0},
    {L"Mountainbiken", L"ciclismo de montaña", L"das", L"", 0, 0},
    {L"Museum", L"museo", L"das", L"Museen", 0, 0},
    {L"Museumsnacht", L"noche de los museos", L"die", L"Museumsnächte", 0, 0},
    {L"Musical", L"musical", L"das", L"Musicals", 0, 0},
    {L"Musik", L"música", L"die", L"", 0, 0},
    {L"Musiker", L"músico", L"der", L"Musiker", 0, 0},
    {L"Musikfestival", L"festival de música", L"das", L"Musikfestivals", 0, 0},
    {L"Musikschule", L"escuela de música", L"die", L"Musikschulen", 0, 0},
    {L"Mutter", L"madre", L"die", L"Mütter", 0, 0},
    {L"Muttersprache", L"lengua materna", L"die", L"Muttersprachen", 0, 0},
    {L"Mädchen", L"chica", L"das", L"Mädchen", 0, 0},
    {L"März", L"marzo", L"der", L"", 0, 0},
    {L"Möglichkeit", L"posibilidad", L"die", L"Möglichkeiten", 0, 0},
    {L"Münze", L"moneda", L"die", L"Münzen", 0, 0},
    {L"Müsli", L"cereal / muesli", L"das", L"Müslis", 0, 0},
    {L"Nachmittag", L"tarde", L"der", L"Nachmittage", 0, 0},
    {L"Nachname", L"apellido", L"der", L"Nachnamen", 0, 0},
    {L"Nachricht", L"mensaje / noticia", L"die", L"Nachrichten", 0, 0},
    {L"Name", L"nombre", L"der", L"Namen", 0, 0},
    {L"Natur", L"naturaleza", L"die", L"", 0, 0},
    {L"Negationsartikel", L"artículo negativo", L"der", L"Negationsartikel", 0, 0},
    {L"Netzwerk", L"red", L"das", L"Netzwerke", 0, 0},
    {L"Netzwerk-WG", L"piso compartido de Netzwerk", L"die", L"Netzwerk-WGs", 0, 0},
    {L"Neuseeland", L"Nueva Zelanda", L"", L"", 0, 0},
    {L"Niederlande", L"Países Bajos", L"die", L"", 0, 0},
    {L"Niveau", L"nivel", L"das", L"Niveaus", 0, 0},
    {L"Nomen", L"sustantivo", L"das", L"Nomen", 0, 0},
    {L"Nominativ", L"nominativo", L"der", L"Nominative", 0, 0},
    {L"Notiz", L"nota", L"die", L"Notizen", 0, 0},
    {L"November", L"noviembre", L"der", L"", 0, 0},
    {L"Nudel", L"fideo", L"die", L"Nudeln", 0, 0},
    {L"Nudelsuppe", L"sopa de fideos", L"die", L"Nudelsuppen", 0, 0},
    {L"Nummer", L"número identificador", L"die", L"Nummern", 0, 0},
    {L"Nutzung", L"uso", L"die", L"Nutzungen", 0, 0},
    {L"Nähe", L"cercanía", L"die", L"", 0, 0},
    {L"Obst", L"fruta", L"das", L"", 0, 0},
    {L"Oktober", L"octubre", L"der", L"", 0, 0},
    {L"Oldie", L"éxito musical antiguo", L"der", L"Oldies", 0, 0},
    {L"Olive", L"aceituna", L"die", L"Oliven", 0, 0},
    {L"Oma", L"abuela (familiar)", L"die", L"Omas", 0, 0},
    {L"Onkel", L"tío", L"der", L"Onkel", 0, 0},
    {L"Online-Übung", L"ejercicio en línea", L"die", L"Online-Übungen", 0, 0},
    {L"Opa", L"abuelo (familiar)", L"der", L"Opas", 0, 0},
    {L"Open-Air-Festival", L"festival al aire libre", L"das", L"Open-Air-Festivals", 0, 0},
    {L"Open-Air-Kino", L"cine al aire libre", L"das", L"Open-Air-Kinos", 0, 0},
    {L"Orange", L"naranja", L"die", L"Orangen", 0, 0},
    {L"Orangenlimonade", L"gaseosa de naranja", L"die", L"Orangenlimonaden", 0, 0},
    {L"Orangensaft", L"jugo de naranja", L"der", L"Orangensäfte", 0, 0},
    {L"Orchester", L"orquesta", L"das", L"Orchester", 0, 0},
    {L"Ordinalzahl", L"número ordinal", L"die", L"Ordinalzahlen", 0, 0},
    {L"Ort", L"lugar", L"der", L"Orte", 0, 0},
    {L"Paar", L"pareja / par", L"das", L"Paare", 0, 0},
    {L"Packung", L"paquete", L"die", L"Packungen", 0, 0},
    {L"Pantomime", L"mímica", L"die", L"", 0, 0},
    {L"Papier", L"papel", L"das", L"Papiere", 0, 0},
    {L"Paprika", L"morrón / pimiento", L"der/die", L"Paprikas", 0, 0},
    {L"Park", L"parque", L"der", L"Parks", 0, 0},
    {L"Partner", L"compañero", L"der", L"Partner", 0, 0},
    {L"Partnerin", L"compañera", L"die", L"Partnerinnen", 0, 0},
    {L"Party", L"fiesta", L"die", L"Partys", 0, 0},
    {L"Patient", L"paciente varón", L"der", L"Patienten", 0, 0},
    {L"Patientin", L"paciente mujer", L"die", L"Patientinnen", 0, 0},
    {L"Person", L"persona", L"die", L"Personen", 0, 0},
    {L"Personalpronomen", L"pronombre personal", L"das", L"Personalpronomen", 0, 0},
    {L"Personen-Memo", L"juego de memoria de personas", L"das", L"Personen-Memos", 0, 0},
    {L"Pfannkuchen", L"panqueque", L"der", L"Pfannkuchen", 0, 0},
    {L"Pfeffer", L"pimienta", L"der", L"Pfeffer", 0, 0},
    {L"Pfeil", L"flecha", L"der", L"Pfeile", 0, 0},
    {L"Phonetikclip", L"video de fonética", L"der", L"Phonetikclips", 0, 0},
    {L"Picknick", L"picnic", L"das", L"Picknicks", 0, 0},
    {L"Pizza", L"pizza", L"die", L"Pizzas/Pizzen", 0, 0},
    {L"Plakat", L"afiche", L"das", L"Plakate", 0, 0},
    {L"Plan", L"plan", L"der", L"Pläne", 0, 0},
    {L"Plattform", L"plataforma", L"die", L"Plattformen", 0, 0},
    {L"Platz", L"espacio disponible", L"der", L"", 0, 0},
    {L"Plural", L"plural", L"der", L"Plurale", 0, 0},
    {L"Pluralendung", L"terminación de plural", L"die", L"Pluralendungen", 0, 0},
    {L"Pluralform", L"forma de plural", L"die", L"Pluralformen", 0, 0},
    {L"Polen", L"Polonia", L"", L"", 0, 0},
    {L"Polizist", L"policía varón", L"der", L"Polizisten", 0, 0},
    {L"Polizistin", L"policía mujer", L"die", L"Polizistinnen", 0, 0},
    {L"Polnisch", L"polaco (idioma)", L"", L"", 0, 0},
    {L"Pommes", L"papas fritas", L"die", L"", 0, 0},
    {L"Pommes frites", L"papas fritas (nombre completo)", L"die", L"", 0, 0},
    {L"Popcorn", L"pochoclo", L"das", L"", 0, 0},
    {L"Portugal", L"Portugal", L"", L"", 0, 0},
    {L"Portugiesisch", L"portugués (idioma)", L"", L"", 0, 0},
    {L"Position", L"posición", L"die", L"Positionen", 0, 0},
    {L"Possessivartikel", L"artículo posesivo", L"der", L"Possessivartikel", 0, 0},
    {L"Postleitzahl", L"código postal", L"die", L"Postleitzahlen", 0, 0},
    {L"Praktikum", L"pasantía", L"das", L"Praktika", 0, 0},
    {L"Praxis", L"consultorio", L"die", L"Praxen", 0, 0},
    {L"Preis", L"precio", L"der", L"Preise", 0, 0},
    {L"Problem", L"problema", L"das", L"Probleme", 0, 0},
    {L"Produkt", L"producto", L"das", L"Produkte", 0, 0},
    {L"Produktion", L"producción", L"die", L"Produktionen", 0, 0},
    {L"Profil", L"perfil", L"das", L"Profile", 0, 0},
    {L"Programm", L"programa", L"das", L"Programme", 0, 0},
    {L"Projekt", L"proyecto", L"das", L"Projekte", 0, 0},
    {L"Pronomen", L"pronombre", L"das", L"Pronomen", 0, 0},
    {L"Protagonist", L"protagonista", L"der", L"Protagonisten", 0, 0},
    {L"Präfix", L"prefijo", L"das", L"Präfixe", 0, 0},
    {L"Präposition", L"preposición", L"die", L"Präpositionen", 0, 0},
    {L"Präsens", L"presente gramatical", L"das", L"", 0, 0},
    {L"Präsentation", L"presentación", L"die", L"Präsentationen", 0, 0},
    {L"Präteritum", L"pretérito alemán", L"das", L"Präterita", 0, 0},
    {L"Prüfer", L"examinador", L"der", L"Prüfer", 0, 0},
    {L"Prüfung", L"examen", L"die", L"Prüfungen", 0, 0},
    {L"Prüfungsaufgabe", L"ejercicio de examen", L"die", L"Prüfungsaufgaben", 0, 0},
    {L"Prüfungssituation", L"situación de examen", L"die", L"Prüfungssituationen", 0, 0},
    {L"Prüfungsteil", L"parte del examen", L"der", L"Prüfungsteile", 0, 0},
    {L"Prüfungstraining", L"práctica para examen", L"das", L"Prüfungstrainings", 0, 0},
    {L"Publikum", L"público", L"das", L"", 0, 0},
    {L"Punkt", L"punto", L"der", L"Punkte", 0, 0},
    {L"Pünktlichkeit", L"puntualidad", L"die", L"", 0, 0},
    {L"Qualität", L"calidad", L"die", L"Qualitäten", 0, 0},
    {L"Quelle", L"fuente", L"die", L"Quellen", 0, 0},
    {L"Quellenverzeichnis", L"lista de fuentes", L"das", L"Quellenverzeichnisse", 0, 0},
    {L"Quiz", L"cuestionario / concurso", L"das", L"Quiz", 0, 0},
    {L"Radiergummi", L"goma de borrar", L"der", L"Radiergummis", 0, 0},
    {L"Radio", L"radio", L"das", L"Radios", 0, 0},
    {L"Radtour", L"excursión en bicicleta", L"die", L"Radtouren", 0, 0},
    {L"Ratebild", L"imagen para adivinar", L"das", L"Ratebilder", 0, 0},
    {L"Rathaus", L"municipalidad", L"das", L"Rathäuser", 0, 0},
    {L"Rechnung", L"cuenta / factura", L"die", L"Rechnungen", 0, 0},
    {L"Recht", L"derecho", L"das", L"Rechte", 0, 0},
    {L"Redaktion", L"redacción editorial", L"die", L"Redaktionen", 0, 0},
    {L"Redemittel", L"expresión comunicativa", L"das", L"Redemittel", 0, 0},
    {L"Regel", L"regla", L"die", L"Regeln", 0, 0},
    {L"Regen", L"lluvia", L"der", L"", 0, 0},
    {L"Region", L"región", L"die", L"Regionen", 0, 0},
    {L"Regisseur", L"director de cine", L"der", L"Regisseure", 0, 0},
    {L"Regisseurin", L"directora de cine", L"die", L"Regisseurinnen", 0, 0},
    {L"Reihenfolge", L"orden / secuencia", L"die", L"Reihenfolgen", 0, 0},
    {L"Reis", L"arroz", L"der", L"", 0, 0},
    {L"Reiseführer", L"guía turístico", L"der", L"Reiseführer", 0, 0},
    {L"Reiseführerin", L"guía turística", L"die", L"Reiseführerinnen", 0, 0},
    {L"Remix", L"remezcla", L"der", L"Remixe", 0, 0},
    {L"Reproduktion", L"reproducción", L"die", L"Reproduktionen", 0, 0},
    {L"Requiem", L"réquiem", L"das", L"Requiems", 0, 0},
    {L"Reservierung", L"reserva", L"die", L"Reservierungen", 0, 0},
    {L"Restaurant", L"restaurante", L"das", L"Restaurants", 0, 0},
    {L"Richtung", L"dirección / rumbo", L"die", L"Richtungen", 0, 0},
    {L"Ring", L"anillo", L"der", L"Ringe", 0, 0},
    {L"Rock", L"rock (música)", L"der", L"", 0, 0},
    {L"Rock-Konzert", L"concierto de rock", L"das", L"Rock-Konzerte", 0, 0},
    {L"Rockmusik", L"música rock", L"die", L"", 0, 0},
    {L"Rollenkarte", L"tarjeta de rol", L"die", L"Rollenkarten", 0, 0},
    {L"Rundfahrt", L"paseo / recorrido circular", L"die", L"Rundfahrten", 0, 0},
    {L"Russisch", L"ruso (idioma)", L"", L"", 0, 0},
    {L"Russland", L"Rusia", L"", L"", 0, 0},
    {L"Rätoromanisch", L"romanche (idioma)", L"", L"", 0, 0},
    {L"Rätsel", L"acertijo", L"das", L"Rätsel", 0, 0},
    {L"S-Bahn", L"tren urbano", L"die", L"S-Bahnen", 0, 0},
    {L"Saft", L"jugo", L"der", L"Säfte", 0, 0},
    {L"Sahne", L"crema de leche", L"die", L"", 0, 0},
    {L"Salami", L"salame", L"die", L"Salamis", 0, 0},
    {L"Salat", L"ensalada", L"der", L"Salate", 0, 0},
    {L"Salz", L"sal", L"das", L"Salze", 0, 0},
    {L"Samstag", L"sábado", L"der", L"Samstage", 0, 0},
    {L"Samstagabend", L"sábado por la noche", L"der", L"Samstagabende", 0, 0},
    {L"Sandwich", L"sándwich", L"der/das", L"Sandwichs/Sandwiches", 0, 0},
    {L"Satz", L"oración", L"der", L"Sätze", 0, 0},
    {L"Satzende", L"final de oración", L"das", L"Satzenden", 0, 0},
    {L"Satzkasten", L"recuadro de oraciones", L"der", L"Satzkästen", 0, 0},
    {L"Satzklammer", L"estructura verbal en dos partes", L"die", L"Satzklammern", 0, 0},
    {L"Satzmelodie", L"entonación de la oración", L"die", L"Satzmelodien", 0, 0},
    {L"Saxofon", L"saxofón", L"das", L"Saxofone", 0, 0},
    {L"Schauspieler", L"actor", L"der", L"Schauspieler", 0, 0},
    {L"Schauspielerin", L"actriz", L"die", L"Schauspielerinnen", 0, 0},
    {L"Schauspielhaus", L"teatro de drama", L"das", L"Schauspielhäuser", 0, 0},
    {L"Schiff", L"barco", L"das", L"Schiffe", 0, 0},
    {L"Schinken", L"jamón", L"der", L"Schinken", 0, 0},
    {L"Schlosspark", L"parque del palacio", L"der", L"Schlossparks", 0, 0},
    {L"Schluss", L"final", L"der", L"Schlüsse", 0, 0},
    {L"Schlüssel", L"llave", L"der", L"Schlüssel", 0, 0},
    {L"Schnitzel", L"milanesa", L"das", L"Schnitzel", 0, 0},
    {L"Schoko", L"chocolate (abreviado)", L"die", L"", 0, 0},
    {L"Schokolade", L"chocolate", L"die", L"Schokoladen", 0, 0},
    {L"Schorle", L"bebida mezclada con soda", L"die", L"Schorlen", 0, 0},
    {L"Schreibaufforderung", L"consigna de escritura", L"die", L"Schreibaufforderungen", 0, 0},
    {L"Schrift", L"escritura / tipografía", L"die", L"Schriften", 0, 0},
    {L"Schulheft", L"cuaderno escolar", L"das", L"Schulhefte", 0, 0},
    {L"Schutz", L"protección", L"der", L"", 0, 0},
    {L"Schweiz", L"Suiza", L"die", L"", 0, 0},
    {L"Schweizer", L"suizo", L"der", L"Schweizer", 0, 0},
    {L"Schwester", L"hermana", L"die", L"Schwestern", 0, 0},
    {L"Schwimmbad", L"pileta de natación", L"das", L"Schwimmbäder", 0, 0},
    {L"Schwimmer", L"nadador", L"der", L"Schwimmer", 0, 0},
    {L"Schüler", L"alumno", L"der", L"Schüler", 0, 0},
    {L"See", L"lago", L"der", L"Seen", 0, 0},
    {L"Seite", L"página / lado", L"die", L"Seiten", 0, 0},
    {L"Sekunde", L"segundo", L"die", L"Sekunden", 0, 0},
    {L"Selbstbedienung", L"autoservicio", L"die", L"", 0, 0},
    {L"Seminar", L"seminario", L"das", L"Seminare", 0, 0},
    {L"Sensation", L"sensación / suceso extraordinario", L"die", L"Sensationen", 0, 0},
    {L"September", L"septiembre", L"der", L"", 0, 0},
    {L"Serbisch", L"serbio (idioma)", L"", L"", 0, 0},
    {L"Serie", L"serie", L"die", L"Serien", 0, 0},
    {L"Serviette", L"servilleta", L"die", L"Servietten", 0, 0},
    {L"Show", L"espectáculo", L"die", L"Shows", 0, 0},
    {L"Sieger", L"ganador", L"der", L"Sieger", 0, 0},
    {L"Singular", L"singular", L"der", L"Singulare", 0, 0},
    {L"Situation", L"situación", L"die", L"Situationen", 0, 0},
    {L"Ski", L"esquí", L"der", L"Ski/Skier", 0, 0},
    {L"Skitour", L"travesía de esquí", L"die", L"Skitouren", 0, 0},
    {L"Slowakei", L"Eslovaquia", L"die", L"", 0, 0},
    {L"Sofa", L"sofá", L"das", L"Sofas", 0, 0},
    {L"Sohn", L"hijo", L"der", L"Söhne", 0, 0},
    {L"Solist", L"solista varón", L"der", L"Solisten", 0, 0},
    {L"Solistin", L"solista mujer", L"die", L"Solistinnen", 0, 0},
    {L"Sommer", L"verano", L"der", L"Sommer", 0, 0},
    {L"Sonne", L"sol", L"die", L"", 0, 0},
    {L"Sonntag", L"domingo", L"der", L"Sonntage", 0, 0},
    {L"Sonntagnachmittag", L"domingo por la tarde", L"der", L"Sonntagnachmittage", 0, 0},
    {L"Sorte", L"variedad / tipo", L"die", L"Sorten", 0, 0},
    {L"Spaghetti", L"espaguetis", L"die", L"", 0, 0},
    {L"Spanien", L"España", L"", L"", 0, 0},
    {L"Spanisch", L"español (idioma)", L"", L"", 0, 0},
    {L"Spaß", L"diversión", L"der", L"", 0, 0},
    {L"Speise", L"plato / comida preparada", L"die", L"Speisen", 0, 0},
    {L"Speisekarte", L"menú / carta", L"die", L"Speisekarten", 0, 0},
    {L"Spiel", L"juego / partido", L"das", L"Spiele", 0, 0},
    {L"Spieler", L"jugador", L"der", L"Spieler", 0, 0},
    {L"Spieler-Paar", L"pareja de jugadores (con guion)", L"das", L"Spieler-Paare", 0, 0},
    {L"Spielerin", L"jugadora", L"die", L"Spielerinnen", 0, 0},
    {L"Spielerpaar", L"pareja de jugadores", L"das", L"Spielerpaare", 0, 0},
    {L"Spielplatz", L"plaza de juegos", L"der", L"Spielplätze", 0, 0},
    {L"Sport", L"deporte", L"der", L"", 0, 0},
    {L"Sportclub", L"club deportivo", L"der", L"Sportclubs", 0, 0},
    {L"Sprache", L"idioma", L"die", L"Sprachen", 0, 0},
    {L"Sprachhandeln", L"acción comunicativa", L"das", L"", 0, 0},
    {L"Sprachkurs", L"curso de idiomas", L"der", L"Sprachkurse", 0, 0},
    {L"Sprachschule", L"escuela de idiomas", L"die", L"Sprachschulen", 0, 0},
    {L"Sprachvergleich", L"comparación de idiomas", L"der", L"Sprachvergleiche", 0, 0},
    {L"Sprechblase", L"globo de diálogo", L"die", L"Sprechblasen", 0, 0},
    {L"Sprechschule", L"práctica de pronunciación", L"die", L"Sprechschulen", 0, 0},
    {L"Spritze", L"jeringa / inyección", L"die", L"Spritzen", 0, 0},
    {L"Spätzle", L"pasta suaba", L"die", L"", 0, 0},
    {L"Stadion", L"estadio", L"das", L"Stadien", 0, 0},
    {L"Stadt-Zentrum", L"centro de la ciudad", L"das", L"Stadt-Zentren", 0, 0},
    {L"Stadtfest", L"fiesta de la ciudad", L"das", L"Stadtfeste", 0, 0},
    {L"Stadthalle", L"sala municipal", L"die", L"Stadthallen", 0, 0},
    {L"Stadttour", L"recorrido por la ciudad", L"die", L"Stadttouren", 0, 0},
    {L"Stapel", L"pila / montón", L"der", L"Stapel", 0, 0},
    {L"Star", L"estrella / famoso", L"der", L"Stars", 0, 0},
    {L"Stargast", L"invitado estrella", L"der", L"Stargäste", 0, 0},
    {L"Start", L"inicio / salida", L"der", L"Starts", 0, 0},
    {L"Station", L"estación / parada", L"die", L"Stationen", 0, 0},
    {L"Stern", L"estrella", L"der", L"Sterne", 0, 0},
    {L"Stichpunkt", L"punto clave", L"der", L"Stichpunkte", 0, 0},
    {L"Stift", L"lápiz / bolígrafo", L"der", L"Stifte", 0, 0},
    {L"Strandbar", L"bar de playa", L"die", L"Strandbars", 0, 0},
    {L"Straße", L"calle", L"die", L"Straßen", 0, 0},
    {L"Straßenbahn", L"tranvía", L"die", L"Straßenbahnen", 0, 0},
    {L"Strategie", L"estrategia", L"die", L"Strategien", 0, 0},
    {L"Stress", L"estrés", L"der", L"", 0, 0},
    {L"Student", L"estudiante universitario", L"der", L"Studenten", 0, 0},
    {L"Studentin", L"estudiante universitaria", L"die", L"Studentinnen", 0, 0},
    {L"Studio", L"estudio", L"das", L"Studios", 0, 0},
    {L"Stunde", L"hora", L"die", L"Stunden", 0, 0},
    {L"Stück", L"pieza / porción", L"das", L"Stücke/Stück", 0, 0},
    {L"Subjekt", L"sujeto gramatical", L"das", L"Subjekte", 0, 0},
    {L"Summe", L"suma", L"die", L"Summen", 0, 0},
    {L"Supermarkt", L"supermercado", L"der", L"Supermärkte", 0, 0},
    {L"Suppe", L"sopa", L"die", L"Suppen", 0, 0},
    {L"Surfer", L"surfista", L"der", L"Surfer", 0, 0},
    {L"Sushi", L"sushi", L"das", L"Sushis", 0, 0},
    {L"Symbol", L"símbolo", L"das", L"Symbole", 0, 0},
    {L"Syrien", L"Siria", L"", L"", 0, 0},
    {L"Szene", L"escena", L"die", L"Szenen", 0, 0},
    {L"Tabelle", L"tabla", L"die", L"Tabellen", 0, 0},
    {L"Tablette", L"pastilla", L"die", L"Tabletten", 0, 0},
    {L"Tafel", L"pizarrón", L"die", L"Tafeln", 0, 0},
    {L"Tafelbild", L"esquema en el pizarrón", L"das", L"Tafelbilder", 0, 0},
    {L"Tagesablauf", L"rutina diaria", L"der", L"Tagesabläufe", 0, 0},
    {L"Tageszeit", L"parte del día", L"die", L"Tageszeiten", 0, 0},
    {L"Tante", L"tía", L"die", L"Tanten", 0, 0},
    {L"Tasse", L"taza", L"die", L"Tassen", 0, 0},
    {L"Taxi", L"taxi", L"das", L"Taxis", 0, 0},
    {L"Taxifahrer", L"taxista varón", L"der", L"Taxifahrer", 0, 0},
    {L"Taxifahrerin", L"taxista mujer", L"die", L"Taxifahrerinnen", 0, 0},
    {L"Taxifahrt", L"viaje en taxi", L"die", L"Taxifahrten", 0, 0},
    {L"Team", L"equipo", L"das", L"Teams", 0, 0},
    {L"Techniker", L"técnico", L"der", L"Techniker", 0, 0},
    {L"Technikerin", L"técnica (profesión)", L"die", L"Technikerinnen", 0, 0},
    {L"Tee", L"té", L"der", L"Tees", 0, 0},
    {L"Teil", L"parte", L"der", L"Teile", 0, 0},
    {L"Telefon", L"teléfono", L"das", L"Telefone", 0, 0},
    {L"Telefongespräch", L"conversación telefónica", L"das", L"Telefongespräche", 0, 0},
    {L"Telefonnummer", L"número de teléfono", L"die", L"Telefonnummern", 0, 0},
    {L"Teller", L"plato", L"der", L"Teller", 0, 0},
    {L"Tennis", L"tenis", L"das", L"", 0, 0},
    {L"Tenniskurs", L"curso de tenis", L"der", L"Tenniskurse", 0, 0},
    {L"Tennisspieler", L"tenista", L"der", L"Tennisspieler", 0, 0},
    {L"Termin", L"cita / turno", L"der", L"Termine", 0, 0},
    {L"Terminkalender", L"agenda", L"der", L"Terminkalender", 0, 0},
    {L"Terminvorschlag", L"propuesta de horario", L"der", L"Terminvorschläge", 0, 0},
    {L"Test", L"prueba / examen", L"der", L"Tests", 0, 0},
    {L"Testheft", L"cuadernillo de pruebas", L"das", L"Testhefte", 0, 0},
    {L"Text", L"texto", L"der", L"Texte", 0, 0},
    {L"Textsorte", L"tipo de texto", L"die", L"Textsorten", 0, 0},
    {L"Thailand", L"Tailandia", L"", L"", 0, 0},
    {L"Theater", L"teatro", L"das", L"Theater", 0, 0},
    {L"Theater-Festival", L"festival de teatro", L"das", L"Theater-Festivals", 0, 0},
    {L"Thema", L"tema", L"das", L"Themen", 0, 0},
    {L"Ticket", L"entrada / ticket", L"das", L"Tickets", 0, 0},
    {L"Ticketbox", L"boletería / punto de venta", L"die", L"Ticketboxen", 0, 0},
    {L"Ticketpreis", L"precio de entrada", L"der", L"Ticketpreise", 0, 0},
    {L"Tipp", L"consejo", L"der", L"Tipps", 0, 0},
    {L"Titel", L"título", L"der", L"Titel", 0, 0},
    {L"Titelbild", L"imagen de portada", L"das", L"Titelbilder", 0, 0},
    {L"Tochter", L"hija", L"die", L"Töchter", 0, 0},
    {L"Tomate", L"tomate", L"die", L"Tomaten", 0, 0},
    {L"Tomatensuppe", L"sopa de tomate", L"die", L"Tomatensuppen", 0, 0},
    {L"Topfilm", L"película destacada", L"der", L"Topfilme", 0, 0},
    {L"Tour", L"recorrido", L"die", L"Touren", 0, 0},
    {L"Touristenattraktion", L"atracción turística", L"die", L"Touristenattraktionen", 0, 0},
    {L"Training", L"entrenamiento", L"das", L"Trainings", 0, 0},
    {L"Transkript", L"transcripción", L"das", L"Transkripte", 0, 0},
    {L"Treffpunkt", L"punto de encuentro", L"der", L"Treffpunkte", 0, 0},
    {L"Trinkgeld", L"propina", L"das", L"Trinkgelder", 0, 0},
    {L"Trompete", L"trompeta", L"die", L"Trompeten", 0, 0},
    {L"Trompetenunterricht", L"clases de trompeta", L"der", L"", 0, 0},
    {L"Tunesien", L"Túnez", L"", L"", 0, 0},
    {L"Turm", L"torre", L"der", L"Türme", 0, 0},
    {L"Turnier", L"torneo", L"das", L"Turniere", 0, 0},
    {L"Türkei", L"Turquía", L"die", L"", 0, 0},
    {L"Türkisch", L"turco (idioma)", L"", L"", 0, 0},
    {L"Tüte", L"bolsa", L"die", L"Tüten", 0, 0},
    {L"U-Bahn", L"subte", L"die", L"U-Bahnen", 0, 0},
    {L"Uhr", L"reloj / hora", L"die", L"Uhren", 0, 0},
    {L"Uhrzeit", L"hora del reloj", L"die", L"Uhrzeiten", 0, 0},
    {L"Ukraine", L"Ucrania", L"die", L"", 0, 0},
    {L"Umfrage", L"encuesta", L"die", L"Umfragen", 0, 0},
    {L"Umlaut", L"vocal con diéresis", L"der", L"Umlaute", 0, 0},
    {L"Umschlaggestaltung", L"diseño de portada", L"die", L"Umschlaggestaltungen", 0, 0},
    {L"Ungarisch", L"húngaro (idioma)", L"", L"", 0, 0},
    {L"Ungarn", L"Hungría", L"", L"", 0, 0},
    {L"Uni", L"universidad (abreviado)", L"die", L"Unis", 0, 0},
    {L"Universität", L"universidad", L"die", L"Universitäten", 0, 0},
    {L"Unterricht", L"clase / enseñanza", L"der", L"", 0, 0},
    {L"Unterrichtspaket", L"paquete didáctico", L"das", L"Unterrichtspakete", 0, 0},
    {L"Unterschied", L"diferencia", L"der", L"Unterschiede", 0, 0},
    {L"Unterstrich", L"guion bajo", L"der", L"Unterstriche", 0, 0},
    {L"USA", L"Estados Unidos", L"die", L"", 0, 0},
    {L"User", L"usuario", L"der", L"User", 0, 0},
    {L"Vater", L"padre", L"der", L"Väter", 0, 0},
    {L"Verabredung", L"encuentro acordado", L"die", L"Verabredungen", 0, 0},
    {L"Veranstaltung", L"evento organizado", L"die", L"Veranstaltungen", 0, 0},
    {L"Veranstaltungstipp", L"recomendación de evento", L"der", L"Veranstaltungstipps", 0, 0},
    {L"Verb", L"verbo", L"das", L"Verben", 0, 0},
    {L"Verbteil", L"parte del verbo", L"der", L"Verbteile", 0, 0},
    {L"Verkehrsmittel", L"medio de transporte", L"das", L"Verkehrsmittel", 0, 0},
    {L"Verkäufer", L"vendedor", L"der", L"Verkäufer", 0, 0},
    {L"Verkäuferin", L"vendedora", L"die", L"Verkäuferinnen", 0, 0},
    {L"Verlag", L"editorial", L"der", L"Verlage", 0, 0},
    {L"Vermutung", L"suposición", L"die", L"Vermutungen", 0, 0},
    {L"Verpackung", L"envase", L"die", L"Verpackungen", 0, 0},
    {L"Verspätung", L"retraso", L"die", L"Verspätungen", 0, 0},
    {L"Verwandte", L"pariente", L"der/die", L"Verwandten", 0, 0},
    {L"Video", L"video", L"das", L"Videos", 0, 0},
    {L"Videodatei", L"archivo de video", L"die", L"Videodateien", 0, 0},
    {L"Viertel", L"cuarto / quince minutos", L"das", L"Viertel", 0, 0},
    {L"Violine", L"violín (nombre formal)", L"die", L"Violinen", 0, 0},
    {L"Violinkonzert", L"concierto de violín", L"das", L"Violinkonzerte", 0, 0},
    {L"Vokal", L"vocal", L"der", L"Vokale", 0, 0},
    {L"Volksbad", L"baño público / piscina pública", L"das", L"Volksbäder", 0, 0},
    {L"Vorliebe", L"preferencia", L"die", L"Vorlieben", 0, 0},
    {L"Vormittag", L"media mañana", L"der", L"Vormittage", 0, 0},
    {L"Vorname", L"nombre de pila", L"der", L"Vornamen", 0, 0},
    {L"Vorspeiseplatte", L"tabla de entradas", L"die", L"Vorspeiseplatten", 0, 0},
    {L"Vorstellung", L"presentación / función", L"die", L"Vorstellungen", 0, 0},
    {L"W-Frage", L"pregunta con palabra interrogativa", L"die", L"W-Fragen", 0, 0},
    {L"W-Wort", L"palabra interrogativa", L"das", L"W-Wörter", 0, 0},
    {L"Wald", L"bosque", L"der", L"Wälder", 0, 0},
    {L"Wasser", L"agua", L"das", L"Wasser/Wässer", 0, 0},
    {L"Wasserfontäne", L"fuente de agua", L"die", L"Wasserfontänen", 0, 0},
    {L"Weg", L"camino", L"der", L"Wege", 0, 0},
    {L"Wegbeschreibung", L"indicaciones de camino", L"die", L"Wegbeschreibungen", 0, 0},
    {L"Welt", L"mundo", L"die", L"Welten", 0, 0},
    {L"Wendung", L"giro / expresión", L"die", L"Wendungen", 0, 0},
    {L"Werk", L"obra", L"das", L"Werke", 0, 0},
    {L"Wetter", L"tiempo meteorológico", L"das", L"", 0, 0},
    {L"Whiteboard", L"pizarra blanca", L"das", L"Whiteboards", 0, 0},
    {L"Wiederholungsspiel", L"juego de repaso", L"das", L"Wiederholungsspiele", 0, 0},
    {L"Wiederhören", L"despedida telefónica", L"das", L"", 0, 0},
    {L"Wiedersehen", L"reencuentro", L"das", L"Wiedersehen", 0, 0},
    {L"Wind", L"viento", L"der", L"Winde", 0, 0},
    {L"Winter", L"invierno", L"der", L"Winter", 0, 0},
    {L"Woche", L"semana", L"die", L"Wochen", 0, 0},
    {L"Wochenende", L"fin de semana", L"das", L"Wochenenden", 0, 0},
    {L"Wochenkalender", L"calendario semanal", L"der", L"Wochenkalender", 0, 0},
    {L"Wochenmarkt", L"mercado semanal", L"der", L"Wochenmärkte", 0, 0},
    {L"Wochentag", L"día de la semana", L"der", L"Wochentage", 0, 0},
    {L"Wohl", L"bienestar", L"das", L"", 0, 0},
    {L"Wohnort", L"lugar de residencia", L"der", L"Wohnorte", 0, 0},
    {L"Wort", L"palabra", L"das", L"Wörter/Worte", 0, 0},
    {L"Wortakzent", L"acento de la palabra", L"der", L"Wortakzente", 0, 0},
    {L"Wortende", L"final de palabra", L"das", L"Wortenden", 0, 0},
    {L"Wortgruppe", L"grupo de palabras", L"die", L"Wortgruppen", 0, 0},
    {L"Wortliste", L"lista de palabras", L"die", L"Wortlisten", 0, 0},
    {L"Wortschatz", L"vocabulario", L"der", L"Wortschätze", 0, 0},
    {L"Wortschlange", L"cadena de palabras", L"die", L"Wortschlangen", 0, 0},
    {L"Wurst", L"salchicha / embutido", L"die", L"Würste", 0, 0},
    {L"Wörterbuch", L"diccionario", L"das", L"Wörterbücher", 0, 0},
    {L"Würfel", L"dado", L"der", L"Würfel", 0, 0},
    {L"Würstchen", L"salchichita", L"das", L"Würstchen", 0, 0},
    {L"Würstel", L"salchicha (austríaco)", L"das", L"Würstel", 0, 0},
    {L"Yoga", L"yoga", L"der", L"", 0, 0},
    {L"Ypsilon", L"letra ípsilon", L"das", L"Ypsilons", 0, 0},
    {L"Zahl", L"número", L"die", L"Zahlen", 0, 0},
    {L"Zeichnung", L"dibujo", L"die", L"Zeichnungen", 0, 0},
    {L"Zeit", L"tiempo", L"die", L"Zeiten", 0, 0},
    {L"Zeitangabe", L"indicación temporal", L"die", L"Zeitangaben", 0, 0},
    {L"Zeitung", L"diario", L"die", L"Zeitungen", 0, 0},
    {L"Zentrum", L"centro", L"das", L"Zentren", 0, 0},
    {L"Zertifikat", L"certificado", L"das", L"Zertifikate", 0, 0},
    {L"Zettel", L"papelito / nota", L"der", L"Zettel", 0, 0},
    {L"Ziel", L"meta / destino", L"das", L"Ziele", 0, 0},
    {L"Zimmer", L"habitación", L"das", L"Zimmer", 0, 0},
    {L"Zimmerer", L"carpintero de estructuras", L"der", L"Zimmerer", 0, 0},
    {L"Zucker", L"azúcar", L"der", L"", 0, 0},
    {L"Zug", L"tren", L"der", L"Züge", 0, 0},
    {L"Zumba", L"zumba", L"das", L"", 0, 0},
    {L"Zuwanderer", L"inmigrante", L"der", L"Zuwanderer", 0, 0},
    {L"Zwiebel", L"cebolla", L"die", L"Zwiebeln", 0, 0},
    {L"Ärztin", L"médica", L"die", L"Ärztinnen", 0, 0},
    {L"Öffnungszeit", L"horario de apertura", L"die", L"Öffnungszeiten", 0, 0},
    {L"Öl", L"aceite", L"das", L"Öle", 0, 0},
    {L"Österreich", L"Austria", L"", L"", 0, 0},
    {L"Österreicher", L"austríaco", L"der", L"Österreicher", 0, 0},
    {L"Überraschung", L"sorpresa", L"die", L"Überraschungen", 0, 0},
    {L"Überraschungstag", L"día de sorpresas", L"der", L"Überraschungstage", 0, 0},
    {L"Überschrift", L"encabezado", L"die", L"Überschriften", 0, 0},
    {L"Übung", L"ejercicio de práctica", L"die", L"Übungen", 0, 0},
    {L"Übungsbuch", L"libro de ejercicios", L"das", L"Übungsbücher", 0, 0},
    {L"Übungsteil", L"sección de ejercicios", L"der", L"Übungsteile", 0, 0}
};

vector<Verbo> verbos = {
    {L"lernen", L"aprender", true, {
        {L"lerne", L"lernst", L"lernt", L"lernen", L"lernt", L"lernen"},
        {L"lernte", L"lerntest", L"lernte", L"lernten", L"lerntet", L"lernten"},
        {L"habe gelernt", L"hast gelernt", L"hat gelernt", L"haben gelernt", L"habt gelernt", L"haben gelernt"}
    }, 0, 0},
    {L"machen", L"hacer", true, {
        {L"mache", L"machst", L"macht", L"machen", L"macht", L"machen"},
        {L"machte", L"machtest", L"machte", L"machten", L"machtet", L"machten"},
        {L"habe gemacht", L"hast gemacht", L"hat gemacht", L"haben gemacht", L"habt gemacht", L"haben gemacht"}
    }, 0, 0},
    {L"spielen", L"jugar / tocar instrumento", true, {
        {L"spiele", L"spielst", L"spielt", L"spielen", L"spielt", L"spielen"},
        {L"spielte", L"spieltest", L"spielte", L"spielten", L"spieltet", L"spielten"},
        {L"habe gespielt", L"hast gespielt", L"hat gespielt", L"haben gespielt", L"habt gespielt", L"haben gespielt"}
    }, 0, 0},
    {L"wohnen", L"vivir / residir", true, {
        {L"wohne", L"wohnst", L"wohnt", L"wohnen", L"wohnt", L"wohnen"},
        {L"wohnte", L"wohntest", L"wohnte", L"wohnten", L"wohntet", L"wohnten"},
        {L"habe gewohnt", L"hast gewohnt", L"hat gewohnt", L"haben gewohnt", L"habt gewohnt", L"haben gewohnt"}
    }, 0, 0},
    {L"sein", L"ser / estar", false, {
        {L"bin", L"bist", L"ist", L"sind", L"seid", L"sind"},
        {L"war", L"warst", L"war", L"waren", L"wart", L"waren"},
        {L"bin gewesen", L"bist gewesen", L"ist gewesen", L"sind gewesen", L"seid gewesen", L"sind gewesen"}
    }, 0, 0},
    {L"haben", L"tener", false, {
        {L"habe", L"hast", L"hat", L"haben", L"habt", L"haben"},
        {L"hatte", L"hattest", L"hatte", L"hatten", L"hattet", L"hatten"},
        {L"habe gehabt", L"hast gehabt", L"hat gehabt", L"haben gehabt", L"habt gehabt", L"haben gehabt"}
    }, 0, 0},
    {L"gehen", L"ir", false, {
        {L"gehe", L"gehst", L"geht", L"gehen", L"geht", L"gehen"},
        {L"ging", L"gingst", L"ging", L"gingen", L"gingt", L"gingen"},
        {L"bin gegangen", L"bist gegangen", L"ist gegangen", L"sind gegangen", L"seid gegangen", L"sind gegangen"}
    }, 0, 0},
    {L"kommen", L"venir", false, {
        {L"komme", L"kommst", L"kommt", L"kommen", L"kommt", L"kommen"},
        {L"kam", L"kamst", L"kam", L"kamen", L"kamt", L"kamen"},
        {L"bin gekommen", L"bist gekommen", L"ist gekommen", L"sind gekommen", L"seid gekommen", L"sind gekommen"}
    }, 0, 0},
    {L"sehen", L"ver", false, {
        {L"sehe", L"siehst", L"sieht", L"sehen", L"seht", L"sehen"},
        {L"sah", L"sahst", L"sah", L"sahen", L"saht", L"sahen"},
        {L"habe gesehen", L"hast gesehen", L"hat gesehen", L"haben gesehen", L"habt gesehen", L"haben gesehen"}
    }, 0, 0},
    {L"sprechen", L"hablar", false, {
        {L"spreche", L"sprichst", L"spricht", L"sprechen", L"sprecht", L"sprechen"},
        {L"sprach", L"sprachst", L"sprach", L"sprachen", L"spracht", L"sprachen"},
        {L"habe gesprochen", L"hast gesprochen", L"hat gesprochen", L"haben gesprochen", L"habt gesprochen", L"haben gesprochen"}
    }, 0, 0},
    {L"abholen", L"recoger / ir a buscar", true, {
        {L"hole ab", L"holst ab", L"holt ab", L"holen ab", L"holt ab", L"holen ab"},
        {L"holte ab", L"holtest ab", L"holte ab", L"holten ab", L"holtet ab", L"holten ab"},
        {L"habe abgeholt", L"hast abgeholt", L"hat abgeholt", L"haben abgeholt", L"habt abgeholt", L"haben abgeholt"}
    }, 0, 0},
    {L"abspielen", L"reproducir audio o video", true, {
        {L"spiele ab", L"spielst ab", L"spielt ab", L"spielen ab", L"spielt ab", L"spielen ab"},
        {L"spielte ab", L"spieltest ab", L"spielte ab", L"spielten ab", L"spieltet ab", L"spielten ab"},
        {L"habe abgespielt", L"hast abgespielt", L"hat abgespielt", L"haben abgespielt", L"habt abgespielt", L"haben abgespielt"}
    }, 0, 0},
    {L"achten", L"prestar atención", true, {
        {L"achte", L"achtest", L"achtet", L"achten", L"achtet", L"achten"},
        {L"achtete", L"achtetest", L"achtete", L"achteten", L"achtetet", L"achteten"},
        {L"habe geachtet", L"hast geachtet", L"hat geachtet", L"haben geachtet", L"habt geachtet", L"haben geachtet"}
    }, 0, 0},
    {L"anfangen", L"empezar", false, {
        {L"fange an", L"fängst an", L"fängt an", L"fangen an", L"fangt an", L"fangen an"},
        {L"fing an", L"fingst an", L"fing an", L"fingen an", L"fingt an", L"fingen an"},
        {L"habe angefangen", L"hast angefangen", L"hat angefangen", L"haben angefangen", L"habt angefangen", L"haben angefangen"}
    }, 0, 0},
    {L"angeben", L"indicar / declarar", false, {
        {L"gebe an", L"gibst an", L"gibt an", L"geben an", L"gebt an", L"geben an"},
        {L"gab an", L"gabst an", L"gab an", L"gaben an", L"gabt an", L"gaben an"},
        {L"habe angegeben", L"hast angegeben", L"hat angegeben", L"haben angegeben", L"habt angegeben", L"haben angegeben"}
    }, 0, 0},
    {L"ankreuzen", L"marcar con una cruz", true, {
        {L"kreuze an", L"kreuzt an", L"kreuzt an", L"kreuzen an", L"kreuzt an", L"kreuzen an"},
        {L"kreuzte an", L"kreuztest an", L"kreuzte an", L"kreuzten an", L"kreuztet an", L"kreuzten an"},
        {L"habe angekreuzt", L"hast angekreuzt", L"hat angekreuzt", L"haben angekreuzt", L"habt angekreuzt", L"haben angekreuzt"}
    }, 0, 0},
    {L"anrufen", L"llamar por teléfono", false, {
        {L"rufe an", L"rufst an", L"ruft an", L"rufen an", L"ruft an", L"rufen an"},
        {L"rief an", L"riefst an", L"rief an", L"riefen an", L"rieft an", L"riefen an"},
        {L"habe angerufen", L"hast angerufen", L"hat angerufen", L"haben angerufen", L"habt angerufen", L"haben angerufen"}
    }, 0, 0},
    {L"ansehen", L"mirar", false, {
        {L"sehe an", L"siehst an", L"sieht an", L"sehen an", L"seht an", L"sehen an"},
        {L"sah an", L"sahst an", L"sah an", L"sahen an", L"saht an", L"sahen an"},
        {L"habe angesehen", L"hast angesehen", L"hat angesehen", L"haben angesehen", L"habt angesehen", L"haben angesehen"}
    }, 0, 0},
    {L"antworten", L"responder", true, {
        {L"antworte", L"antwortest", L"antwortet", L"antworten", L"antwortet", L"antworten"},
        {L"antwortete", L"antwortetest", L"antwortete", L"antworteten", L"antwortetet", L"antworteten"},
        {L"habe geantwortet", L"hast geantwortet", L"hat geantwortet", L"haben geantwortet", L"habt geantwortet", L"haben geantwortet"}
    }, 0, 0},
    {L"arbeiten", L"trabajar", true, {
        {L"arbeite", L"arbeitest", L"arbeitet", L"arbeiten", L"arbeitet", L"arbeiten"},
        {L"arbeitete", L"arbeitetest", L"arbeitete", L"arbeiteten", L"arbeitetet", L"arbeiteten"},
        {L"habe gearbeitet", L"hast gearbeitet", L"hat gearbeitet", L"haben gearbeitet", L"habt gearbeitet", L"haben gearbeitet"}
    }, 0, 0},
    {L"aufhören", L"terminar / dejar de hacer", true, {
        {L"höre auf", L"hörst auf", L"hört auf", L"hören auf", L"hört auf", L"hören auf"},
        {L"hörte auf", L"hörtest auf", L"hörte auf", L"hörten auf", L"hörtet auf", L"hörten auf"},
        {L"habe aufgehört", L"hast aufgehört", L"hat aufgehört", L"haben aufgehört", L"habt aufgehört", L"haben aufgehört"}
    }, 0, 0},
    {L"aufpassen", L"prestar atención / cuidar", true, {
        {L"passe auf", L"passt auf", L"passt auf", L"passen auf", L"passt auf", L"passen auf"},
        {L"passte auf", L"passtest auf", L"passte auf", L"passten auf", L"passtet auf", L"passten auf"},
        {L"habe aufgepasst", L"hast aufgepasst", L"hat aufgepasst", L"haben aufgepasst", L"habt aufgepasst", L"haben aufgepasst"}
    }, 0, 0},
    {L"aufstehen", L"levantarse", false, {
        {L"stehe auf", L"stehst auf", L"steht auf", L"stehen auf", L"steht auf", L"stehen auf"},
        {L"stand auf", L"standest auf", L"stand auf", L"standen auf", L"standet auf", L"standen auf"},
        {L"bin aufgestanden", L"bist aufgestanden", L"ist aufgestanden", L"sind aufgestanden", L"seid aufgestanden", L"sind aufgestanden"}
    }, 0, 0},
    {L"aufstellen", L"colocar / disponer", true, {
        {L"stelle auf", L"stellst auf", L"stellt auf", L"stellen auf", L"stellt auf", L"stellen auf"},
        {L"stellte auf", L"stelltest auf", L"stellte auf", L"stellten auf", L"stelltet auf", L"stellten auf"},
        {L"habe aufgestellt", L"hast aufgestellt", L"hat aufgestellt", L"haben aufgestellt", L"habt aufgestellt", L"haben aufgestellt"}
    }, 0, 0},
    {L"ausfüllen", L"completar un formulario", true, {
        {L"fülle aus", L"füllst aus", L"füllt aus", L"füllen aus", L"füllt aus", L"füllen aus"},
        {L"füllte aus", L"fülltest aus", L"füllte aus", L"füllten aus", L"fülltet aus", L"füllten aus"},
        {L"habe ausgefüllt", L"hast ausgefüllt", L"hat ausgefüllt", L"haben ausgefüllt", L"habt ausgefüllt", L"haben ausgefüllt"}
    }, 0, 0},
    {L"backen", L"hornear", false, {
        {L"backe", L"bäckst/backst", L"bäckt/backt", L"backen", L"backt", L"backen"},
        {L"backte", L"backtest", L"backte", L"backten", L"backtet", L"backten"},
        {L"habe gebacken", L"hast gebacken", L"hat gebacken", L"haben gebacken", L"habt gebacken", L"haben gebacken"}
    }, 0, 0},
    {L"beantworten", L"contestar", true, {
        {L"beantworte", L"beantwortest", L"beantwortet", L"beantworten", L"beantwortet", L"beantworten"},
        {L"beantwortete", L"beantwortetest", L"beantwortete", L"beantworteten", L"beantwortetet", L"beantworteten"},
        {L"habe beantwortet", L"hast beantwortet", L"hat beantwortet", L"haben beantwortet", L"habt beantwortet", L"haben beantwortet"}
    }, 0, 0},
    {L"beginnen", L"comenzar", false, {
        {L"beginne", L"beginnst", L"beginnt", L"beginnen", L"beginnt", L"beginnen"},
        {L"begann", L"begannst", L"begann", L"begannen", L"begannt", L"begannen"},
        {L"habe begonnen", L"hast begonnen", L"hat begonnen", L"haben begonnen", L"habt begonnen", L"haben begonnen"}
    }, 0, 0},
    {L"bekommen", L"recibir", false, {
        {L"bekomme", L"bekommst", L"bekommt", L"bekommen", L"bekommt", L"bekommen"},
        {L"bekam", L"bekamst", L"bekam", L"bekamen", L"bekamt", L"bekamen"},
        {L"habe bekommen", L"hast bekommen", L"hat bekommen", L"haben bekommen", L"habt bekommen", L"haben bekommen"}
    }, 0, 0},
    {L"benennen", L"denominar", false, {
        {L"benenne", L"benennst", L"benennt", L"benennen", L"benennt", L"benennen"},
        {L"benannte", L"benanntest", L"benannte", L"benannten", L"benanntet", L"benannten"},
        {L"habe benannt", L"hast benannt", L"hat benannt", L"haben benannt", L"habt benannt", L"haben benannt"}
    }, 0, 0},
    {L"benutzen", L"utilizar", true, {
        {L"benutze", L"benutzt", L"benutzt", L"benutzen", L"benutzt", L"benutzen"},
        {L"benutzte", L"benutztest", L"benutzte", L"benutzten", L"benutztet", L"benutzten"},
        {L"habe benutzt", L"hast benutzt", L"hat benutzt", L"haben benutzt", L"habt benutzt", L"haben benutzt"}
    }, 0, 0},
    {L"berichten", L"informar / relatar", true, {
        {L"berichte", L"berichtest", L"berichtet", L"berichten", L"berichtet", L"berichten"},
        {L"berichtete", L"berichtetest", L"berichtete", L"berichteten", L"berichtetet", L"berichteten"},
        {L"habe berichtet", L"hast berichtet", L"hat berichtet", L"haben berichtet", L"habt berichtet", L"haben berichtet"}
    }, 0, 0},
    {L"beschreiben", L"describir", false, {
        {L"beschreibe", L"beschreibst", L"beschreibt", L"beschreiben", L"beschreibt", L"beschreiben"},
        {L"beschrieb", L"beschriebst", L"beschrieb", L"beschrieben", L"beschriebt", L"beschrieben"},
        {L"habe beschrieben", L"hast beschrieben", L"hat beschrieben", L"haben beschrieben", L"habt beschrieben", L"haben beschrieben"}
    }, 0, 0},
    {L"bestellen", L"pedir / encargar", true, {
        {L"bestelle", L"bestellst", L"bestellt", L"bestellen", L"bestellt", L"bestellen"},
        {L"bestellte", L"bestelltest", L"bestellte", L"bestellten", L"bestelltet", L"bestellten"},
        {L"habe bestellt", L"hast bestellt", L"hat bestellt", L"haben bestellt", L"habt bestellt", L"haben bestellt"}
    }, 0, 0},
    {L"besuchen", L"visitar", true, {
        {L"besuche", L"besuchst", L"besucht", L"besuchen", L"besucht", L"besuchen"},
        {L"besuchte", L"besuchtest", L"besuchte", L"besuchten", L"besuchtet", L"besuchten"},
        {L"habe besucht", L"hast besucht", L"hat besucht", L"haben besucht", L"habt besucht", L"haben besucht"}
    }, 0, 0},
    {L"bezahlen", L"pagar", true, {
        {L"bezahle", L"bezahlst", L"bezahlt", L"bezahlen", L"bezahlt", L"bezahlen"},
        {L"bezahlte", L"bezahltest", L"bezahlte", L"bezahlten", L"bezahltet", L"bezahlten"},
        {L"habe bezahlt", L"hast bezahlt", L"hat bezahlt", L"haben bezahlt", L"habt bezahlt", L"haben bezahlt"}
    }, 0, 0},
    {L"bilden", L"formar", true, {
        {L"bilde", L"bildest", L"bildet", L"bilden", L"bildet", L"bilden"},
        {L"bildete", L"bildetest", L"bildete", L"bildeten", L"bildetet", L"bildeten"},
        {L"habe gebildet", L"hast gebildet", L"hat gebildet", L"haben gebildet", L"habt gebildet", L"haben gebildet"}
    }, 0, 0},
    {L"bitten", L"pedir / rogar", false, {
        {L"bitte", L"bittest", L"bittet", L"bitten", L"bittet", L"bitten"},
        {L"bat", L"batest", L"bat", L"baten", L"batet", L"baten"},
        {L"habe gebeten", L"hast gebeten", L"hat gebeten", L"haben gebeten", L"habt gebeten", L"haben gebeten"}
    }, 0, 0},
    {L"bleiben", L"quedarse", false, {
        {L"bleibe", L"bleibst", L"bleibt", L"bleiben", L"bleibt", L"bleiben"},
        {L"blieb", L"bliebst", L"blieb", L"blieben", L"bliebt", L"blieben"},
        {L"bin geblieben", L"bist geblieben", L"ist geblieben", L"sind geblieben", L"seid geblieben", L"sind geblieben"}
    }, 0, 0},
    {L"brauchen", L"necesitar", true, {
        {L"brauche", L"brauchst", L"braucht", L"brauchen", L"braucht", L"brauchen"},
        {L"brauchte", L"brauchtest", L"brauchte", L"brauchten", L"brauchtet", L"brauchten"},
        {L"habe gebraucht", L"hast gebraucht", L"hat gebraucht", L"haben gebraucht", L"habt gebraucht", L"haben gebraucht"}
    }, 0, 0},
    {L"bringen", L"traer", false, {
        {L"bringe", L"bringst", L"bringt", L"bringen", L"bringt", L"bringen"},
        {L"brachte", L"brachtest", L"brachte", L"brachten", L"brachtet", L"brachten"},
        {L"habe gebracht", L"hast gebracht", L"hat gebracht", L"haben gebracht", L"habt gebracht", L"haben gebracht"}
    }, 0, 0},
    {L"buchstabieren", L"deletrear", true, {
        {L"buchstabiere", L"buchstabierst", L"buchstabiert", L"buchstabieren", L"buchstabiert", L"buchstabieren"},
        {L"buchstabierte", L"buchstabiertest", L"buchstabierte", L"buchstabierten", L"buchstabiertet", L"buchstabierten"},
        {L"habe buchstabiert", L"hast buchstabiert", L"hat buchstabiert", L"haben buchstabiert", L"habt buchstabiert", L"haben buchstabiert"}
    }, 0, 0},
    {L"da sein", L"estar presente", false, {
        {L"bin da", L"bist da", L"ist da", L"sind da", L"seid da", L"sind da"},
        {L"war da", L"warst da", L"war da", L"waren da", L"wart da", L"waren da"},
        {L"bin da gewesen", L"bist da gewesen", L"ist da gewesen", L"sind da gewesen", L"seid da gewesen", L"sind da gewesen"}
    }, 0, 0},
    {L"denken", L"pensar", false, {
        {L"denke", L"denkst", L"denkt", L"denken", L"denkt", L"denken"},
        {L"dachte", L"dachtest", L"dachte", L"dachten", L"dachtet", L"dachten"},
        {L"habe gedacht", L"hast gedacht", L"hat gedacht", L"haben gedacht", L"habt gedacht", L"haben gedacht"}
    }, 0, 0},
    {L"diktieren", L"dictar", true, {
        {L"diktiere", L"diktierst", L"diktiert", L"diktieren", L"diktiert", L"diktieren"},
        {L"diktierte", L"diktiertest", L"diktierte", L"diktierten", L"diktiertet", L"diktierten"},
        {L"habe diktiert", L"hast diktiert", L"hat diktiert", L"haben diktiert", L"habt diktiert", L"haben diktiert"}
    }, 0, 0},
    {L"dirigieren", L"dirigir una orquesta", true, {
        {L"dirigiere", L"dirigierst", L"dirigiert", L"dirigieren", L"dirigiert", L"dirigieren"},
        {L"dirigierte", L"dirigiertest", L"dirigierte", L"dirigierten", L"dirigiertet", L"dirigierten"},
        {L"habe dirigiert", L"hast dirigiert", L"hat dirigiert", L"haben dirigiert", L"habt dirigiert", L"haben dirigiert"}
    }, 0, 0},
    {L"downloaden", L"descargar", true, {
        {L"downloade", L"downloadest", L"downloadet", L"downloaden", L"downloadet", L"downloaden"},
        {L"downloadete", L"downloadetest", L"downloadete", L"downloadeten", L"downloadetet", L"downloadeten"},
        {L"habe downgeloadet/habe gedownloadet", L"hast downgeloadet/hast gedownloadet", L"hat downgeloadet/hat gedownloadet", L"haben downgeloadet/haben gedownloadet", L"habt downgeloadet/habt gedownloadet", L"haben downgeloadet/haben gedownloadet"}
    }, 0, 0},
    {L"drankommen", L"tocar el turno", false, {
        {L"komme dran", L"kommst dran", L"kommt dran", L"kommen dran", L"kommt dran", L"kommen dran"},
        {L"kam dran", L"kamst dran", L"kam dran", L"kamen dran", L"kamt dran", L"kamen dran"},
        {L"bin drangekommen", L"bist drangekommen", L"ist drangekommen", L"sind drangekommen", L"seid drangekommen", L"sind drangekommen"}
    }, 0, 0},
    {L"duschen", L"ducharse", true, {
        {L"dusche", L"duschst", L"duscht", L"duschen", L"duscht", L"duschen"},
        {L"duschte", L"duschtest", L"duschte", L"duschten", L"duschtet", L"duschten"},
        {L"habe geduscht", L"hast geduscht", L"hat geduscht", L"haben geduscht", L"habt geduscht", L"haben geduscht"}
    }, 0, 0},
    {L"einfallen", L"ocurrirse (es / a alguien)", false, {
        {L"", L"", L"fällt ein", L"", L"", L""},
        {L"", L"", L"fiel ein", L"", L"", L""},
        {L"", L"", L"ist eingefallen", L"", L"", L""}
    }, 0, 0},
    {L"einkaufen", L"hacer las compras", true, {
        {L"kaufe ein", L"kaufst ein", L"kauft ein", L"kaufen ein", L"kauft ein", L"kaufen ein"},
        {L"kaufte ein", L"kauftest ein", L"kaufte ein", L"kauften ein", L"kauftet ein", L"kauften ein"},
        {L"habe eingekauft", L"hast eingekauft", L"hat eingekauft", L"haben eingekauft", L"habt eingekauft", L"haben eingekauft"}
    }, 0, 0},
    {L"einladen", L"invitar", false, {
        {L"lade ein", L"lädst ein", L"lädt ein", L"laden ein", L"ladet ein", L"laden ein"},
        {L"lud ein", L"ludest ein", L"lud ein", L"luden ein", L"ludet ein", L"luden ein"},
        {L"habe eingeladen", L"hast eingeladen", L"hat eingeladen", L"haben eingeladen", L"habt eingeladen", L"haben eingeladen"}
    }, 0, 0},
    {L"einsammeln", L"recoger / recolectar", true, {
        {L"sammle ein/sammele ein", L"sammelst ein", L"sammelt ein", L"sammeln ein", L"sammelt ein", L"sammeln ein"},
        {L"sammelte ein", L"sammeltest ein", L"sammelte ein", L"sammelten ein", L"sammeltet ein", L"sammelten ein"},
        {L"habe eingesammelt", L"hast eingesammelt", L"hat eingesammelt", L"haben eingesammelt", L"habt eingesammelt", L"haben eingesammelt"}
    }, 0, 0},
    {L"enden", L"finalizar", true, {
        {L"ende", L"endest", L"endet", L"enden", L"endet", L"enden"},
        {L"endete", L"endetest", L"endete", L"endeten", L"endetet", L"endeten"},
        {L"habe geendet", L"hast geendet", L"hat geendet", L"haben geendet", L"habt geendet", L"haben geendet"}
    }, 0, 0},
    {L"entdecken", L"descubrir", true, {
        {L"entdecke", L"entdeckst", L"entdeckt", L"entdecken", L"entdeckt", L"entdecken"},
        {L"entdeckte", L"entdecktest", L"entdeckte", L"entdeckten", L"entdecktet", L"entdeckten"},
        {L"habe entdeckt", L"hast entdeckt", L"hat entdeckt", L"haben entdeckt", L"habt entdeckt", L"haben entdeckt"}
    }, 0, 0},
    {L"entscheiden", L"decidir", false, {
        {L"entscheide", L"entscheidest", L"entscheidet", L"entscheiden", L"entscheidet", L"entscheiden"},
        {L"entschied", L"entschiedest", L"entschied", L"entschieden", L"entschiedet", L"entschieden"},
        {L"habe entschieden", L"hast entschieden", L"hat entschieden", L"haben entschieden", L"habt entschieden", L"haben entschieden"}
    }, 0, 0},
    {L"entschuldigen", L"disculpar", true, {
        {L"entschuldige", L"entschuldigst", L"entschuldigt", L"entschuldigen", L"entschuldigt", L"entschuldigen"},
        {L"entschuldigte", L"entschuldigtest", L"entschuldigte", L"entschuldigten", L"entschuldigtet", L"entschuldigten"},
        {L"habe entschuldigt", L"hast entschuldigt", L"hat entschuldigt", L"haben entschuldigt", L"habt entschuldigt", L"haben entschuldigt"}
    }, 0, 0},
    {L"erfragen", L"averiguar preguntando", true, {
        {L"erfrage", L"erfragst", L"erfragt", L"erfragen", L"erfragt", L"erfragen"},
        {L"erfragte", L"erfragtest", L"erfragte", L"erfragten", L"erfragtet", L"erfragten"},
        {L"habe erfragt", L"hast erfragt", L"hat erfragt", L"haben erfragt", L"habt erfragt", L"haben erfragt"}
    }, 0, 0},
    {L"ergänzen", L"completar", true, {
        {L"ergänze", L"ergänzt", L"ergänzt", L"ergänzen", L"ergänzt", L"ergänzen"},
        {L"ergänzte", L"ergänztest", L"ergänzte", L"ergänzten", L"ergänztet", L"ergänzten"},
        {L"habe ergänzt", L"hast ergänzt", L"hat ergänzt", L"haben ergänzt", L"habt ergänzt", L"haben ergänzt"}
    }, 0, 0},
    {L"erkennen", L"reconocer", false, {
        {L"erkenne", L"erkennst", L"erkennt", L"erkennen", L"erkennt", L"erkennen"},
        {L"erkannte", L"erkanntest", L"erkannte", L"erkannten", L"erkanntet", L"erkannten"},
        {L"habe erkannt", L"hast erkannt", L"hat erkannt", L"haben erkannt", L"habt erkannt", L"haben erkannt"}
    }, 0, 0},
    {L"erzählen", L"contar / narrar", true, {
        {L"erzähle", L"erzählst", L"erzählt", L"erzählen", L"erzählt", L"erzählen"},
        {L"erzählte", L"erzähltest", L"erzählte", L"erzählten", L"erzähltet", L"erzählten"},
        {L"habe erzählt", L"hast erzählt", L"hat erzählt", L"haben erzählt", L"habt erzählt", L"haben erzählt"}
    }, 0, 0},
    {L"essen", L"comer", false, {
        {L"esse", L"isst", L"isst", L"essen", L"esst", L"essen"},
        {L"aß", L"aßest/aßt", L"aß", L"aßen", L"aßt", L"aßen"},
        {L"habe gegessen", L"hast gegessen", L"hat gegessen", L"haben gegessen", L"habt gegessen", L"haben gegessen"}
    }, 0, 0},
    {L"fahren", L"ir en vehículo", false, {
        {L"fahre", L"fährst", L"fährt", L"fahren", L"fahrt", L"fahren"},
        {L"fuhr", L"fuhrst", L"fuhr", L"fuhren", L"fuhrt", L"fuhren"},
        {L"bin gefahren", L"bist gefahren", L"ist gefahren", L"sind gefahren", L"seid gefahren", L"sind gefahren"}
    }, 0, 0},
    {L"fehlen", L"faltar", true, {
        {L"fehle", L"fehlst", L"fehlt", L"fehlen", L"fehlt", L"fehlen"},
        {L"fehlte", L"fehltest", L"fehlte", L"fehlten", L"fehltet", L"fehlten"},
        {L"habe gefehlt", L"hast gefehlt", L"hat gefehlt", L"haben gefehlt", L"habt gefehlt", L"haben gefehlt"}
    }, 0, 0},
    {L"feiern", L"festejar", true, {
        {L"feiere", L"feierst", L"feiert", L"feiern", L"feiert", L"feiern"},
        {L"feierte", L"feiertest", L"feierte", L"feierten", L"feiertet", L"feierten"},
        {L"habe gefeiert", L"hast gefeiert", L"hat gefeiert", L"haben gefeiert", L"habt gefeiert", L"haben gefeiert"}
    }, 0, 0},
    {L"finden", L"encontrar / parecer", false, {
        {L"finde", L"findest", L"findet", L"finden", L"findet", L"finden"},
        {L"fand", L"fandest", L"fand", L"fanden", L"fandet", L"fanden"},
        {L"habe gefunden", L"hast gefunden", L"hat gefunden", L"haben gefunden", L"habt gefunden", L"haben gefunden"}
    }, 0, 0},
    {L"formulieren", L"formular", true, {
        {L"formuliere", L"formulierst", L"formuliert", L"formulieren", L"formuliert", L"formulieren"},
        {L"formulierte", L"formuliertest", L"formulierte", L"formulierten", L"formuliertet", L"formulierten"},
        {L"habe formuliert", L"hast formuliert", L"hat formuliert", L"haben formuliert", L"habt formuliert", L"haben formuliert"}
    }, 0, 0},
    {L"fotografieren", L"fotografiar", true, {
        {L"fotografiere", L"fotografierst", L"fotografiert", L"fotografieren", L"fotografiert", L"fotografieren"},
        {L"fotografierte", L"fotografiertest", L"fotografierte", L"fotografierten", L"fotografiertet", L"fotografierten"},
        {L"habe fotografiert", L"hast fotografiert", L"hat fotografiert", L"haben fotografiert", L"habt fotografiert", L"haben fotografiert"}
    }, 0, 0},
    {L"fragen", L"preguntar", true, {
        {L"frage", L"fragst", L"fragt", L"fragen", L"fragt", L"fragen"},
        {L"fragte", L"fragtest", L"fragte", L"fragten", L"fragtet", L"fragten"},
        {L"habe gefragt", L"hast gefragt", L"hat gefragt", L"haben gefragt", L"habt gefragt", L"haben gefragt"}
    }, 0, 0},
    {L"freihaben", L"tener libre", false, {
        {L"habe frei", L"hast frei", L"hat frei", L"haben frei", L"habt frei", L"haben frei"},
        {L"hatte frei", L"hattest frei", L"hatte frei", L"hatten frei", L"hattet frei", L"hatten frei"},
        {L"habe freigehabt", L"hast freigehabt", L"hat freigehabt", L"haben freigehabt", L"habt freigehabt", L"haben freigehabt"}
    }, 0, 0},
    {L"frühstücken", L"desayunar", true, {
        {L"frühstücke", L"frühstückst", L"frühstückt", L"frühstücken", L"frühstückt", L"frühstücken"},
        {L"frühstückte", L"frühstücktest", L"frühstückte", L"frühstückten", L"frühstücktet", L"frühstückten"},
        {L"habe gefrühstückt", L"hast gefrühstückt", L"hat gefrühstückt", L"haben gefrühstückt", L"habt gefrühstückt", L"haben gefrühstückt"}
    }, 0, 0},
    {L"führen", L"conducir / mantener una conversación", true, {
        {L"führe", L"führst", L"führt", L"führen", L"führt", L"führen"},
        {L"führte", L"führtest", L"führte", L"führten", L"führtet", L"führten"},
        {L"habe geführt", L"hast geführt", L"hat geführt", L"haben geführt", L"habt geführt", L"haben geführt"}
    }, 0, 0},
    {L"füllen", L"llenar", true, {
        {L"fülle", L"füllst", L"füllt", L"füllen", L"füllt", L"füllen"},
        {L"füllte", L"fülltest", L"füllte", L"füllten", L"fülltet", L"füllten"},
        {L"habe gefüllt", L"hast gefüllt", L"hat gefüllt", L"haben gefüllt", L"habt gefüllt", L"haben gefüllt"}
    }, 0, 0},
    {L"geben", L"dar", false, {
        {L"gebe", L"gibst", L"gibt", L"geben", L"gebt", L"geben"},
        {L"gab", L"gabst", L"gab", L"gaben", L"gabt", L"gaben"},
        {L"habe gegeben", L"hast gegeben", L"hat gegeben", L"haben gegeben", L"habt gegeben", L"haben gegeben"}
    }, 0, 0},
    {L"genießen", L"disfrutar", false, {
        {L"genieße", L"genießt", L"genießt", L"genießen", L"genießt", L"genießen"},
        {L"genoss", L"genossest/genosst", L"genoss", L"genossen", L"genosst", L"genossen"},
        {L"habe genossen", L"hast genossen", L"hat genossen", L"haben genossen", L"habt genossen", L"haben genossen"}
    }, 0, 0},
    {L"glauben", L"creer", true, {
        {L"glaube", L"glaubst", L"glaubt", L"glauben", L"glaubt", L"glauben"},
        {L"glaubte", L"glaubtest", L"glaubte", L"glaubten", L"glaubtet", L"glaubten"},
        {L"habe geglaubt", L"hast geglaubt", L"hat geglaubt", L"haben geglaubt", L"habt geglaubt", L"haben geglaubt"}
    }, 0, 0},
    {L"grillen", L"hacer un asado", true, {
        {L"grille", L"grillst", L"grillt", L"grillen", L"grillt", L"grillen"},
        {L"grillte", L"grilltest", L"grillte", L"grillten", L"grilltet", L"grillten"},
        {L"habe gegrillt", L"hast gegrillt", L"hat gegrillt", L"haben gegrillt", L"habt gegrillt", L"haben gegrillt"}
    }, 0, 0},
    {L"grüßen", L"saludar", true, {
        {L"grüße", L"grüßt", L"grüßt", L"grüßen", L"grüßt", L"grüßen"},
        {L"grüßte", L"grüßtest", L"grüßte", L"grüßten", L"grüßtet", L"grüßten"},
        {L"habe gegrüßt", L"hast gegrüßt", L"hat gegrüßt", L"haben gegrüßt", L"habt gegrüßt", L"haben gegrüßt"}
    }, 0, 0},
    {L"heißen", L"llamarse / significar", false, {
        {L"heiße", L"heißt", L"heißt", L"heißen", L"heißt", L"heißen"},
        {L"hieß", L"hießest/hießt", L"hieß", L"hießen", L"hießt", L"hießen"},
        {L"habe geheißen", L"hast geheißen", L"hat geheißen", L"haben geheißen", L"habt geheißen", L"haben geheißen"}
    }, 0, 0},
    {L"helfen", L"ayudar", false, {
        {L"helfe", L"hilfst", L"hilft", L"helfen", L"helft", L"helfen"},
        {L"half", L"halfst", L"half", L"halfen", L"halft", L"halfen"},
        {L"habe geholfen", L"hast geholfen", L"hat geholfen", L"haben geholfen", L"habt geholfen", L"haben geholfen"}
    }, 0, 0},
    {L"holen", L"ir a buscar", true, {
        {L"hole", L"holst", L"holt", L"holen", L"holt", L"holen"},
        {L"holte", L"holtest", L"holte", L"holten", L"holtet", L"holten"},
        {L"habe geholt", L"hast geholt", L"hat geholt", L"haben geholt", L"habt geholt", L"haben geholt"}
    }, 0, 0},
    {L"hängen", L"estar colgado", false, {
        {L"hänge", L"hängst", L"hängt", L"hängen", L"hängt", L"hängen"},
        {L"hing", L"hingst", L"hing", L"hingen", L"hingt", L"hingen"},
        {L"habe gehangen", L"hast gehangen", L"hat gehangen", L"haben gehangen", L"habt gehangen", L"haben gehangen"}
    }, 0, 0},
    {L"hören", L"escuchar", true, {
        {L"höre", L"hörst", L"hört", L"hören", L"hört", L"hören"},
        {L"hörte", L"hörtest", L"hörte", L"hörten", L"hörtet", L"hörten"},
        {L"habe gehört", L"hast gehört", L"hat gehört", L"haben gehört", L"habt gehört", L"haben gehört"}
    }, 0, 0},
    {L"joggen", L"trotar", true, {
        {L"jogge", L"joggst", L"joggt", L"joggen", L"joggt", L"joggen"},
        {L"joggte", L"joggtest", L"joggte", L"joggten", L"joggtet", L"joggten"},
        {L"habe gejoggt/bin gejoggt", L"hast gejoggt/bist gejoggt", L"hat gejoggt/ist gejoggt", L"haben gejoggt/sind gejoggt", L"habt gejoggt/seid gejoggt", L"haben gejoggt/sind gejoggt"}
    }, 0, 0},
    {L"kaufen", L"comprar", true, {
        {L"kaufe", L"kaufst", L"kauft", L"kaufen", L"kauft", L"kaufen"},
        {L"kaufte", L"kauftest", L"kaufte", L"kauften", L"kauftet", L"kauften"},
        {L"habe gekauft", L"hast gekauft", L"hat gekauft", L"haben gekauft", L"habt gekauft", L"haben gekauft"}
    }, 0, 0},
    {L"kennen", L"conocer", false, {
        {L"kenne", L"kennst", L"kennt", L"kennen", L"kennt", L"kennen"},
        {L"kannte", L"kanntest", L"kannte", L"kannten", L"kanntet", L"kannten"},
        {L"habe gekannt", L"hast gekannt", L"hat gekannt", L"haben gekannt", L"habt gekannt", L"haben gekannt"}
    }, 0, 0},
    {L"klettern", L"escalar", true, {
        {L"klettere", L"kletterst", L"klettert", L"klettern", L"klettert", L"klettern"},
        {L"kletterte", L"klettertest", L"kletterte", L"kletterten", L"klettertet", L"kletterten"},
        {L"habe geklettert/bin geklettert", L"hast geklettert/bist geklettert", L"hat geklettert/ist geklettert", L"haben geklettert/sind geklettert", L"habt geklettert/seid geklettert", L"haben geklettert/sind geklettert"}
    }, 0, 0},
    {L"klingen", L"sonar", false, {
        {L"klinge", L"klingst", L"klingt", L"klingen", L"klingt", L"klingen"},
        {L"klang", L"klangst", L"klang", L"klangen", L"klangt", L"klangen"},
        {L"habe geklungen", L"hast geklungen", L"hat geklungen", L"haben geklungen", L"habt geklungen", L"haben geklungen"}
    }, 0, 0},
    {L"klopfen", L"golpear / dar golpecitos", true, {
        {L"klopfe", L"klopfst", L"klopft", L"klopfen", L"klopft", L"klopfen"},
        {L"klopfte", L"klopftest", L"klopfte", L"klopften", L"klopftet", L"klopften"},
        {L"habe geklopft", L"hast geklopft", L"hat geklopft", L"haben geklopft", L"habt geklopft", L"haben geklopft"}
    }, 0, 0},
    {L"kochen", L"cocinar", true, {
        {L"koche", L"kochst", L"kocht", L"kochen", L"kocht", L"kochen"},
        {L"kochte", L"kochtest", L"kochte", L"kochten", L"kochtet", L"kochten"},
        {L"habe gekocht", L"hast gekocht", L"hat gekocht", L"haben gekocht", L"habt gekocht", L"haben gekocht"}
    }, 0, 0},
    {L"kontrollieren", L"controlar / revisar", true, {
        {L"kontrolliere", L"kontrollierst", L"kontrolliert", L"kontrollieren", L"kontrolliert", L"kontrollieren"},
        {L"kontrollierte", L"kontrolliertest", L"kontrollierte", L"kontrollierten", L"kontrolliertet", L"kontrollierten"},
        {L"habe kontrolliert", L"hast kontrolliert", L"hat kontrolliert", L"haben kontrolliert", L"habt kontrolliert", L"haben kontrolliert"}
    }, 0, 0},
    {L"korrigieren", L"corregir", true, {
        {L"korrigiere", L"korrigierst", L"korrigiert", L"korrigieren", L"korrigiert", L"korrigieren"},
        {L"korrigierte", L"korrigiertest", L"korrigierte", L"korrigierten", L"korrigiertet", L"korrigierten"},
        {L"habe korrigiert", L"hast korrigiert", L"hat korrigiert", L"haben korrigiert", L"habt korrigiert", L"haben korrigiert"}
    }, 0, 0},
    {L"kosten", L"costar", true, {
        {L"koste", L"kostest", L"kostet", L"kosten", L"kostet", L"kosten"},
        {L"kostete", L"kostetest", L"kostete", L"kosteten", L"kostetet", L"kosteten"},
        {L"habe gekostet", L"hast gekostet", L"hat gekostet", L"haben gekostet", L"habt gekostet", L"haben gekostet"}
    }, 0, 0},
    {L"kreisen", L"girar en círculos", true, {
        {L"kreise", L"kreist", L"kreist", L"kreisen", L"kreist", L"kreisen"},
        {L"kreiste", L"kreistest", L"kreiste", L"kreisten", L"kreistet", L"kreisten"},
        {L"habe gekreist", L"hast gekreist", L"hat gekreist", L"haben gekreist", L"habt gekreist", L"haben gekreist"}
    }, 0, 0},
    {L"können", L"poder", false, {
        {L"kann", L"kannst", L"kann", L"können", L"könnt", L"können"},
        {L"konnte", L"konntest", L"konnte", L"konnten", L"konntet", L"konnten"},
        {L"habe gekonnt", L"hast gekonnt", L"hat gekonnt", L"haben gekonnt", L"habt gekonnt", L"haben gekonnt"}
    }, 0, 0},
    {L"laufen", L"correr / caminar", false, {
        {L"laufe", L"läufst", L"läuft", L"laufen", L"lauft", L"laufen"},
        {L"lief", L"liefst", L"lief", L"liefen", L"lieft", L"liefen"},
        {L"bin gelaufen", L"bist gelaufen", L"ist gelaufen", L"sind gelaufen", L"seid gelaufen", L"sind gelaufen"}
    }, 0, 0},
    {L"leben", L"vivir", true, {
        {L"lebe", L"lebst", L"lebt", L"leben", L"lebt", L"leben"},
        {L"lebte", L"lebtest", L"lebte", L"lebten", L"lebtet", L"lebten"},
        {L"habe gelebt", L"hast gelebt", L"hat gelebt", L"haben gelebt", L"habt gelebt", L"haben gelebt"}
    }, 0, 0},
    {L"legen", L"poner en horizontal", true, {
        {L"lege", L"legst", L"legt", L"legen", L"legt", L"legen"},
        {L"legte", L"legtest", L"legte", L"legten", L"legtet", L"legten"},
        {L"habe gelegt", L"hast gelegt", L"hat gelegt", L"haben gelegt", L"habt gelegt", L"haben gelegt"}
    }, 0, 0},
    {L"leidtun", L"dar pena (es / a alguien)", false, {
        {L"", L"", L"tut leid", L"", L"", L""},
        {L"", L"", L"tat leid", L"", L"", L""},
        {L"", L"", L"hat leidgetan", L"", L"", L""}
    }, 0, 0},
    {L"lesen", L"leer", false, {
        {L"lese", L"liest", L"liest", L"lesen", L"lest", L"lesen"},
        {L"las", L"lasest/last", L"las", L"lasen", L"last", L"lasen"},
        {L"habe gelesen", L"hast gelesen", L"hat gelesen", L"haben gelesen", L"habt gelesen", L"haben gelesen"}
    }, 0, 0},
    {L"lieben", L"amar / encantar", true, {
        {L"liebe", L"liebst", L"liebt", L"lieben", L"liebt", L"lieben"},
        {L"liebte", L"liebtest", L"liebte", L"liebten", L"liebtet", L"liebten"},
        {L"habe geliebt", L"hast geliebt", L"hat geliebt", L"haben geliebt", L"habt geliebt", L"haben geliebt"}
    }, 0, 0},
    {L"los sein", L"pasar / suceder (es)", false, {
        {L"", L"", L"ist los", L"", L"", L""},
        {L"", L"", L"war los", L"", L"", L""},
        {L"", L"", L"ist los gewesen", L"", L"", L""}
    }, 0, 0},
    {L"markieren", L"marcar", true, {
        {L"markiere", L"markierst", L"markiert", L"markieren", L"markiert", L"markieren"},
        {L"markierte", L"markiertest", L"markierte", L"markierten", L"markiertet", L"markierten"},
        {L"habe markiert", L"hast markiert", L"hat markiert", L"haben markiert", L"habt markiert", L"haben markiert"}
    }, 0, 0},
    {L"merken", L"notar / recordar", true, {
        {L"merke", L"merkst", L"merkt", L"merken", L"merkt", L"merken"},
        {L"merkte", L"merktest", L"merkte", L"merkten", L"merktet", L"merkten"},
        {L"habe gemerkt", L"hast gemerkt", L"hat gemerkt", L"haben gemerkt", L"habt gemerkt", L"haben gemerkt"}
    }, 0, 0},
    {L"mitbringen", L"traer consigo", false, {
        {L"bringe mit", L"bringst mit", L"bringt mit", L"bringen mit", L"bringt mit", L"bringen mit"},
        {L"brachte mit", L"brachtest mit", L"brachte mit", L"brachten mit", L"brachtet mit", L"brachten mit"},
        {L"habe mitgebracht", L"hast mitgebracht", L"hat mitgebracht", L"haben mitgebracht", L"habt mitgebracht", L"haben mitgebracht"}
    }, 0, 0},
    {L"mitkommen", L"venir con alguien", false, {
        {L"komme mit", L"kommst mit", L"kommt mit", L"kommen mit", L"kommt mit", L"kommen mit"},
        {L"kam mit", L"kamst mit", L"kam mit", L"kamen mit", L"kamt mit", L"kamen mit"},
        {L"bin mitgekommen", L"bist mitgekommen", L"ist mitgekommen", L"sind mitgekommen", L"seid mitgekommen", L"sind mitgekommen"}
    }, 0, 0},
    {L"mitlesen", L"seguir la lectura", false, {
        {L"lese mit", L"liest mit", L"liest mit", L"lesen mit", L"lest mit", L"lesen mit"},
        {L"las mit", L"lasest mit/last mit", L"las mit", L"lasen mit", L"last mit", L"lasen mit"},
        {L"habe mitgelesen", L"hast mitgelesen", L"hat mitgelesen", L"haben mitgelesen", L"habt mitgelesen", L"haben mitgelesen"}
    }, 0, 0},
    {L"mitmachen", L"participar", true, {
        {L"mache mit", L"machst mit", L"macht mit", L"machen mit", L"macht mit", L"machen mit"},
        {L"machte mit", L"machtest mit", L"machte mit", L"machten mit", L"machtet mit", L"machten mit"},
        {L"habe mitgemacht", L"hast mitgemacht", L"hat mitgemacht", L"haben mitgemacht", L"habt mitgemacht", L"haben mitgemacht"}
    }, 0, 0},
    {L"mitnehmen", L"llevar consigo", false, {
        {L"nehme mit", L"nimmst mit", L"nimmt mit", L"nehmen mit", L"nehmt mit", L"nehmen mit"},
        {L"nahm mit", L"nahmst mit", L"nahm mit", L"nahmen mit", L"nahmt mit", L"nahmen mit"},
        {L"habe mitgenommen", L"hast mitgenommen", L"hat mitgenommen", L"haben mitgenommen", L"habt mitgenommen", L"haben mitgenommen"}
    }, 0, 0},
    {L"mitsprechen", L"repetir al mismo tiempo", false, {
        {L"spreche mit", L"sprichst mit", L"spricht mit", L"sprechen mit", L"sprecht mit", L"sprechen mit"},
        {L"sprach mit", L"sprachst mit", L"sprach mit", L"sprachen mit", L"spracht mit", L"sprachen mit"},
        {L"habe mitgesprochen", L"hast mitgesprochen", L"hat mitgesprochen", L"haben mitgesprochen", L"habt mitgesprochen", L"haben mitgesprochen"}
    }, 0, 0},
    {L"möchten", L"quisiera / desearía (Konjunktiv II)", false, {
        {L"möchte", L"möchtest", L"möchte", L"möchten", L"möchtet", L"möchten"},
        {L"", L"", L"", L"", L"", L""},
        {L"", L"", L"", L"", L"", L""}
    }, 0, 0},
    {L"mögen", L"gustar", false, {
        {L"mag", L"magst", L"mag", L"mögen", L"mögt", L"mögen"},
        {L"mochte", L"mochtest", L"mochte", L"mochten", L"mochtet", L"mochten"},
        {L"habe gemocht", L"hast gemocht", L"hat gemocht", L"haben gemocht", L"habt gemocht", L"haben gemocht"}
    }, 0, 0},
    {L"müssen", L"tener que / deber", false, {
        {L"muss", L"musst", L"muss", L"müssen", L"müsst", L"müssen"},
        {L"musste", L"musstest", L"musste", L"mussten", L"musstet", L"mussten"},
        {L"habe gemusst", L"hast gemusst", L"hat gemusst", L"haben gemusst", L"habt gemusst", L"haben gemusst"}
    }, 0, 0},
    {L"nachsprechen", L"repetir lo oído", false, {
        {L"spreche nach", L"sprichst nach", L"spricht nach", L"sprechen nach", L"sprecht nach", L"sprechen nach"},
        {L"sprach nach", L"sprachst nach", L"sprach nach", L"sprachen nach", L"spracht nach", L"sprachen nach"},
        {L"habe nachgesprochen", L"hast nachgesprochen", L"hat nachgesprochen", L"haben nachgesprochen", L"habt nachgesprochen", L"haben nachgesprochen"}
    }, 0, 0},
    {L"nehmen", L"tomar / llevar", false, {
        {L"nehme", L"nimmst", L"nimmt", L"nehmen", L"nehmt", L"nehmen"},
        {L"nahm", L"nahmst", L"nahm", L"nahmen", L"nahmt", L"nahmen"},
        {L"habe genommen", L"hast genommen", L"hat genommen", L"haben genommen", L"habt genommen", L"haben genommen"}
    }, 0, 0},
    {L"nennen", L"nombrar", false, {
        {L"nenne", L"nennst", L"nennt", L"nennen", L"nennt", L"nennen"},
        {L"nannte", L"nanntest", L"nannte", L"nannten", L"nanntet", L"nannten"},
        {L"habe genannt", L"hast genannt", L"hat genannt", L"haben genannt", L"habt genannt", L"haben genannt"}
    }, 0, 0},
    {L"notieren", L"anotar", true, {
        {L"notiere", L"notierst", L"notiert", L"notieren", L"notiert", L"notieren"},
        {L"notierte", L"notiertest", L"notierte", L"notierten", L"notiertet", L"notierten"},
        {L"habe notiert", L"hast notiert", L"hat notiert", L"haben notiert", L"habt notiert", L"haben notiert"}
    }, 0, 0},
    {L"nummerieren", L"numerar", true, {
        {L"nummeriere", L"nummerierst", L"nummeriert", L"nummerieren", L"nummeriert", L"nummerieren"},
        {L"nummerierte", L"nummeriertest", L"nummerierte", L"nummerierten", L"nummeriertet", L"nummerierten"},
        {L"habe nummeriert", L"hast nummeriert", L"hat nummeriert", L"haben nummeriert", L"habt nummeriert", L"haben nummeriert"}
    }, 0, 0},
    {L"nutzen", L"usar / aprovechar", true, {
        {L"nutze", L"nutzt", L"nutzt", L"nutzen", L"nutzt", L"nutzen"},
        {L"nutzte", L"nutztest", L"nutzte", L"nutzten", L"nutztet", L"nutzten"},
        {L"habe genutzt", L"hast genutzt", L"hat genutzt", L"haben genutzt", L"habt genutzt", L"haben genutzt"}
    }, 0, 0},
    {L"ordnen", L"ordenar", true, {
        {L"ordne", L"ordnest", L"ordnet", L"ordnen", L"ordnet", L"ordnen"},
        {L"ordnete", L"ordnetest", L"ordnete", L"ordneten", L"ordnetet", L"ordneten"},
        {L"habe geordnet", L"hast geordnet", L"hat geordnet", L"haben geordnet", L"habt geordnet", L"haben geordnet"}
    }, 0, 0},
    {L"organisieren", L"organizar", true, {
        {L"organisiere", L"organisierst", L"organisiert", L"organisieren", L"organisiert", L"organisieren"},
        {L"organisierte", L"organisiertest", L"organisierte", L"organisierten", L"organisiertet", L"organisierten"},
        {L"habe organisiert", L"hast organisiert", L"hat organisiert", L"haben organisiert", L"habt organisiert", L"haben organisiert"}
    }, 0, 0},
    {L"passen", L"encajar / quedar bien", true, {
        {L"passe", L"passt", L"passt", L"passen", L"passt", L"passen"},
        {L"passte", L"passtest", L"passte", L"passten", L"passtet", L"passten"},
        {L"habe gepasst", L"hast gepasst", L"hat gepasst", L"haben gepasst", L"habt gepasst", L"haben gepasst"}
    }, 0, 0},
    {L"passieren", L"suceder (es)", true, {
        {L"", L"", L"passiert", L"", L"", L""},
        {L"", L"", L"passierte", L"", L"", L""},
        {L"", L"", L"ist passiert", L"", L"", L""}
    }, 0, 0},
    {L"planen", L"planificar", true, {
        {L"plane", L"planst", L"plant", L"planen", L"plant", L"planen"},
        {L"plante", L"plantest", L"plante", L"planten", L"plantet", L"planten"},
        {L"habe geplant", L"hast geplant", L"hat geplant", L"haben geplant", L"habt geplant", L"haben geplant"}
    }, 0, 0},
    {L"probieren", L"probar", true, {
        {L"probiere", L"probierst", L"probiert", L"probieren", L"probiert", L"probieren"},
        {L"probierte", L"probiertest", L"probierte", L"probierten", L"probiertet", L"probierten"},
        {L"habe probiert", L"hast probiert", L"hat probiert", L"haben probiert", L"habt probiert", L"haben probiert"}
    }, 0, 0},
    {L"präsentieren", L"presentar", true, {
        {L"präsentiere", L"präsentierst", L"präsentiert", L"präsentieren", L"präsentiert", L"präsentieren"},
        {L"präsentierte", L"präsentiertest", L"präsentierte", L"präsentierten", L"präsentiertet", L"präsentierten"},
        {L"habe präsentiert", L"hast präsentiert", L"hat präsentiert", L"haben präsentiert", L"habt präsentiert", L"haben präsentiert"}
    }, 0, 0},
    {L"raten", L"adivinar / aconsejar", false, {
        {L"rate", L"rätst", L"rät", L"raten", L"ratet", L"raten"},
        {L"riet", L"rietest", L"riet", L"rieten", L"rietet", L"rieten"},
        {L"habe geraten", L"hast geraten", L"hat geraten", L"haben geraten", L"habt geraten", L"haben geraten"}
    }, 0, 0},
    {L"rauchen", L"fumar", true, {
        {L"rauche", L"rauchst", L"raucht", L"rauchen", L"raucht", L"rauchen"},
        {L"rauchte", L"rauchtest", L"rauchte", L"rauchten", L"rauchtet", L"rauchten"},
        {L"habe geraucht", L"hast geraucht", L"hat geraucht", L"haben geraucht", L"habt geraucht", L"haben geraucht"}
    }, 0, 0},
    {L"reagieren", L"reaccionar", true, {
        {L"reagiere", L"reagierst", L"reagiert", L"reagieren", L"reagiert", L"reagieren"},
        {L"reagierte", L"reagiertest", L"reagierte", L"reagierten", L"reagiertet", L"reagierten"},
        {L"habe reagiert", L"hast reagiert", L"hat reagiert", L"haben reagiert", L"habt reagiert", L"haben reagiert"}
    }, 0, 0},
    {L"recherchieren", L"investigar", true, {
        {L"recherchiere", L"recherchierst", L"recherchiert", L"recherchieren", L"recherchiert", L"recherchieren"},
        {L"recherchierte", L"recherchiertest", L"recherchierte", L"recherchierten", L"recherchiertet", L"recherchierten"},
        {L"habe recherchiert", L"hast recherchiert", L"hat recherchiert", L"haben recherchiert", L"habt recherchiert", L"haben recherchiert"}
    }, 0, 0},
    {L"reisen", L"viajar", true, {
        {L"reise", L"reist", L"reist", L"reisen", L"reist", L"reisen"},
        {L"reiste", L"reistest", L"reiste", L"reisten", L"reistet", L"reisten"},
        {L"bin gereist", L"bist gereist", L"ist gereist", L"sind gereist", L"seid gereist", L"sind gereist"}
    }, 0, 0},
    {L"sagen", L"decir", true, {
        {L"sage", L"sagst", L"sagt", L"sagen", L"sagt", L"sagen"},
        {L"sagte", L"sagtest", L"sagte", L"sagten", L"sagtet", L"sagten"},
        {L"habe gesagt", L"hast gesagt", L"hat gesagt", L"haben gesagt", L"habt gesagt", L"haben gesagt"}
    }, 0, 0},
    {L"sammeln", L"reunir / coleccionar", true, {
        {L"sammle/sammele", L"sammelst", L"sammelt", L"sammeln", L"sammelt", L"sammeln"},
        {L"sammelte", L"sammeltest", L"sammelte", L"sammelten", L"sammeltet", L"sammelten"},
        {L"habe gesammelt", L"hast gesammelt", L"hat gesammelt", L"haben gesammelt", L"habt gesammelt", L"haben gesammelt"}
    }, 0, 0},
    {L"scannen", L"escanear", true, {
        {L"scanne", L"scannst", L"scannt", L"scannen", L"scannt", L"scannen"},
        {L"scannte", L"scanntest", L"scannte", L"scannten", L"scanntet", L"scannten"},
        {L"habe gescannt", L"hast gescannt", L"hat gescannt", L"haben gescannt", L"habt gescannt", L"haben gescannt"}
    }, 0, 0},
    {L"schenken", L"regalar", true, {
        {L"schenke", L"schenkst", L"schenkt", L"schenken", L"schenkt", L"schenken"},
        {L"schenkte", L"schenktest", L"schenkte", L"schenkten", L"schenktet", L"schenkten"},
        {L"habe geschenkt", L"hast geschenkt", L"hat geschenkt", L"haben geschenkt", L"habt geschenkt", L"haben geschenkt"}
    }, 0, 0},
    {L"schicken", L"enviar", true, {
        {L"schicke", L"schickst", L"schickt", L"schicken", L"schickt", L"schicken"},
        {L"schickte", L"schicktest", L"schickte", L"schickten", L"schicktet", L"schickten"},
        {L"habe geschickt", L"hast geschickt", L"hat geschickt", L"haben geschickt", L"habt geschickt", L"haben geschickt"}
    }, 0, 0},
    {L"schlafen", L"dormir", false, {
        {L"schlafe", L"schläfst", L"schläft", L"schlafen", L"schlaft", L"schlafen"},
        {L"schlief", L"schliefst", L"schlief", L"schliefen", L"schlieft", L"schliefen"},
        {L"habe geschlafen", L"hast geschlafen", L"hat geschlafen", L"haben geschlafen", L"habt geschlafen", L"haben geschlafen"}
    }, 0, 0},
    {L"schmecken", L"tener sabor / gustar", true, {
        {L"schmecke", L"schmeckst", L"schmeckt", L"schmecken", L"schmeckt", L"schmecken"},
        {L"schmeckte", L"schmecktest", L"schmeckte", L"schmeckten", L"schmecktet", L"schmeckten"},
        {L"habe geschmeckt", L"hast geschmeckt", L"hat geschmeckt", L"haben geschmeckt", L"habt geschmeckt", L"haben geschmeckt"}
    }, 0, 0},
    {L"schneiden", L"cortar", false, {
        {L"schneide", L"schneidest", L"schneidet", L"schneiden", L"schneidet", L"schneiden"},
        {L"schnitt", L"schnittest", L"schnitt", L"schnitten", L"schnittet", L"schnitten"},
        {L"habe geschnitten", L"hast geschnitten", L"hat geschnitten", L"haben geschnitten", L"habt geschnitten", L"haben geschnitten"}
    }, 0, 0},
    {L"schreiben", L"escribir", false, {
        {L"schreibe", L"schreibst", L"schreibt", L"schreiben", L"schreibt", L"schreiben"},
        {L"schrieb", L"schriebst", L"schrieb", L"schrieben", L"schriebt", L"schrieben"},
        {L"habe geschrieben", L"hast geschrieben", L"hat geschrieben", L"haben geschrieben", L"habt geschrieben", L"haben geschrieben"}
    }, 0, 0},
    {L"schwimmen", L"nadar", false, {
        {L"schwimme", L"schwimmst", L"schwimmt", L"schwimmen", L"schwimmt", L"schwimmen"},
        {L"schwamm", L"schwammst", L"schwamm", L"schwammen", L"schwammt", L"schwammen"},
        {L"bin geschwommen/habe geschwommen", L"bist geschwommen/hast geschwommen", L"ist geschwommen/hat geschwommen", L"sind geschwommen/haben geschwommen", L"seid geschwommen/habt geschwommen", L"sind geschwommen/haben geschwommen"}
    }, 0, 0},
    {L"schälen", L"pelar", true, {
        {L"schäle", L"schälst", L"schält", L"schälen", L"schält", L"schälen"},
        {L"schälte", L"schältest", L"schälte", L"schälten", L"schältet", L"schälten"},
        {L"habe geschält", L"hast geschält", L"hat geschält", L"haben geschält", L"habt geschält", L"haben geschält"}
    }, 0, 0},
    {L"sich bedanken", L"agradecer", true, {
        {L"bedanke mich", L"bedankst dich", L"bedankt sich", L"bedanken uns", L"bedankt euch", L"bedanken sich"},
        {L"bedankte mich", L"bedanktest dich", L"bedankte sich", L"bedankten uns", L"bedanktet euch", L"bedankten sich"},
        {L"habe mich bedankt", L"hast dich bedankt", L"hat sich bedankt", L"haben uns bedankt", L"habt euch bedankt", L"haben sich bedankt"}
    }, 0, 0},
    {L"sich verabreden", L"quedar / acordar una cita", true, {
        {L"verabrede mich", L"verabredest dich", L"verabredet sich", L"verabreden uns", L"verabredet euch", L"verabreden sich"},
        {L"verabredete mich", L"verabredetest dich", L"verabredete sich", L"verabredeten uns", L"verabredetet euch", L"verabredeten sich"},
        {L"habe mich verabredet", L"hast dich verabredet", L"hat sich verabredet", L"haben uns verabredet", L"habt euch verabredet", L"haben sich verabredet"}
    }, 0, 0},
    {L"sich verabschieden", L"despedirse", true, {
        {L"verabschiede mich", L"verabschiedest dich", L"verabschiedet sich", L"verabschieden uns", L"verabschiedet euch", L"verabschieden sich"},
        {L"verabschiedete mich", L"verabschiedetest dich", L"verabschiedete sich", L"verabschiedeten uns", L"verabschiedetet euch", L"verabschiedeten sich"},
        {L"habe mich verabschiedet", L"hast dich verabschiedet", L"hat sich verabschiedet", L"haben uns verabschiedet", L"habt euch verabschiedet", L"haben sich verabschiedet"}
    }, 0, 0},
    {L"singen", L"cantar", false, {
        {L"singe", L"singst", L"singt", L"singen", L"singt", L"singen"},
        {L"sang", L"sangst", L"sang", L"sangen", L"sangt", L"sangen"},
        {L"habe gesungen", L"hast gesungen", L"hat gesungen", L"haben gesungen", L"habt gesungen", L"haben gesungen"}
    }, 0, 0},
    {L"sitzen", L"estar sentado", false, {
        {L"sitze", L"sitzt", L"sitzt", L"sitzen", L"sitzt", L"sitzen"},
        {L"saß", L"saßest/saßt", L"saß", L"saßen", L"saßt", L"saßen"},
        {L"habe gesessen/bin gesessen", L"hast gesessen/bist gesessen", L"hat gesessen/ist gesessen", L"haben gesessen/sind gesessen", L"habt gesessen/seid gesessen", L"haben gesessen/sind gesessen"}
    }, 0, 0},
    {L"sortieren", L"clasificar / ordenar", true, {
        {L"sortiere", L"sortierst", L"sortiert", L"sortieren", L"sortiert", L"sortieren"},
        {L"sortierte", L"sortiertest", L"sortierte", L"sortierten", L"sortiertet", L"sortierten"},
        {L"habe sortiert", L"hast sortiert", L"hat sortiert", L"haben sortiert", L"habt sortiert", L"haben sortiert"}
    }, 0, 0},
    {L"speichern", L"guardar", true, {
        {L"speichere", L"speicherst", L"speichert", L"speichern", L"speichert", L"speichern"},
        {L"speicherte", L"speichertest", L"speicherte", L"speicherten", L"speichertet", L"speicherten"},
        {L"habe gespeichert", L"hast gespeichert", L"hat gespeichert", L"haben gespeichert", L"habt gespeichert", L"haben gespeichert"}
    }, 0, 0},
    {L"starten", L"iniciar", true, {
        {L"starte", L"startest", L"startet", L"starten", L"startet", L"starten"},
        {L"startete", L"startetest", L"startete", L"starteten", L"startetet", L"starteten"},
        {L"habe gestartet", L"hast gestartet", L"hat gestartet", L"haben gestartet", L"habt gestartet", L"haben gestartet"}
    }, 0, 0},
    {L"stehen", L"estar de pie", false, {
        {L"stehe", L"stehst", L"steht", L"stehen", L"steht", L"stehen"},
        {L"stand", L"standest", L"stand", L"standen", L"standet", L"standen"},
        {L"habe gestanden/bin gestanden", L"hast gestanden/bist gestanden", L"hat gestanden/ist gestanden", L"haben gestanden/sind gestanden", L"habt gestanden/seid gestanden", L"haben gestanden/sind gestanden"}
    }, 0, 0},
    {L"stellen", L"poner / plantear", true, {
        {L"stelle", L"stellst", L"stellt", L"stellen", L"stellt", L"stellen"},
        {L"stellte", L"stelltest", L"stellte", L"stellten", L"stelltet", L"stellten"},
        {L"habe gestellt", L"hast gestellt", L"hat gestellt", L"haben gestellt", L"habt gestellt", L"haben gestellt"}
    }, 0, 0},
    {L"stimmen", L"ser correcto", true, {
        {L"stimme", L"stimmst", L"stimmt", L"stimmen", L"stimmt", L"stimmen"},
        {L"stimmte", L"stimmtest", L"stimmte", L"stimmten", L"stimmtet", L"stimmten"},
        {L"habe gestimmt", L"hast gestimmt", L"hat gestimmt", L"haben gestimmt", L"habt gestimmt", L"haben gestimmt"}
    }, 0, 0},
    {L"streichen", L"tachar", false, {
        {L"streiche", L"streichst", L"streicht", L"streichen", L"streicht", L"streichen"},
        {L"strich", L"strichst", L"strich", L"strichen", L"stricht", L"strichen"},
        {L"habe gestrichen", L"hast gestrichen", L"hat gestrichen", L"haben gestrichen", L"habt gestrichen", L"haben gestrichen"}
    }, 0, 0},
    {L"strukturieren", L"estructurar", true, {
        {L"strukturiere", L"strukturierst", L"strukturiert", L"strukturieren", L"strukturiert", L"strukturieren"},
        {L"strukturierte", L"strukturiertest", L"strukturierte", L"strukturierten", L"strukturiertet", L"strukturierten"},
        {L"habe strukturiert", L"hast strukturiert", L"hat strukturiert", L"haben strukturiert", L"habt strukturiert", L"haben strukturiert"}
    }, 0, 0},
    {L"studieren", L"estudiar en la universidad", true, {
        {L"studiere", L"studierst", L"studiert", L"studieren", L"studiert", L"studieren"},
        {L"studierte", L"studiertest", L"studierte", L"studierten", L"studiertet", L"studierten"},
        {L"habe studiert", L"hast studiert", L"hat studiert", L"haben studiert", L"habt studiert", L"haben studiert"}
    }, 0, 0},
    {L"suchen", L"buscar", true, {
        {L"suche", L"suchst", L"sucht", L"suchen", L"sucht", L"suchen"},
        {L"suchte", L"suchtest", L"suchte", L"suchten", L"suchtet", L"suchten"},
        {L"habe gesucht", L"hast gesucht", L"hat gesucht", L"haben gesucht", L"habt gesucht", L"haben gesucht"}
    }, 0, 0},
    {L"tanzen", L"bailar", true, {
        {L"tanze", L"tanzt", L"tanzt", L"tanzen", L"tanzt", L"tanzen"},
        {L"tanzte", L"tanztest", L"tanzte", L"tanzten", L"tanztet", L"tanzten"},
        {L"habe getanzt", L"hast getanzt", L"hat getanzt", L"haben getanzt", L"habt getanzt", L"haben getanzt"}
    }, 0, 0},
    {L"tauschen", L"intercambiar", true, {
        {L"tausche", L"tauschst", L"tauscht", L"tauschen", L"tauscht", L"tauschen"},
        {L"tauschte", L"tauschtest", L"tauschte", L"tauschten", L"tauschtet", L"tauschten"},
        {L"habe getauscht", L"hast getauscht", L"hat getauscht", L"haben getauscht", L"habt getauscht", L"haben getauscht"}
    }, 0, 0},
    {L"telefonieren", L"hablar por teléfono", true, {
        {L"telefoniere", L"telefonierst", L"telefoniert", L"telefonieren", L"telefoniert", L"telefonieren"},
        {L"telefonierte", L"telefoniertest", L"telefonierte", L"telefonierten", L"telefoniertet", L"telefonierten"},
        {L"habe telefoniert", L"hast telefoniert", L"hat telefoniert", L"haben telefoniert", L"habt telefoniert", L"haben telefoniert"}
    }, 0, 0},
    {L"trainieren", L"entrenar", true, {
        {L"trainiere", L"trainierst", L"trainiert", L"trainieren", L"trainiert", L"trainieren"},
        {L"trainierte", L"trainiertest", L"trainierte", L"trainierten", L"trainiertet", L"trainierten"},
        {L"habe trainiert", L"hast trainiert", L"hat trainiert", L"haben trainiert", L"habt trainiert", L"haben trainiert"}
    }, 0, 0},
    {L"treffen", L"encontrarse con alguien", false, {
        {L"treffe", L"triffst", L"trifft", L"treffen", L"trefft", L"treffen"},
        {L"traf", L"trafst", L"traf", L"trafen", L"traft", L"trafen"},
        {L"habe getroffen", L"hast getroffen", L"hat getroffen", L"haben getroffen", L"habt getroffen", L"haben getroffen"}
    }, 0, 0},
    {L"trinken", L"beber", false, {
        {L"trinke", L"trinkst", L"trinkt", L"trinken", L"trinkt", L"trinken"},
        {L"trank", L"trankst", L"trank", L"tranken", L"trankt", L"tranken"},
        {L"habe getrunken", L"hast getrunken", L"hat getrunken", L"haben getrunken", L"habt getrunken", L"haben getrunken"}
    }, 0, 0},
    {L"tun", L"hacer", false, {
        {L"tue", L"tust", L"tut", L"tun", L"tut", L"tun"},
        {L"tat", L"tatest", L"tat", L"taten", L"tatet", L"taten"},
        {L"habe getan", L"hast getan", L"hat getan", L"haben getan", L"habt getan", L"haben getan"}
    }, 0, 0},
    {L"unterstreichen", L"subrayar", false, {
        {L"unterstreiche", L"unterstreichst", L"unterstreicht", L"unterstreichen", L"unterstreicht", L"unterstreichen"},
        {L"unterstrich", L"unterstrichst", L"unterstrich", L"unterstrichen", L"unterstricht", L"unterstrichen"},
        {L"habe unterstrichen", L"hast unterstrichen", L"hat unterstrichen", L"haben unterstrichen", L"habt unterstrichen", L"haben unterstrichen"}
    }, 0, 0},
    {L"variieren", L"variar", true, {
        {L"variiere", L"variierst", L"variiert", L"variieren", L"variiert", L"variieren"},
        {L"variierte", L"variiertest", L"variierte", L"variierten", L"variiertet", L"variierten"},
        {L"habe variiert", L"hast variiert", L"hat variiert", L"haben variiert", L"habt variiert", L"haben variiert"}
    }, 0, 0},
    {L"verbinden", L"unir / conectar", false, {
        {L"verbinde", L"verbindest", L"verbindet", L"verbinden", L"verbindet", L"verbinden"},
        {L"verband", L"verbandest", L"verband", L"verbanden", L"verbandet", L"verbanden"},
        {L"habe verbunden", L"hast verbunden", L"hat verbunden", L"haben verbunden", L"habt verbunden", L"haben verbunden"}
    }, 0, 0},
    {L"vereinbaren", L"acordar", true, {
        {L"vereinbare", L"vereinbarst", L"vereinbart", L"vereinbaren", L"vereinbart", L"vereinbaren"},
        {L"vereinbarte", L"vereinbartest", L"vereinbarte", L"vereinbarten", L"vereinbartet", L"vereinbarten"},
        {L"habe vereinbart", L"hast vereinbart", L"hat vereinbart", L"haben vereinbart", L"habt vereinbart", L"haben vereinbart"}
    }, 0, 0},
    {L"vergleichen", L"comparar", false, {
        {L"vergleiche", L"vergleichst", L"vergleicht", L"vergleichen", L"vergleicht", L"vergleichen"},
        {L"verglich", L"verglichst", L"verglich", L"verglichen", L"verglicht", L"verglichen"},
        {L"habe verglichen", L"hast verglichen", L"hat verglichen", L"haben verglichen", L"habt verglichen", L"haben verglichen"}
    }, 0, 0},
    {L"verstehen", L"entender", false, {
        {L"verstehe", L"verstehst", L"versteht", L"verstehen", L"versteht", L"verstehen"},
        {L"verstand", L"verstandest", L"verstand", L"verstanden", L"verstandet", L"verstanden"},
        {L"habe verstanden", L"hast verstanden", L"hat verstanden", L"haben verstanden", L"habt verstanden", L"haben verstanden"}
    }, 0, 0},
    {L"vorbereiten", L"preparar", true, {
        {L"bereite vor", L"bereitest vor", L"bereitet vor", L"bereiten vor", L"bereitet vor", L"bereiten vor"},
        {L"bereitete vor", L"bereitetest vor", L"bereitete vor", L"bereiteten vor", L"bereitetet vor", L"bereiteten vor"},
        {L"habe vorbereitet", L"hast vorbereitet", L"hat vorbereitet", L"haben vorbereitet", L"habt vorbereitet", L"haben vorbereitet"}
    }, 0, 0},
    {L"vorstellen", L"presentar a alguien", true, {
        {L"stelle vor", L"stellst vor", L"stellt vor", L"stellen vor", L"stellt vor", L"stellen vor"},
        {L"stellte vor", L"stelltest vor", L"stellte vor", L"stellten vor", L"stelltet vor", L"stellten vor"},
        {L"habe vorgestellt", L"hast vorgestellt", L"hat vorgestellt", L"haben vorgestellt", L"habt vorgestellt", L"haben vorgestellt"}
    }, 0, 0},
    {L"wandern", L"hacer senderismo", true, {
        {L"wandere", L"wanderst", L"wandert", L"wandern", L"wandert", L"wandern"},
        {L"wanderte", L"wandertest", L"wanderte", L"wanderten", L"wandertet", L"wanderten"},
        {L"bin gewandert", L"bist gewandert", L"ist gewandert", L"sind gewandert", L"seid gewandert", L"sind gewandert"}
    }, 0, 0},
    {L"warten", L"esperar", true, {
        {L"warte", L"wartest", L"wartet", L"warten", L"wartet", L"warten"},
        {L"wartete", L"wartetest", L"wartete", L"warteten", L"wartetet", L"warteten"},
        {L"habe gewartet", L"hast gewartet", L"hat gewartet", L"haben gewartet", L"habt gewartet", L"haben gewartet"}
    }, 0, 0},
    {L"waschen", L"lavar", false, {
        {L"wasche", L"wäschst", L"wäscht", L"waschen", L"wascht", L"waschen"},
        {L"wusch", L"wuschst", L"wusch", L"wuschen", L"wuscht", L"wuschen"},
        {L"habe gewaschen", L"hast gewaschen", L"hat gewaschen", L"haben gewaschen", L"habt gewaschen", L"haben gewaschen"}
    }, 0, 0},
    {L"wechseln", L"cambiar", true, {
        {L"wechsle/wechsele", L"wechselst", L"wechselt", L"wechseln", L"wechselt", L"wechseln"},
        {L"wechselte", L"wechseltest", L"wechselte", L"wechselten", L"wechseltet", L"wechselten"},
        {L"habe gewechselt", L"hast gewechselt", L"hat gewechselt", L"haben gewechselt", L"habt gewechselt", L"haben gewechselt"}
    }, 0, 0},
    {L"werden", L"volverse / llegar a ser", false, {
        {L"werde", L"wirst", L"wird", L"werden", L"werdet", L"werden"},
        {L"wurde", L"wurdest", L"wurde", L"wurden", L"wurdet", L"wurden"},
        {L"bin geworden", L"bist geworden", L"ist geworden", L"sind geworden", L"seid geworden", L"sind geworden"}
    }, 0, 0},
    {L"werfen", L"lanzar", false, {
        {L"werfe", L"wirfst", L"wirft", L"werfen", L"werft", L"werfen"},
        {L"warf", L"warfst", L"warf", L"warfen", L"warft", L"warfen"},
        {L"habe geworfen", L"hast geworfen", L"hat geworfen", L"haben geworfen", L"habt geworfen", L"haben geworfen"}
    }, 0, 0},
    {L"wiederholen", L"repetir", true, {
        {L"wiederhole", L"wiederholst", L"wiederholt", L"wiederholen", L"wiederholt", L"wiederholen"},
        {L"wiederholte", L"wiederholtest", L"wiederholte", L"wiederholten", L"wiederholtet", L"wiederholten"},
        {L"habe wiederholt", L"hast wiederholt", L"hat wiederholt", L"haben wiederholt", L"habt wiederholt", L"haben wiederholt"}
    }, 0, 0},
    {L"wissen", L"saber", false, {
        {L"weiß", L"weißt", L"weiß", L"wissen", L"wisst", L"wissen"},
        {L"wusste", L"wusstest", L"wusste", L"wussten", L"wusstet", L"wussten"},
        {L"habe gewusst", L"hast gewusst", L"hat gewusst", L"haben gewusst", L"habt gewusst", L"haben gewusst"}
    }, 0, 0},
    {L"wollen", L"querer", false, {
        {L"will", L"willst", L"will", L"wollen", L"wollt", L"wollen"},
        {L"wollte", L"wolltest", L"wollte", L"wollten", L"wolltet", L"wollten"},
        {L"habe gewollt", L"hast gewollt", L"hat gewollt", L"haben gewollt", L"habt gewollt", L"haben gewollt"}
    }, 0, 0},
    {L"wählen", L"elegir", true, {
        {L"wähle", L"wählst", L"wählt", L"wählen", L"wählt", L"wählen"},
        {L"wählte", L"wähltest", L"wählte", L"wählten", L"wähltet", L"wählten"},
        {L"habe gewählt", L"hast gewählt", L"hat gewählt", L"haben gewählt", L"habt gewählt", L"haben gewählt"}
    }, 0, 0},
    {L"würfeln", L"tirar el dado", true, {
        {L"würfle/würfele", L"würfelst", L"würfelt", L"würfeln", L"würfelt", L"würfeln"},
        {L"würfelte", L"würfeltest", L"würfelte", L"würfelten", L"würfeltet", L"würfelten"},
        {L"habe gewürfelt", L"hast gewürfelt", L"hat gewürfelt", L"haben gewürfelt", L"habt gewürfelt", L"haben gewürfelt"}
    }, 0, 0},
    {L"zahlen", L"pagar la cuenta", true, {
        {L"zahle", L"zahlst", L"zahlt", L"zahlen", L"zahlt", L"zahlen"},
        {L"zahlte", L"zahltest", L"zahlte", L"zahlten", L"zahltet", L"zahlten"},
        {L"habe gezahlt", L"hast gezahlt", L"hat gezahlt", L"haben gezahlt", L"habt gezahlt", L"haben gezahlt"}
    }, 0, 0},
    {L"zeichnen", L"dibujar", true, {
        {L"zeichne", L"zeichnest", L"zeichnet", L"zeichnen", L"zeichnet", L"zeichnen"},
        {L"zeichnete", L"zeichnetest", L"zeichnete", L"zeichneten", L"zeichnetet", L"zeichneten"},
        {L"habe gezeichnet", L"hast gezeichnet", L"hat gezeichnet", L"haben gezeichnet", L"habt gezeichnet", L"haben gezeichnet"}
    }, 0, 0},
    {L"zeigen", L"mostrar", true, {
        {L"zeige", L"zeigst", L"zeigt", L"zeigen", L"zeigt", L"zeigen"},
        {L"zeigte", L"zeigtest", L"zeigte", L"zeigten", L"zeigtet", L"zeigten"},
        {L"habe gezeigt", L"hast gezeigt", L"hat gezeigt", L"haben gezeigt", L"habt gezeigt", L"haben gezeigt"}
    }, 0, 0},
    {L"ziehen", L"sacar / tirar de algo", false, {
        {L"ziehe", L"ziehst", L"zieht", L"ziehen", L"zieht", L"ziehen"},
        {L"zog", L"zogst", L"zog", L"zogen", L"zogt", L"zogen"},
        {L"habe gezogen", L"hast gezogen", L"hat gezogen", L"haben gezogen", L"habt gezogen", L"haben gezogen"}
    }, 0, 0},
    {L"zu Mittag essen", L"almorzar", false, {
        {L"esse zu Mittag", L"isst zu Mittag", L"isst zu Mittag", L"essen zu Mittag", L"esst zu Mittag", L"essen zu Mittag"},
        {L"aß zu Mittag", L"aßest zu Mittag/aßt zu Mittag", L"aß zu Mittag", L"aßen zu Mittag", L"aßt zu Mittag", L"aßen zu Mittag"},
        {L"habe zu Mittag gegessen", L"hast zu Mittag gegessen", L"hat zu Mittag gegessen", L"haben zu Mittag gegessen", L"habt zu Mittag gegessen", L"haben zu Mittag gegessen"}
    }, 0, 0},
    {L"zubereiten", L"preparar comida", true, {
        {L"bereite zu", L"bereitest zu", L"bereitet zu", L"bereiten zu", L"bereitet zu", L"bereiten zu"},
        {L"bereitete zu", L"bereitetest zu", L"bereitete zu", L"bereiteten zu", L"bereitetet zu", L"bereiteten zu"},
        {L"habe zubereitet", L"hast zubereitet", L"hat zubereitet", L"haben zubereitet", L"habt zubereitet", L"haben zubereitet"}
    }, 0, 0},
    {L"zuordnen", L"asignar / relacionar", true, {
        {L"ordne zu", L"ordnest zu", L"ordnet zu", L"ordnen zu", L"ordnet zu", L"ordnen zu"},
        {L"ordnete zu", L"ordnetest zu", L"ordnete zu", L"ordneten zu", L"ordnetet zu", L"ordneten zu"},
        {L"habe zugeordnet", L"hast zugeordnet", L"hat zugeordnet", L"haben zugeordnet", L"habt zugeordnet", L"haben zugeordnet"}
    }, 0, 0},
    {L"zusammengehören", L"pertenecer al mismo conjunto", true, {
        {L"gehöre zusammen", L"gehörst zusammen", L"gehört zusammen", L"gehören zusammen", L"gehört zusammen", L"gehören zusammen"},
        {L"gehörte zusammen", L"gehörtest zusammen", L"gehörte zusammen", L"gehörten zusammen", L"gehörtet zusammen", L"gehörten zusammen"},
        {L"habe zusammengehört", L"hast zusammengehört", L"hat zusammengehört", L"haben zusammengehört", L"habt zusammengehört", L"haben zusammengehört"}
    }, 0, 0},
    {L"zusammenpassen", L"combinar / encajar entre sí", true, {
        {L"passe zusammen", L"passt zusammen", L"passt zusammen", L"passen zusammen", L"passt zusammen", L"passen zusammen"},
        {L"passte zusammen", L"passtest zusammen", L"passte zusammen", L"passten zusammen", L"passtet zusammen", L"passten zusammen"},
        {L"habe zusammengepasst", L"hast zusammengepasst", L"hat zusammengepasst", L"haben zusammengepasst", L"habt zusammengepasst", L"haben zusammengepasst"}
    }, 0, 0},
    {L"üben", L"practicar", true, {
        {L"übe", L"übst", L"übt", L"üben", L"übt", L"üben"},
        {L"übte", L"übtest", L"übte", L"übten", L"übtet", L"übten"},
        {L"habe geübt", L"hast geübt", L"hat geübt", L"haben geübt", L"habt geübt", L"haben geübt"}
    }, 0, 0},
    {L"überlegen", L"pensar / considerar", true, {
        {L"überlege", L"überlegst", L"überlegt", L"überlegen", L"überlegt", L"überlegen"},
        {L"überlegte", L"überlegtest", L"überlegte", L"überlegten", L"überlegtet", L"überlegten"},
        {L"habe überlegt", L"hast überlegt", L"hat überlegt", L"haben überlegt", L"habt überlegt", L"haben überlegt"}
    }, 0, 0},
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
            if (c == L'Ä') c = L'ä';
            if (c == L'Ö') c = L'ö';
            if (c == L'Ü') c = L'ü';
            salida += (wchar_t)towlower(c);
            espacioPendiente = false;
        }
    }
    return salida;
}

// Alternatives use / in data, e.g. Pizzas/Pizzen.
bool RespuestaValida(const wstring& entrada, const wstring& alternativas) {
    wstring respuesta = Normalizar(entrada);
    if (respuesta.empty()) return false;
    size_t inicio = 0;
    while (inicio <= alternativas.size()) {
        size_t fin = alternativas.find(L'/', inicio);
        wstring opcion = alternativas.substr(inicio, fin == wstring::npos ? fin : fin - inicio);
        if (respuesta == Normalizar(opcion)) return true;
        if (fin == wstring::npos) break;
        inicio = fin + 1;
    }
    return false;
}

vector<wstring> Significados(const wstring& texto) {
    vector<wstring> partes;
    wistringstream entrada(texto);
    wstring parte;
    while (getline(entrada, parte, L'/')) {
        size_t nota = parte.find(L'(');
        if (nota != wstring::npos) parte = parte.substr(0, nota);
        parte = Normalizar(parte);
        if (!parte.empty()) partes.push_back(parte);
    }
    return partes;
}

bool CompartenSignificado(const wstring& a, const wstring& b) {
    vector<wstring> aa = Significados(a), bb = Significados(b);
    for (const auto& x : aa) for (const auto& y : bb)
        if (!x.empty() && x == y) return true;
    return false;
}

bool RespuestaAlemanValida(const wstring& respuesta, int indice) {
    if (Normalizar(respuesta).empty()) return false;
    for (const auto& p : palabras) {
        if (!CompartenSignificado(p.espanol, palabras[indice].espanol)) continue;
        if (Normalizar(respuesta) == Normalizar(p.singular)) return true;
        for (const auto& articulo : Significados(p.articulo))
            if (Normalizar(respuesta) == Normalizar(articulo + L" " + p.singular)) return true;
    }
    return false;
}

int DistanciaEdicion(const wstring& a, const wstring& b) {
    vector<int> anterior(b.size() + 1), actual(b.size() + 1);
    for (size_t j = 0; j <= b.size(); ++j) anterior[j] = (int)j;
    for (size_t i = 1; i <= a.size(); ++i) {
        actual[0] = (int)i;
        for (size_t j = 1; j <= b.size(); ++j)
            actual[j] = min(min(actual[j-1]+1, anterior[j]+1),
                            anterior[j-1] + (a[i-1] == b[j-1] ? 0 : 1));
        anterior.swap(actual);
    }
    return anterior[b.size()];
}

int Parecido(const wstring& a, const wstring& b) {
    wstring x = Normalizar(a), y = Normalizar(b);
    int largo = (int)max(x.size(), y.size());
    if (!largo) return 0;
    return 100 - 100 * DistanciaEdicion(x, y) / largo;
}

int PuntajeDistractor(int objetivo, int candidato) {
    int puntaje = 0;
    for (const auto& a : Significados(palabras[objetivo].espanol))
        for (const auto& b : Significados(palabras[candidato].espanol))
            puntaje = max(puntaje, Parecido(a, b));
    // Also favour related German word families (Lehrer/Lehrerin, etc.).
    puntaje = max(puntaje, Parecido(palabras[objetivo].singular,
                                  palabras[candidato].singular) * 9 / 10);
    return puntaje;
}

bool EstaDominada(const Palabra& p) {
    int total = p.correctas + p.incorrectas;
    return p.correctas - p.incorrectas >= 10 ||
           (total >= 3 && p.correctas * 100 / total >= 75);
}

// Minimum additional correct answers with no new errors, via either rule.
int AciertosParaDominar(const Palabra& p) {
    int porPorcentaje = max(0, max(3 - p.correctas - p.incorrectas,
                                 3 * p.incorrectas - p.correctas));
    int porVentaja = max(0, 10 - p.correctas + p.incorrectas);
    return min(porPorcentaje, porVentaja);
}

bool EstaCercaDeDominar(const Palabra& p) {
    return p.correctas + p.incorrectas > 0 && !EstaDominada(p) &&
           AciertosParaDominar(p) <= 3;
}

vector<int> IndicesCercanas() {
    vector<int> indices;
    for (size_t i = 0; i < palabras.size(); ++i)
        if (EstaCercaDeDominar(palabras[i])) indices.push_back((int)i);
    sort(indices.begin(), indices.end(), [](int a, int b) {
        int faltanA = AciertosParaDominar(palabras[a]);
        int faltanB = AciertosParaDominar(palabras[b]);
        if (faltanA != faltanB) return faltanA < faltanB;
        return Normalizar(palabras[a].singular) < Normalizar(palabras[b].singular);
    });
    return indices;
}

vector<int> IndicesDominadas() {
    vector<int> indices;
    for (size_t i = 0; i < palabras.size(); ++i)
        if (EstaDominada(palabras[i])) indices.push_back((int)i);
    sort(indices.begin(), indices.end(), [](int a, int b) {
        return Normalizar(palabras[a].singular) < Normalizar(palabras[b].singular);
    });
    return indices;
}

bool PalabraDisponible(int indice, ModoPractica modo) {
    if (modo == MODO_PLURALES) return !palabras[indice].plural.empty();
    if (modo == MODO_ARTICULOS) return !palabras[indice].articulo.empty();
    return true;
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
    for (size_t i = 0; i < cantidadPalabras; ++i) {
        int bien = 0, mal = 0;
        if (!(archivo >> bien >> mal)) return;
        if (i < palabras.size()) { palabras[i].correctas = bien; palabras[i].incorrectas = mal; }
    }

    size_t cantidadVerbos = 0;
    archivo >> cantidadVerbos;
    for (size_t i = 0; i < cantidadVerbos; ++i) {
        int bien = 0, mal = 0;
        if (!(archivo >> bien >> mal)) return;
        if (i < verbos.size()) { verbos[i].correctas = bien; verbos[i].incorrectas = mal; }
    }
}

double PrecisionGlobal() {
    if (stats.total == 0) return 0.0;
    return (100.0 * stats.correctas) / stats.total;
}

int PalabrasDominadas() {
    return (int)IndicesDominadas().size();
}

void AgregarFilaDominadas(const wstring& texto) {
    SendMessageW(listaDominadas, LB_ADDSTRING, 0, (LPARAM)texto.c_str());
}

void ActualizarListaDominadas() {
    SendMessageW(listaDominadas, LB_RESETCONTENT, 0, 0);
    vector<int> dominadas = IndicesDominadas();
    vector<int> cercanas = IndicesCercanas();

    if (!dominadas.empty()) {
        AgregarFilaDominadas(L"[SECCION] DOMINADAS");
        for (int indice : dominadas) {
            const Palabra& p = palabras[indice];
            int total = p.correctas + p.incorrectas;
            int porcentaje = total > 0 ? p.correctas * 100 / total : 0;
            wstring linea = L"[DOMINADA] " + (p.articulo.empty() ? L"" : p.articulo + L" ") + p.singular +
                L" — " + p.espanol + L"  |  Plural: " + (p.plural.empty() ? L"no se practica" : p.plural) +
                L"  |  " + to_wstring(p.correctas) + L" aciertos / " + to_wstring(p.incorrectas) +
                L" errores (" + to_wstring(porcentaje) + L"%)";
            AgregarFilaDominadas(linea);
        }
    }

    if (!cercanas.empty()) {
        AgregarFilaDominadas(L"[SECCION] CERCA DE DOMINAR");
        for (int indice : cercanas) {
            const Palabra& p = palabras[indice];
            int total = p.correctas + p.incorrectas;
            int porcentaje = total > 0 ? p.correctas * 100 / total : 0;
            int faltan = AciertosParaDominar(p);
            wstring linea = L"[CERCA] " + (p.articulo.empty() ? L"" : p.articulo + L" ") + p.singular +
                L" — " + p.espanol + L"  |  Plural: " + (p.plural.empty() ? L"no se practica" : p.plural) +
                L"  |  faltan " + to_wstring(faltan) +
                (faltan == 1 ? L" acierto" : L" aciertos") + L"  ·  " +
                to_wstring(p.correctas) + L" / " + to_wstring(p.incorrectas) +
                L" (" + to_wstring(porcentaje) + L"%)";
            AgregarFilaDominadas(linea);
        }
    }
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
    MostrarControl(btnHomeEspanol, inicio);
    MostrarControl(btnDominadas, pantallaActual == PANTALLA_PROGRESO);
    MostrarControl(listaDominadas, pantallaActual == PANTALLA_DOMINADAS && (PalabrasDominadas() > 0 || !IndicesCercanas().empty()));

    for (int i = 0; i < 3; ++i) MostrarControl(btnRespuesta[i], opciones);
    MostrarControl(btnSiguiente, (opciones || texto) && respondida);

    MostrarControl(editRespuesta, texto);
    MostrarControl(btnComprobar, texto && !respondida);
    MostrarControl(btnCharAE, texto && !respondida);
    MostrarControl(btnCharOE, texto && !respondida);
    MostrarControl(btnCharUE, texto && !respondida);
    MostrarControl(btnCharSS, texto && !respondida);

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
    MoveWindow(btnHomeVerbos, x, y3, cardW, 64, TRUE);
    MoveWindow(btnHomeEspanol, x + cardW + cardGap, y3, cardW, 64, TRUE);
    MoveWindow(btnDominadas, x, 555, cardW * 2 + cardGap, 44, TRUE);
    MoveWindow(listaDominadas, x, 394, anchoContenido - margen * 2, max(120, (int)rc.bottom - 414), TRUE);

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
    vector<int> candidatos;
    for (size_t i = 0; i < palabras.size(); ++i)
        if (PalabraDisponible((int)i, modoActual) && (int)i != indicePalabraActual)
            candidatos.push_back((int)i);
    if (candidatos.empty()) return indicePalabraActual;
    uniform_int_distribution<int> dist(0, (int)candidatos.size() - 1);
    return candidatos[dist(generador)];
}

int ElegirPalabraDebil() {
    vector<pair<int, int> > ranking;

    for (size_t i = 0; i < palabras.size(); ++i) {
        int puntaje = palabras[i].incorrectas * 4 - palabras[i].correctas;
        ranking.push_back(make_pair(puntaje, (int)i));
    }

    shuffle(ranking.begin(), ranking.end(), generador);
    stable_sort(ranking.begin(), ranking.end(),
         [](const pair<int, int>& a, const pair<int, int>& b) {
             return a.first > b.first;
         });

    int candidatos = min(5, (int)ranking.size());
    uniform_int_distribution<int> dist(0, candidatos - 1);
    return ranking[dist(generador)].second;
}

vector<wstring> OpcionesVocabulario(int indiceCorrecto) {
    vector<wstring> opciones(1, palabras[indiceCorrecto].espanol);
    // Concrete near-spelling contrast requested for Tennis.
    if (palabras[indiceCorrecto].singular == L"Tennis") {
        opciones.push_back(L"tenista");
        opciones.push_back(L"tesis");
    } else {
        vector<pair<int, int> > candidatos;
        for (size_t i = 0; i < palabras.size(); ++i) {
            if ((int)i == indiceCorrecto || CompartenSignificado(palabras[i].espanol, opciones[0])) continue;
            candidatos.push_back(make_pair(PuntajeDistractor(indiceCorrecto, (int)i), (int)i));
        }
        shuffle(candidatos.begin(), candidatos.end(), generador);
        stable_sort(candidatos.begin(), candidatos.end(), [](const pair<int,int>& a, const pair<int,int>& b) {
            return a.first > b.first;
        });
        // Rotate among the closest five instead of unrelated random words.
        size_t limite = min((size_t)5, candidatos.size());
        shuffle(candidatos.begin(), candidatos.begin() + limite, generador);
        for (const auto& c : candidatos) {
            const wstring& texto = palabras[c.second].espanol;
            bool ambiguo = false;
            for (const auto& opcion : opciones)
                if (CompartenSignificado(texto, opcion)) ambiguo = true;
            if (!ambiguo) opciones.push_back(texto);
            if (opciones.size() == 3) break;
        }
    }
    shuffle(opciones.begin(), opciones.end(), generador);
    return opciones;
}

vector<int> VerbosFiltrados() {
    vector<int> indices;
    for (size_t i = 0; i < verbos.size(); ++i) {
        bool disponible = false;
        for (int p = 0; p < 6; ++p)
            if (!verbos[i].formas[(int)tiempoSeleccionado][p].empty()) disponible = true;
        if (!disponible) continue;
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
        preguntaEsArticulo = (tipo(generador) == 0) && !palabras[indicePalabraActual].articulo.empty();
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

    if (preguntaEspanol) {
        indicePalabraActual = modoActual == MODO_REPASO ? ElegirPalabraDebil() : ElegirPalabraAleatoria();
        respuestaCorrecta = palabras[indicePalabraActual].singular;
    } else if (modoActual == MODO_PLURALES) {
        indicePalabraActual = ElegirPalabraAleatoria();
        respuestaCorrecta = palabras[indicePalabraActual].plural;
    } else if (modoActual == MODO_VERBOS) {
        vector<int> candidatos = VerbosFiltrados();
        if (candidatos.empty()) return;

        uniform_int_distribution<int> verboDist(0, (int)candidatos.size() - 1);
        indiceVerboActual = candidatos[verboDist(generador)];
        vector<int> pronombresDisponibles;
        for (int p = 0; p < 6; ++p)
            if (!verbos[indiceVerboActual].formas[(int)tiempoSeleccionado][p].empty())
                pronombresDisponibles.push_back(p);
        uniform_int_distribution<int> pronDist(0, (int)pronombresDisponibles.size() - 1);
        indicePronombreActual = pronombresDisponibles[pronDist(generador)];
        respuestaCorrecta = verbos[indiceVerboActual].formas[(int)tiempoSeleccionado][indicePronombreActual];
    }
}

void PrepararSiguientePregunta() {
    preguntaEspanol = (modoActual == MODO_VOCABULARIO && (soloEspanol || preguntaSesion % 2 == 0)) ||
                      (modoActual == MODO_REPASO && preguntaSesion % 3 == 0);
    bool escrita = preguntaEspanol || modoActual == MODO_PLURALES || modoActual == MODO_VERBOS;
    pantallaActual = escrita ? PANTALLA_PRACTICA_TEXTO : PANTALLA_PRACTICA_OPCIONES;
    if (escrita) PrepararPreguntaTexto(); else PrepararPreguntaOpciones();
}

void IniciarPractica(ModoPractica modo, bool escribirAleman = false) {
    modoActual = modo;
    soloEspanol = escribirAleman;
    preguntaSesion = 1;
    correctasSesion = 0;
    PrepararSiguientePregunta();
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
    bool correcta = preguntaEsArticulo ? RespuestaValida(texto, respuestaCorrecta)
                                       : (wstring(texto) == respuestaCorrecta);

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

    bool correcta = preguntaEspanol ? RespuestaAlemanValida(respuesta, indicePalabraActual)
                                   : RespuestaValida(respuesta, respuestaCorrecta);

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
    PrepararSiguientePregunta();
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
    wstring biblioteca = L"Netzwerk neu A1.1 · " + to_wstring(palabras.size()) +
                         L" sustantivos y " + to_wstring(verbos.size()) + L" verbos / expresiones";
    DibujarTexto(hdc, biblioteca,
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
        DibujarTexto(hdc, palabras[indicePalabraActual].singular, palabra,
                     palabras[indicePalabraActual].singular.size() > 20 ? fuenteSubtitulo : fuenteGrande,
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
        DibujarTexto(hdc, palabras[indicePalabraActual].singular, palabra,
                     palabras[indicePalabraActual].singular.size() > 20 ? fuenteSubtitulo : fuenteGrande,
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
    wstring titulo = preguntaEspanol ? L"Español → alemán · escribir"
        : modoActual == MODO_PLURALES ? L"Plurales · respuesta escrita"
        : L"Verbos · " + NombreTiempo();

    DibujarCabeceraPractica(hdc, rc, titulo);

    RECT tarjeta = {275, 160, rc.right - 75, 365};
    DibujarRectRedondeado(hdc, tarjeta, COLOR_BLANCO, COLOR_BORDE, 24, 1);

    if (preguntaEspanol) {
        RECT ins = {tarjeta.left + 18, tarjeta.top + 18, tarjeta.right - 18, tarjeta.top + 50};
        DibujarTexto(hdc, L"¿Cómo se dice en alemán? Escribí la palabra sin artículo.", ins,
                     fuenteNormal, COLOR_TEXTO_SUAVE, DT_CENTER | DT_WORDBREAK);
        RECT palabra = {tarjeta.left + 20, tarjeta.top + 65, tarjeta.right - 20, tarjeta.bottom - 20};
        DibujarTexto(hdc, palabras[indicePalabraActual].espanol, palabra, fuenteTitulo,
                     COLOR_TEXTO, DT_CENTER | DT_WORDBREAK);
    } else if (modoActual == MODO_PLURALES) {
        RECT ins = {tarjeta.left + 20, tarjeta.top + 20, tarjeta.right - 20, tarjeta.top + 50};
        DibujarTexto(hdc, L"Escribí el plural sin artículo", ins, fuenteNormal,
                     COLOR_TEXTO_SUAVE, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        wstring singular = palabras[indicePalabraActual].articulo + L" " +
                           palabras[indicePalabraActual].singular;
        RECT palabra = {tarjeta.left + 20, tarjeta.top + 58, tarjeta.right - 20, tarjeta.top + 120};
        DibujarTexto(hdc, singular, palabra, singular.size() > 22 ? fuenteSubtitulo : fuenteGrande,
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
        DibujarTexto(hdc, v.infinitivo, verbo, v.infinitivo.size() > 20 ? fuenteSubtitulo : fuenteGrande, COLOR_TEXTO,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        wstring detalle = wstring((verbos[indiceVerboActual].formas[(int)tiempoSeleccionado][0].empty()
                     ? L"es" : PRONOMBRES[indicePronombreActual])) +
                          L"   •   " + (v.infinitivo == L"möchten" ? wstring(L"Konjunktiv II") : NombreTiempo()) +
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

void DibujarDominadas(HDC hdc, RECT rc) {
    const int x = 252;
    const int derecha = rc.right - 45;
    const int ancho = derecha - x;
    const int gap = 16;

    RECT titulo = {x, 34, derecha, 78};
    DibujarTexto(hdc, L"Mi biblioteca de progreso", titulo, fuenteTitulo,
                 COLOR_TEXTO, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT subtitulo = {x, 82, derecha, 116};
    DibujarTexto(hdc, L"Revisá lo que ya aprendiste y lo que está a pocos aciertos de quedar dominado.",
                 subtitulo, fuenteNormal, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    int tarjetaW = (ancho - gap) / 2;
    RECT dominadas = {x, 140, x + tarjetaW, 250};
    RECT cercanas = {x + tarjetaW + gap, 140, derecha, 250};
    DibujarRectRedondeado(hdc, dominadas, COLOR_VERDE_CLARO, RGB(201, 235, 207), 22, 1);
    DibujarRectRedondeado(hdc, cercanas, COLOR_NARANJA_CLARO, RGB(247, 218, 174), 22, 1);

    RECT barraVerde = {dominadas.left, dominadas.top, dominadas.left + 9, dominadas.bottom};
    RECT barraNaranja = {cercanas.left, cercanas.top, cercanas.left + 9, cercanas.bottom};
    HBRUSH bv = CreateSolidBrush(COLOR_VERDE); FillRect(hdc, &barraVerde, bv); DeleteObject(bv);
    HBRUSH bn = CreateSolidBrush(COLOR_NARANJA); FillRect(hdc, &barraNaranja, bn); DeleteObject(bn);

    RECT dLabel = {dominadas.left + 27, dominadas.top + 15, dominadas.right - 20, dominadas.top + 42};
    DibujarTexto(hdc, L"PALABRAS DOMINADAS", dLabel, fuentePequena, COLOR_VERDE_OSCURO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT dNum = {dominadas.left + 27, dominadas.top + 37, dominadas.right - 20, dominadas.top + 87};
    DibujarTexto(hdc, to_wstring(PalabrasDominadas()), dNum, fuenteTitulo, COLOR_VERDE_OSCURO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT dHint = {dominadas.left + 112, dominadas.top + 55, dominadas.right - 20, dominadas.top + 87};
    DibujarTexto(hdc, L"listas para repasar", dHint, fuentePequena, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT cLabel = {cercanas.left + 27, cercanas.top + 15, cercanas.right - 20, cercanas.top + 42};
    DibujarTexto(hdc, L"CERCA DE DOMINAR", cLabel, fuentePequena, RGB(193, 117, 22),
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT cNum = {cercanas.left + 27, cercanas.top + 37, cercanas.right - 20, cercanas.top + 87};
    DibujarTexto(hdc, to_wstring(IndicesCercanas().size()), cNum, fuenteTitulo, RGB(193, 117, 22),
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT cHint = {cercanas.left + 112, cercanas.top + 55, cercanas.right - 20, cercanas.top + 87};
    DibujarTexto(hdc, L"a pocos aciertos", cHint, fuentePequena, COLOR_TEXTO_SUAVE,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT regla = {x, 270, derecha, 344};
    DibujarRectRedondeado(hdc, regla, COLOR_BLANCO, COLOR_BORDE, 18, 1);
    RECT reglaMarca = {regla.left, regla.top, regla.left + 6, regla.bottom};
    HBRUSH rm = CreateSolidBrush(COLOR_AZUL); FillRect(hdc, &reglaMarca, rm); DeleteObject(rm);
    RECT reglaTitulo = {regla.left + 22, regla.top + 12, regla.right - 20, regla.top + 36};
    DibujarTexto(hdc, L"¿Cuándo se domina una palabra?", reglaTitulo, fuenteSubtitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT reglaTexto = {regla.left + 22, regla.top + 39, regla.right - 20, regla.bottom - 9};
    DibujarTexto(hdc, L"Con 3 respuestas y al menos 75% de aciertos, o cuando los aciertos superan a los errores por 10. El porcentaje deja de importar en ese segundo caso.",
                 reglaTexto, fuentePequena, COLOR_TEXTO_SUAVE, DT_LEFT | DT_WORDBREAK);

    RECT listado = {x, 360, derecha, 390};
    DibujarTexto(hdc, L"Tu recorrido", listado, fuenteSubtitulo, COLOR_TEXTO,
                 DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    if (PalabrasDominadas() == 0 && IndicesCercanas().empty()) {
        RECT vacio = {x, 400, derecha, 585};
        DibujarRectRedondeado(hdc, vacio, COLOR_BLANCO, COLOR_BORDE, 20, 1);
        RECT icono = {x + 28, 434, x + 88, 494};
        HBRUSH hi = CreateSolidBrush(COLOR_AZUL_CLARO); FillRect(hdc, &icono, hi); DeleteObject(hi);
        DibujarTexto(hdc, L"✓", icono, fuenteTitulo, COLOR_AZUL, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        RECT mensaje = {x + 112, 425, derecha - 25, 530};
        DibujarTexto(hdc, L"Todavía no hay palabras en esta sección.\nPracticá un poco más y tus avances van a aparecer acá.",
                     mensaje, fuenteSubtitulo, COLOR_TEXTO, DT_LEFT | DT_WORDBREAK);
    }
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

void DibujarFilaDominadas(const DRAWITEMSTRUCT* dis) {
    if (dis->itemID == (UINT)-1) return;
    wchar_t texto[1024] = L"";
    SendMessageW(dis->hwndItem, LB_GETTEXT, dis->itemID, (LPARAM)texto);
    wstring fila = texto;
    RECT rc = dis->rcItem;
    bool seccion = fila.find(L"[SECCION]") == 0;
    bool dominada = fila.find(L"[SECCION] DOMINADAS") == 0 || fila.find(L"[DOMINADA]") == 0;
    bool seleccionada = (dis->itemState & ODS_SELECTED) != 0;

    COLORREF fondo = seccion ? (dominada ? COLOR_VERDE_CLARO : COLOR_NARANJA_CLARO)
                             : ((dis->itemID % 2) ? RGB(250, 251, 254) : COLOR_BLANCO);
    COLORREF textoColor = seccion ? (dominada ? COLOR_VERDE_OSCURO : RGB(193, 117, 22)) : COLOR_TEXTO;
    COLORREF marca = dominada ? COLOR_VERDE : COLOR_NARANJA;
    if (seleccionada && !seccion) fondo = COLOR_AZUL_CLARO;

    HBRUSH brocha = CreateSolidBrush(fondo);
    FillRect(dis->hDC, &rc, brocha);
    DeleteObject(brocha);

    RECT marcaRc = {rc.left, rc.top, rc.left + (seccion ? 7 : 4), rc.bottom};
    HBRUSH bm = CreateSolidBrush(seccion ? marca : (seleccionada ? COLOR_AZUL : marca));
    FillRect(dis->hDC, &marcaRc, bm);
    DeleteObject(bm);

    RECT textoRc = rc;
    textoRc.left += seccion ? 18 : 16;
    textoRc.right -= 12;
    textoRc.top += seccion ? 14 : 8;
    textoRc.bottom -= seccion ? 10 : 7;
    HFONT fuente = seccion ? fuenteBoton : fuentePequena;
    wstring visible = fila;
    if (!seccion && visible.find(L"[DOMINADA] ") == 0) visible.erase(0, 11);
    if (!seccion && visible.find(L"[CERCA] ") == 0) visible.erase(0, 8);
    DibujarTexto(dis->hDC, seccion ? (dominada ? L"DOMINADAS" : L"CERCA DE DOMINAR") : visible,
                 textoRc, fuente, textoColor, DT_LEFT | DT_VCENTER | (seccion ? DT_SINGLELINE : DT_WORDBREAK));
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
        (id == ID_NAV_PROGRESO && (pantallaActual == PANTALLA_PROGRESO || pantallaActual == PANTALLA_DOMINADAS));

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

    if (id == ID_DOMINADAS) {
        fondo = presionado ? COLOR_AZUL_CLARO : COLOR_BLANCO;
        borde = COLOR_AZUL;
        textoColor = COLOR_AZUL;
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
            if (preguntaEsArticulo ? RespuestaValida(txt, respuestaCorrecta) : txt == respuestaCorrecta) {
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

    if (id >= ID_RESPUESTA_1 && id <= ID_RESPUESTA_3) {
        InflateRect(&caja, -7, -7);
        DibujarTexto(hdc, texto, caja, fuentePequena, textoColor, DT_CENTER | DT_WORDBREAK);
    } else {
        DibujarTexto(hdc, texto, caja, fuenteBoton, textoColor,
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
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
            btnHomeVocab = CrearBoton(hwnd, ID_HOME_VOCAB, L"Vocabulario · mixto");
            btnHomeEspanol = CrearBoton(hwnd, ID_HOME_ESPANOL, L"Español → alemán");
            btnDominadas = CrearBoton(hwnd, ID_DOMINADAS, L"Ver dominadas y cercanas");
            listaDominadas = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", L"",
                WS_CHILD | WS_TABSTOP | WS_VSCROLL | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT,
                0, 0, 100, 100, hwnd, (HMENU)(INT_PTR)ID_LISTA_DOMINADAS, GetModuleHandleW(NULL), NULL);
            SendMessageW(listaDominadas, WM_SETFONT, (WPARAM)fuenteNormal, TRUE);
            btnHomeArticulos = CrearBoton(hwnd, ID_HOME_ARTICULOS, L"Artículos");
            btnHomePlurales = CrearBoton(hwnd, ID_HOME_PLURALES, L"Plurales · escribir");
            btnHomeVerbos = CrearBoton(hwnd, ID_HOME_VERBOS, L"Verbos · escribir");

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

                case ID_HOME_ESPANOL:
                    IniciarPractica(MODO_VOCABULARIO, true);
                    break;

                case ID_DOMINADAS:
                    ActualizarListaDominadas();
                    pantallaActual = PANTALLA_DOMINADAS;
                    ActualizarControles();
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

        case WM_DRAWITEM: {
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
            if (dis->CtlType == ODT_LISTBOX) DibujarFilaDominadas(dis);
            else DibujarBoton(dis);
            return TRUE;
        }

        case WM_MEASUREITEM: {
            MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
            if (mis->CtlType == ODT_LISTBOX) {
                mis->itemHeight = 58;
                return TRUE;
            }
            break;
        }

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
            } else if (pantallaActual == PANTALLA_DOMINADAS) {
                DibujarDominadas(hdc, rc);
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

