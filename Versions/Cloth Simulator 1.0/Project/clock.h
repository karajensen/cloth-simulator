/********************************************************************
 File: clock.h
 Author: Kara Jensen                                
 Last Modified by: Kara Jensen              
 Date Modified: 10/10/10  
 Comments: Clock class derived from text class
 *******************************************************************/

#ifndef CLOCK_H_
#define CLOCK_H_

#include "includes.h"
#include "text.h"
#include <ctime>
#include <sstream>

class CLOCK : public DTEXT
{
private:
	stringstream Stream;
	long int SecondsStart;
	long int SecondsPassed;
	long int ClockSecondsStart;
	long int ClockSecondsPassed;
	long int ClockTarget;
	bool Ticking;
	bool CountingUp;
	bool ShowMinutes;
	bool ReachedTarget;

public:
	void InitialiseClock(long int Start, long int Target, bool minutes);
	void StartClock();
	void StopClock();
	void CalculateTimePassed();
	long int GetSecondsPassed(){return ClockSecondsPassed;};
	long int GetStartingTime(){return ClockSecondsStart;};
	bool HasReachedTarget(){return ReachedTarget;};
};

#endif