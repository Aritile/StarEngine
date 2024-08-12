#include "DTiming.h"
#include "../IMGUI/imgui.h"

TimingBuffer* Timing::AddTimer(std::string _Name)
{
	timings.emplace_back();
	TimingBuffer* timing = &timings.back();
	timing->SetName(_Name);
	return timing;
}
void TimingBuffer::StartTimer()
{
	startTime = std::chrono::high_resolution_clock::now();
	isActive = true;
}
void TimingBuffer::StopTimer()
{
	endTime = std::chrono::high_resolution_clock::now();
	isActive = false;
}
TimingBuffer* Timing::GetTimer(std::string _Name)
{
	for (size_t i = 0; i < timings.size(); i++)
	{
		if (timings[i].GetName().compare(_Name) == 0)
		{
			TimingBuffer* timing = &timings.at(i);
			return timing;
		}
	}

	return NULL;
}
void TimingBuffer::SetName(std::string _Name)
{
	name = _Name;
}
std::string TimingBuffer::GetName()
{
	return name;
}
float TimingBuffer::GetElapsedTime()
{
	return std::chrono::duration<float>(endTime - startTime).count();
}
bool TimingBuffer::IsActive()
{
	return isActive;
}
Timing* Timing::GetSingleton()
{
	static Timing timing;
	return &timing;
}
std::vector<TimingBuffer>* Timing::GetTimings()
{
	return &timings;
}
void TimingBuffer::SetTotalTime(float _TotalTime)
{
	totalTime = _TotalTime;
}
float TimingBuffer::GetTotalTime()
{
	return totalTime;
}
void TimingBuffer::AddTotalTime(float _TotalTime)
{
	totalTime += _TotalTime;
}
void TimingBuffer::DrawPlotLines()
{
	if (refresh_time == 0.0)
		refresh_time = ImGui::GetTime();
	while (refresh_time < ImGui::GetTime())
	{
		lastData[values_offset] = GetTotalTime();
		values_offset = (values_offset + 1) % (int)lastData.size();
		refresh_time += 1.0f / 60.0f;
	}
	{
		float average = 0.0f;
		for (int n = 0; n < lastData.size(); n++)
			average += lastData[n];
		average /= (float)lastData.size();
		char overlay[32];
		sprintf(overlay, "avg %.3f", average);
		std::string x = "##" + GetName();
		ImGui::PlotLines(x.c_str(), lastData.data(), lastData.size(), values_offset, overlay, 0.0f, 0.1f, ImVec2(0, 80.0f));
	}
}