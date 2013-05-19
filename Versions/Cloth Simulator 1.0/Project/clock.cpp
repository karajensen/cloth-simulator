
#include "clock.h"

//=============================================================
//Initialise the clock
//=============================================================
void CLOCK::InitialiseClock(long int Start, long int Target, bool minutes)
{
	ShowMinutes = minutes;
	ClockSecondsStart = Start;
	ClockTarget = Target;
	ReachedTarget = false;

	if(Target > Start)
		CountingUp = true;
	else
		CountingUp = false;
}

//=============================================================
//Start the clock
//=============================================================
void CLOCK::StartClock()
{
	Ticking = true;
	time_t Seconds = time(NULL);
	SecondsStart = long(Seconds);
}

//=============================================================
//Stop the clock
//=============================================================
void CLOCK::StopClock()
{
	Ticking = false;
}

//=============================================================
//Calculate the amount of time left
//=============================================================
void CLOCK::CalculateTimePassed()
{
	static int FrameCount = 0;

	if(Ticking) //if the clock has started
	{
		FrameCount++;
		if(FrameCount >= 30) //only check every 30 frames
		{
			time_t Seconds = time(NULL);
			SecondsPassed = long(Seconds);
			long SecondsDifference = SecondsPassed - SecondsStart;
	
			if(CountingUp)
				ClockSecondsPassed = ClockSecondsStart + SecondsDifference;
			else
				ClockSecondsPassed = ClockSecondsStart - SecondsDifference;
			
			if(ShowMinutes)	//convert to minutes/seconds
			{
				long int secs,mins;
				mins = ClockSecondsPassed/60;
				secs = ClockSecondsPassed - (mins*60);
				if((secs < 10)&&(mins < 10))
					Stream << "0" << mins << ":0" << secs;
				else if(secs < 10)
					Stream << mins << ":0" << secs;
				else if(mins < 10)
					Stream << "0" << mins << ":" << secs;
				else
					Stream << mins << ":" << secs;
			}
			else
				Stream << ClockSecondsPassed;

			string time;
			getline(Stream,time);
			Stream.clear();

			this->SetText(time);
			FrameCount = 0;

			//check if reached target
			if(ClockSecondsPassed == ClockTarget)
				ReachedTarget = true;

		}
	}
}