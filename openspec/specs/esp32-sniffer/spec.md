# ESP32 — recepção UART da FPGA

## Requirements

### REQ-ESP-SERIAL2

O sniffer e o jogo DEVEM usar `Serial2` em **GPIO 32** (RX), **115200 8N1**.

### REQ-ESP-SNIFFER-MODE

No modo sniffer (testes T1/T2), o firmware DEVE apenas reencaminhar bytes recebidos para `Serial` (USB), sem inicializar TFT nem game loop.

### REQ-ESP-PHYSICAL-ONLY

Testes T1 e T2 na placa são **manuais**; a validação RTL prévia é feita por simulação Icarus (ver `simulation/spec.md`).

### REQ-ESP-SNIFFER-SKETCH

O sketch em `tools/uart_sniffer` DEVE imprimir cada byte de `Serial2` no monitor USB em hex e ASCII imprimível.

### REQ-T1-BEACON-ACCEPT

Com ROM `uart_beacon.c`, o monitor DEVE mostrar fluxo de `RX: 0x55 'U'`.

### REQ-T2-KEYPAD-ACCEPT

Com ROM `ardoomino_controller.c`, teclas 8/2/4/6/7/9/*/# DEVEM gerar `W`/`S`/`A`/`D`/`Q`/`E`/`F`/`R`; soltar envia espaço.

Roteiro: `docs/PHYSICAL_TESTS.md`.
