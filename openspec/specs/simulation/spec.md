# Simulação FPGA (Icarus Verilog)

## Requirements

### REQ-SIM-TOOL

O projeto DEVE usar Icarus Verilog (`iverilog`, `vvp`) com flag `-DSIMULATION` nos testbenches.

#### Scenario: Execução automatizada

- **WHEN** `run_fpga_sim.ps1` ou `Quartus\sim\run_tests.ps1 -Level All` é executado
- **THEN** L1, L2 e L3 terminam com exit code 0 e mensagem `ALL SELECTED TESTS FINISHED`

### REQ-SIM-L1 (gate para teste físico T1)

L1 DEVE validar `uart_tx.v` isolado: idle alto, bytes `W`/`S`/espaço, sem erro de framing.

#### Scenario: UART unitário passa

- **WHEN** `tb_uart_tx.v` roda com `uart_monitor.v`
- **THEN** summary reporta `fail=0` e `ALL TESTS PASSED`

### REQ-SIM-L2 (gate para teste físico T2 — keypad)

L2 DEVE validar `keypad.v` com modelo virtual: 8 teclas do jogo + release → `segment=0`.

#### Scenario: Keypad unitário passa

- **WHEN** `tb_keypad.v` roda com `keypad_model.v`
- **THEN** 16 checks passam (8 teclas + 8 releases)

### REQ-SIM-L3 (gate para teste físico T2 — UART)

L3 DEVE validar CPU + `rom_os.txt` (firmware `ardoomino_controller.c`): tecla → byte UART + soltar → espaço.

#### Scenario: Integração passa

- **WHEN** `tb_ardoomino_uart.v` roda com ROM atual
- **THEN** mapa tecla→ASCII (`W`,`S`,`A`,`D`,`Q`,`E`,`F`,`R`) e releases com `0x20` passam

### REQ-SIM-ROM

Antes de L3, `Quartus\rom_os.txt` DEVE existir e ser gerado a partir do controller:

```powershell
cd C:\Projetos\Compilador\156711
make run ardoomino_controller.c
Copy-Item binary_output.txt C:\Projetos\Quartus\rom_os.txt -Force
```

## Mapeamento sim → físico

| Sim | Teste físico (manual) |
|-----|------------------------|
| L1 | **T1** — beacon UART no ESP sniffer |
| L2 + L3 | **T2** — teclado matricial → UART no ESP |

L1 **não** substitui T1 na placa (cabo, níveis, baud real). L2+L3 **não** substituem T2 (hardware do teclado no JP5).
