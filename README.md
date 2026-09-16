# DeutschCoach

DeutschCoach es una aplicación en C++ para acompañar el aprendizaje de alemán desde nivel inicial. El objetivo es que el programa registre el rendimiento del usuario y, progresivamente, priorice aquello que más necesita repasar.

## Versión actual: v0.2 GUI

La versión actual ya es una **aplicación gráfica de Windows** y está pensada para compilarse con **Embarcadero Dev-C++ 6.3 / MinGW**.

### Incluye

- Interfaz gráfica sin consola.
- Pantalla principal visual.
- Navegación lateral.
- Nivel inicial A1.
- Práctica de artículos `der`, `die`, `das` mediante botones.
- Práctica de vocabulario mediante opciones múltiples.
- Sesiones de 10 preguntas.
- Repaso inteligente que prioriza palabras con más errores.
- Estadísticas globales.
- Racha actual y mejor racha.
- Conteo de palabras dominadas.
- Guardado automático del progreso entre sesiones.

El progreso se guarda automáticamente en:

`deutschcoach_progreso.txt`

## Cómo abrirlo en Embarcadero Dev-C++ 6.3

1. Descargá el repositorio desde GitHub con **Code > Download ZIP**.
2. Descomprimilo en una carpeta permanente, por ejemplo `C:\Proyectos\DeutschCoach`.
3. Abrí **Embarcadero Dev-C++ 6.3**.
4. Elegí **File > New > Project**.
5. Seleccioná **Windows Application** y lenguaje **C++**.
6. Poné como nombre del proyecto `DeutschCoach`.
7. Dev-C++ va a crear un archivo fuente inicial. Reemplazá su contenido completo por el contenido de `DeutschCoachGUI.cpp`.
8. Guardá.
9. Usá **Execute > Compile & Run** (o F11 según la configuración del IDE).

### Importante

El proyecto debe ser de tipo **Windows Application**, no `Console Application`. De esa manera se abre directamente la ventana gráfica y no aparece una consola negra detrás.

DeutschCoachGUI.cpp utiliza únicamente la API nativa de Windows y la biblioteca estándar de C++. No requiere instalar librerías gráficas externas.

## Archivo anterior

`DeutschCoach.cpp` corresponde a la v0.1 de consola y se mantiene solamente como referencia histórica. El desarrollo principal continúa en `DeutschCoachGUI.cpp`.

## Próximas versiones

- Vocabulario cargado desde archivos externos.
- Estadísticas individuales por palabra más completas.
- Sistema de niveles y experiencia.
- Repaso espaciado según fecha y dificultad.
- Verbos básicos y conjugaciones.
- Frases y construcción de oraciones.
- Nuevos temas A1 desbloqueables.
- Configuración del perfil del estudiante.
- Mejoras visuales y animaciones.

## Objetivo final

DeutschCoach deberá poder detectar qué palabras, artículos y temas generan más errores y construir sesiones de estudio adaptadas al usuario, convirtiéndose en una herramienta personal de aprendizaje de alemán y no solamente en un cuestionario fijo.
