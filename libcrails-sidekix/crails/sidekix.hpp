#ifndef  SIDEKIX_HPP
# define SIDEKIX_HPP

# include "sidekix/database.hpp"

namespace Sidekix
{
  std::string async_task(const std::string& name, Data params);
  std::string async_task(const std::string& name, DataTree& params) { return async_task(name, params.as_data()); }
  std::string async_task(const std::string& name) { return async_task(name, DataTree()); }
  std::string schedule_task(std::time_t timestamp, const std::string& name, Data params);
  std::string schedule_task(std::time_t timestamp, const std::string& name, DataTree& params) { return schedule_task(timestamp, name, params.as_data()); }
  std::string schedule_task(std::time_t timestamp, const std::string& name) { return schedule_task(timestamp, name, DataTree()); }
  std::string schedule_task_in(std::time_t interval, const std::string& name, Data params);
  std::string schedule_task_in(std::time_t interval, const std::string& name, DataTree& params) { return schedule_task_in(interval, name, params.as_data()); }
  std::string schedule_task_in(std::time_t interval, const std::string& name) { return schedule_task_in(interval, name, DataTree()); }
};

#endif
