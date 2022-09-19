#include "sidekix.hpp"
#include <crails/environment.hpp>

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
}
