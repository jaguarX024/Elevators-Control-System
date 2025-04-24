#include "centcom.h"
#include<iostream>
using namespace std;


class Tester{
public:
    //CentCom Tests
    bool testCentComConstructorNormalCase();
    bool testCentComConstructorErrorCase();
    bool testCentComAddElevatorNormalCase();
    bool testCentComAddElevatorErrorCase();

    //Elevator Tests
    bool testElevatorSetUpErrorCase();
    bool testElevatorInsertFloorErrorCase();
    bool testElevatorPushButtonNormalCase();
    bool testElevatorPushButtonErrorCase();
    bool testElevatorProcessNextRequestNormalCase();
    bool testElevatorProcessNextRequestErrorCase();

};


//CentCom Test Implementations

bool Tester::testCentComConstructorNormalCase(){
    CentCom centcom(5,1); // Create a CentCom object with valid parameters
    return (centcom.m_numElevators == 5 && centcom.m_id == 1 && centcom.m_elevatorsList != nullptr); // Check if the constructor initialized members correctly
}

bool Tester::testCentComConstructorErrorCase(){
    try{
        CentCom centcom(-3,1); // Attempt to create a CentCom object with an invalid number of elevators
    }
    catch(const invalid_argument& e){
        return true; // Expect an invalid_argument exception to be thrown
    }
    return false; // If no exception is thrown, the test fails
}

bool Tester::testCentComAddElevatorNormalCase(){
    CentCom centcom(5,1); // Create a CentCom object
    bool result= centcom.addElevator(2,0,10); // Add an elevator with a valid ID and floor range
    if(!result)
    {
        return false; // If addElevator fails, the test fails
    }

    Elevator* elevator= centcom.getElevator(2); // Get the elevator that was just added
    if (elevator== nullptr)
    {
        return false; // If getElevator returns nullptr, the test fails
    }


    return ( elevator->m_bottom->m_floorNum== 0 && elevator->m_top->m_floorNum == 10); // Check if the elevator's floor range was set correctly
}

bool Tester::testCentComAddElevatorErrorCase(){
    CentCom centcom(5,1); // Create a CentCom object
    bool result= centcom.addElevator(-1,0,10); // Attempt to add an elevator with an invalid ID
    return (!result); // Expect addElevator to return false for an invalid ID
}

//Elevator Test Implementations
bool Tester::testElevatorSetUpErrorCase(){
    Elevator elevator(1); // Create an Elevator object
    elevator.setUp(10,5); // Attempt to set up floors with an invalid range (first > last)
    return (elevator.m_bottom == nullptr && elevator.m_top == nullptr); // Expect m_bottom and m_top to remain nullptr
}

bool Tester::testElevatorInsertFloorErrorCase(){
    Elevator elevator(1); // Create an Elevator object
    elevator.setUp(1,10); // Set up a valid floor range

    bool result= elevator.insertFloor(15); // Attempt to insert a floor outside the initial range (above top)
    return (!result); // Expect insertFloor to return false
}

bool Tester::testElevatorPushButtonNormalCase(){
    Elevator elevator(1); // Create an Elevator object
    elevator.setUp(1,10); // Set up a valid floor range
    bool result = elevator.pushButton(5); // Push a button for a floor within the range
    if (!result)
    {
        return false; // If pushButton fails, the test fails
    }

    return (elevator.m_upRequests != nullptr && elevator.m_upRequests->m_floorNum== 5); // Check if the request was added to the upRequests list
}

bool Tester::testElevatorPushButtonErrorCase(){
    Elevator elevator(1); // Create an Elevator object
    elevator.setUp(1,10); // Set up a valid floor range
    bool result = elevator.pushButton(15); // Push a button for a floor outside the range
    return (!result); // Expect pushButton to return false
}

bool Tester::testElevatorProcessNextRequestNormalCase(){
    Elevator elevator(1); // Create an Elevator object
    elevator.setUp(1,10); // Set up a valid floor range
    elevator.pushButton(5); // Add a request to go to floor 5

    bool result= elevator.processNextRequest(); // Process the next request

    if (!result)
    {
        return false; // If processNextRequest fails, the test fails
    }


    return (elevator.m_currentFloor->m_floorNum == 5); // Check if the current floor has been updated to the requested floor
}

bool Tester::testElevatorProcessNextRequestErrorCase(){
    Elevator elevator(1); // Create an Elevator object
    elevator.setUp(1,10); // Set up a valid floor range
    elevator.pushEmergency(true); // Simulate an emergency
    bool result= elevator.processNextRequest(); // Attempt to process the next request during an emergency
    return (!result); // Expect processNextRequest to return false during an emergency
}


int main(){
    Tester tester;

    //CentCom Tests
    cout<<"Testing CentCom constructor normal case: "<< (tester.testCentComConstructorNormalCase()? "Passed":"Failed")<<endl;
    cout<<"Testing Centcom constructor error case: "<< (tester.testCentComConstructorErrorCase()? "Passed":"Failed")<<endl;
    cout<<"Testing CentCom addElevator normal case: "<< (tester.testCentComAddElevatorNormalCase()? "Passed":"Failed")<<endl;
    cout<<"Testing CentCom addElevator error case: "<< (tester.testCentComAddElevatorErrorCase()? "Passed":"Failed")<<endl;

    //Elevator Tests
    cout<<"Testing Elevator setUp error case: "<< (tester.testElevatorSetUpErrorCase()? "Passed":"Failed")<<endl;
    cout<<"Testing Elevator insertFloor error case: "<< (tester.testElevatorInsertFloorErrorCase()? "Passed": "Failed")<<endl;
    cout<<"Testing Elevator pushButton normal Case: "<< (tester.testElevatorPushButtonNormalCase()? "Passed": "Failed")<<endl;
    cout<<"Testing Elevator pushButton error case: "<< (tester.testElevatorPushButtonErrorCase()? "Passed": "Failed")<<endl;
    cout<<"Testing Elevator processNextRequest normal case: "<< (tester.testElevatorProcessNextRequestNormalCase()? "Passed":"Failed")<<endl;
    cout<<"Testing ELevator processNextRequest error case: "<< (tester.testElevatorProcessNextRequestErrorCase()? "Passed":"Failed")<<endl;


    return 0;
}
