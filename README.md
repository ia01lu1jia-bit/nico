# DeutschCoach

DeutschCoach es una aplicación de escritorio en C++ para acompañar el aprendizaje de alemán desde nivel inicial. El objetivo es que el programa registre el rendimiento del usuario y, progresivamente, priorice aquello que más necesita repasar.

## Versión actual: v0.3 GUI

La versión actual es una **aplicación gráfica de Windows** pensada para **Embarcadero Dev-C++ 6.3 / MinGW**.

### Cambios principales de v0.3

- Interfaz renovada con navegación lateral oscura, tarjetas y colores por módulo.
- Pantalla principal con resumen de precisión, racha y palabras dominadas.
- Vocabulario mediante opciones múltiples.
- Artículos `der`, `die`, `das` mediante botones.
- Nuevo módulo de **plurales con respuesta escrita**.
- Botones rápidos `ä`, `ö`, `ü`, `ß` para respuestas escritas.
- Nuevo módulo de **verbos con respuesta escrita**.
- Configuración de verbos antes de comenzar una sesión:
  - `Präsens`
  - `Präteritum`
  - `Perfekt`
  - solo regulares
  - solo irregulares
  - ambos
- En cada ejercicio verbal se muestra infinitivo, significado, pronombre, tiempo y tipo de verbo.
- Sesiones de 10 preguntas.
- Repaso inteligente de vocabulario/artículos basado en errores previos.
- Estadísticas globales y por módulos.
- Guardado automático del progreso entre sesiones.

> El vocabulario y los verbos incluidos ahora son solamente una base pequeña de prueba. Más adelante se reemplazarán o ampliarán con el contenido del libro de alemán del usuario.

El progreso se guarda automáticamente en:

`deutschcoach_progreso.txt`

La v0.3 usa un formato de progreso nuevo (`DEUTSCHCOACH_V3`). Si existía un archivo de progreso de una versión anterior, la aplicación comenzará las estadísticas nuevas sin intentar interpretar datos incompatibles.

## Cómo abrirlo en Embarcadero Dev-C++ 6.3

1. Descargá el repositorio desde GitHub con **Code > Download ZIP**.
2. Descomprimilo en una carpeta permanente, por ejemplo `C:\Proyectos\DeutschCoach`.
3. Abrí **Embarcadero Dev-C++ 6.3**.
4. Elegí **File > New > Project**.
5. Seleccioná un proyecto **Win32 GUI / Windows Application** y lenguaje **C++**.
6. Poné como nombre del proyecto `DeutschCoach`.
7. Dev-C++ va a crear un archivo fuente inicial. Reemplazá su contenido completo por el contenido de `DeutschCoachGUI.cpp`.
8. Abrí **Project > Project Options**.
9. Confirmá que el tipo de proyecto sea **Win32 GUI**.
10. En las opciones del **Linker** agregá: `-lgdi32 -mwindows`
11. Guardá el proyecto.
12. Usá **Execute > Compile & Run**.

### Si aparece `undefined reference` a SelectObject, CreateFontW, CreateSolidBrush, RoundRect, etc.

Eso significa que falta enlazar la biblioteca gráfica de Windows. Confirmá que figure:

`-lgdi32 -mwindows`

`gdi32` contiene las funciones de dibujo utilizadas por DeutschCoach y `-mwindows` genera el programa gráfico sin una consola negra detrás.

### Importante

No conviene abrir `DeutschCoachGUI.cpp` y compilarlo como **single file**. DeutschCoach debe compilarse dentro de un proyecto **Win32 GUI / Windows Application**.

Dev-C++ 6.3 soporta proyectos Win32 GUI y opciones adicionales del linker. DeutschCoach utiliza únicamente la API nativa de Windows y la biblioteca estándar de C++, por lo que no necesita frameworks gráficos externos.

## Estructura actual del aprendizaje

- **Repaso inteligente**: mezcla vocabulario y artículos priorizando errores.
- **Vocabulario**: reconocer significado.
- **Artículos**: elegir `der`, `die` o `das`.
- **Plurales**: escribir el plural correcto.
- **Verbos**: escribir la conjugación según pronombre, tiempo y filtro elegido.
- **Progreso**: revisar precisión, rachas y actividad por módulo.

## Próximas etapas

- Cargar vocabulario, plurales y verbos desde archivos externos en lugar de dejarlos dentro del código.
- Importar el contenido del libro de alemán cuando esté disponible.
- Repaso espaciado según fecha y dificultad.
- Estadísticas individuales por palabra y por verbo.
- Frases y construcción de oraciones.
- Sistema de unidades/lecciones A1.
- Mejoras visuales, iconos y pequeñas animaciones.

## Objetivo final

DeutschCoach deberá detectar qué palabras, artículos, plurales, verbos y estructuras generan más errores y construir sesiones de estudio adaptadas al usuario, convirtiéndose en una herramienta personal de aprendizaje de alemán y no solamente en un cuestionario fijo.
