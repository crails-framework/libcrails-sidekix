#ifndef  SIDEKIX_SIDETASKS_HPP
# define SIDEKIX_SIDETASKS_HPP

# include <map>
# include <functional>
# include <crails/params.hpp>

namespace Sidekix
{
  typedef std::map<std::string, std::function<void(Crails::Params&)>> Sidetasks;
}

#endif
