/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

namespace novus
{

class Timer
{
public:
	Timer();

	void Start();
	void Pause();
	void Reset();
	void Tick();

	double GetTotalTime() const;
	double GetDeltaTime() const;

private:
	double SecondsPerCount;
	double DeltaTime;

	__int64 BaseTime;
	__int64 PausedTime;
	__int64 StopTime;
	__int64 PreviousTime;
	__int64 CurrentTime;

	bool bIsPaused;
};

};