#include "centcom.h"

CentCom::CentCom(int numElevators, int buildingID) {
    if (buildingID < 0)
        throw invalid_argument("Building ID cannot be negative"); // Check for negative building ID

    if (numElevators < 0)
        throw invalid_argument("Number of elevators cannot be negative"); // Check for negative number of elevators

    m_id = buildingID;
    m_numElevators = numElevators;
    m_elevatorsList = new Elevator *[m_numElevators]; // Allocate memory for the elevator list
    for (int i = 0; i < m_numElevators; i++) {
        m_elevatorsList[i] = nullptr; // Initialize each elevator pointer to nullptr
    }
}

CentCom::~CentCom() {
    for (int i = 0; i < m_numElevators; i++) {
        if (m_elevatorsList[i] != nullptr) {
            delete m_elevatorsList[i]; // Delete dynamically allocated Elevator object
            m_elevatorsList[i] = nullptr; // Set pointer to nullptr after deleting
        }
    }
    delete[] m_elevatorsList; // Delete the array of elevator pointers
    m_id = 0;
    m_numElevators = 0;
    m_elevatorsList = nullptr;
}

bool CentCom::addElevator(int ID, int bottomFloor, int topFloor) {
    if (ID < 0 || ID >= m_numElevators)
        return false; // Return false if ID is out of bounds
    if (bottomFloor > topFloor)
        return false; // Return false if bottom floor is greater than top floor

    if (m_elevatorsList[ID] != nullptr) {
        delete m_elevatorsList[ID]; // Delete existing elevator if one exists at this ID
        m_elevatorsList[ID] = nullptr;
    }

    Elevator *newElevator = new Elevator(ID); // Create a new Elevator object
    newElevator->setUp(bottomFloor, topFloor); // Initialize the elevator's floors

    m_elevatorsList[ID] = newElevator; // Assign the new elevator to the list
    return true;
}

Elevator *CentCom::getElevator(int ID) {
    if (ID >= m_numElevators || ID < 0 || m_elevatorsList == nullptr)
        return nullptr; // Return nullptr if ID is out of bounds or list is null

    return m_elevatorsList[ID]; // Return the Elevator object at the given ID
}

bool CentCom::setSecure(int ID, int floorNum, bool yes_no) {
    if (ID < 0 || ID >= m_numElevators || m_elevatorsList == nullptr)
        return false; // Return false if ID is out of bounds or list is null

    if (m_elevatorsList[ID] == nullptr)
        return false; // Return false if there is no elevator at this ID

    if (floorNum < m_elevatorsList[ID]->m_bottom->m_floorNum || floorNum > m_elevatorsList[ID]->m_top->m_floorNum)
        return false; // Return false if floor number is outside the elevator's range

    Floor *temp = m_elevatorsList[ID]->m_bottom; // Start from the bottom floor
    while (temp != nullptr) {
        if (temp->m_floorNum == floorNum) {
            temp->m_secured = yes_no; // Set the secured status of the floor
            return true;
        } else
            temp = temp->m_next; // Move to the next floor
    }
    return false; // Return false if the floor number is not found
}

bool CentCom::clearEmergency(int ID) {
    if (ID < 0 || ID >= m_numElevators || m_elevatorsList == nullptr)
        return false; // Return false if ID is out of bounds or list is null

    if (m_elevatorsList[ID] == nullptr)
        return false; // Return false if there is no elevator at this ID

    m_elevatorsList[ID]->m_emergency = false; // Clear the emergency status of the elevator

    return true;
}

Elevator::Elevator(int ID) {
    m_id = ID;
    m_bottom = nullptr;
    m_top = nullptr;
    m_upRequests = nullptr;
    m_downRequests = nullptr;
    m_currentFloor = nullptr;
    m_moveState = IDLE;
    m_doorState = OPEN;
    m_emergency = false;
    m_load = 0;
}

