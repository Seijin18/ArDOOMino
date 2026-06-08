# UART sniffer (testes físicos T1 e T2)

Sketch mínimo: lê `Serial2` (GPIO 32) e imprime no monitor USB.

## Cabo

- FPGA **GPIO35** (TX) → ESP32 **GPIO 32** (RX)
- **GND** comum

## Uso

Requer PlatformIO CLI (`pio` no PATH ou extensão VS Code / Cursor).

```powershell
cd C:\Projetos\ArDOOMino\tools\uart_sniffer
pio run -t upload
pio device monitor -b 115200
```

Se `pio` não for encontrado, use o caminho típico:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run -t upload -d C:\Projetos\ArDOOMino\tools\uart_sniffer
```

## FPGA (ROM)

```powershell
# T1 — beacon
powershell -File C:\Projetos\ArDOOMino\scripts\build_rom.ps1 -Target beacon

# T2 — teclado
powershell -File C:\Projetos\ArDOOMino\scripts\build_rom.ps1 -Target controller
```

Depois: Quartus Compile → Programmer → **SW[17] UP** (switch mais à esquerda).

Simulação (L1–L3): `..\..\scripts\run_fpga_sim.ps1`.

Roteiro físico: `..\..\docs\PHYSICAL_TESTS.md`.
