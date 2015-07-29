#include "Timer.h"
#include <Windows.h>
#include <ctime>

namespace novus
{

Timer::Timer()
	:SecondsPerCount(0.0),
	DeltaTime(0.0),
	BaseTime(0),
	PausedTime(0),
	PreviousTime(0),
	CurrentTime(0),
	bIsPaused(false)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSecond));
	SecondsPerCount = 1.0 / static_cast<double>(countsPerSecond);
	
	Reset();
	Tick();
}

void Timer::Start()
{
	__int64 startTime;

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTime));

	if (bIsPaused)
	{
		PausedTime += startTime - PausedTime;

		PreviousTime = startTime;
		StopTime = 0;
		bIsPaused = false;
	}
}

void Timer::Pause()
{
	if (!bIsPaused)
	{
		__int64 currTime;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));

		StopTime = currTime;
		bIsPaused = false;
	}
}

void Timer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));

	BaseTime = currTime;
	PreviousTime = currTime;
	PausedTime = 0;
	StopTime = currTime;
	bIsPaused = false;
}

void Timer::Tick()
{
	if (bIsPaused)
	{
		DeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));
	CurrentTime = currTime;

	DeltaTime = (CurrentTime - PreviousTime) * SecondsPerCount;

	PreviousTime = CurrentTime;

	if (DeltaTime < 0.0)
	{
		DeltaTime = 0.0;
	}
}

double Timer::GetTotalTime() const
{
	if (bIsPaused)
	{
		return ((StopTime - PausedTime) - BaseTime) * SecondsPerCount;
	}
	else
	{
		return ((CurrentTime - PausedTime) - BaseTime) * SecondsPerCount;
	}
}

double Timer::GetDeltaTime() const
{
	return DeltaTime;
}

}