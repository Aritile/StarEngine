#include "Profiler.h"
#include "../../IMGUI/imgui.h"
#include "../../DEBUG/DTiming.h"

static Timing* timing = Timing::GetSingleton();

ProfilerWindow* ProfilerWindow::GetSingleton()
{
	static ProfilerWindow profilerWindow;
	return &profilerWindow;
}

void ProfilerWindow::Render()
{
	if (!renderProfiler) return;
	ImGui::Begin("Profiler", &renderProfiler);
	{
		for (size_t i = 0; i < timing->GetTimings()->size(); i++)
		{
			TimingBuffer* timingBuffer = &timing->GetTimings()->at(i);
			ImGui::Text("%s", timingBuffer->GetName().c_str());
			timingBuffer->DrawPlotLines();
			ImGui::Separator();
		}
	}
	ImGui::End();
}