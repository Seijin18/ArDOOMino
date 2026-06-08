# ArDOOMino — integração FPGA (DE2-115) + ESP32

## Repositórios

| Caminho | Papel |
|---------|--------|
| `C:\Projetos\Quartus` | CPU MIPS, `uart_tx`, `keypad`, `rom_os.txt`, simulação Icarus |
| `C:\Projetos\Compilador\156711` | C → ROM (`make run foo.c` → `binary_output.txt`) |
| `C:\Projetos\ArDOOMino` | ESP32 raycaster, OpenSpec, guias |

## Link UART (placa)

- **115200 8N1**, unidirecional FPGA → ESP32
- FPGA `uart_tx` → **GPIO[35]** (`PIN_AG26`)
- ESP32 **GPIO 32** = RX (`Serial2`)
- **GND comum** obrigatório
- Não conectar GPIO 33 (TX ESP) à FPGA

## Divisão de testes

| Quem | O quê |
|------|--------|
| **Agente / CI / script** | Simulação Icarus (L1, L2, L3) — ver `openspec/specs/simulation/spec.md` |
| **Você (placa)** | T1 beacon UART + T2 keypad→UART — ver `openspec/changes/validate-uart-hardware/tasks.md` |

## Comandos rápidos

```powershell
# Simulação (automático, antes da placa)
powershell -ExecutionPolicy Bypass -File C:\Projetos\ArDOOMino\scripts\run_fpga_sim.ps1

# ROM para T1 ou T2
powershell -ExecutionPolicy Bypass -File C:\Projetos\ArDOOMino\scripts\build_rom.ps1 -Target beacon
powershell -ExecutionPolicy Bypass -File C:\Projetos\ArDOOMino\scripts\build_rom.ps1 -Target controller
```

Testes físicos: [docs/PHYSICAL_TESTS.md](../docs/PHYSICAL_TESTS.md).
