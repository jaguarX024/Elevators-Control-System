# ElevatorsProject
This project implements a basic centralized elevator control system in C++.

**Classes:**

* **CentCom**: This class acts as the central command center for a building's elevators. It manages a list of Elevator objects, allowing for the addition of new elevators and providing methods to interact with them (setting secure floors, clearing emergencies).
* **Elevator**: This class represents an individual elevator within the system. It manages the elevator's state (current floor, movement direction, door status, load), handles floor requests, and processes the next request in its queue.

**Relationship:**

The CentCom class contains and manages multiple instances of the Elevator class. CentCom allows control and monitor the collective behavior of the elevators in the building. Individual Elevator objects handle their own movement and requests based on commands and data managed by the CentCom.

**Key Features:**

* Manages multiple elevators within a building (CentCom).
* Represents individual elevators with their own state and request queues (Elevator).
* Handles elevator requests for different floors.
* Supports basic elevator states (idle, moving up/down, doors open/closed).
* Includes functionality for setting secure floors and handling emergency situations.

This repository provides the core logic for simulating and controlling elevators.
