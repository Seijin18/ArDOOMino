#include <Arduino.h>

static HardwareSerial SerialFPGA(2);

static constexpr int PIN_RX = 32;
static unsigned long last_rx_ms = 0;
static unsigned long last_hint_ms = 0;
static uint32_t byte_count = 0;

static void print_line_state(const char *label) {
    int level = digitalRead(PIN_RX);
    Serial.printf("%s GPIO%d=%d (UART idle esperado=1, sem FPGA/cabo)\n", label, PIN_RX, level);
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println();
    Serial.println("=== FPGA UART sniffer (Serial2 RX=GPIO32) ===");
    Serial.println("Baud: 115200 8N1");
    Serial.println("Cabo: FPGA GPIO[35] (JP5 pino 40, PIN_AG26) -> ESP32 GPIO32 + GND comum");
    Serial.println("ROM T1: rom_os_beacon.txt copiado para rom_os.txt ANTES do Compile Quartus");
    Serial.println();

    pinMode(PIN_RX, INPUT);
    SerialFPGA.begin(115200, SERIAL_8N1, PIN_RX, 33);

    print_line_state("Antes de esperar dados:");
    Serial.println("Aguardando bytes... (T1: fluxo de 'U'; T2: pressione tecla 8 -> 'W')");
    Serial.println();

    last_rx_ms = millis();
    last_hint_ms = millis();
}

void loop() {
    while (SerialFPGA.available()) {
        int b = SerialFPGA.read();
        if (b < 0) {
            break;
        }
        byte_count++;
        last_rx_ms = millis();
        char c = (char)b;
        if (c >= 32 && c < 127) {
            Serial.printf("RX #%lu: 0x%02X '%c'\n", (unsigned long)byte_count, b, c);
        } else {
            Serial.printf("RX #%lu: 0x%02X\n", (unsigned long)byte_count, b);
        }
    }

    unsigned long now = millis();
    if (byte_count == 0 && now - last_hint_ms >= 4000) {
        last_hint_ms = now;
        Serial.println("--- sem bytes ainda ---");
        print_line_state("  Linha");
        Serial.println("  Checklist:");
        Serial.println("  1) GND FPGA <-> GND ESP (obrigatorio)");
        Serial.println("  2) Sinal: GPIO35 FPGA -> GPIO32 ESP (NAO GPIO33)");
        Serial.println("  3) Quartus le so rom_os.txt: copie rom_os_beacon.txt e RECOMPILE");
        Serial.println("  4) Apos programar FPGA: SW[17] (switch esquerda) para CIMA");
        Serial.println("  5) T2: com ROM controller, tecla 8 deve enviar 'W'");
        Serial.println();
    } else if (byte_count > 0 && now - last_rx_ms >= 3000 && now - last_hint_ms >= 3000) {
        last_hint_ms = now;
        Serial.printf("--- %lu bytes OK; silencio ha %lus ---\n",
                      (unsigned long)byte_count, (now - last_rx_ms) / 1000);
    }
}
