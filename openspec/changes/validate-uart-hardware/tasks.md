# Tasks: validate-uart-hardware

## Simulação (agente / script — você não precisa rodar)

- [x] L1 UART — `run_fpga_sim.ps1` ou `run_tests.ps1 -Level 1`
- [x] L2 Keypad — Level 2
- [x] L3 CPU+ROM+UART — Level 3 (requer `rom_os.txt` do controller)
- [x] Documentar mapeamento L1→T1, L2+L3→T2 em `sim/TEST_PLAN.md` e OpenSpec

## Artefatos (implementados e verificados)

- [x] OpenSpec: `openspec/` + change `validate-uart-hardware`
- [x] `uart_beacon.c` + `scripts/build_rom.ps1` (gera backups nomeados)
- [x] ROMs prontas: `rom_os_beacon.txt` (26) e `rom_os_controller.txt` (78)
- [x] T2 usa `ardoomino_controller.c` (ver `docs/PHYSICAL_TESTS.md`)
- [x] `tools/uart_sniffer` (PlatformIO) — build OK
- [x] `raycaster` env `esp32dev` corrigido (build do jogo OK)
- [x] LEDs de debug do teclado em `CPU.v` + `.qsf` (LEDR/LEDG)
- [x] `GUIA_AULA.md` + `docs/PRE_AULA.md` (o que já está pronto)
- [x] Change futuro `integrate-ardoomino-game` (scaffold)

## Físico (só você — roteiro em docs/PHYSICAL_TESTS.md)

### Pré-requisitos placa

- [ ] Quartus: Compile (top `CPU`) + gerar `.sof` (ver `docs/PRE_AULA.md` §B)
- [ ] Cabo: GND + GPIO35 (FPGA) → GPIO32 (ESP)

### T1 — Beacon UART

- [ ] Ativar ROM: `Copy-Item rom_os_beacon.txt rom_os.txt -Force` → recompilar/programar
- [ ] Subir `tools/uart_sniffer` + monitor 115200
- [ ] Fluxo contínuo de `U` no monitor; para ao desconectar cabo

### T2 — Keypad → UART

- [ ] Ativar ROM: `Copy-Item rom_os_controller.txt rom_os.txt -Force` → recompilar/programar
- [ ] Sniffer ativo; checklist teclas + LEDs verdes em PHYSICAL_TESTS.md

### Depois (outro change)

- [ ] `integrate-ardoomino-game` — jogo raycaster + SO (quando T1/T2 OK)
