#pragma once
#include <crails/logger.hpp>
#include <crails/datatree.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace Sidekix
{
  class SidetaskLogger
  {
    std::string output;
    std::stringstream stream;
    int status = -1;
  public:
    SidetaskLogger(Data params);
    ~SidetaskLogger();

    static std::string read_logs(const std::filesystem::path& path, Data params);

    inline SidetaskLogger& operator<<(Crails::Logger::Symbol symbol)
    {
      Crails::logger << symbol;
      return *this;
    }

    template<typename T>
    inline SidetaskLogger& operator<<(const T& value)
    {
      Crails::logger << value;
      stream << value;
      return *this;
    }

    inline std::ostream& get_output_stream()
    {
      return stream;
    }

    inline int exit_status(int value)
    {
      status = value;
      return status;
    }
  };
}
