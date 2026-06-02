# ArDOOMino

A 3D Raycasting game engine built for the **ESP32**, rendering to a **240x240 ST7789 IPS Display** (ZJY-IPS130-V2.0) via SPI. 

To bypass the ESP32's strict `.dram0` memory limits, the game renders natively at **240x160 pixels** and uses DMA block image transfers (`pushImage`) with letterboxing (black bars on the top and bottom) for high-framerate gameplay.

---

## 🛠️ Hardware Requirements

1.  **ESP32 Development Board** (e.g., NodeMCU ESP32-WROOM / ESP32-D0WD)
2.  **ST7789 240x240 TFT IPS Display** (Specific model tested: `ZJY-IPS130-V2.0`)
3.  Micro-USB cable for flashing and serial communication.
4.  Jumper wires.

### 📌 Wiring Configuration

This project is configured right out of the box to run on the standard ESP32 VSPI pins. Since many 240x240 ST7789 displays do **not** have a `CS` (Chip Select) pin, the configuration ignores it.

| Display Pin (ST7789) | ESP32 GPIO Pin | Description |
| :--- | :--- | :--- |
| **VCC / VDD** | **3.3V** | Power Supply (Use 5V *only* if the board has a regulator) |
| **GND**| **GND** | Ground |
| **SCL / SCLK** | **GPIO 18** | SPI Clock |
| **SDA / MOSI** | **GPIO 23** | SPI Data (Master Out Slave In) |
| **RES / RST** | **GPIO 16** | Reset |
| **DC / RS** | **GPIO 17** | Data / Command |
| **BLK**| **3.3V** | Backlight Control (MUST be connected for the screen to light up and not remain black!) |
| *CS / SS (If present)*| *Not needed* | *Not defined in hardware. Leave disconnected or tie to GND.* |

*Note on I2C:* The ESP32 `Wire` instance is initialized on pins `21` (SDA) and `22` (SCL) to support an external controller/I2C slave, but it is currently optional.

---

## 💻 Software Prerequisites

To build and upload the project, you need the following IDE and extensions:

1.  [Visual Studio Code](https://code.visualstudio.com/)
2.  [PlatformIO IDE Extension](https://platformio.org/install/ide?install=vscode) for VS Code.

*Dependencies:* The project relies on the excellent `TFT_eSPI` library by Bodmer. You do **not** need to modify the library's `User_Setup.h` file! All screen configurations, offsets, and pin definitions are injected directly through the `build_flags` in `platformio.ini`.

---

## 🚀 How to Build and Run

1.  **Clone / Open the Project:** Open the `raycaster` folder in VS Code. PlatformIO should automatically detect the `platformio.ini` file and configure the workspace workspace.
2.  **Install Dependencies:** PlatformIO will automatically fetch the necessary frameworks and the `TFT_eSPI` library on the first build.
3.  **Connect the Board:** Plug the ESP32 into your computer via USB.
4.  **Upload:** Click the **PlatformIO: Upload** button (the rightward arrow `→` in the bottom blue toolbar) or run the following command in the terminal:
    ```bash
    platformio run --target upload
    ```
5.  **Monitor:** Once uploaded, the screen will start drawing. Open the **Serial Monitor** (plug symbol in the bottom toolbar) to view the framerate and control the game. Ensure the baud rate is set to `115200`.

---

## 🎮 Game Controls (Serial Monitor)

The game is currently configured to accept inputs via the Serial Monitor. Open your Serial Monitor at `115200` baud and send the following characters (case-insensitive) to control the player:

| Key | Action |
| :---: | :--- |
| **W** | Move Forward |
| **S** | Move Backward |
| **A** | Strafe Left |
| **D** | Strafe Right |
| **Q** | Turn Left |
| **E** | Turn Right |
| **F** | Shoot / Attack |
| **R** | Action (Open Doors / Use) |
| **[Space]** | Stop moving/turning |

*(Because it's reading serial events, sending `W` makes the player walk forward continuously until you send `[Space]` to easily maintain movement without spamming the serial line).*

---

## ⚙️ Technical Notes

*   **Display Colors Inversion:** Because DMA block transfers (`pushImage`) send data in a big-endian format and ESP32 memory is little-endian, RGB channels get swapped natively. The function `tft.setSwapBytes(true)` is called before rendering to correct this.
*   **SPI Speed:** The frame buffer requires heavy data transfer. The `SPI_FREQUENCY` is pushed securely to `40000000` (40 MHz) in `platformio.ini` to guarantee a smooth framerate.
*   **Rendering Optimization:** Rather than iterating through every single pixel using `tft.pushColor()`, the engine now uses `tft.pushImage()` sending a pointer of the entire 16-bit color array `Screen::_screen` to the TFT. 
*   **Letterboxing:** The top and bottom 40 pixels are painted black once in the `setup()` function. The loop only repaints the center 240x160 block.