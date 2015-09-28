# DroneCommander
A project designed to simulate a drone command station, along with the logic of a drone itself.  Written entirely in C

1. DCAS is installed and operates at the drone control center (distributed all over the drone space) and monitors and controls the operation of the drones landing and taking of at these centers and if needed at their destinations.

2. Once the drone leaves the area /vicinity of a control center and reaches a cruising altitude the monitoring the handed-over to an intermediate DCAS station. So DCAS system is indeed a truly distributed realtime monitoring system.(This handover from one DCAS center to another is similar how you cell phone is handed over from one cell tower to another.)

3. You need to worry about only one DCAS center. It is possible that the model of operation for the DCAS is that it totally controls its assigned fleet. The various activities taken by a drone include:
a.takeoff b.cruise c.deliver d.return e.avoid collision (navigate obstacles).

4. Regular flights have runways for takeoff and landing. We are assuming vertical takeoff and landing for the drones. However only one drone can take off from a center/station at any time. Same with landing. Here is the basic interpretation: a.A drone is assigned the task b.The drone picks up the payload c.The drone requests clearance to take off d.Once clearance is given the drone takes off.

5. DCAS software also keeps track of a.number of drones, b.whether they are activeand c.their state.

6. When a drone requests landing, if a landing space is available it is allocated. If not, the drone keeps hovering around in a holding pattern. All the drones requesting landing are queued up and are served first come first serve basis.

7. Collision detection instrumentation (sensors, alarms etc.) will detect imminent mid-collision, run-way collision, and any collision within the DCAS station when the dronesare moving around. Appropriate action to be taken when collision detection happens.

8. Life cycle of a drone: a.DCAS registers an active drone, it is at gate, gets clearance to take-off gets clearance for takeoff, takes-off, reaches cruising altitude, delivers payload and returns. Exceptional situations are collision detection and the corrective operations that may happen at various points.

9. Drones operate concurrently. So you need simulate then using concurrent control structures such as threads (pthreads,for example).

10. Assume that the drones move in a x-y coordinate system that has it (0,0) at the top left corner of your drone zone of a DCAS station.

11. Assume that path planned for a drone comprises of movements along the x-axis and y-axis, not diagonal. That is, it is a square grid with 4 directional movements. Use Manhattan distance heuristics.

12. Assume that drones may change speed (eg. Move in 2-cell distance per move instead of 1 cell) at anytime.

13. Assume other conditions and make sure you document your assumptions.

14. Design this realtime system, implement it and test it with a system handling 10 drones and 1 station
