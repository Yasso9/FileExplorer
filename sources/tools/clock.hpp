#include <chrono>

class Clock
{
    std::chrono::high_resolution_clock::time_point m_startTime;

  public:
    Clock();
    virtual ~Clock() = default;

    // restart the clock
    void  reset ();
    // return the elapsed time in seconds
    float get_elapsed_time () const;
};
