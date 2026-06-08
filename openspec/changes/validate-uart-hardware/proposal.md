# Proposal: validar UART FPGA → ESP32 (hardware)

## Why

Antes de integrar SO, compilador completo e jogo, precisamos provar o link serial e o teclado matricial na placa real.

## What

Dois testes físicos manuais (T1, T2), precedidos por simulação Icarus **automatizada** (L1–L3).

## Scope

**In:** pinagem AG26, firmware ROM de teste, sketch sniffer ESP32, specs OpenSpec, script `run_fpga_sim.ps1`.

**Out:** OS/HD/scheduler, integração raycaster (fase seguinte).

## Approach

1. Agente/CI roda L1→L2→L3 (Icarus).
2. Usuário executa só T1 e T2 na bancada.
