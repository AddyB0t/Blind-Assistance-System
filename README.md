# Blind Assistance System

A smart, wearable device designed to help visually impaired individuals navigate their surroundings safely and independently through real-time obstacle detection and audio feedback.

![Circuit Diagram](https://github.com/AddyB0t/Blind-Assistance-System/blob/main/circuit%20diagram.png)
![Demonstration](https://github.com/AddyB0t/Blind-Assistance-System/blob/main/demonstration.png)

## Features

- 360° obstacle detection using multiple ultrasonic sensors
- Distance-based audio feedback system with directional awareness
- Battery-powered with low power consumption for all-day use
- Compact, lightweight, and weather-resistant design
- Emergency alert system with SOS button
- Customizable distance thresholds and audio patterns

## Hardware Requirements

- Arduino Nano (recommended for smaller form factor)
- HC-SR04 Ultrasonic Sensors (x3)
- Piezo Buzzer (active)
- Li-Po Battery (3.7V, 2000mAh recommended)
- TP4056 Charging Module
- Power Switch
- Momentary Push Button (for emergency)
- 3D Printed Case
- Elastic Band (for wearing)

## Software Requirements

- Arduino IDE (1.8.x or newer)
- Required Libraries:
  - [NewPing](https://github.com/microflo/NewPing) - For more reliable ultrasonic readings
  - [Tone](https://github.com/bhagman/Tone) - For better tone generation

## Installation

1. Clone this repository
2. Open `code.ino` in Arduino IDE
3. Install required libraries through Arduino Library Manager
   - Sketch > Include Library > Manage Libraries...
   - Search for "NewPing" and "Tone" and install them
4. Connect the hardware components as per the wiring diagram
5. Upload the code to your Arduino board

## Wiring Diagram

```
Ultrasonic Sensor 1 (Left):
- VCC -> 5V
- GND -> GND
- TRIG -> D2
- ECHO -> D3

Ultrasonic Sensor 2 (Center):
- VCC -> 5V
- GND -> GND
- TRIG -> D4
- ECHO -> D5

Ultrasonic Sensor 3 (Right):
- VCC -> 5V
- GND -> GND
- TRIG -> D6
- ECHO -> D7

Buzzer:
- Positive -> D8
- Negative -> GND

Emergency Button:
- One terminal -> D9
- Other terminal -> GND (with 10k pull-up resistor to 5V)

Battery Circuit:
- Li-Po -> TP4056 -> Power Switch -> Vin (Arduino)
```

## Usage

1. Charge the device fully before first use
2. Wear the device on your chest or waist using the elastic band
3. Turn on the power switch
4. The system will:
   - Continuously scan for obstacles in left, center, and right directions
   - Provide audio feedback with different patterns based on distance and direction:
     - Higher frequency tones for distant objects
     - Lower frequency tones for closer objects
     - Different patterns of beeps indicate direction (1 beep for left, 2 for center, 3 for right)
5. Press the emergency button for 3 seconds to activate SOS signal

## Customization

You can customize the device by modifying these parameters in the code:
- Distance thresholds (`VERY_CLOSE_DISTANCE`, `CLOSE_DISTANCE`, `MEDIUM_DISTANCE`)
- Tone frequencies (`FAR_TONE`, `MEDIUM_TONE`, `CLOSE_TONE`, `VERY_CLOSE_TONE`)
- Scanning frequency (modify delay values)

## Troubleshooting

Common issues and solutions:
- Inconsistent readings: Check sensor connections and orientation
- Short battery life: Use a higher capacity battery or adjust scanning frequency
- No audio feedback: Check buzzer connections and pins
- False readings: Adjust MAX_DISTANCE value or sensor positioning

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the project
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Inspired by similar projects at [Hackster.io](https://www.hackster.io/)
- Special thanks to contributors and testers
