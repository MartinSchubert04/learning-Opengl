# deleteEXE.ps1

Remove-Item -Path *.exe -Force -ErrorAction SilentlyContinue
Write-Output "Todos los archivos .exe han sido eliminados."