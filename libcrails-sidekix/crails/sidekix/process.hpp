#ifndef  SIDEKIX_PROCESS_HPP
# define SIDEKIX_PROCESS_HPP

# include <crails/utils/singleton.hpp>
# include <crails/logger.hpp>
# include <crails/program_options.hpp>
# include <boost/process.hpp>
# include <signal.h>
# include <filesystem>
# include <thread>
# include <mutex>
# ifdef _WIN32
#  include <Windows.h>
# endif

namespace Sidekix
{
  class Process
  {
    SINGLETON(Process)
    std::unique_ptr<boost::process::child> process;
    std::unique_ptr<std::thread> watcher;
    boost::process::detail::api::pid_t pid;
    bool auto_restart = true;
    std::mutex mutex;
  public:
    Process(int argc, const char** argv)
    {
      Crails::ProgramOptions options(argc, const_cast<const char**>(argv));
      std::stringstream command;

      command << sidekix_bin();
      if (options.get_log_file().length() > 0)
        command << " -l \"" << transform_logpath(options.get_log_file()) << '"';
      if (options.get_error_log_file().length() > 0)
        command << " -e \"" << transform_logpath(options.get_error_log_file()) << '"';
      Crails::logger << Crails::Logger::Info << "Starting sidekix (" << command.str() << ')' << Crails::Logger::endl;
      watcher.reset(new std::thread(std::bind(&Sidekix::Process::watch, this, command.str())));
    }

    ~Process()
    {
      Crails::logger << Crails::Logger::Info << "Stopping sidekix" << Crails::Logger::endl;
      mutex.lock();
      auto_restart = false;
      kill(SIGINT, process->id());
      mutex.unlock();
      watcher->join();
      Crails::logger << Crails::Logger::Info  << "Sidekix stopped" << Crails::Logger::endl;
    }

  private:
    void watch(std::string command)
    {
      process.reset(new boost::process::child(command));
      mutex.lock();
      pid = process->id();
      mutex.unlock();
      process->wait();
      if (process->exit_code() != 0)
        Crails::logger << Crails::Logger::Error << "Sidekix process exited with code " << process->exit_code() << Crails::Logger::endl;
      if (auto_restart)
        watch(command);
    }

    static std::string sidekix_bin()
    {
      std::filesystem::path exe_path;

#ifdef _WIN32
      wchar_t buffer[MAX_PATH];
      GetModuleFileName(NULL, buffer, MAX_PATH);
      exe_path = std::filesystem::canonical(buffer);
#else
      exe_path = std::filesystem::canonical("/proc/self/exe");
#endif
      return exe_path.parent_path().string() + "/tasks/sidekix/task";
    }

    static std::string transform_logpath(const std::string& path)
    {
      std::string suffix = "sidekix";
      std::size_t last_index = path.find_last_of('.');

      if (last_index == std::string::npos)
        return path + '.' + suffix;
      return path.substr(0, last_index) + '.' + suffix + path.substr(last_index);
    }
  };
}

#endif
