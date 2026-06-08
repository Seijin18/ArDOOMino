#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>

#include "game/game.hpp"
#include "game/system.h"

TFT_eSPI tft;

static HardwareSerial SerialFPGA(2);

static void applyInputChar(char c, InputData &data)
{
    switch (c)
    {
    case 'w':
    case 'W':
        data.flags = (1 << UP);
        data.x = 0;
        break;
    case 's':
    case 'S':
        data.flags = (1 << DOWN);
        data.x = 0;
        break;
    case 'a':
    case 'A':
        data.flags = (1 << LEFT);
        data.x = 0;
        break;
    case 'd':
    case 'D':
        data.flags = (1 << RIGHT);
        data.x = 0;
        break;
    case 'q':
    case 'Q':
        data.flags = 0;
        data.x = KEYPAD_TURN_RATE;
        break;
    case 'e':
    case 'E':
        data.flags = 0;
        data.x = -KEYPAD_TURN_RATE;
        break;
    case 'f':
    case 'F':
        data.flags |= (1 << 7);
        break;
    case 'r':
    case 'R':
        data.flags |= (1 << 6);
        break;
    case ' ':
        data.flags = 0;
        data.x = 0;
        break;
    }
}

void render() {
    const int GAME_WIDTH = Screen::SCREEN_WIDTH;   // 240
    const int GAME_HEIGHT = Screen::SCREEN_HEIGHT; // 160
    const int DISPLAY_HEIGHT = 240;
    const int Y_OFFSET = (DISPLAY_HEIGHT - GAME_HEIGHT) / 2; // 40 pixels down

    tft.setSwapBytes(true);
    tft.pushImage(0, Y_OFFSET, GAME_WIDTH, GAME_HEIGHT, Screen::_screen);
}

InputData receiveData()
{
    static InputData data = {0, 0, 0, true};

    data.flags &= ~((1 << 7) | (1 << 6));

    // FPGA has priority: drain the whole UART queue each frame.
    while (SerialFPGA.available())
    {
        char c = SerialFPGA.read();
        applyInputChar(c, data);
    }

    // USB serial fallback for bench debug without FPGA.
    while (Serial.available())
    {
        char c = Serial.read();
        applyInputChar(c, data);
    }

    data.correct = true;
    return data;
}

void setup() {
    Wire.begin(21, 22);
    Serial.begin(115200);
    SerialFPGA.begin(115200, SERIAL_8N1, 32, 33);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    Screen::fillScreen(screen_start);
    render();
    Game::setup();
    delay(1000);

    Serial.println("ArDOOMino raycaster ready (Serial2 RX=GPIO32, 115200)");
}

bool over = false;

void loop() {
    static float delta = 0.0f;
    auto t = getTime();

    InputData input = receiveData();

    if(input.rightClick() || input.leftClick()) {
        if(over)
            Game::reset(&input);
        over = false;
    }

    if(!over) over = Game::loop(delta, input);

    render();

    delta = float(getTime() - t) / 1000000.0f;
}
