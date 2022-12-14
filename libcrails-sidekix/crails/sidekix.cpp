#include "sidekix.hpp"
#include <chrono>

using namespace std;

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
    return schedule_task(
      chrono::system_clock::to_time_t(chrono::system_clock::now() + chrono::seconds(interval)),
      name, params
    );
  }

  string async_task(const string& name, DataTree& params) { return async_task(name, params.as_data()); }
  string async_task(const string& name) { return async_task(name, DataTree()); }
  string schedule_task(time_t timestamp, const string& name, DataTree& params) { return schedule_task(timestamp, name, params.as_data()); }
  string schedule_task(time_t timestamp, const string& name) { return schedule_task(timestamp, name, DataTree()); }
  string schedule_task(chrono::time_point<chrono::system_clock> timepoint, const string& name, Data params) { return schedule_task(chrono::system_clock::to_time_t(timepoint), name, params); }
  string schedule_task(chrono::time_point<chrono::system_clock> timepoint, const string& name, DataTree& params) { return schedule_task(timepoint, name, params.as_data()); }
  string schedule_task(chrono::time_point<chrono::system_clock> timepoint, const string& name) { return schedule_task(timepoint, name, DataTree()); }
  string schedule_task_in(time_t interval, const string& name, DataTree& params) { return schedule_task_in(interval, name, params.as_data()); }
  string schedule_task_in(time_t interval, const string& name) { return schedule_task_in(interval, name, DataTree()); }
  string schedule_task_in(chrono::seconds interval, const string& name, Data params) { return schedule_task_in(interval.count(), name, params); }
  string schedule_task_in(chrono::seconds interval, const string& name, DataTree& params) { return schedule_task_in(interval.count(), name, params); }
  string schedule_task_in(chrono::seconds interval, const string& name) { return schedule_task_in(interval.count(), name); }
}
