# Checklist pré-aula — validação completa

Tudo que **não** precisa de hardware já foi rodado e está verde. Na aula sobra apenas o que exige FPGA + ESP32 físicos.

## A. Já validado no PC (não refazer)

| # | Item | Como foi verificado | Estado |
|---|------|---------------------|:------:|
| 1 | Toolchain | `iverilog` (C:\iverilog\bin), `make` (choco), `gcc` (Strawberry), `pio` (.platformio) | OK |
| 2 | ROM beacon (T1) | `build_rom.ps1 -Target beacon` → 26 linhas | OK |
| 3 | ROM controller (T2/jogo) | `build_rom.ps1 -Target controller` → 78 linhas | OK |
| 4 | Sim L1 (uart_tx) | `run_fpga_sim.ps1 -Level 1` → 3/3 | OK |
| 5 | Sim L2 (keypad) | `-Level 2` → 16/16 | OK |
| 6 | Sim L3 (CPU+ROM+UART) | `-Level 3` → 15/15 | OK |
| 7 | Build ESP32 sniffer | `pio run` em `tools/uart_sniffer` | OK |
| 8 | Build ESP32 jogo | `pio run -e esp32dev` em `raycaster` | OK |
| 9 | Pinagem `CPU` + LEDs | `ProcessadorMIPS.qsf` (AG26, keypad GPIO, LEDR/LEDG) | OK |

Reconferir tudo de uma vez (≈ 30 s):

```powershell
powershell -ExecutionPolicy Bypass -File C:\Projetos\ArDOOMino\scripts\run_fpga_sim.ps1
```

## B. Você precisa fazer no Quartus (uma vez, antes da aula)

Só isto exige o Quartus (não automatizável aqui):

1. Abrir `C:\Projetos\Quartus\ProcessadorMIPS.qpf`.
2. Conferir top-level entity = **`CPU`**.
3. **Compile** (Processing → Start Compilation) com `rom_os.txt` = controller (estado atual).
4. Salvar o `.sof` gerado em `output_files/`.
5. (Opcional) Repetir o Compile com a ROM beacon ativa, para já ter os dois `.sof`.

> A síntese dos `.v` (`uart_tx`, `keypad`, `CPU`, etc.) acontece junto nesse Compile. Não há projeto separado por módulo — a validação isolada de `uart_tx` e `keypad` é a simulação Icarus (L1/L2).

## C. Na aula — só físico

Sequência mínima (roteiro detalhado em [PHYSICAL_TESTS.md](PHYSICAL_TESTS.md)):

1. **Cabo:** GND comum + GPIO[35] (FPGA, PIN_AG26) → GPIO 32 (ESP32). Não ligar GPIO 33.
2. **ESP sniffer:** `pio run -t upload` em `tools/uart_sniffer` + `pio device monitor -b 115200`.
3. **T1:** ativar `rom_os_beacon.txt` → gravar FPGA → ver `U` no monitor.
4. **T2:** ativar `rom_os_controller.txt` → gravar FPGA → teclas geram `W/S/A/D/Q/E/F/R`; LEDs verdes acendem por tecla.
5. **Jogo:** `pio run -t upload` em `raycaster` → jogar com o teclado.

Trocar ROM ativa sem compilador:

```powershell
Copy-Item C:\Projetos\Quartus\rom_os_beacon.txt     C:\Projetos\Quartus\rom_os.txt -Force   # T1
Copy-Item C:\Projetos\Quartus\rom_os_controller.txt C:\Projetos\Quartus\rom_os.txt -Force   # T2/jogo
```

> Após trocar `rom_os.txt`, é preciso **recompilar no Quartus** e regravar (a ROM entra na síntese). Por isso o passo B opcional (gerar os dois `.sof` antes) economiza tempo na aula.

## D. O que levar

- [ ] Notebook com Quartus + os `.sof` já compilados (controller e, se possível, beacon)
- [ ] DE2-115 + cabo USB-Blaster
- [ ] ESP32 + display + cabo USB
- [ ] 2 jumpers (GND + sinal) para GPIO35→GPIO32
- [ ] *(Opcional)* adaptador USB-serial 3.3 V para sniffer sem ESP32
