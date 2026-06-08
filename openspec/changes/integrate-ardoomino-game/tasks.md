# Tasks: integrate-ardoomino-game

Pré-requisito: T1 OK na placa (beacon). T2 recomendado antes do jogo.

## Firmware / ROM (PC)

- [x] `receiveData()` drena `Serial2` a cada frame (`raycaster/src/main.cpp`)
- [x] ROM controller em `rom_os.txt` (`build_rom.ps1 -Target controller -Use`)
- [x] Build raycaster OK (`pio run` em `raycaster/`)

## Placa (manual)

- [ ] Quartus: **recompilar** com `rom_os.txt` = controller → programar → **SW[17] UP**
- [ ] Cabo: GND + GPIO35 (FPGA, JP5 pino 40) → GPIO32 (ESP)
- [ ] Upload **raycaster** (não o sniffer): `pio run -t upload` em `raycaster/`
- [ ] Display acende com mapa; tecla **8** anda; soltar para
- [ ] (Opcional) T2 com sniffer antes do jogo — confirma `W`/espaço no monitor

## Depois

- [ ] Marcar T2 concluído em `validate-uart-hardware/tasks.md`
- [ ] Arquivar changes OpenSpec quando jogo estiver estável
