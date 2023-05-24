#include "rex_engine/cmdline.h"

#include "rex_std/vector.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/log.h"
#include "rex_engine/cmd_args.h"

namespace rex
{
  namespace cmdline
  {
    struct ActiveArgument
    {
      rsl::string_view argument;
      rsl::string_view value;
    };

    class CommandLineArguments
    {
    public:
      explicit CommandLineArguments(rsl::string_view cmdLine)
      {
        // verify the auto generated command line arguments
        verify_args(g_command_line_args.data(), g_command_line_args.size());

        if (cmdLine.empty())
        {
          REX_LOG(LogEngine, "No command line arguments to parse");
          return;
        }

        parse_cmd_line(rsl::string_view(cmdLine));
      }

      rsl::optional<rsl::string_view> get_argument(rsl::string_view arg)
      {
        rsl::medium_stack_string arg_lower(arg);
        arg_lower.lower();
        for (ActiveArgument active_arg : m_arguments)
        {
          rsl::medium_stack_string str(active_arg.argument);
          str.lower();

          if (str == arg_lower)
          {
            return active_arg.value;
          }
        }

        return rsl::nullopt;
      }

    private:
      void parse_cmd_line(rsl::string_view cmdLine)
      {
        // all we have to do is split the command line based with spaces
        // there's no need to perform any heap allocation for that.
        count_t start_pos = cmdLine.find_first_not_of(' ', 0); // it's possible, although unlikely, that we start with a space

        // all executables should be called with at least 1 argument
        // the first argument should always be the path to the executable itself
        REX_ASSERT_X(start_pos != -1, "No arguments given to your exe, there should always be at least 1 arg.");

        // the module we're launching is always the first argument on the commandline
        // we skip this
        count_t space_pos = cmdLine.find_first_of(' ', start_pos);
        start_pos = space_pos + 1;

        const rsl::string_view arg_prefix = "-"; // all arguments should start with a '-'
        while (start_pos != -1 && space_pos != -1)
        {
          const count_t length = space_pos - start_pos;
          add_argument(cmdLine.substr(start_pos + arg_prefix.size(), length));
          start_pos = cmdLine.find_first_not_of(' ', space_pos); // skip all additional spaces
          space_pos = cmdLine.find_first_of(' ', start_pos);
        }

        if (start_pos != -1)
        {
          add_argument(cmdLine.substr(start_pos + arg_prefix.size())); // + 1 to ignore '-'
        }
      }

      void add_argument(rsl::string_view arg)
      {
        count_t equal_pos = arg.find('=');
        rsl::string_view key = ""; 
        rsl::string_view value = ""; 
        
        // if the argument is of type -MyArg=Something
        if (equal_pos != -1)
        {
          key = arg.substr(0, equal_pos);
          value = arg.substr(equal_pos + 1);
        }
        // if the argument is of type -EnableSomething
        else
        {
          key = arg;
          value = "1"; // this is so we can easily convert to bool/int
        }

        auto cmd_it = rsl::find_if(g_command_line_args.cbegin(), g_command_line_args.cend(),
          [key](const CommandLineArgument& cmdArg)
          {
            return key == cmdArg.name;
          });

        if (cmd_it == g_command_line_args.cend())
        {
            REX_WARN(LogEngine, "Command {} passed in but it's not recognised as a valid command so will be ignored", arg);
            return;
        }
         
        m_arguments.push_back({key, value});
      }

      void verify_args(const CommandLineArgument* args, count_t arg_count)
      {
        for (count_t i = 0; i < arg_count; ++i)
        {
          const CommandLineArgument& lhs_arg = args[i];
          for (count_t j = 0; j < arg_count; ++j)
          {
            if (i == j)
              continue;

            const CommandLineArgument& rhs_arg = args[j];
            REX_ASSERT_X(lhs_arg.name != rhs_arg.name, "Command line arg {} exists in both engine args and project args, this is not allowed", lhs_arg.name);
          }
        }
      }

    private:
      rsl::vector<ActiveArgument> m_arguments;
    };

    rsl::unique_ptr<CommandLineArguments> g_cmd_line_args;

    void init(rsl::string_view cmdLine)
    {
      g_cmd_line_args = rsl::make_unique<CommandLineArguments>(cmdLine);
    }

    rsl::optional<rsl::string_view> get_argument(rsl::string_view arg)
    {
      return g_cmd_line_args->get_argument(arg);
    }
  }
}