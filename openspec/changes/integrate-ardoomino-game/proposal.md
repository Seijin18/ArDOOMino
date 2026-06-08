# Proposal: integrar UART validada com o jogo ArDOOMino

## Status

**Desbloqueado** — T1 (beacon) validado na placa. Próximo: T2 + raycaster.

## Why

Após validar o link serial e o teclado matricial com o sniffer, conectar o mesmo protocolo ao loop do raycaster em `raycaster/src/main.cpp` (`Serial2` / `receiveData`).

## Scope

**In:** firmware `ardoomino_controller` na FPGA, build raycaster, teste de jogo com keypad.

**Out:** carregar controller via SO/HD (fase posterior).
