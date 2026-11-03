# Development Journal

This file will document the progress as I work on this project, anything before November is retrospective, as I did not plan to journal until that point. These aren't very serious, as they are just my thoughts as they came to me, but they give a good example of the design of this project.

## Development Summary

### 1. Initial Concept & Planning
I initially had this idea while brainstorming what I wanted my senior capstone project to be, and I knew I wanted it to be big. One measurable goal that I had was to get l1 certified with either NAR or Tripoli. Another constraint that I had with this was that I wanted it to look big, even if it sacrificed some altitude. I'm aiming for it to be exactly 6 feet tall, so when I am standing next to it, it will be just taller than me. I've done a very small avionics project in a smaller rocket I made (E motor), so I knew I wanted it to have some extensive sensors. I also have been looking a lot at radio communication, so I knew I wanted it to transmit that data live down to a ground station during flight. But with all data coming down, there needs to be a way to visualize it, so the last major constraint with this project is I want to have a cool, but also reliable telemetry display that is custom made.

### 2. CAD Design
I started with a very similar approach to other 3d-printed rockets I have made, making a parametric, but simple model to be able to get a feel for what it would look like. Once I had figured out the basic constraints I started to do some research on if my constraints are realistic for flight. Once I had the idea verified, I started in Onshape, and made a replica of how I would assemble it, and a rough estimate of what I think the avionics bay will look like. The only 3d-printed part in this rocket is going to be is parts of the avionics bay, which I'm hoping will be secure, but also able to easily slide out on the field.

### 3. Simulation
After having the CAD designed I moved to importing those dimensions into a simulation software, and I used OpenRocket. I had initially designed the fins off what looked best, but I knew that that wouldn't be the final design. Because I am aiming for a F-117 inspired design, the one constraint I put in for the fins was the 67.5 degree sweep angle it has. I then optimized the fins, optimizing for a stability caliber between 1 and 2, and maximizing apogee. The final preliminary simulation shows an apogee of 1419 m, which is well above what I expected, but of course it may change.

### 4. Electronics & Software Demo
Initially I planned to use AI to make the whole demo, and use CRSF to transmit the telemetry over my radio link, but after trying for hours it was never able to figure it out. After that, I decided I needed to make the demo myself. Instead of using CRSF, which was very complicated, and overkill for what I needed it for, I looked for a protocol that could handle IMU data at a relatively high HZ. MavLink was promising, and I think thats what I will use for the final telemetry system, but it was also very complicated. Finally I found Airport, which makes ELRS transmitters and receivers act as a serial wire, which would be easy to implement, and customizable for what I need it. I initially had a constraint of only sending 5 byte packets (not sure where I got this constraint from), so I decided to send 3 bytes of gyroscopic data, 1 for each axis, 1 accelerometer axis, and one starting byte that would always be 255. The receiving side would look for 255, and read the next 4 bytes to get the data. This initially worked, but there kept being random spikes in the data, which ended up being traced to ELRS injecting link statistics in between packets. V1 of this software worked, but the low resolution and sloppy code introduced many points where the model could drift, and I initially did not want to use quaternions as they seemed to complex in the moment. This resulted in the model being almost unable to go upside down. V2 introduced many improvements, first of which was changing the gyro data from 1 byte per axis to 2. It also added 2 start bytes, a 4 byte timestamp, a packet counter, and CRC, which helped drastically to reduce the random spikes in data. The code was also much more organized, which made it much easier to add debugging, and fix bugs as they came.

## Development Log

Future updates will be documented here as development continues.

### November 2026
...
