# FPGA UART (DE2-115)

## Requirements

### REQ-FPGA-UART-PIN

A saída serial `uart_tx` DEVE estar em **PIN_AG26** (GPIO[35]), padrão **3.3-V LVTTL**.

### REQ-FPGA-UART-PROTO

- Baud: **115200**
- Formato: **8N1**
- Idle: **nível alto**
- Acionamento: opcode `UART_WRITE_CHAR` / `serial_write` → `uart_tx_strobe` + `rs[7:0]` em `CPU.v`

### REQ-FPGA-UART-BUSY

Enquanto `uart_tx` está `busy`, um novo `write_strobe` DEVE ser ignorado (sem fila).

### REQ-FPGA-ROM-BUILD

| Alvo | Fonte C | Uso |
|------|---------|-----|
| T1 beacon | `Compilador/156711/uart_beacon.c` | `build_rom.ps1 -Target beacon` |
| T2 keypad | `Compilador/156711/ardoomino_controller.c` | `build_rom.ps1 -Target controller` |

Saída copiada para `Quartus/rom_os.txt`.
