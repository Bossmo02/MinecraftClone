#pragma once
#include<random>
#include <time.h>

class Random
{
public:
	
	static Random& get();

	int getInt(int min, int max);
	float getFloat(float min, float max);
	double getDouble(double &min, double &max);
	
	template<class T>
	std::vector<T> getRandVec(size_t size, int min, int max);
	template<class T>
	std::vector<T> & getRandVec(size_t size, double min, double max);

private:

	std::mt19937 m_engine;

	Random();

};

Random::Random()
{
	m_engine.seed((unsigned int)time(NULL));
}

Random & Random::get()
{
	static Random r;
	return r;
}

int Random::getInt(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(m_engine);
}

float Random::getFloat(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(m_engine);
}

double Random::getDouble(double& min, double& max)
{
	std::uniform_real_distribution<double> dist(min, max);
	return dist(m_engine);
}

template<class T>
std::vector<T> Random::getRandVec(size_t size, int min, int max)
{
	std::vector<T> t;
	t.reserve(size);

	for (size_t i = 0; i < size; ++i)
		t.push_back(Random::get().getInt(min, max));

	return t;
}

template<class T>
std::vector<T> & Random::getRandVec(size_t size, double min, double max)
{
	std::vector<T> t;
	t.reserve(size);

	for (size_t i = 0; i < size; ++i)
		t.push_back(Random::get().getDouble(min, max));

	return t;
}
