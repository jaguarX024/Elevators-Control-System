#include "centcom.h"
int main(){
    /*********************Scenario 1*********************************/
    Elevator anElev(10);
    anElev.setUp(0,12);
    anElev.pushButton(8);
    anElev.dump();// here must move up
    cout << "\n=====================\n" << endl;
    anElev.processNextRequest();
    anElev.dump();// current should be 8
    cout << "\n=====================\n" << endl;
    /*********************Scenario 2*********************************/
    CentCom controller(10);
    controller.addElevator(6,-5,12);// set up elevator 6 with floors -5 to 12
    controller.setSecure(6,4,true);
    Elevator * Elev2 = controller.getElevator(6);
    Elev2->dump();// it is idle and current is -5
    cout << "\n=====================\n" << endl;
    Elev2->pushButton(2);
    Elev2->processNextRequest();
    Elev2->dump();// it is moving up and current is 2
    cout << "\n=====================\n" << endl;
    Elev2->pushButton(4);// this floor is secured
    Elev2->processNextRequest();
    Elev2->dump();// it is moving up and current is 2, it ignores 4
    cout << "\n=====================\n" << endl;
    Elev2->pushButton(12);
    Elev2->processNextRequest();
    Elev2->dump();//  current is 12
    
   
    
    return 0;
}