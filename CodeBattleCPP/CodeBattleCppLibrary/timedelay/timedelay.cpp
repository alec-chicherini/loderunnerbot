#include "timedelay.h"


std::string timedelay::queueTest()
{
	std::string str;
	str += ".empty=";
	str += std::to_string(stopwatchQueue.empty());
	str += ".size=";
	str += std::to_string(stopwatchQueue.size());

	return str;
}

void timedelay::addAtQueue(float checkTime)
{
	tPoint nowT = std::chrono::high_resolution_clock::now();
	stopwatch sW = { checkTime,nowT };
	stopwatchQueue.push(sW);//add to the end
}

bool timedelay::checkQueue()
{
	if (stopwatchQueue.empty())return false;

	tPoint nowT = std::chrono::high_resolution_clock::now();
	duration<float> timerDuration = nowT - stopwatchQueue.front().stopwatchPoint;
	if (timerDuration.count() >= stopwatchQueue.front().checkTime) {
		stopwatchQueue.pop();
		return true; }


	return false;
}

bool timedelay::isEmptyQueue()
{
	return stopwatchQueue.empty();
}

void timedelay::addTimer(std::string name, float checkTime)
{
	tPoint nowT = std::chrono::high_resolution_clock::now();
	

	stopwatch sW = { checkTime,nowT };

	//if timer with this name exist update value, if not create new
	tArray.insert_or_assign(name, sW);
}

void timedelay::addTimer(std::string name)
{
	addTimer(name, 0);
	
}

bool timedelay::checkTimer(std::string name)
{
	if (tArray.count(name) == 0)return true;

  std::map<std::string, stopwatch>::iterator it = tArray.find(name);
  //if wrong name - do not exist in array return -1.0
 
  if (it == tArray.end()) { 
	
#ifdef _CONSOLE
	#ifdef _DEBUG
	  std::cout << "DEBUG::At " << __FUNCTION__ << " wrong name param." << std::endl;
	#endif
#endif
	 
  }
   tPoint nowT= std::chrono::high_resolution_clock::now();
  //if time in array more then  now to check time value return true
  duration<float> timerDuration =  nowT - it->second.stopwatchPoint;
  if (timerDuration.count() >= it->second.checkTime) return true;

	return false;
}

float timedelay::readTimer(std::string name)
{
	std::map<std::string, stopwatch>::iterator it = tArray.find(name);
	//if wrong name - do not exist in array return -1.0
	if (it == tArray.end()) { 
#ifdef _CONSOLE
	#ifdef _DEBUG
		std::cout << "DEBUG::At " << __FUNCTION__ << " wrong name param." << std::endl;
	#endif
#endif
		return -1.0; }
	tPoint nowT = std::chrono::high_resolution_clock::now();
	//if time in array more then  now to check time value return true
	duration<float> timerDuration = nowT - it->second.stopwatchPoint;
	return timerDuration.count();

	
}

int timedelay::cycleTime()
{
	tPoint nowT = std::chrono::high_resolution_clock::now();

	int delay = duration_cast<milliseconds>(nowT - prev).count();
	prev = nowT;

	return delay;
}
