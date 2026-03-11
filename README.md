# 4-DOF Robotic Arm Control

A web-based interface for controlling a 4 Degree of Freedom (DOF) Robotic Arm. This project uses a Python backend (Flask) to handle a web dashboard, which acts as the control interface, and communicates with an Arduino over a serial connection to actuate the arm's servos.

## Project Structure

```text
|-- app.py                # Flask application handling web requests and Serial communication.
|-- templates/            # Contains the HTML user interface
|   \-- index.html        # Main web dashboard for the robotic arm
|-- arduino/              # Arduino sketches for the microcontroller
|   |-- Stage1_ServoTest/      # Script to independently test servo connections
|   \-- Stage2_SerialControl/  # Main script to parse serial commands from Python/Web
\-- README.md             # Project documentation
```

## Features
- **Web Interface:** Easy-to-use HTML control panel to manipulate the 4 DOFs.
- **Serial Communication Backend:** Python/Flask application bridging the web inputs to the hardware via PySerial.
- **Threaded Serial Listening:** Concurrent background reading from the Arduino to avoid locking the web UI.

## Getting Started

### 1. Arduino Setup
1. Open the [Arduino IDE](https://www.arduino.cc/en/software).
2. To test your servo wiring, upload `arduino/Stage1_ServoTest/Stage1_ServoTest.ino` to your Arduino.
3. Once verified, open `arduino/Stage2_SerialControl/Stage2_SerialControl.ino` and upload it. Keep your Arduino plugged via USB.

### 2. Python Environment Setup
1. Ensure you have Python installed.
2. Create and activate a virtual environment:
   ```bash
   python -m venv .venv
   # Windows:
   .venv\Scripts\activate
   # macOS/Linux:
   source .venv/bin/activate
   ```
3. Install the required dependencies:
   ```bash
   pip install Flask pyserial
   ```

### 3. Run the Application
1. Make sure your Arduino is connected to your computer.
2. Run the Flask application:
   ```bash
   python app.py
   ```
3. Open a web browser and go to `http://127.0.0.1:5000`. You will see the control interface where you can connect to the corresponding serial COM port automatically listed.

## License
MIT License
