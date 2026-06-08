## ADDED Requirements

### REQ-SIM-GATE-PHYSICAL

A simulação Icarus L1–L3 DEVE ser executada automaticamente antes de qualquer teste físico T1/T2 documentado neste change.

#### Scenario: Script único na raiz ArDOOMino

- **WHEN** `scripts/run_fpga_sim.ps1` é invocado
- **THEN** delega para `Quartus\sim\run_tests.ps1 -Level All` e propaga exit code
