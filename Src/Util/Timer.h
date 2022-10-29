#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <iostream>


class Timer
{
public:
	Timer() 
	{
		m_startTimePoint = std::chrono::high_resolution_clock::now();
		m_additionalConsoleOutput = "";
	}

	Timer(const char* additionalConsoleOutput)
	{
		m_startTimePoint = std::chrono::high_resolution_clock::now();
		m_additionalConsoleOutput = additionalConsoleOutput;
	}
	
	~Timer()
	{ 
		auto endPoint = std::chrono::high_resolution_clock::now(); 
		auto timeTaken = endPoint - m_startTimePoint;
		std::cout << m_additionalConsoleOutput << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeTaken).count() << "ms\n";
	}

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimePoint;
	const char* m_additionalConsoleOutput;
};

#endif // !TIMER_HPP

