#ifndef  SIDEKIX_HPP
# define SIDEKIX_HPP

# include "sidekix/database.hpp"
# include <chrono>

namespace Sidekix
{
  std::string async_task(const std::string& name, Data params);
  std::string async_task(const std::string& name, DataTree& params);
  std::string async_task(const std::string& name);
  std::string schedule_task(std::time_t timestamp, const std::string& name, Data params);
  std::string schedule_task(std::time_t timestamp, const std::string& name, DataTree& params);
  std::string schedule_task(std::time_t timestamp, const std::string& name);
  std::string schedule_task(std::chrono::time_point<std::chrono::system_clock> timepoint, const std::string& name, Data params);
  std::string schedule_task(std::chrono::time_point<std::chrono::system_clock> timepoint, const std::string& name, DataTree& params);
  std::string schedule_task(std::chrono::time_point<std::chrono::system_clock> timepoint, const std::string& name);
  std::string schedule_task_in(std::time_t interval, const std::string& name, Data params);
  std::string schedule_task_in(std::time_t interval, const std::string& name, DataTree& params);
  std::string schedule_task_in(std::time_t interval, const std::string& name);
  std::string schedule_task_in(std::chrono::seconds interval, const std::string& name, Data params);
  std::string schedule_task_in(std::chrono::seconds interval, const std::string& name, DataTree& params);
  std::string schedule_task_in(std::chrono::seconds interval, const std::string& name);
};

#endif
