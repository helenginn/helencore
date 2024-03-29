//
//  Timer.h
//  vagabond
//
//  Created by Helen Ginn on 13/07/2017.
//  Copyright (c) 2017 Strubi. All rights reserved.
//

#ifndef __vagabond__Timer__
#define __vagabond__Timer__

#include <time.h>
#include <iostream>
#include <string>
#include <chrono>

/**
 * \class Timer
 * \brief Start, stop and report timing in minutes/seconds within other
 * areas of Vagabond.
 */

class Timer
{
public:
	Timer();	
	Timer(std::string name, bool start = false);	
	
	void start();
	void stop(bool quick = false);

	void setFine(bool fine)
	{
		_fine = fine;
	}
	
	void clear()
	{
		wall = 0;
		accumulative = 0;
		_milliseconds = 0;
	}
	
	void quickReport();
	void report(std::ostream *stream = &std::cout);
private:
	time_t wall;
	time_t accumulative;
	
	std::chrono::high_resolution_clock::time_point _tStart;
	std::chrono::high_resolution_clock::time_point _tEnd;
	size_t _milliseconds;
	bool _fine;

	std::string _name;
};


#endif

