#ifndef CENTCOM_H
#define CENTCOM_H
#include <string>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
using namespace std;
enum DIRECTION {IDLE,UP,DOWN};  // possible states
enum DOOR {OPEN,CLOSED};        // possible states
const int LOADLIMIT = 2000;     // lbs, max load that an elevator can lift
const int INVALIDID = -1;       // Elevator ID is positive and starts at zero
class Tester;

class Floor{
    friend class Tester;
    friend class Elevator;
    friend class CentCom;
    public:
    Floor(int floor){
        m_floorNum = floor; 
        m_secured = false;  // by default all floors are accessible
        m_next=nullptr;
        m_previous=nullptr;
        }
    ~Floor(){}
    private:
    int m_floorNum;
    Floor* m_next;      
    Floor* m_previous;  
    bool m_secured;     
};
class Elevator{
    friend class Tester;
    friend class CentCom;
    public:
    Elevator(int ID = INVALIDID);
    ~Elevator();
    void setUp(int firstFloor, int lastFloor);  // this creates nodes in the linked list
    bool insertFloor(int floor);                
    bool pushButton(int floor);
    void pushEmergency(bool pushed);    // this can only set to true
    void enter(int load);               // new load enters the car
    int exit(int load);                 // a load exits, returns current load after exit
    bool checkSecure(int floor);        // check whether floor is secure
    void dump();                        // for debugging purposes
    void clear();
    bool processNextRequest();

    private:
    int m_id;           // the elevator ID (unique)
    Floor* m_bottom;    // this is the head of the doubly linked list
    Floor* m_top;       // this is the tail of the doubly linked list
    Floor* m_upRequests;   // a linked list of pushed buttons in order to be processed
    Floor* m_downRequests; // a linked list of pushed buttons in order to be processed
    Floor* m_currentFloor; // this stores the current floor
    DIRECTION m_moveState; // state of moving, stores either of IDLE,UP,DOWN
    DOOR m_doorState;      // state of door, either of OPEN, CLOSED
    bool m_emergency;      // true means emergency button is pushed by a passenger
    int m_load;            // this is load weight in pounds (lbs)


};
class CentCom{
    friend class Tester;
    public:
    CentCom(int numElevators=0, int buildingID=0);
    ~CentCom();
    bool addElevator(int ID, int bottomFloor, int topFloor);
    bool setSecure(int ID, int floorNum, bool yes_no);
    Elevator* getElevator(int ID);
    bool clearEmergency(int ID);
    private:
    int m_id;           // the building ID (unique), it is positive and starts at zero
    int m_numElevators; // number of elevators (array size, not number of created elevators)
    Elevator ** m_elevatorsList; // this is an array holding elevator pointers.

};
#endif