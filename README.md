# UDP Ground Station and Vehicle Simulation
This is a work-in-progress project that simulates udp link communication between a simulated vehicle and ground station. The vehicle process repeatedly publishes its state to the ground station on a fixed cycle, and the ground station can publish command and control messages to the vehicle. Simulation and UDP link communication run in separate threads.

### Build 
```
g++ -std=c++17 -o vehicle_sim apps/vehicle_sim.cpp net/udp_link.cpp proto/protocol.cpp -lpthread
g++ -std=c++17 -o ground_station apps/ground_station.cpp net/udp_link.cpp proto/protocol.cpp -lpthread
```
### Usage
Start the ground station

``./ground_station``

Start the vehicle simulation

``./vehicle_sim``

Observe output
  - The ground station will print messages received from the vehicle.
  - The vehicle simulator will print its own state and send it over UDP.

### Protocol
Messages are formatted as:

``SEQ=<sequence> X=<xpos> Y=<ypos> SPD=<speed> HDG=<heading> BAT=<battery>``

