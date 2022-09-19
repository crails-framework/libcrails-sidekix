#ifndef  SIDEKIX_PROCESS_HPP
# define SIDEKIX_PROCESS_HPP

# include <crails/utils/singleton.hpp>
# include <crails/logger.hpp>
# include <boost/process.hpp>
# include <signal.h>

namespace Sidekix
{
  class Process
  {
    SINGLETON(Process)
    std::unique_ptr<boost::process::child> process;

    Process()
    {
      process.reset(
        new boost::process::child(sidekix_bin())
      );
    }

    ~Process()
    {
      Crails::logger << Crails::Logger::Info << "Stopping sidekix" << Crails::Logger::endl;
      kill(SIGINT, process->id());
      process->wait();
      Crails::logger << Crails::Logger::Info  << "Sidekix stopped" << Crails::Logger::endl;
    }

    static std::string sidekix_bin()
    {
      return "build/tasks/sidekix/task";
    }
  };
}

#endif
