#include "centcom.h" 

// CentCom constructor: Initializes central control for building elevators
CentCom::CentCom(int numElevators, int buildingID) {
    // Input validation
    if (buildingID < 0)
        throw invalid_argument("Building ID cannot be negative");
    if (numElevators < 0)
        throw invalid_argument("Number of elevators cannot be negative");

    m_id = buildingID;
    m_numElevators = numElevators;
    m_elevatorsList = new Elevator *[m_numElevators]; // Allocate array for elevators
    // Initialize elevator pointers to null
    for (int i = 0; i < m_numElevators; i++) {
        m_elevatorsList[i] = nullptr;
    }
}

// CentCom destructor: Cleans up memory
CentCom::~CentCom() {
    // Delete each Elevator object and then the array
    for (int i = 0; i < m_numElevators; i++) {
        if (m_elevatorsList[i] != nullptr) {
            delete m_elevatorsList[i];
            m_elevatorsList[i] = nullptr;
        }
    }
    delete[] m_elevatorsList;
    // Reset member variables
    m_id = 0;
    m_numElevators = 0;
    m_elevatorsList = nullptr;
}

// Adds an elevator to the system
bool CentCom::addElevator(int ID, int bottomFloor, int topFloor) {
    // Validate ID and floor range
    if (ID < 0 || ID >= m_numElevators)
        return false;
    if (bottomFloor > topFloor)
        return false;

    // Delete existing elevator if ID is reused
    if (m_elevatorsList[ID] != nullptr) {
        delete m_elevatorsList[ID];
        m_elevatorsList[ID] = nullptr;
    }

    Elevator *newElevator = new Elevator(ID); // Create new Elevator
    newElevator->setUp(bottomFloor, topFloor); // Set up elevator's floors

    m_elevatorsList[ID] = newElevator; // Assign to list
    return true;
}

// Retrieves an Elevator object by its ID
Elevator *CentCom::getElevator(int ID) {
    // Basic validation
    if (ID >= m_numElevators || ID < 0 || m_elevatorsList == nullptr)
        return nullptr;

    return m_elevatorsList[ID]; // Return elevator
}

// Sets the secure status of a specific floor for an elevator
bool CentCom::setSecure(int ID, int floorNum, bool yes_no) {
    // Basic validation
    if (ID < 0 || ID >= m_numElevators || m_elevatorsList == nullptr)
        return false;
    if (m_elevatorsList[ID] == nullptr)
        return false;

    // Check if floor is within elevator's range
    if (floorNum < m_elevatorsList[ID]->m_bottom->m_floorNum || floorNum > m_elevatorsList[ID]->m_top->m_floorNum)
        return false;

    // Traverse floors to find and set status
    Floor *temp = m_elevatorsList[ID]->m_bottom;
    while (temp != nullptr) {
        if (temp->m_floorNum == floorNum) {
            temp->m_secured = yes_no;
            return true;
        } else
            temp = temp->m_next;
    }
    return false;
}

// Clears the emergency status of an elevator
bool CentCom::clearEmergency(int ID) {
    // Basic validation
    if (ID < 0 || ID >= m_numElevators || m_elevatorsList == nullptr)
        return false;
    if (m_elevatorsList[ID] == nullptr)
        return false;

    m_elevatorsList[ID]->m_emergency = false; // Clear emergency flag

    return true;
}

// Elevator constructor: Initializes an Elevator object
Elevator::Elevator(int ID) {
    m_id = ID;
    // Initialize all pointers to null
    m_bottom = nullptr;
    m_top = nullptr;
    m_upRequests = nullptr;
    m_downRequests = nullptr;
    m_currentFloor = nullptr;
    // Set initial states
    m_moveState = IDLE;
    m_doorState = OPEN;
    m_emergency = false;
    m_load = 0;
}

// Elevator destructor: Deallocates memory for floors and requests
Elevator::~Elevator() {
    // Delete linked list of floors
    Floor *node = m_bottom;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node;
        node = newnode;
    }
    // Delete linked list of up requests
    node = m_upRequests;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node;
        node = newnode;
    }
    // Delete linked list of down requests
    node = m_downRequests;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node;
        node = newnode;
    }
    // Reset all pointers and states
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

// Clears all floor and request linked lists and resets elevator state
void Elevator::clear() {
    // Delete all floor nodes
    Floor *node = m_bottom;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node;
        node = newnode;
    }
    // Delete up request nodes
    node = m_upRequests;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node;
        node = newnode;
    }
    // Delete down request nodes
    node = m_downRequests;
    while (node != nullptr) {
        Floor *newnode = node->m_next;
        delete node;
        node = newnode;
    }
    // Reset pointers and states
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

// Sets up the range of floors for the elevator
void Elevator::setUp(int firstFloor, int lastFloor) {
    if (m_bottom != nullptr) {
        return; // Already set up
    }
    if (firstFloor > lastFloor) {
        cout << "First floor number is greater than last floor number." << endl;
        return;
    }

    m_bottom = new Floor(firstFloor); // Create first floor
    m_top = m_bottom;
    Floor *current = m_bottom;

    // Create remaining floors and link them
    for (int i = firstFloor + 1; i <= lastFloor; i++) {
        Floor *newFloor = new Floor(i);
        current->m_next = newFloor;
        newFloor->m_previous = current;
        current = newFloor;
    }
    m_top = current; // Set top floor
    m_currentFloor = m_bottom; // Set initial current floor
}

