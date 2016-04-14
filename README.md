# OSCtoCV Calibrator

---
![OSCtoCV Calibrator](https://github.com/gtbts/OSCtoCV-Calibrator/blob/master/screenshot.png)

## Overview

---
+ The main purpose of this is to store calibration data for **[mbed OSC to CV Converter for Modular Synthesizer](https://developer.mbed.org/users/casiotone401/code/OSCtoCVConverter/)** and **[MIDItoOSC plug-in](https://github.com/gtbts/MIDItoOSC)** using the relation between frequency detection and **[OSC (Open Sound Control)](http://opensoundcontrol.org/introduction-osc)** message.
+ With this, **[OSCtoCV Converter ](https://developer.mbed.org/users/casiotone401/code/OSCtoCVConverter/)** can generate CV (control voltage) with an accurate pitch tracking by using the collected calibration data.

## How it works.

---
The two text boxes in the middle of the plug-in set target IP address & port, you can input target address, port number and press the return key to connect on a UDP socket.
Likewise, text box under the target IP address set target OSC address pattern (By default, **/fader1**),  you can set a custom address.w

By default, value type sent to target is **float** __0.0f ~ 1.0f__, if the **Int** button on, you can send **int** value and set maximum **int** value using the text box.

### Calibration

---
1. Conncet **[OSCtoCV Converter ](https://developer.mbed.org/users/casiotone401/code/OSCtoCVConverter/)** CV out to the VCO's CV input and VCO's audio out to your audio interface audio input (ch.1).

+ The frequency counter in the middle of the plug-in indicate current VCO pitch and adjust frequency roughly 8Hz (MIDI C-0 8.1757989156Hz) by using VCO's tune knob.

+ Click the **Start Calibration** button to start calibration, when the start calibration, status text box indicates **MIDI :: (currently stored MIDI note number)   OSC :: (current OSC value)** and when finished, indicate **Calibration complete**.

+ When calibration successfully completed, you can check the calibration state by using a MIDI keyboard.

### Save Calibration Data

---
The **Save Calibration Data** button is save a **_calibration data_** to text file. The calibration data text file consists of **float** or **int** value, one per line and the line number corresponds to the MIDI note numbers.

### Requirements

---
+ **[The JUCE cross-platform C++ framework](https://www.juce.com/)**.

### License

---
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.
