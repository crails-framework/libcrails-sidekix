#ifndef  SIDEKIX_REDIS_DATABASE_HPP
# define SIDEKIX_REDIS_DATABASE_HPP

# include "database.hpp"
# include <crails/redis/connection.hpp>
# include <crails/utils/random_string.hpp>
# include <chrono>

namespace Sidekix
{
  class RedisDatabase : public SideTaskDatabase
  {
    typedef Crails::Redis::SortedList<std::string, Crails::Redis::Connection> List;

    Crails::Redis::Connection connection;
    std::string storage_key;
    mutable List task_list;
  public:
    RedisDatabase(const std::string& database = "", const std::string& key = "") :
      connection(database),
      storage_key(key.length() == 0 ? "sidekix-task" : key),
      task_list(connection.make_sorted_list<std::string>(storage_key))
    {
    }

  private:
    std::vector<std::string> pending_tasks() const override
    {
      using namespace std;
      vector<string> results, uids;
      auto timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());

      task_list.copy(results, 0, timestamp);
      for (const string& entry : results)
      {
        DataTree peek;
        peek.from_json(entry);
        uids.push_back(peek["sidekix"]["task_uid"].as<string>());
      }
      return uids;
    }

    bool take(const std::string&, DataTree& task_data) const override
    {
      throw boost_ext::runtime_error("Sidekix::RedisDatabase does not support `take`");
    }

    bool pop(DataTree& task_data) const override
    {
      auto result = task_list.pop_front();

      if (result)
      {
        task_data.from_json(result->first);
        return true;
      }
      return false;
    }

    std::string schedule(const std::string& name, Data params) const override
    {
      using namespace std;
      auto timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());

      return schedule(timestamp, name, params);
    }

    std::string schedule(std::time_t timestamp, const std::string& name, Data params) const override
    {
      using namespace std;
      string uid = Crails::generate_random_string("abcdefghijklmnopqrstwxyz0123456789", 10);

      params["sidekix"]["task_uid"] = uid;
      params["sidekix"]["type"] = name;
      task_list.emplace(params.to_json(), timestamp);
      return uid;
    }
  };
}

#endif
