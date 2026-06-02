# ArDOOMino — Guia de teste para aula

Montagem **FPGA (DE2-115) + teclado + ESP32 (raycaster)** com roteiro de **setup**, **testes por componente** e **diagnóstico em caso de falha**.

---

## Como usar este guia

| Situação | Onde ir |
|----------|---------|
| Montar tudo pela primeira vez | [§4 Setup completo](#4-setup-completo-happy-path) |
| Algo falhou no teste final | [§5 Árvore de diagnóstico](#5-árvore-de-diagnóstico) |
| Testar um módulo isolado | [§6 Testes por componente](#6-testes-por-componente) |
| Referência de cabos e teclas | [§2 Hardware](#2-hardware) |

**Ideia central:** subir a pilha camada a camada. Se o nível *N* falha, não avance para *N+1* — volte ao teste isolado daquela camada.

```
PC (sim/compilador) → ROM → FPGA programada → ESP32 sozinho → cabo UART → sistema completo
```

---

## 1. O que você precisa

| Item | Função |
|------|--------|
| Placa **DE2-115** | CPU MIPS + keypad + UART TX |
| **ESP32** + display **ST7789** 240×240 | Raycaster |
| Cabos USB + **2 jumpers** (GND + UART) | Programação e link serial |
| PC: **Quartus**, **PlatformIO**, **make/gcc** | Build |
| *(Opcional)* adaptador **USB‑serial 3.3 V** | Ver bytes da FPGA sem ESP32 |

Pastas:

| Pasta | Conteúdo |
|-------|----------|
| `C:\Projetos\Quartus` | Verilog, Quartus, simulação (`sim/`) |
| `C:\Projetos\Compilador\156711` | Compilador C → ROM |
| `C:\Projetos\ArDOOMino\raycaster` | Jogo ESP32 |

---

## 2. Hardware

### 2.1 Display → ESP32

| Display | ESP32 |
|---------|-------|
| VCC | 3.3 V |
| GND | GND |
| SCL | GPIO **18** |
| SDA / MOSI | GPIO **23** |
| RES | GPIO **16** |
| DC | GPIO **17** |
| BLK | 3.3 V *(obrigatório — sem isso tela preta)* |

### 2.2 FPGA ↔ ESP32 (UART 115200 8N1)

| DE2-115 | ESP32 |
|---------|-------|
| **GPIO[35]** (`PIN_F16`, saída `uart_tx`) | **GPIO 32** (RX do `Serial2`) |
| **GND** | **GND** |

- Comunicação **unidirecional**: FPGA transmite, ESP32 recebe.
- **Não** conectar GPIO 33 (TX do ESP32) à FPGA.
- Tensão **3.3 V** em ambos os lados.

### 2.3 DE2-115

- Teclado **4×4** integrado na placa.
- **KEY0** (`PIN_M23`) — botão do processador.
- Após programar a FPGA, dê **reset** (KEY0 ou religar).

### 2.4 Mapa tecla → jogo

| Tecla | Ação | UART |
|:-----:|------|:----:|
| 8 | Frente | `W` |
| 2 | Trás | `S` |
| 4 | Esquerda | `A` |
| 6 | Direita | `D` |
| 7 | Girar esq. | `Q` |
| 9 | Girar dir. | `E` |
| * | Atirar | `F` |
| # | Ação | `R` |
| Soltar | Parar | espaço |

---

## 3. Comandos rápidos

```powershell
# Compilar controlador → ROM da FPGA
cd C:\Projetos\Compilador\156711
make run ardoomino_controller.c
Copy-Item binary_output.txt C:\Projetos\Quartus\rom_os.txt -Force

# Simulação (ver §6.2)
cd C:\Projetos\Quartus
powershell -ExecutionPolicy Bypass -File .\sim\run_tests.ps1 -Level All
powershell -ExecutionPolicy Bypass -File .\sim\run_tests.ps1 -Level 1   # só UART
powershell -ExecutionPolicy Bypass -File .\sim\run_tests.ps1 -Level 2   # só keypad
powershell -ExecutionPolicy Bypass -File .\sim\run_tests.ps1 -Level 3   # integração

# ESP32
cd C:\Projetos\ArDOOMino\raycaster
platformio run --target upload
platformio device monitor -b 115200
```

---

## 4. Setup completo (happy path)

Ordem recomendada na aula:

1. **ROM** — `make run ardoomino_controller.c` + copiar `binary_output.txt` → `Quartus\rom_os.txt`  
   *(Nunca use `rom_os_kernel.txt` neste modo — é o SO, não o jogo.)*
2. **FPGA** — Quartus: abrir `ProcessadorMIPS.qpf` → Compile → Programmer → gravar `.sof` → reset.
3. **ESP32** — `platformio run --target upload`.
4. **Cabos** — GND comum + GPIO35 (FPGA) → GPIO32 (ESP32).
5. **Teste** — display com mapa; tecla **8** move; soltar para.

**Critério de sucesso:** personagem responde ao keypad e para ao soltar.

---

## 5. Árvore de diagnóstico

Use quando o teste final (§4) falhar. Siga de cima para baixo até achar a camada quebrada.

```mermaid
flowchart TD
    A[Sistema completo falhou] --> B{Display ESP32 OK?}
    B -->|Não| C[Teste E1–E2: ESP32 isolado]
    B -->|Sim| D{Teclado move via USB Serial?}
    D -->|Sim| F[Problema: cabo UART ou TX FPGA]
    D -->|Não| E{Sim L3 passou?}
    E -->|Não| G[Problema: ROM / firmware / CPU sim]
    E -->|Sim| H{Sim L1 e L2 passaram?}
    H -->|L1 falhou| I[Corrigir uart_tx.v / timing]
    H -->|L2 falhou| J[Corrigir keypad.v / modelo]
    H -->|Sim| K[Problema: placa física ou Quartus]
    F --> L[Teste F2: sniffer UART]
    K --> M[Teste F1 + cabos + reprogramar]
```

### Tabela sintoma → camada

| Sintoma | Camada provável | Teste isolado |
|---------|-----------------|---------------|
| Tela preta | ESP32 / display | [E1](#e1-display-e-boot-do-esp32) |
| Jogo visível, nada responde | UART ou ROM | [E3](#e3-controle-via-usb-serial) depois [F2](#f2-uart-da-fpga-sniffer) |
| Responde no USB, não no keypad | Cabo / FPGA TX | [F2](#f2-uart-da-fpga-sniffer) |
| Sim L3 falha | ROM ou Verilog | [C1](#c1-compilador--rom) + [S3](#s3-l3--cpu--rom--keypad--uart) |
| Sim L1 falha | Módulo `uart_tx` | [S1](#s1-l1--módulo-uart-tx) |
| Sim L2 falha | Módulo `keypad` | [S2](#s2-l2--módulo-keypad) |
| FPGA “travada” / loop | ROM errada (kernel) | [C1](#c1-compilador--rom) |
| Movimento sem parar | Release / keypad latch | [S2](#s2-l2--módulo-keypad) + tecla solta |

---

## 6. Testes por componente

Cada bloco tem: **objetivo**, **como rodar**, **passou**, **falhou → corrigir**.

---

### C — Compilador e ROM (sem placa)

#### C1 — Compilador + ROM

**Objetivo:** garantir que o firmware C vira o binário certo para a FPGA.

```powershell
cd C:\Projetos\Compilador\156711
make run ardoomino_controller.c
```

**Passou se:**

- Comandos terminam sem erro.
- `assembly_output.asm` contém `READ_KEYPAD` e `LCD_WRITE_CHAR`.
- `binary_output.txt` tem ~**78 linhas** (não centenas como o kernel).
- Último loop é `J` de volta ao `READ_KEYPAD`, **não** `J 157` + `HALT` do kernel.

```powershell
Copy-Item binary_output.txt C:\Projetos\Quartus\rom_os.txt -Force
(Get-Content C:\Projetos\Quartus\rom_os.txt | Measure-Object -Line).Lines
```

**Falhou → corrigir em:** `Compilador/156711` (`ardoomino_controller.c`, `assembler.c`, `bison.y`).

---

### S — Simulação no PC (Icarus Verilog)

Requisito: `iverilog` + `vvp` ([Icarus Verilog](https://bleyer.org/icarus/)) ou `run_tests.ps1 -InstallIcarus`.

Todos os testes em `C:\Projetos\Quartus`. Detalhes em `sim/TEST_PLAN.md`.

#### S1 — L1: módulo UART TX

**Objetivo:** validar `uart_tx.v` isolado — idle alto, bytes `W`/`S`/espaço, sem erro de framing.

```powershell
cd C:\Projetos\Quartus
powershell -ExecutionPolicy Bypass -File .\sim\run_tests.ps1 -Level 1
```

**Passou se:** `*** ALL TESTS PASSED ***`, 0 framing errors.

**Falhou → corrigir em:** `Quartus/uart_tx.v`, `sim/uart_monitor.v`, `sim/tb_uart_tx.v`.

---

#### S2 — L2: módulo keypad

**Objetivo:** scanner + latch do teclado; cada tecla do jogo gera o one-hot correto e release zera.

```powershell
powershell -ExecutionPolicy Bypass -File .\sim\run_tests.ps1 -Level 2
```

**Passou se:** 16/16 checks (8 teclas + 8 releases). Tecla 7 = `0x0100`.

**Falhou → corrigir em:** `Quartus/keypad.v`, `sim/keypad_model.v`, `sim/tb_keypad.v`.

---

#### S3 — L3: CPU + ROM + keypad + UART

**Objetivo:** integração completa **sem hardware** — pressionar tecla virtual → byte UART esperado.

**Pré-requisito:** [C1](#c1-compilador--rom) com `rom_os.txt` atualizado.

```powershell
powershell -ExecutionPolicy Bypass -File .\sim\run_tests.ps1 -Level 3
```

**Passou se:** `15/15` (8 teclas + release + `#`/`R`).

**Falhou → corrigir em:**

| Falha | Onde olhar |
|-------|------------|
| Timeout UART, CPU roda | `rom_os.txt` errado, `ProgramCounter.v`, `Controle.v` |
| Tecla errada | `ardoomino_controller.c`, mapa §2.4 |
| Só L3 falha, L1/L2 OK | `CPU.v` (ligação UART), `ROMSinglePort.v` |

**Ordem recomendada se L3 falha:** S1 → S2 → C1 → S3 (nunca pule L1/L2).

---

### E — ESP32 isolado (sem FPGA)

Desconecte o jumper UART da FPGA. Alimente só o ESP32.

#### E1 — Display e boot do ESP32

**Objetivo:** hardware do display e firmware base.

```powershell
cd C:\Projetos\ArDOOMino\raycaster
platformio run --target upload
```

**Passou se:** tela acende, mapa do jogo visível (letterbox preto em cima/baixo).

**Falhou → corrigir em:** cabos §2.1 (especialmente **BLK**), `platformio.ini`, `raycaster/`.

---

#### E2 — Serial USB (monitor)

**Objetivo:** loop do jogo e parsing de input funcionam **sem** FPGA.

```powershell
platformio device monitor -b 115200
```

No monitor, envie: `W` → personagem anda; ` ` (espaço) → para.

**Passou se:** movimento obedece à tabela §2.4 via teclado do PC.

**Falhou → corrigir em:** `raycaster/src/main.cpp` (`applyInputChar`, `receiveData`).

---

#### E3 — Controle via USB Serial

**Objetivo:** confirmar que o jogo **não** depende da FPGA para lógica de movimento.

Mesmo teste que E2. Se E1+E2 passam, o ESP32 está OK.

**Se E2 passa mas sistema completo não:** problema está na **FPGA** ou no **cabo UART**, não no jogo.

---

### F — FPGA isolada (sem depender do jogo)

#### F1 — Programação Quartus

**Objetivo:** bitstream válido na DE2-115.

1. [C1](#c1-compilador--rom) feito.
2. Quartus → Compile → Programmer → 100% success.
3. Reset na placa.

**Passou se:** placa programa sem erro; após reset, CPU executa (sem travamento visível — difícil ver sem UART).

**Falhou → corrigir em:** erros de sintese no Quartus, pinos em `ProcessadorMIPS.qsf`.

---

#### F2 — UART da FPGA (sniffer)

**Objetivo:** ver bytes reais na saída `uart_tx` ao pressionar teclas.

**Opção A — ESP32 como sniffer** *(sem alterar código)*  
1. Conecte GND + GPIO35 → GPIO32.  
2. Suba o jogo ([E1](#e1-display-e-boot-do-esp32)).  
3. Pressione **8** no keypad da FPGA.  
4. Personagem deve andar para frente.

**Opção B — Adaptador USB‑serial 3.3 V** *(melhor para debug puro)*  
1. FPGA GND → GND do adaptador.  
2. GPIO35 (TX FPGA) → **RX** do adaptador.  
3. Terminal serial **115200 8N1**.  
4. Pressione **8** → deve aparecer `W`; solte → espaço.

**Passou se:** bytes corretos conforme §2.4.

**Falhou → corrigir em:**

| Observação | Causa provável |
|------------|----------------|
| Nenhum byte | ROM errada, `uart_tx.v`, ligação `CPU.v` → `lcd_write_enable` |
| Lixo / baud errado | 115200, TX idle em nível **alto** |
| Byte errado por tecla | `keypad.v`, `ardoomino_controller.c` |
| Só no sniffer falha, sim OK | Quartus não recompilou após trocar `rom_os.txt` |

---

#### F3 — Keypad na placa (indireto)

Não há LED por tecla no firmware mínimo. Use:

1. [S2](#s2-l2--módulo-keypad) no PC — prova o Verilog.
2. [F2](#f2-uart-da-fpga-sniffer) — pressionar tecla gera byte certo.

**Passou se:** cada tecla §2.4 gera o char esperado no sniffer.

**Falhou → corrigir em:** hardware do teclado na DE2-115 (raro) ou `keypad.v`.

---

### L — Link FPGA → ESP32

**Pré-requisitos:** E1+E2 OK, F2 OK (bytes corretos no sniffer ou tecla 8 → `W`).

1. GND comum **obrigatório**.
2. GPIO35 → GPIO32 (único fio de sinal).
3. Suba ESP32 com jogo; FPGA programada e resetada.
4. **Não** abra monitor USB enviando `W` ao mesmo tempo — prioridade é `Serial2`, mas debug fica confuso.

**Passou se:** keypad da FPGA controla o jogo; soltar para.

**Falhou → corrigir em:**

| E2 OK, F2 OK, link falha | Verificar |
|--------------------------|-----------|
| Cabo solto / GPIO errado | GPIO **32** = RX, não 33 |
| GND ausente | Referência comum |
| Baud | 115200 nos dois lados |

---

## 7. Matriz de decisão rápida

| S1 | S2 | S3 | E1 | E2 | F2 | Diagnóstico |
|:--:|:--:|:--:|:--:|:--:|:--:|-------------|
| ✓ | ✓ | ✓ | ✓ | ✓ | ✗ | FPGA / ROM na placa / Quartus |
| ✓ | ✓ | ✗ | — | — | — | ROM ou integração Verilog |
| ✗ | — | — | — | — | — | `uart_tx.v` |
| — | ✗ | — | — | — | — | `keypad.v` |
| ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | Cabo ESP32↔FPGA ou prioridade serial |
| ✓ | ✓ | ✓ | ✗ | — | — | Display / ESP32 hardware |

*(✓ = passou, ✗ = falhou, — = não testado ainda)*

---

## 8. Checklist antes da apresentação

**No PC (5 min):**

- [ ] [C1](#c1-compilador--rom) — ROM ~78 linhas, copiada para `Quartus`
- [ ] [S1](#s1-l1--módulo-uart-tx) + [S2](#s2-l2--módulo-keypad) + [S3](#s3-l3--cpu--rom--keypad--uart) passam

**Na bancada:**

- [ ] [E1](#e1-display-e-boot-do-esp32) — display OK
- [ ] [F1](#f1--programação-quartus) — FPGA programada
- [ ] Jumpers: **GND** + **GPIO35 → GPIO32**
- [ ] Tecla **8** anda; soltar para
- [ ] *(Opcional)* [F2 opção B](#f2-uart-da-fpga-sniffer) com USB‑serial

---

## 9. Problemas comuns (referência)

| Sintoma | Primeiro teste | Correção típica |
|---------|----------------|-----------------|
| Tela preta | E1 | BLK em 3.3 V, reupload |
| Jogo OK, keypad morto | F2 | ROM, cabo UART, GND |
| Sim OK, placa não | F1 + F2 | Recompilar Quartus após trocar ROM |
| Loop / FPGA “travada” | C1 | `rom_os.txt` ≠ kernel |
| Movimento não para | S2, F3 | Soltar tecla; `keypad.v` latch |
| `make` não encontrado | — | Git Bash / MSYS com `make` no PATH |
| `iverilog` não encontrado | — | `run_tests.ps1 -InstallIcarus` ou reiniciar terminal |

---

## 10. Fluxo sugerido na aula (com tempo)

| Tempo | Atividade |
|-------|-----------|
| 0–5 min | C1 + S1/S2/S3 no PC |
| 5–15 min | F1 programar FPGA + E1 upload ESP32 |
| 15–20 min | E2 teste USB; depois L link completo |
| Se falhar | §5 árvore → teste isolado da camada indicada |

Boa aula.
