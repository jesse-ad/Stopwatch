# STM32 Stopwatch

A simple digital stopwatch developed using an STM32 microcontroller. The project demonstrates embedded programming concepts including timer configuration, GPIO input, and real-time control.

## Features

* Start and stop the stopwatch
* Reset the stopwatch
* Displays elapsed time
* Uses an STM32 hardware timer for timekeeping
* Button-based user controls

## Hardware

* STM32 microcontroller
* Display
* Push buttons
* STM32 development board
* USB connection for programming and debugging

## Controls

| Control    | Function                       |
| ---------- | ------------------------------ |
| Start/Stop | Starts or pauses the stopwatch |
| Reset      | Resets the elapsed time        |

## Build & Run

1. Clone the repository:

   ```bash
   git clone <repository-url>
   ```
2. Open the project in **STM32CubeIDE**.
3. Build the project using **Project → Build Project**.
4. Connect the STM32 development board via USB/ST-Link.
5. Flash the program to the microcontroller.
6. Use the buttons to operate the stopwatch.

## Technologies

* C
* STM32
* STM32CubeIDE
* STM32 HAL
* Hardware timers
* GPIO
