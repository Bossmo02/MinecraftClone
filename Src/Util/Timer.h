#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <iostream>


class Timer
{
public:
	Timer() 
	{
		startTimePoint = std::chrono::high_resolution_clock::now(); 
	}
	
	~Timer()
	{ 
		auto endPoint = std::chrono::high_resolution_clock::now(); 
		auto timeTaken = endPoint - startTimePoint;
		std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeTaken).count() << "ms\n";
	}

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint;

};

#endif // !TIMER_HPP

