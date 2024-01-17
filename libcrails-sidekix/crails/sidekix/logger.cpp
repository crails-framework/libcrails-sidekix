#include "logger.hpp"
#include "database.hpp"
#include <fstream>

using namespace Sidekix;

SidetaskLogger::SidetaskLogger(Data params)
{
  output = params[output_file_param].defaults_to<std::string>("");
}

SidetaskLogger::~SidetaskLogger()
{
  std::ofstream file;

  file.open(output);
  file << "exit-code: " << status << "\n";
  file << "--\n";
  file << stream.str();
  file.close();
}

std::string SidetaskLogger::read_logs(const std::filesystem::path& path, Data params)
{
  std::ifstream file(path.string());
  std::string line;
  std::string output;
  bool vars_parsed = false;

  while (std::getline(file, line))
  {
    if (line.find("--") == 0)
      vars_parsed = true;
    else if (vars_parsed)
      output += line + '\n';
    else
    {
      auto colon = line.find(':');
      std::string key = line.substr(0, colon);
      std::string value = line.substr(colon + 2, -1);
      if (key.length() > 0)
        params[key] = value;
    }
  }
  return output;
}