Elevator::~Elevator() {
    Floor *node = m_bottom;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node; // Delete each Floor object in the linked list
        node = newnode;
    }
    node = m_upRequests;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node; // Delete each requested Floor object (up)
        node = newnode;
    }
    node = m_downRequests;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node; // Delete each requested Floor object (down)
        node = newnode;
    }
    m_bottom = nullptr;
    m_top = nullptr;
    m_upRequests = nullptr;
    m_downRequests = nullptr;
    m_currentFloor = nullptr;
    m_moveState = IDLE;
    m_doorState = CLOSED;
    m_emergency = false;
    m_load = 0;
}

void Elevator::clear() {
    Floor *node = m_bottom;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node; // Delete each Floor object
        node = newnode;
    }
    node = m_upRequests;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node; // Delete each up request
        node = newnode;
    }
    node = m_downRequests;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node; // Delete each down request
        node = newnode;
    }
    m_bottom = nullptr;
    m_top = nullptr;
    m_upRequests = nullptr;
    m_downRequests = nullptr;
    m_currentFloor = nullptr;
    m_moveState = IDLE;
    m_doorState = OPEN;
    m_emergency = false;
    m_load = 0;
}

void Elevator::setUp(int firstFloor, int lastFloor) {
    if (m_bottom != nullptr) {
        return; // Do nothing if floors are already set up
    }
    if (firstFloor > lastFloor) {
        cout << "First floor number is greater than last floor number." << endl;
        return;
    }

    m_bottom = new Floor(firstFloor); // Create the first floor
    m_top = m_bottom;
    Floor *current = m_bottom;

    for (int i = firstFloor + 1; i <= lastFloor; i++) {
        Floor *newFloor = new Floor(i); // Create a new floor
        current->m_next = newFloor; // Link the current floor to the new floor
        newFloor->m_previous = current; // Link the new floor to the previous floor
        current = newFloor; // Move to the new floor
    }
    m_top = current; // Set the top floor
    m_currentFloor = m_bottom; // Set the initial current floor to the bottom
}

bool Elevator::insertFloor(int floor) {
    if (m_bottom == nullptr) {
        Floor *newFloor = new Floor(floor);
        m_bottom = newFloor;
        m_top = newFloor;
        m_currentFloor = newFloor;
    } else {
        if (floor >= m_bottom->m_floorNum) {
            return false; // Cannot insert a floor above or equal to the bottom floor
        }

        Floor *newFloor = new Floor(floor);
        m_bottom->m_previous = newFloor; // Link the new floor to the current bottom
        newFloor->m_next = m_bottom; // Link the new floor to the next (old bottom)
        m_bottom = newFloor; // Update the bottom floor
        m_currentFloor = newFloor; // Update the current floor to the new bottom

        return true;
    }
    return true;
}

bool Elevator::checkSecure(int floor) {
    if (m_bottom == nullptr || floor < m_bottom->m_floorNum || floor > m_top->m_floorNum)
        return false; // Return false if floors are not set or floor is out of range

    Floor *temp = m_bottom;
    while (temp != nullptr) {
        if (temp->m_floorNum == floor)
            return temp->m_secured; // Return the secured status of the floor

        temp = temp->m_next; // Move to the next floor
    }
    return false; // Return false if the floor is not found
}

void Elevator::pushEmergency(bool pushed) {
    if (pushed)
        m_emergency = true; // Set emergency status to true if pushed is true
}

bool Elevator::pushButton(int floor) {
    if (floor < m_bottom->m_floorNum || floor > m_top->m_floorNum) {
        return false; // Return false if the requested floor is out of range
    }

    if (floor == m_currentFloor->m_floorNum)
        return false; // Return false if the requested floor is the current floor

    Floor *temp = m_bottom;
    while (temp != nullptr && temp->m_floorNum != floor) {
        temp = temp->m_next; // Find the requested floor in the building's structure
    }

    if (temp != nullptr && temp->m_secured == true) {
        return false; // Return false if the requested floor is secured
    }

    if (floor < m_currentFloor->m_floorNum) {
        Floor *newRequest = new Floor(floor); // Create a new request for the floor
        newRequest->m_next = m_downRequests; // Add to the beginning of down requests
        if (m_downRequests != nullptr)
            m_downRequests->m_previous = newRequest;

        m_downRequests = newRequest;

        if (m_moveState == IDLE)
            m_moveState = DOWN; // Set move state to DOWN if idle
    } else {
        if (floor > m_currentFloor->m_floorNum) {
            Floor *newRequest = new Floor(floor); // Create a new request for the floor
            newRequest->m_next = m_upRequests; // Add to the beginning of up requests
            if (m_upRequests != nullptr)
                m_upRequests->m_previous = newRequest;

            m_upRequests = newRequest;
            if (m_moveState == IDLE)
                m_moveState = UP; // Set move state to UP if idle
        }
    }
    return true;
}

