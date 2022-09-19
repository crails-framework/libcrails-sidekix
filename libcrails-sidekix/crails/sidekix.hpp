#ifndef  SIDEKIX_HPP
# define SIDEKIX_HPP

# include "sidekix/database.hpp"

namespace Sidekix
{
  std::string async_task(const std::string& name, Data params);
  std::string schedule_task(std::time_t timestamp, const std::string& name, Data params);
};

#endif
