# Design: validação UART

## Simulação (automática)

```mermaid
flowchart LR
  L1[L1 tb_uart_tx]
  L2[L2 tb_keypad]
  L3[L3 tb_ardoomino_uart]
  L1 --> L2 --> L3
```

- Implementação: [Quartus/sim/run_tests.ps1](C:/Projetos/Quartus/sim/run_tests.ps1)
- ROM L3: `ardoomino_controller.c` → `rom_os.txt`

## Físico (manual)

| Teste | FPGA ROM | ESP32 |
|-------|----------|-------|
| T1 beacon | `uart_beacon.c` (a criar) | sniffer |
| T2 keypad | `ardoomino_controller.c` ou `keypad_uart_debug.c` | sniffer |

## Gate

Não iniciar T1 na placa se L1 falhar. Não iniciar T2 se L2 ou L3 falhar.
