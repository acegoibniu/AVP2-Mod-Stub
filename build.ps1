# Builds the four AvP2 binaries (Win32 Release) and copies them to .\out
#   .\build.ps1
#   .\build.ps1 -Config Debug
param(
    [ValidateSet('Release', 'Debug')]
    [string]$Config = 'Release'
)

$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot

# CMake ships inside Visual Studio 2022; find it with vswhere.
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path $vswhere)) { throw "vswhere.exe not found; is Visual Studio 2022 installed?" }
$vsPath = & $vswhere -latest -version '[17.0,18.0)' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsPath) { throw "Visual Studio 2022 with the C++ tools was not found." }
$cmake = Join-Path $vsPath 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
if (-not (Test-Path $cmake)) { throw "CMake not found at $cmake" }

$buildDir = Join-Path $root 'build'
$outDir = Join-Path $root 'out'

# AvP2 is 32-bit: always -A Win32.
& $cmake -S $root -B $buildDir -G 'Visual Studio 17 2022' -A Win32
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed." }

& $cmake --build $buildDir --config $Config
if ($LASTEXITCODE -ne 0) { throw "Build failed." }

New-Item -ItemType Directory -Force $outDir | Out-Null
foreach ($file in 'cshell.dll', 'object.lto', 'cres.dll', 'sres.dll') {
    Copy-Item (Join-Path $buildDir "$Config\$file") $outDir -Force
}

Write-Host "`nBuilt $Config binaries in $outDir"
Get-ChildItem $outDir | Format-Table Name, Length, LastWriteTime -AutoSize