bool Elevator::processNextRequest() {
    if (m_load > LOADLIMIT)
        return false; // Cannot process request if load limit is exceeded

    if (m_emergency)
        return false; // Cannot process request if emergency button is pressed

    Floor *nextRequest = nullptr;
    if (m_moveState == UP) {
        nextRequest = m_upRequests; // Start with the first up request
        Floor *temp = m_upRequests;
        while (temp != nullptr) {
            if (temp->m_floorNum < nextRequest->m_floorNum)
                nextRequest = temp; // Find the lowest floor number in up requests
            temp = temp->m_next;
        }
    } else if (m_moveState == DOWN) {
        nextRequest = m_downRequests; // Start with the first down request
        Floor *temp = m_downRequests;
        while (temp != nullptr) {
            if (temp->m_floorNum > nextRequest->m_floorNum)
                nextRequest = temp; // Find the highest floor number in down requests
            temp = temp->m_next;
        }
    } else
        return false; // No next request if elevator is idle

    if (nextRequest == nullptr)
        return false; // No pending requests

    m_currentFloor = nextRequest; // Move the elevator to the next requested floor

    if (m_moveState == UP) {
        if (nextRequest->m_previous != nullptr)
            nextRequest->m_previous->m_next = nextRequest->m_next; // Remove from the linked list
        else
            m_upRequests = nextRequest->m_next; // Update head of the list

        if (nextRequest->m_next != nullptr)
            nextRequest->m_next->m_previous = nextRequest->m_previous; // Update previous pointer
    } else if (m_moveState == DOWN) {
        if (nextRequest->m_previous != nullptr)
            nextRequest->m_previous->m_next = nextRequest->m_next; // Remove from the linked list
        else
            m_downRequests = nextRequest->m_next; // Update head of the list

        if (nextRequest->m_next != nullptr)
            nextRequest->m_next->m_previous = nextRequest->m_previous; // Update previous pointer
    }

    m_doorState = OPEN; // Open the door at the requested floor
    return true;
}

void Elevator::enter(int load) {
    m_load += load; // Increase the current load
}

int Elevator::exit(int load) {
    m_load -= load; // Decrease the current load
    if (m_load < 0)
        m_load = 0; // Ensure load doesn't go below zero

    return m_load; // Return the updated load
}

void Elevator::dump() {
    if (m_moveState == IDLE) cout << "Elevator " << m_id << " is idle.";
    else if (m_moveState == UP) cout << "Elevator " << m_id << " is moving up.";
    else if (m_moveState == DOWN) cout << "Elevator " << m_id << " is moving down.";
    cout << endl;
    if (m_emergency == true) cout << "someone pushed the emergency button!" << endl;
    if (m_top != nullptr) {
        Floor *temp = m_top;
        cout << "Top" << endl;
        while (temp->m_previous != nullptr) {
            cout << temp->m_floorNum;
            if (temp->m_floorNum == m_currentFloor->m_floorNum) cout << " current ";
            if (temp->m_secured == true) cout << " secured ";
            cout << endl;
            temp = temp->m_previous;
        }
        cout << temp->m_floorNum;
        if (temp->m_floorNum == m_currentFloor->m_floorNum) cout << " current ";
        if (temp->m_secured == true) cout << " secured ";
        cout << endl;
        cout << "Bottom" << endl;
    }
}