// Inserts a new floor below the current bottom floor
bool Elevator::insertFloor(int floor) {
    if (m_bottom == nullptr) {
        // If no floors, set this as the first
        Floor *newFloor = new Floor(floor);
        m_bottom = newFloor;
        m_top = newFloor;
        m_currentFloor = newFloor;
    } else {
        if (floor >= m_bottom->m_floorNum) {
            return false; // Cannot insert if not strictly below bottom
        }

        Floor *newFloor = new Floor(floor);
        m_bottom->m_previous = newFloor;
        newFloor->m_next = m_bottom;
        m_bottom = newFloor; // Update bottom floor
        m_currentFloor = newFloor; // Update current floor
    }
    return true;
}

// Checks if a specific floor is secured
bool Elevator::checkSecure(int floor) {
    if (m_bottom == nullptr || floor < m_bottom->m_floorNum || floor > m_top->m_floorNum)
        return false;

    // Traverse floors to find and return secure status
    Floor *temp = m_bottom;
    while (temp != nullptr) {
        if (temp->m_floorNum == floor)
            return temp->m_secured;
        temp = temp->m_next;
    }
    return false;
}

// Sets the emergency state
void Elevator::pushEmergency(bool pushed) {
    if (pushed)
        m_emergency = true;
}

// Simulates pushing a button for a floor request
bool Elevator::pushButton(int floor) {
    // Validate requested floor
    if (floor < m_bottom->m_floorNum || floor > m_top->m_floorNum) {
        return false;
    }

    // Do nothing if already at target floor
    if (floor == m_currentFloor->m_floorNum)
        return false;

    // Find the floor node in the main building structure
    Floor *temp = m_bottom;
    while (temp != nullptr && temp->m_floorNum != floor) {
        temp = temp->m_next;
    }

    // Deny request if floor is secured
    if (temp != nullptr && temp->m_secured == true) {
        return false;
    }

    // Add request to appropriate queue (down or up)
    if (floor < m_currentFloor->m_floorNum) {
        Floor *newRequest = new Floor(floor);
        newRequest->m_next = m_downRequests;
        if (m_downRequests != nullptr)
            m_downRequests->m_previous = newRequest;
        m_downRequests = newRequest;
        if (m_moveState == IDLE)
            m_moveState = DOWN; // Set direction if idle
    } else {
        if (floor > m_currentFloor->m_floorNum) {
            Floor *newRequest = new Floor(floor);
            newRequest->m_next = m_upRequests;
            if (m_upRequests != nullptr)
                m_upRequests->m_previous = newRequest;
            m_upRequests = newRequest;
            if (m_moveState == IDLE)
                m_moveState = UP; // Set direction if idle
        }
    }
    return true;
}

// Processes the next highest priority request in the current direction
bool Elevator::processNextRequest() {
    // Do not process if load limit exceeded or emergency is active
    if (m_load > LOADLIMIT)
        return false;
    if (m_emergency)
        return false;

    Floor *nextRequest = nullptr;
    // Determine next request based on move state
    if (m_moveState == UP) {
        nextRequest = m_upRequests;
        Floor *temp = m_upRequests;
        while (temp != nullptr) {
            if (temp->m_floorNum < nextRequest->m_floorNum) // Finds lowest floor in up requests
                nextRequest = temp;
            temp = temp->m_next;
        }
    } else if (m_moveState == DOWN) {
        nextRequest = m_downRequests;
        Floor *temp = m_downRequests;
        while (temp != nullptr) {
            if (temp->m_floorNum > nextRequest->m_floorNum) // Finds highest floor in down requests
                nextRequest = temp;
            temp = temp->m_next;
        }
    } else
        return false; // No request if idle

    if (nextRequest == nullptr)
        return false; // No pending requests

    m_currentFloor = nextRequest; // Move elevator to requested floor

    // Remove the processed request from its list
    if (m_moveState == UP) {
        if (nextRequest->m_previous != nullptr)
            nextRequest->m_previous->m_next = nextRequest->m_next;
        else
            m_upRequests = nextRequest->m_next;
        if (nextRequest->m_next != nullptr)
            nextRequest->m_next->m_previous = nextRequest->m_previous;
    } else if (m_moveState == DOWN) {
        if (nextRequest->m_previous != nullptr)
            nextRequest->m_previous->m_next = nextRequest->m_next;
        else
            m_downRequests = nextRequest->m_next;
        if (nextRequest->m_next != nullptr)
            nextRequest->m_next->m_previous = nextRequest->m_previous;
    }

    m_doorState = OPEN; // Open door at destination
    return true;
}

// Increases the current load of the elevator
void Elevator::enter(int load) {
    m_load += load;
}

// Decreases the current load of the elevator
int Elevator::exit(int load) {
    m_load -= load;
    if (m_load < 0)
        m_load = 0; // Prevent negative load

    return m_load;
}

// Dumps the current state and floor information of the elevator
void Elevator::dump() {
    // Print elevator's movement status
    if (m_moveState == IDLE) cout << "Elevator " << m_id << " is idle.";
    else if (m_moveState == UP) cout << "Elevator " << m_id << " is moving up.";
    else if (m_moveState == DOWN) cout << "Elevator " << m_id << " is moving down.";
    cout << endl;

    // Print emergency status
    if (m_emergency == true) cout << "someone pushed the emergency button!" << endl;

    // Print all accessible floors from top to bottom
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
        cout << temp->m_floorNum; // Print the bottom-most floor
        if (temp->m_floorNum == m_currentFloor->m_floorNum) cout << " current ";
        if (temp->m_secured == true) cout << " secured ";
        cout << endl;
        cout << "Bottom" << endl;
    }
}