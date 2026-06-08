# Run FPGA Icarus tests (L1-L3). Automates pre-check before physical T1/T2.
# Usage: .\scripts\run_fpga_sim.ps1 [-Level 1|2|3|All] [-RegenRom] [-InstallIcarus]

param(
    [ValidateSet("1", "2", "3", "All")]
    [string]$Level = "All",
    [switch]$RegenRom,
    [switch]$InstallIcarus
)

$ErrorActionPreference = "Stop"
$ArDOOMinoRoot = Split-Path -Parent $PSScriptRoot
$QuartusRoot = "C:\Projetos\Quartus"
$CompilerRoot = "C:\Projetos\Compilador\156711"

if ($RegenRom -or ($Level -eq "3" -or $Level -eq "All")) {
    $romPath = Join-Path $QuartusRoot "rom_os.txt"
    if ($RegenRom -or -not (Test-Path $romPath)) {
        Write-Host "Regenerating rom_os.txt from ardoomino_controller.c ..." -ForegroundColor Cyan
        Push-Location $CompilerRoot
        try {
            & make run ardoomino_controller.c
            if ($LASTEXITCODE -ne 0) { throw "make run failed" }
            Copy-Item -Force (Join-Path $CompilerRoot "binary_output.txt") $romPath
            Write-Host "ROM copied to $romPath"
        }
        finally {
            Pop-Location
        }
    }
}

$simScript = Join-Path $QuartusRoot "sim\run_tests.ps1"
if (-not (Test-Path $simScript)) {
    throw "Missing $simScript"
}

$args = @("-ExecutionPolicy", "Bypass", "-File", $simScript, "-Level", $Level)
if ($InstallIcarus) { $args += "-InstallIcarus" }

Write-Host "ArDOOMino FPGA simulation (Icarus) - gate before physical T1/T2" -ForegroundColor Green
Write-Host '  L1 maps to physical T1 (UART beacon)' -ForegroundColor DarkGray
Write-Host '  L2+L3 map to physical T2 (keypad to UART)' -ForegroundColor DarkGray
Write-Host ""

& powershell @args
exit $LASTEXITCODE
