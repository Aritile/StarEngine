## Async loading system
### For voids
```cpp
job->CreateJob<void>(
    []() {
        // Here code works in a new thread, without blocking the main thread
        std::this_thread::sleep_for(std::chrono::seconds(3)); // example of some work
    },
    []() {
        // Get work
        // Main thread, called once when the second thread finished work
    }
);
```
### For vars
```cpp
job->CreateJob<float>(
    [_Value]() -> float
    {
        float x = 0.0f;
        x = 5.0f + _Value;
        return x;
        // or return NULL;
    },
    [this](float _Value)
    {
        // Set value from the second finished thread to main thread
        this->x = _Value;
    }
);
```
