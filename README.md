
<div align="center">

# N I G H T H A W K

**A Student Designed High-Power Rocket**

*Mechanics · Avionics · Software*

![Nighthawk Concept](media/images/nighthawk-banner.png)

<sub>Concept visualization of Nighthawk</sub>

</div>

---

## The Project

Nighthawk is my independently developed Level 1 high-power model rocket, combining mechanical design, electronics, and software. My main technical goal is to earn my Level 1 rocketry certification, but I also want to create something I'm genuinely proud of: a rocket that's thoughtfully designed, feature-rich, and polished from the electronics to the finished appearance.

I'm Landon Maynard, a high school student from Idaho, and I’ve always enjoyed building things. After designing and building a smaller custom 3D-printed rocket for a school project earlier this year, I wanted to take on something more ambitious. Nighthawk is my project to explore different fields of engineering and learn through designing, programming, and building something from the ground up.

## Design & Simulation

Nighthawk takes its name and visual inspiration from the Lockheed F-117 Nighthawk. I wanted to capture the aircraft's angular geometry and sleek black appearance while developing a functional high-power rocket.

I've completed the initial assembly in Onshape and preliminary flight simulations in OpenRocket. The design is still being refined as I work through structural requirements, component placement, and the final bill of materials.


<p align="center">
  <img src="media/images/nighthawk-cad.png" alt="Nighthawk CAD" width="100%">
  <br>
  <sub><i>Nighthawk 3D assembly modeled in Onshape.</i></sub>
</p>

<p align="center">
  <img src="media/images/simulation.png" alt="Nighthawk Simulation" width="100%">
  <br>
  <sub><i>Nighthawk flight simulation in OpenRocket.</i></sub>
</p>


## Electronics & Software

I'm developing custom electronics and software for Nighthawk. My first major working prototype is a wireless IMU telemetry demonstration using a **Teensy 4.1, 6-DOF IMU, and ExpressLRS radio link**.

The Teensy collects accelerometer and gyroscope measurements, encodes them into a custom packet format with CRC error detection, and transmits the data over ELRS Airport mode. A Python application on my laptop decodes the packets, graphs the measurements, and updates a 3D orientation visualization in real time.

I wrote the packet encoding, decoding, and visualization logic myself, using existing libraries for supporting functionality.

https://github.com/user-attachments/assets/1c7845f0-3e1f-4eb7-aabd-1b40c6df530c

<div align="center">
  <sub>Wireless IMU Telemetry — Version 2</sub>
</div>

| Component | Implementation |
|---|---|
| Microcontroller | Teensy 4.1 |
| Sensor | 6-DOF IMU |
| Communication | ExpressLRS Airport |
| Data Format | Custom packets with CRC |
| Visualization | Python, live graphs, and 3D viewer |

One of the biggest challenges was tracking down unexpected bytes corrupting the telemetry stream. After investigating the problem, I modified the Ranger Micro firmware to prevent link-statistics data from interfering with my packets.

[Read the development journal →](docs/DEVELOPMENT.md)

## Progress & Roadmap

| Completed | Up Next |
|---|---|
| Initial CAD assembly | Finalize engineering constraints |
| Preliminary OpenRocket simulations | Finalize the BOM |
| Initial bill of materials | Secure funding and materials |
| Wireless IMU demo V2 | Develop onboard software |

**Target:** Secure funding by the end of 2026, with project completion and a potential certification launch in March–April 2027, subject to development progress and launch readiness.

[View the full project roadmap →](docs/ROADMAP.md)

## Funding & Support

**Funding Goal: $800**

Nighthawk is an independent project that I'm responsible for funding myself. I'm currently balancing my high school classes and saving for an upcoming two-year volunteer mission, so I'm seeking grants and sponsorships to help cover the cost of materials and equipment.

I've completed the initial design, simulations, and electronics demonstration. Funding will allow me to begin physical construction and continue developing the onboard systems.

[View the detailed budget and bill of materials →](docs/BUDGET.md)

For sponsorship inquiries or questions, contact me at **[landonmaynard13@gmail.com](mailto:landonmaynard13@gmail.com)**.

## Looking Ahead

I plan to keep designing and building projects long after Nighthawk. I want to pursue engineering professionally, and I'm particularly interested in multidisciplinary fields like robotics, where mechanical design, electronics, and programming come together. I'm still exploring which specialty I want to pursue, and projects like Nighthawk are helping me figure that out.

