<#
setup_and_build.ps1

Script profesional para preparar el entorno de compilación en Windows,
intentar instalar Visual Studio Build Tools si es posible, configurar CMake,
compilar el proyecto y ejecutar tests.

USO:
  - Abrir PowerShell como Administrador
  - Ejecutar: .\scripts\setup_and_build.ps1

Nota: El script intenta ejecutar instalaciones con `winget` cuando sea necesario.
Si la instalación requiere interacción o falla, el script imprimirá instrucciones
para que continúes manualmente.
#>

function Write-Err([string]$msg) { Write-Host "[ERROR] $msg" -ForegroundColor Red }
function Write-Ok([string]$msg) { Write-Host "[OK]    $msg" -ForegroundColor Green }
function Write-Info([string]$msg) { Write-Host "[INFO]  $msg" -ForegroundColor Cyan }

# Comprueba si se está ejecutando como Administrador
function Test-IsAdmin {
    $current = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($current)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

if (-not (Test-IsAdmin)) {
    Write-Host "Este script requiere privilegios de administrador."
    Write-Host "Abre PowerShell como Administrador y ejecuta:`n    .\scripts\setup_and_build.ps1"
    exit 1
}

Set-StrictMode -Version Latest

Push-Location $PSScriptRoot/.. | Out-Null

Write-Info "Detectando herramientas en PATH..."
function CmdExists($name) { return (Get-Command $name -ErrorAction SilentlyContinue) -ne $null }

$hasNinja = CmdExists ninja
$hasCl = CmdExists cl
$hasGpp = CmdExists g++
$hasMingw = CmdExists mingw32-make
$hasClang = CmdExists clang

Write-Info "ninja: $hasNinja | cl: $hasCl | g++: $hasGpp | mingw32-make: $hasMingw"

if (-not ($hasCl -or $hasGpp -or $hasClang)) {
    Write-Info "No se detectó compilador C++ en PATH. Intentando instalar Visual Studio Build Tools con winget..."
    if (CmdExists winget) {
        try {
            winget install --id Microsoft.VisualStudio.2022.BuildTools -e --accept-package-agreements --accept-source-agreements -h
            Write-Ok "Solicitud de instalación enviada (winget). Si la instalación requiere interacción, complétala y vuelve a ejecutar este script."
        } catch {
            Write-Err "Instalación con winget falló: $($_.Exception.Message)"
            Write-Host "Por favor instala Visual Studio Build Tools manualmente desde:`nhttps://visualstudio.microsoft.com/downloads/`nSelecciona 'Build Tools for Visual Studio' y marca 'Desktop development with C++'." -ForegroundColor Yellow
            Pop-Location | Out-Null
            exit 1
        }
        Write-Info "Esperando 10 segundos para que el instalador comience..."
        Start-Sleep -Seconds 10
        Write-Info "Reinicia PowerShell o abre 'Developer PowerShell for VS' si la instalación se completó. Luego vuelve a ejecutar este script."
        Pop-Location | Out-Null
        exit 0
    } else {
        Write-Err "winget no está disponible. Instala Visual Studio Build Tools manualmente y vuelve a ejecutar.";
        Write-Host "Descarga: https://visualstudio.microsoft.com/downloads/" -ForegroundColor Yellow
        Pop-Location | Out-Null
        exit 1
    }
}

# Selección de generador según toolchain disponible
if (CmdExists cl) {
    $generator = 'Visual Studio 17 2022'
    $generatorArgs = "-A x64"
    Write-Info "Usando MSVC (cl) con generador: $generator"
} elseif ($hasNinja -and $hasGpp) {
    $generator = 'Ninja'
    $generatorArgs = ''
    Write-Info "Usando Ninja + g++"
} elseif ($hasGpp -and $hasMingw) {
    $generator = 'MinGW Makefiles'
    $generatorArgs = ''
    Write-Info "Usando MinGW Makefiles + mingw32-make"
} else {
    Write-Err "No se detecta un generador/cadena de herramientas soportada. Asegúrate de tener MSVC, g++ o clang en PATH."
    Pop-Location | Out-Null
    exit 1
}

# Limpiar build anterior para evitar conflictos de generador
if (Test-Path -Path .\build) {
    Write-Info "Eliminando carpeta build/ previa para reconfiguración limpia..."
    Remove-Item -Recurse -Force .\build
}

Write-Info "Configurando CMake (generador: $generator)"
$cmakeArgs = "-S . -B build -G `"$generator`" $generatorArgs -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
Write-Info "Comando: cmake $cmakeArgs"

$configExit = & cmake $cmakeArgs
if ($LASTEXITCODE -ne 0) {
    Write-Err "CMake configuró con errores. Salida: $LASTEXITCODE"
    Pop-Location | Out-Null
    exit $LASTEXITCODE
}

Write-Info "Construyendo proyecto..."
if ($generator -like 'Visual Studio*') {
    & cmake --build build --config Debug
} else {
    & cmake --build build -- -j
}

if ($LASTEXITCODE -ne 0) {
    Write-Err "La compilación falló (exit code $LASTEXITCODE)."
    Pop-Location | Out-Null
    exit $LASTEXITCODE
}

Write-Ok "Compilación completada. Ejecutando tests (ctest)..."
& ctest --test-dir build --output-on-failure

if ($LASTEXITCODE -ne 0) {
    Write-Err "Algunos tests fallaron. Código: $LASTEXITCODE"
    Pop-Location | Out-Null
    exit $LASTEXITCODE
}

Write-Ok "Todos los tests pasaron (o no hay tests). Entrega completada."
Pop-Location | Out-Null
exit 0
