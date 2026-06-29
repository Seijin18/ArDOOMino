# ArDOOMino

A 3D Raycasting game engine built for the **ESP32**, rendering to a **128x160 ST7735 TFT Display** in landscape orientation, via SPI.

To bypass the ESP32's strict `.dram0` memory limits, the game renders natively at **240x160 pixels** and uses DMA block image transfers (`pushImage`), downscaled row-by-row to the panel's 158x107 landscape window for high-framerate gameplay.

---

## 🛠️ Hardware Requirements

1.  **ESP32 Development Board** (e.g., NodeMCU ESP32-WROOM / ESP32-D0WD)
2.  **ST7735 128x160 TFT Display** (BLACKTAB variant), used in landscape orientation
3.  Micro-USB cable for flashing and serial communication.
4.  Jumper wires.

### 📌 Wiring Configuration

This project is configured right out of the box to run on the standard ESP32 VSPI pins.

| Display Pin (ST7735) | ESP32 GPIO Pin | Description |
| :--- | :--- | :--- |
| **VCC / VDD** | **3.3V** | Power Supply (Use 5V *only* if the board has a regulator) |
| **GND**| **GND** | Ground |
| **SCL / SCLK** | **GPIO 18** | SPI Clock |
| **SDA / MOSI** | **GPIO 23** | SPI Data (Master Out Slave In) |
| **RES / RST** | **GPIO 16** | Reset |
| **DC / RS** | **GPIO 17** | Data / Command |
| **CS / SS** | **GPIO 5** | Chip Select |
| **BLK**| **3.3V** | Backlight Control (MUST be connected for the screen to light up and not remain black!) |

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
*   **SPI Speed:** The frame buffer requires heavy data transfer. The `SPI_FREQUENCY` is set to `27000000` (27 MHz) in `platformio.ini` to guarantee a smooth framerate.
*   **Rendering Optimization:** Rather than iterating through every single pixel using `tft.pushColor()`, the engine pushes each scanline with `tft.pushPixels()` after downscaling it from the 240x160 source buffer to the panel's landscape window.
*   **Landscape scaling:** `setRotation(1)` puts the 128x160 panel into a 160x128 landscape window. The game keeps its 3:2 aspect ratio by rendering into a 158x107 area (small offsets compensate for GRAM column glitches on some ST7735 modules), letterboxed within the panel.