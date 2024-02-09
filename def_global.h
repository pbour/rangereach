#pragma once
#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <fstream>
#include <string>
#include <vector>
using namespace std;



//#define BOOST_GEOMETRIES

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;


typedef unsigned int    ID;

class Timer
{
private:
	using Clock = std::chrono::high_resolution_clock;
	Clock::time_point start_time, stop_time;
	
public:
	Timer()
	{
		start();
	}
	
	void start()
	{
		start_time = Clock::now();
	}
	
	
	double getElapsedTimeInSeconds()
	{
		return std::chrono::duration<double>(stop_time - start_time).count();
	}
	
	
	double stop()
	{
		stop_time = Clock::now();
		return getElapsedTimeInSeconds();
	}
};
#endif // _GLOBAL_DEF_H_
