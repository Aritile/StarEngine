#pragma once

#include <chrono>
#include <vector>

class TimingBuffer
{
public:
	TimingBuffer()
	{
		lastData.resize(90, 0.0f);
	}

private:
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point endTime;
	std::string name;
	bool isActive = false;
	float totalTime = 0.0f;

public:
	void SetName(std::string _Name);
	std::string GetName();
	void StartTimer();
	void StopTimer();
	float GetElapsedTime();
	bool IsActive();
	void SetTotalTime(float _TotalTime);
	float GetTotalTime();
	void AddTotalTime(float _TotalTime);
	void DrawPlotLines();

private:
	int values_offset = 0;
	double refresh_time = 0.0;
	std::vector<float> lastData;
};

class Timing
{
public:
	Timing() {}

public:
	TimingBuffer* AddTimer(std::string _Name);
	TimingBuffer* GetTimer(std::string _Name);
	std::vector<TimingBuffer>* GetTimings();

private:
	std::vector<TimingBuffer> timings;

public:
	static Timing* GetSingleton();
};