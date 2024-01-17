#ifndef  SIDEKIX_DATABASE_HPP
# define SIDEKIX_DATABASE_HPP

# include <crails/datatree.hpp>
# include <vector>
# include <string>
# include <ctime>

namespace Sidekix
{
  extern const std::string run_at_param;
  extern const std::string task_uid_param;
  extern const std::string task_type_param;
  extern const std::string output_file_param;

  class SideTaskDatabase
  {
  public:
    virtual ~SideTaskDatabase() {}
    virtual std::vector<std::string> pending_tasks() const = 0;
    virtual bool take(const std::string& id, DataTree& params) const = 0;
    virtual bool pop(DataTree& params) const = 0;
    virtual std::string schedule(const std::string& name, Data params) const = 0;
    virtual std::string schedule(std::time_t timestamp, const std::string& name, Data params) const
    {
      params[Sidekix::run_at_param] = timestamp;
      return schedule(name, params);
    }

    static SideTaskDatabase* initialize();
    static SideTaskDatabase* instance();
  };
}

#endif
