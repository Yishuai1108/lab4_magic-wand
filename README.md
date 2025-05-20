# TECHIN515 Lab4 - Gesture Data Capture

This directory contains tools for capturing accelerometer data from an ESP32 with MPU6050 for gesture recognition.

## Contents

- `gesture_capture.ino`: Arduino sketch for the ESP32 that captures accelerometer data
- `process_gesture_data.py`: Python script to automatically save gesture data to CSV files
- `requirements.txt`: Python dependencies

## Project Structure

### Hardware Components
- ESP32C3 XIAO
- MPU6050 (connected via I2C)
- RGB LED (connected to GPIO2, 3, 4)
- Push button (connected to GPIO5 and GND)
- Battery (for standalone operation)
- Enclosure

### Hardware Setup
### 1. Clone the Repository

git clone https://github.com/your-username/TECHIN515-magic-wand.git
cd TECHIN515-magic-wand

1. Connect the MPU6050 to your ESP32:
   - VCC → 3.3V
   - GND → GND
   - SCL → GPIO22 (or your I2C clock pin)
   - SDA → GPIO21 (or your I2C data pin)

#### **Note**: The PIN number may vary depending on your developmen board.

### 2. Install Python Environment (for Data Collection)
- cd
- python -m venv .venv


### Windows
- .venv\Scripts\activate
### Mac/Linux
- source .venv/bin/activate

- pip install -r requirements.txt

### 3. Upload Data Capture Sketch
- Open the project folder in VSCode.

- Open gesture_capture sketch (src/sketches/gesture_capture/).

- Connect your ESP32C3 XIAO via USB.

- Click the "Upload" arrow in PlatformIO, upload `gesture_capture.ino` (transfer to PlatformIO version)

- Open the Serial Monitor or run the Python script:
   - cd src/python-scripts
   - python process_gesture_data.py --gesture "Z" --person "your_name"

### 4. Train Your Model on Edge Impulse
- Go to Edge Impulse

- Create a new project

- Upload your collected data (in dataset/Z, O, V)

- Design the impulse (DSP block + NN)

- Add processing block

- Add learning block

- Process features

- Train and test model

- Go to "Deployment" → Select Arduino Library + Quantized (int8) → Build

- Extract .zip, copy model-parameters and tflite-model headers into src/sketches/wand_inference/

### 5. Upload Inference Sketch
- Modify wand.ino to include model header file (Line 18)

- Upload .zip file to project libraries (directly copy to Platform IO folder)

- Upload the sketch using PlatformIO

- Open Serial Monitor or press the button to trigger inference



## Button-triggered Inference
- Connect button to GPIO3 and GND

- Gesture is classified using the onboard Edge Impulse model

- LED lights up based on prediction:

   - Green = "Z" (Fire Bolt)

   - Blue = "O" (Reflect Shield)

   - Red = "V" (Healing Spell)

## Python Setup

1. Install Python 3.6 or newer
2. Install the required Python package:
   
   ```
   pip install -r requirements.txt
   ```

## Usage

### Data Collection

1. Connect your ESP32 to your computer
2. Run the Python script:
   ```bash
   pytho3 process_gesture_data.py --gesture "W" --person "your_name"
   ```
   
3. The script will:
   - Auto-detect your ESP32's serial port
   - Create a directory structure for your data
   - Wait for you to press 'o' to start a capture
   - Automatically save each gesture capture to a CSV file

4. To capture a gesture:
   - Type 'o' and press Enter to start recording
   - Perform your gesture (recording will stop automatically after 1 second)
   - Repeat for multiple samples

5. Press Ctrl+C to exit the script when done

### Command Line Options

```bash
python3 process_gesture_data.py [options]

Options:
  --port PORT       Serial port to use (default: auto-detect)
  --baud BAUD       Baud rate (default: 115200)
  --gesture NAME    Gesture name (default: "gesture")
  --person NAME     Person name (default: "user")
  --output DIR      Output directory (default: "data")
```

## Data Format

The CSV files contain four columns:

- Timestamp
- x: X-axis acceleration (in m/s²)
- y: Y-axis acceleration (in m/s²)
- z: Z-axis acceleration (in m/s²)

| Gesture | Spell           | Label |
|---------|------------------|-------|
| Z       | Fire Bolt        | Z (LED Green)    |
| O       | Reflect Shield   | O (LED Blue)     |
| V       | Healing Spell    | V (LED Red)    |

Each file contains approximately 100 samples (at 100Hz for 1 second).



## Battery and Enclosure
- Use a 3.7V Li-Po battery with JST connector, design your wand enclosure to secure ESP32C3, MPU6050, LED, and button



## Troubleshooting

- **Port not found**: Specify the port manually with `--port COM3` (Windows) or `--port /dev/ttyUSB0` (Linux/Mac)
- **Permission denied**: On Linux/Mac, you may need to run `sudo chmod 666 /dev/ttyUSB0` (replace with your port)
- **No data received**: Make sure the ESP32 is properly connected and the sketch is uploaded correctly 