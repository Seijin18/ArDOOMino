# Testes físicos T1 e T2 (UART FPGA → ESP32)

> **Já validado no PC (não refazer na aula):** simulações Icarus L1/L2/L3, compilação das ROMs (`beacon` 26 linhas, `controller` 78 linhas) e build dos dois projetos ESP32 (`uart_sniffer` e `raycaster`). Detalhes em [PRE_AULA.md](PRE_AULA.md).

Pré-requisito (opcional reconferir): simulação OK no PC:

```powershell
powershell -ExecutionPolicy Bypass -File C:\Projetos\ArDOOMino\scripts\run_fpga_sim.ps1
```

## ROMs já geradas (troca sem compilador na aula)

As duas ROMs já estão prontas em `C:\Projetos\Quartus`:

| Arquivo | Conteúdo | Linhas |
|---------|----------|:------:|
| `rom_os_beacon.txt` | T1 (envia `U`) | 26 |
| `rom_os_controller.txt` | T2 / jogo (teclado→UART) | 78 |
| `rom_os.txt` | **ROM ativa** (no momento = controller) | — |

Para trocar a ROM ativa **sem make/gcc**, só copie o backup e recompile no Quartus:

```powershell
# T1
Copy-Item C:\Projetos\Quartus\rom_os_beacon.txt     C:\Projetos\Quartus\rom_os.txt -Force
# T2 / jogo
Copy-Item C:\Projetos\Quartus\rom_os_controller.txt C:\Projetos\Quartus\rom_os.txt -Force
```

## LEDs de debug do teclado

O top `CPU` liga a leitura do matricial aos LEDs da placa (sem depender do firmware):

| LED | Significado |
|-----|-------------|
| **LEDR[15:0]** | Máscara one-hot bruta (`keypad_segment`); um LED vermelho por posição da matriz |
| **LEDG[0]** | Tecla **8** (frente) |
| **LEDG[1]** | Tecla **2** |
| **LEDG[2]** | Tecla **4** |
| **LEDG[3]** | Tecla **6** |
| **LEDG[4]** | Tecla **7** |
| **LEDG[5]** | Tecla **9** |
| **LEDG[6]** | Tecla **\*** |
| **LEDG[7]** | Tecla **#** |
| **LEDG[8]** | Qualquer tecla pressionada |

Pressionar e soltar: os LEDs acompanham o scan (pode haver breve persistência até o keypad limpar a máscara). Use isso no **T2** junto com o sniffer UART.

## Cabo

| FPGA (DE2-115) | ESP32 |
|----------------|-------|
| GPIO[35] TX (`PIN_AG26`) | GPIO **32** (RX `Serial2`) |
| GND | GND |

115200 8N1. Não ligar GPIO 33 (TX do ESP) à FPGA.

## ESP32 — sniffer

```powershell
cd C:\Projetos\ArDOOMino\tools\uart_sniffer
pio run -t upload
pio device monitor -b 115200
```

---

## T1 — Beacon UART

**Objetivo:** bytes chegam ao ESP sem teclado.

1. Ativar ROM beacon: `Copy-Item C:\Projetos\Quartus\rom_os_beacon.txt C:\Projetos\Quartus\rom_os.txt -Force`
2. Quartus: Compile → Programmer → gravar → **SW[17] para cima (UP)**
3. Sniffer rodando (comando acima)
4. **Passa se:** monitor mostra repetidamente `RX: 0x55 'U'`
5. **Passa se:** desconectar o fio de sinal interrompe o fluxo

| Falha | Verificar |
|-------|-----------|
| Sniffer só mostra banner, `GPIO32=1`, zero bytes | Cabo no pino errado do header, ou **não recompilou** após copiar ROM (Quartus só lê `rom_os.txt`, não `rom_os_beacon.txt`) |
| Sniffer `GPIO32=0` fixo | Curto, TX preso em baixo, ou fio no pino errado |
| LEDs do teclado OK mas UART morto | LEDs vêm do scanner (50 MHz); UART vem da **CPU** — não provam beacon/UART |
| Um `U` e para | ROM antiga com delay grande — regenere `uart_beacon.c` e recompile |
| Lixo / caracteres estranhos | baud 115200, TX idle alto, GND comum |
| T2: pressionar 8 não gera `W` | ROM controller ativa? **SW[17] UP** após gravar? |

---

## T2 — Keypad → UART

**Objetivo:** teclas geram caracteres no sniffer (mesmo protocolo do jogo).

1. Ativar ROM controller: `Copy-Item C:\Projetos\Quartus\rom_os_controller.txt C:\Projetos\Quartus\rom_os.txt -Force`
2. Quartus: recompilar → programar → reset
3. Sniffer ativo (e observe os LEDs §LEDs de debug)

| Tecla | Esperado no monitor |
|:-----:|:-------------------:|
| 8 | `W` |
| 2 | `S` |
| 4 | `A` |
| 6 | `D` |
| 7 | `Q` |
| 9 | `E` |
| * | `F` |
| # | `R` |
| Soltar | espaço |

Marque conclusão em `openspec/changes/validate-uart-hardware/tasks.md`.

---

## Jogo completo (raycaster + keypad FPGA)

**Pré-requisitos:** T1 OK; recomendado T2 com sniffer.

1. **ROM controller ativa**
   ```powershell
   powershell -ExecutionPolicy Bypass -File C:\Projetos\ArDOOMino\scripts\build_rom.ps1 -Target controller -Use
   ```
2. **Quartus:** Compile → Programmer → gravar → **SW[17] UP** (switch mais à esquerda).
3. **Cabos:** GND comum + GPIO35 (FPGA) → GPIO32 (ESP). Display ST7789 conforme §2.1 do guia (BLK em 3.3 V).
4. **ESP32 — jogo** (substitui o sniffer):
   ```powershell
   cd C:\Projetos\ArDOOMino\raycaster
   pio run -t upload
   ```
   Opcional: `pio device monitor -b 115200` — deve aparecer `ArDOOMino raycaster ready`.

| Tecla | Ação no jogo |
|:-----:|--------------|
| 8 | Frente |
| 2 | Trás |
| 4 / 6 | Strafe |
| 7 / 9 | Girar |
| * | Atirar |
| # | Interagir |
| Soltar | Para |

**Passa se:** mapa visível no TFT **e** personagem responde ao keypad da FPGA.

| Falha | Verificar |
|-------|-----------|
| Tela OK, sem movimento | ROM controller + recompilar Quartus; cabo UART; SW[17] UP |
| Movimento no sniffer, não no jogo | Gravou `raycaster/` e não `uart_sniffer` |
| Para ao soltar não funciona | Mesmo protocolo T2 — espaço (0x20) no release |
