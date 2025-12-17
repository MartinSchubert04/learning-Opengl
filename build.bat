@echo off
setlocal

REM ---- Verificar Ninja ----
where ninja >nul 2>nul
if %errorlevel% neq 0 (
    echo Ninja build system not found. Installing Ninja...
    winget install Ninja-build.Ninja -e --source winget
    if %errorlevel% neq 0 exit /b %errorlevel%
) 

REM ---- Configurar si no existe build ----
if not exist build (
    echo Build directory not found, building with CMake...
    cmake -S . -B build -G Ninja
    if %errorlevel% neq 0 exit /b %errorlevel%
)

REM ---- Ejecutar CMake (todo lo demas lo maneja CMake) ----
cmake --build build --target run_and_clean

endlocal
