## ADDED Requirements

### REQ-T1-BEACON-ACCEPT

Com FPGA programada com ROM beacon e sniffer ativo, o monitor USB DEVE mostrar fluxo contínuo de linhas `RX: 0x55 'U'`.

#### Scenario: Cabo conectado

- **WHEN** GPIO35 (FPGA) → GPIO32 (ESP), GND comum, 115200
- **THEN** caracteres `U` aparecem no monitor serial USB

#### Scenario: Cabo desconectado

- **WHEN** o jumper de sinal é removido
- **THEN** o fluxo de `U` para

### REQ-T2-KEYPAD-ACCEPT

Com ROM `ardoomino_controller`, cada tecla do mapa DEVE gerar o ASCII documentado; soltar DEVE enviar espaço (0x20).

#### Scenario: Tecla 8

- **WHEN** o usuário pressiona tecla 8 no matricial
- **THEN** sniffer imprime `RX: 0x57 'W'`
