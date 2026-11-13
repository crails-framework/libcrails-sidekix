#ifndef  SIDEKIX_PROCESS_HPP
# define SIDEKIX_PROCESS_HPP

# include <atomic>
# include <crails/utils/singleton.hpp>
# include <crails/logger.hpp>
# include <crails/program_options.hpp>
# include <boost/asio/io_context.hpp>
# include <boost/asio/executor_work_guard.hpp>
# include <boost/process.hpp>
# include <signal.h>
# include <filesystem>
# include <thread>
# include <mutex>
# include <vector>
# ifdef _WIN32
#  include <Windows.h>
# endif

namespace Sidekix
{
  class Process
  {
    SINGLETON(Process)
    boost::asio::io_context ioc;
    std::optional<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> work_guard;
    std::unique_ptr<std::thread> asio_thread;
    std::unique_ptr<boost::process::v2::process> process;
    std::unique_ptr<std::thread> watcher;
    boost::process::v2::pid_type pid;
    std::atomic_bool auto_restart;
    std::mutex mutex;

  public:
    Process(int argc, const char** argv)
    {
      Crails::ProgramOptions options(argc, const_cast<const char**>(argv));
      std::string log_file = options.get_log_file();
      std::string error_log_file = options.get_error_log_file();

      Crails::logger << Crails::Logger::Info << "Starting sidekix (" << sidekix_bin() << ')' << Crails::Logger::endl;

      // Keep the io_context running in a background thread for v2 process monitoring/async features
      work_guard.emplace(boost::asio::make_work_guard(ioc));
      asio_thread = std::make_unique<std::thread>([this]() { ioc.run(); });

      watcher.reset(new std::thread(std::bind(&Sidekix::Process::watch, this, log_file, error_log_file)));
    }

    ~Process()
    {
      Crails::logger << Crails::Logger::Info << "Stopping sidekix" << Crails::Logger::endl;
      mutex.lock();
      auto_restart = false;
      if (process)
      {
        boost::system::error_code ec;
        process->request_exit(ec);
      }
      mutex.unlock();

      watcher->join();

      // Stop the Asio io_context and join its thread
      work_guard.reset();
      ioc.stop();
      if (asio_thread && asio_thread->joinable())
        asio_thread->join();

      Crails::logger << Crails::Logger::Info << "Sidekix stopped" << Crails::Logger::endl;
    }

  private:
    void watch(std::string log_file, std::string error_log_file)
    {
      try
      {
        std::vector<std::string> args;
        if (!log_file.empty())
        {
          args.push_back("-l");
          args.push_back(transform_logpath(log_file));
        }
        if (!error_log_file.empty())
        {
          args.push_back("-e");
          args.push_back(transform_logpath(error_log_file));
        }

        // Directly pass executable path and argument vector to avoid shell parsing errors
        process = std::make_unique<boost::process::v2::process>(
            ioc, 
            sidekix_bin(), 
            args
        );

        mutex.lock();
        pid = process->id();
        mutex.unlock();

        process->wait();

        int exit_code = process->exit_code();
        if (exit_code != 0)
          Crails::logger << Crails::Logger::Error << "Sidekix process exited with code " << exit_code << Crails::Logger::endl;
      }
      catch (const std::exception& e)
      {
        Crails::logger << Crails::Logger::Error << "Sidekix watcher exception: " << e.what() << Crails::Logger::endl;
      }
      catch (...)
      {
        Crails::logger << Crails::Logger::Error << "Sidekix watcher unknown exception" << Crails::Logger::endl;
      }

      if (auto_restart)
        watch(log_file, error_log_file);
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
