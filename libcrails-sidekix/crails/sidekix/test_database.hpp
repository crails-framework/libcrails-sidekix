#ifndef  SIDEKIX_TEST_DATABASE_HPP
# define SIDEKIX_TEST_DATABASE_HPP

# include "database.hpp"
# include <crails/datatree.hpp>
# include <crails/utils/random_string.hpp>
# include <map>

namespace Sidekix
{
  namespace Tests
  {
    class Database : public ::Sidekix::SideTaskDatabase
    {
      mutable std::map<std::string, std::vector<DataTree>> tasks;
    public:
      std::time_t current_timestamp = 0;

      std::vector<std::string> pending_tasks() const override
      {
        using namespace std;
        vector<string> result;
        each_task([this, &result](const string&, const DataTree& params)
        {
          Data run_at = params["sidekix"]["run_at"];

          if (!run_at.exists() || run_at.as<time_t>() < current_timestamp)
            result.push_back(params["sidekix"]["task_uid"]);
        });
        return result;
      }

      bool take(const std::string& id, DataTree& task_data) const override
      {
        using namespace std;
        for (auto it = tasks.begin() ; it != tasks.end() ; ++it)
        {
          for (auto task_data = it->second.begin() ; task_data != it->second.end() ; ++task_data)
          {
            string task_uid = (*task_data)["sidekix"]["task_uid"];
            if (task_uid == id)
            {
              task_data->as_data().merge(*task_data);
              return true;
            }
          }
        }
        return false;
      }

      bool pop(DataTree& task_data) const override
      {
        auto pending = pending_tasks();
        if (pending.size() > 0)
          return take(*pending.begin(), task_data);
        return false;
      }

      std::string schedule(const std::string& name, Data params) const override
      {
        using namespace std;
        string   uid      = Crails::generate_random_string("abcdefghijklmnopqrstwxyz0123456789", 10);
        auto task_store = tasks.find(name);

        if (task_store == tasks.end())
        {
          vector<DataTree> array;
          array.reserve(4096);
          task_store = tasks.emplace(name, array).first;
        }
        vector<DataTree> task_list = task_store->second;
        task_list.push_back(DataTree());
        (*task_list.rbegin()).as_data().merge(params);
        (*task_list.rbegin())["sidekix"]["task_uid"] = uid;
        (*task_list.rbegin())["sidekix"]["type"] = name;
        return uid;
      }

      void each_task(std::function<void(const std::string&,const DataTree&)> callback) const
      {
        for (auto it = tasks.begin() ; it != tasks.end() ; ++it)
        {
          for (auto task_data = it->second.begin() ; task_data != it->second.end() ; ++task_data)
            callback(it->first, *task_data);
        }
      }

      std::size_t count(const std::string& name) const
      {
        auto task_store = tasks.find(name);

        return task_store == tasks.end() ? 0 : task_store->second.size();
      }

      bool has_task(const std::string& name) const { return count(name) > 0; }

      bool scheduled_at(const std::string& name, std::time_t timestamp)
      {
        bool result = false;

        each_task([&result, name, timestamp](const std::string& entry, const DataTree& params)
        {
          if (!result && entry == name)
            result = params["sidekix"]["run_at"].defaults_to<std::time_t>(0) == timestamp;
        });
        return result;
      }
    };
  }
}

#endif
