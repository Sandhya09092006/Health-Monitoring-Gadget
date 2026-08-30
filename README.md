# Health-Monitoring-Gadget
ESP32-based health monitoring wearable gadget for elderly people with temperature,heart rate,fall detection,battery monitoring and emergency alert
## 🔧 Simulation Note

This project was developed and tested as a virtual prototype using Wokwi.

For demonstration purposes in the simulation:

- **Push Button (Heart Rate)** – Used to simulate heart-rate input and observe changes in the monitoring system.
- **Push Button (SOS)** – Used as an SOS emergency button to trigger emergency alerts through Telegram.
- **Potentiometer** – Used to simulate battery-level variation.
- **DS18B20** – Used for body-temperature monitoring.
- **MPU6050**(accelerometer+gyroscope) – Used for fall-detection simulation.
- **Buzzer** – Used to provide a local audible alert during emergency or warning conditions.
  

The push button and potentiometer used for simulation were included as convenient inputs because the virtual environment does not fully replicate the intended real-world sensing setup.

## 🚀 Future Improvements

In a physical implementation, the simulated inputs can be replaced with appropriate real-world sensors and a dedicated battery-monitoring circuit.

## 👥 Team / Contributors

### [SANDHYA.A]
- Co-developed the project idea
- Circuit design and Wokwi implementation
- ThingSpeak integration
- Telegram bot integration
- Testing and project documentation

### [VASUKI.B]
- Co-developed the project idea
- Project planning and discussion
- Assisted with project development 
