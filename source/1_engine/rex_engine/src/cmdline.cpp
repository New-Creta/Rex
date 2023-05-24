#include "rex_engine/cmdline.h"

#include "rex_std/vector.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/log.h"

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
      explicit CommandLineArguments()
      {
      }

      void register_commands(CommandLineArgumentsView args)
      {
        verify_args(args);
      }

      void init(rsl::string_view cmdLine)
      {
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

        auto engine_args_begin = m_engine_args.args;
        auto engine_args_end = m_engine_args.args + m_engine_args.count;

        auto project_args_begin = m_project_args.args;
        auto project_args_end = m_project_args.args + m_project_args.count;

        auto engine_it = rsl::find_if(engine_args_begin, engine_args_end,
          [key](const CommandLineArgument& cmdArg)
          {
            return key == cmdArg.name;
          });

        auto project_it = project_args_end;
        if (engine_it == engine_args_end)
        {
          project_it = rsl::find_if(project_args_begin, project_args_end,
            [key](const CommandLineArgument& cmdArg)
            {
              return key == cmdArg.name;
            });
        }

        if (engine_it == engine_args_end && project_it == project_args_end)
        {
            REX_WARN(LogEngine, "Command {} passed in but it's not recognised as a valid command so will be ignored", arg);
            return;
        }
         
        m_arguments.push_back({key, value});
      }

      void verify_args(CommandLineArgumentsView args)
      {
        for (count_t i = 0; i < engineArgs.count; ++i)
        {
          for (count_t j = 0; j < projectArgs.count; ++j)
          {
            REX_ASSERT_X(engineArgs.args[i].name != projectArgs.args[j].name, "Command line arg {} exists in both engine args and project args, this is not allowed", engineArgs.args[i].name)
          }
        }
      }

    private:
      rsl::vector<ActiveArgument> m_arguments;
      CommandLineArgumentsView m_engine_args;
      CommandLineArgumentsView m_project_args;
    };

    CommandLineArguments g_cmd_line_args;

    void register_commands(CommandLineArgumentsView args)
    {

    }

    void init(rsl::string_view cmdLine, CommandLineArgumentsView engineArgs, CommandLineArgumentsView projectArgs)
    {
      g_cmd_line_args = rsl::make_unique<CommandLineArguments>(cmdLine, engineArgs, projectArgs);
    }

    rsl::optional<rsl::string_view> get_argument(rsl::string_view arg)
    {
      return g_cmd_line_args->get_argument(arg);
    }
  }
}