@echo off
REM --- Crear carpeta build si no existe ---
if not exist build (
    mkdir build
)

REM --- Generar archivos de compilación con CMake ---
cmake -S . -B build

REM --- Compilar ---
cmake --build build --config Debug
