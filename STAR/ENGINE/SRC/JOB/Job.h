#pragma once

#include <future>

// 2024

class IJobBuffer
{
public:
    virtual ~IJobBuffer() = default; // this is ugly, but it is important
    virtual void Update() = 0;
    virtual bool IsDone() = 0;
};

template<typename T>
class JobBuffer : public IJobBuffer
{
public:
    using Task = std::function<T()>;
    using Callback = std::function<void(T)>;

    JobBuffer(Task _Task, Callback _Callback)
        : task(std::async(std::launch::async, _Task)), callback(_Callback), done(false), result(T{}) {}

    void Update() override
    {
        if (!done)
        {
            if (task.valid() && task.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                result = task.get();
                callback(result);
                done = true;
            }
        }
    }
    bool IsDone() override { return done; }
    T GetResult() { return result; }

private:
    std::future<T> task;
    Callback callback;
    T result;
    bool done;
};

template<>
class JobBuffer<void> : public IJobBuffer
{
public:
    using Task = std::function<void()>;
    using Callback = std::function<void()>;

    JobBuffer(Task _Task, Callback _Callback)
        : task(std::async(std::launch::async, _Task)), callback(_Callback), done(false) {}

    void Update() override
    {
        if (!done)
        {
            if (task.valid() && task.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                task.get();
                callback();
                done = true;
            }
        }
    }
    bool IsDone() override { return done; }

private:
    std::future<void> task;
    Callback callback;
    bool done;
};

class Job
{
public:
    template<typename T>
    void CreateJob(typename JobBuffer<T>::Task _Task, typename JobBuffer<T>::Callback _Callback)
    {
        //Star::AddLog("[Job] -> New job started.");
        jobBuffers.push_back(new JobBuffer<T>(_Task, _Callback));
        totalJobs++;
    }
    void Update();
    void Release();
    bool IsDone();
    int GetJobsSize();
    int GetCompletedJobsSize();
    static Job* GetSingleton();
    void ResetJobs();

private:
    std::list<IJobBuffer*> jobBuffers;
    int completedJobs = 0;
    int totalJobs = 0;
};