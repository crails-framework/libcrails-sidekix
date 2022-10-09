#ifndef  SIDEKIX_FILE_DATABASE_HPP
# define SIDEKIX_FILE_DATABASE_HPP

# include "database.hpp"
# include <crails/utils/backtrace.hpp>
# include <crails/read_file.hpp>
# include <crails/utils/random_string.hpp>
# include <crails/environment.hpp>
# include <fstream>
# include <filesystem>
# include <chrono>

namespace Sidekix
{
  class FileDatabase : public SideTaskDatabase
  {
    std::filesystem::path storage_path;

    bool should_run_after(const std::string& id, std::time_t timestamp) const
    {
      using namespace std;
      string contents;

      try
      {
        if (Crails::read_file(id, contents))
        {
          DataTree peek;
          peek.from_json(contents);
          Data run_at = peek[Sidekix::run_at_param];
          return !run_at.exists() || run_at.as<std::time_t>() < timestamp;
        }
      } catch (...)
      {
      }
      return false;
    }
  public:
    FileDatabase(const std::filesystem::path& storage_path) : storage_path(storage_path)
    {
      std::filesystem::create_directories(storage_path);
    }

    std::vector<std::string> pending_tasks() const override
    {
      using namespace std;
      using namespace std::filesystem;
      auto timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
      vector<string> tasks;
      directory_iterator it(storage_path);
      directory_iterator end_it;

      for (; it != end_it ; ++it)
      {
        if (is_regular_file(it->path()) && should_run_after(it->path().string(), timestamp))
          tasks.push_back(it->path().string());
      }
      return tasks;
    }

    bool take(const std::string& id, DataTree& task_data) const override
    {
      using namespace std;
      using namespace std::filesystem;
      string contents;

      if (Crails::read_file(id, contents))
      {
        remove(path(id));
        task_data.from_json(contents);
        return true;
      }
      return false;
    }

    bool pop(DataTree& task_data) const override
    {
      auto list = pending_tasks();

      if (list.size() > 0)
        return take(*list.begin(), task_data);
      return false;
    }

    std::string schedule(const std::string& name, Data params) const override
    {
      using namespace std;
      string   uid      = Crails::generate_random_string("abcdefghijklmnopqrstwxyz0123456789", 10);
      string   filename = filesystem::canonical(storage_path).string() + '/' + uid;
      ofstream file(filename.c_str());

      if (file.is_open())
      {
        params[Sidekix::task_uid_param] = uid;
        params[Sidekix::task_type_param] = name;
        file << params.to_json();
        file.close();
      }
      else
        throw boost_ext::runtime_error("sidekix cannot create file " + filename);
      return uid;
    }
  };
}

#endif
