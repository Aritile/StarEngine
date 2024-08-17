#include "Job.h"
#include "../STAR/Star.h"

Job* Job::GetSingleton()
{
    static Job job;
    return &job;
}
void Job::Update()
{
    for (auto it = jobBuffers.begin(); it != jobBuffers.end();)
    {
        (*it)->Update();
        if ((*it)->IsDone())
        {
            //Star::AddLog("[Job] -> Job is completed.");
            delete* it;
            it = jobBuffers.erase(it);
            completedJobs++;
        }
        else
            ++it;
    }
}
void Job::Release()
{
    for (auto it = jobBuffers.begin(); it != jobBuffers.end();)
    {
        delete* it;
        it = jobBuffers.erase(it);
    }
}
bool Job::IsDone()
{
    return jobBuffers.empty();
}

int Job::GetJobsSize()
{
    return totalJobs;
}

int Job::GetCompletedJobsSize()
{
    return completedJobs;
}

void Job::ResetJobs()
{
    completedJobs = 0;
    totalJobs = 0;
}