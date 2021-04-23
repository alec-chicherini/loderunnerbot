#pragma once
//class using std::chrono to make some stopwatch in milliseconds.
//return bool value every call about timer is over or not
// access to the elements by string name or in queue logic. 

#include <iostream>
#include <chrono>
#include <queue>
#include <map>
#include <string>


using namespace std::chrono;

typedef time_point<high_resolution_clock> tPoint;

//consist point in time when it started and time delay how long it will be exist.
struct stopwatch{
	float checkTime=0;
	tPoint stopwatchPoint;
};

class timedelay
{private:
	//tree to store different timers and have access to it by string.
	std::map<std::string, stopwatch> tArray;

	//queue to adding some stopwatch in queue logic. FIFO adding and deleting stopwatches.
	std::queue<stopwatch> stopwatchQueue;

	tPoint prev;

public:
	/////////////////////////////////////////////////////////////////////////////////////
	//in this part functions for using stopwatchQueue
	//

	//test parameters of queue output
	std::string queueTest();


	//adding new element in queue at the end of queue.(FIFO)
	void addAtQueue(float checkTime);

	//checking the queue first element. if the timer in first element is over:
	//1)delete first element
	//2)return true result
	// if  first timer on this check is not over return false without deleting.
	bool checkQueue();

	//checking if stopwatch empty
	bool isEmptyQueue();

	//update nowTime value
	//void nowUpdate();

	//////////////////////////////////////////////////////////////////////////////////////
	//in this part like timers which start counting time from creating of timer addTimer()
	//and then check bool state using checkTimer() if timer is over it return 1 before that 0
    //and another one return the time difference between creating and call ReadTimer()

	//starting new timer in map by name and delay time.
	void addTimer(std::string name, float checkTime);

	//starting new timer in map by name.
	void addTimer(std::string name);

	//checking the array string by name element.
	//if the timer by this name is over:
	//   return true result
	//                    else return false
	bool checkTimer(std::string name);

	//return difference between stored value and now. if name wrong return -1.
	float readTimer(std::string name);


	//
	//function to mesure the time which one cycle complete in milliseconds
	int cycleTime();






};

