#include "sidekix.hpp"
#include <chrono>

using namespace std;
using namespace Crails;

namespace Sidekix
{
  string async_task(const string& name, Data params)
  {
    return SideTaskDatabase::instance()->schedule(name, params);
  }

  string schedule_task(time_t timestamp, const string& name, Data params)
  {
    return SideTaskDatabase::instance()->schedule(timestamp, name, params);
  }

  string schedule_task_in(time_t interval, const string& name, Data params)
  {
    time_t timestamp =
        chrono::system_clock::to_time_t(chrono::system_clock::now())
      + interval;

    schedule_task(timestamp, name, params);
  }
}
