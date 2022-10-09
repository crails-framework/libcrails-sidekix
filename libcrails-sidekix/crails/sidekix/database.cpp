#include "database.hpp"

using namespace Sidekix;

namespace Sidekix
{
  const std::string run_at_param("sidekix.run_at");
  const std::string task_uid_param("sidekix.task_uid");
  const std::string task_type_param("sidekix.type");
}

Sidekix::SideTaskDatabase* Sidekix::SideTaskDatabase::instance()
{
  static thread_local const std::unique_ptr<SideTaskDatabase> database(initialize());

  return database.get();
}
