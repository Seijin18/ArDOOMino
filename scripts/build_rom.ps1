# Build ROM from C source and copy to Quartus rom_os.txt
# Usage: .\scripts\build_rom.ps1 -Target beacon|controller [-Use]
#   (no -Use)  : compila e salva backup nomeado (rom_os_<target>.txt); NAO mexe em rom_os.txt
#                a menos que ele ainda nao exista.
#   -Use       : alem do backup, define rom_os.txt = essa ROM (ativa para o Quartus).
#
# Para a aula, sem compilador: troque a ROM ativa copiando o backup:
#   Copy-Item C:\Projetos\Quartus\rom_os_beacon.txt     C:\Projetos\Quartus\rom_os.txt -Force
#   Copy-Item C:\Projetos\Quartus\rom_os_controller.txt C:\Projetos\Quartus\rom_os.txt -Force

param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("beacon", "controller")]
    [string]$Target,
    [switch]$Use
)

$ErrorActionPreference = "Stop"
$CompilerRoot = "C:\Projetos\Compilador\156711"
$QuartusDir = "C:\Projetos\Quartus"
$QuartusRom = Join-Path $QuartusDir "rom_os.txt"
$Backup = Join-Path $QuartusDir "rom_os_$Target.txt"

$source = switch ($Target) {
    "beacon"     { "uart_beacon.c" }
    "controller" { "ardoomino_controller.c" }
}

Push-Location $CompilerRoot
try {
    Write-Host "Compiling $source ..."
    & make run $source
    if ($LASTEXITCODE -ne 0) { throw "make run failed" }
    $bin = Join-Path $CompilerRoot "binary_output.txt"
    if (-not (Test-Path $bin)) { throw "binary_output.txt not found" }

    Copy-Item -Force $bin $Backup
    $lines = (Get-Content $Backup | Measure-Object -Line).Lines
    Write-Host "Backup salvo: $Backup ($lines linhas)" -ForegroundColor Green

    if ($Use -or -not (Test-Path $QuartusRom)) {
        Copy-Item -Force $Backup $QuartusRom
        Write-Host "ROM ativa: rom_os.txt = $Target" -ForegroundColor Green
    } else {
        Write-Host "rom_os.txt mantido. Use -Use para ativar, ou copie o backup." -ForegroundColor DarkGray
    }
}
finally {
    Pop-Location
}
