@echo off
setlocal

cd /d "%~dp0"

REM --- Crear carpeta build si no existe ---
if not exist build mkdir build

REM --- Ir a build ---
cd build

REM --- Configurar con CMake (genera compile_commands.json) ---
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -G "Ninja"
if %errorlevel% neq 0 exit /b %errorlevel%

copy build\compile_commands.json compile_commands.json > nul


REM --- Compilar ---
cmake --build . --config Debug
if %errorlevel% neq 0 exit /b %errorlevel%

REM
cd ..

REM
.\OpenGL-learn.exe

REM
del ".\\OpenGL-learn.exe"

endlocal
