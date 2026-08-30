# Floatix — Notas flotantes

Widget de escritorio en C++/Qt6, siempre visible por encima de las demás ventanas, que se compacta a un ícono pequeño y se expande al pasar el mouse para escribir una nota rápida.

## Características

- **Modo compacto**: ícono pequeño en la esquina de la pantalla.
- **Modo expandido**: editor de texto plano para escribir la nota.
- Guardado manual a archivo (`Save`), eligiendo ubicación y formato (`.txt`, `.md`, `.tex`; `.docx` aún no está implementado y cae automáticamente a `.txt` con un aviso).
- Límite de 2000 bloques de texto en el editor.

## Requisitos

- Qt6 6.5+ (Core, Widgets)
- CMake 3.19+

## Compilación

```bash
cmake -B build
cmake --build build
```

## Nota de plataforma

El ícono del modo compacto se carga desde una ruta local del autor; si no se encuentra en otra máquina, el programa cae automáticamente a mostrar la letra "N" en su lugar — no rompe la ejecución, solo cambia el ícono visualmente.

## ⚠️ Limitación conocida

Las notas no se guardan automáticamente ni se recuerdan al reabrir la app — cada vez que le das a "Save" tienes que elegir manualmente dónde guardarla.

## Nota de plataforma

El ícono del modo compacto se carga desde una ruta absoluta local del autor (no relativa al proyecto). Se incluye el archivo en `iconos/waos.png` como referencia, pero el programa no lo detectará automáticamente en otra máquina — caerá a mostrar la letra "N" en su lugar. No rompe la ejecución, solo cambia el ícono visualmente.
