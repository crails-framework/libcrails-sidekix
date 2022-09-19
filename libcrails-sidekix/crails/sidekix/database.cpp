#include "database.hpp"

using namespace Sidekix;

Sidekix::SideTaskDatabase* Sidekix::SideTaskDatabase::instance()
{
  static thread_local const std::unique_ptr<SideTaskDatabase> database(initialize());

  return database.get();
}
