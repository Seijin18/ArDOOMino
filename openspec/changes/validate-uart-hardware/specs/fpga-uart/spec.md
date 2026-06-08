## ADDED Requirements

### REQ-T1-BEACON-ROM

O firmware `uart_beacon.c` DEVE gerar ROM que envia repetidamente o byte ASCII `U` (0x55) com atraso por loop vazio entre `serial_write`.

#### Scenario: Build da ROM beacon

- **WHEN** `scripts/build_rom.ps1 -Target beacon` é executado
- **THEN** `Quartus\rom_os.txt` é atualizado a partir de `binary_output.txt`

### REQ-T2-CONTROLLER-ROM

O teste físico T2 DEVE usar `ardoomino_controller.c` (mesmo mapa tecla→UART validado em L3).

#### Scenario: Build da ROM controller

- **WHEN** `scripts/build_rom.ps1 -Target controller` é executado
- **THEN** `rom_os.txt` contém o binário do controller
