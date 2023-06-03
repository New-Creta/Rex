#include "rex_engine/cmdline.h"

#include "rex_engine/cmd_args.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/log.h"
#include "rex_std/cstring.h"
#include "rex_std/vector.h"

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
        REX_ASSERT_X(verify_args(g_command_line_args.data(), g_command_line_args.size()), "You have ambuguous command line arguments");

        if(cmdLine.empty())
        {
          REX_LOG(LogEngine, "No command line arguments to parse");
          return;
        }

        parse_cmd_line(rsl::string_view(cmdLine));
      }

      rsl::optional<rsl::string_view> get_argument(rsl::string_view arg)
      {
        for(ActiveArgument active_arg: m_arguments)
        {
          // early optimization that strincmp can't do
          if(arg.length() != active_arg.argument.length())
            continue;

          if(rsl::strincmp(arg.data(), active_arg.argument.data(), arg.length()) == 0)
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

        // if we have no arguments, we just return
        // this is possible when running without a debugger
        if (space_pos == -1)
        {
          return;
        }

        start_pos         = space_pos + 1;

        // get the new space pos
        space_pos = cmdLine.find_first_of(' ', start_pos);

        const rsl::string_view arg_prefix = "-"; // all arguments should start with a '-'
        while(start_pos != -1 && space_pos != -1)
        {
          const count_t length            = space_pos - start_pos - arg_prefix.size();
          const rsl::string_view argument = cmdLine.substr(start_pos + arg_prefix.size(), length);
          add_argument(argument);
          start_pos = cmdLine.find_first_not_of(' ', space_pos); // skip all additional spaces
          space_pos = cmdLine.find_first_of(' ', start_pos);
        }

        if(start_pos != -1)
        {
          add_argument(cmdLine.substr(start_pos + arg_prefix.size())); // + 1 to ignore '-'
        }
      }

      void add_argument(rsl::string_view arg)
      {
        const count_t equal_pos = arg.find('=');
        rsl::string_view key    = "";
        rsl::string_view value  = "";

        // if the argument is of type -MyArg=Something
        if(equal_pos != -1)
        {
          key   = arg.substr(0, equal_pos);
          value = arg.substr(equal_pos + 1);
        }
        // if the argument is of type -EnableSomething
        else
        {
          key   = arg;
          value = "1"; // this is so we can easily convert to bool/int
        }

        auto cmd_it = rsl::find_if(g_command_line_args.cbegin(), g_command_line_args.cend(), [key](const Argument& cmdArg) { return key == cmdArg.name; });

        if(cmd_it == g_command_line_args.cend())
        {
          REX_WARN(LogEngine, "Command {} passed in but it's not recognised as a valid command so will be ignored", arg);
          return;
        }

        auto active_it = rsl::find_if(m_arguments.cbegin(), m_arguments.cend(), [key](const ActiveArgument& activeArg) { return activeArg.argument == key; });

        if(active_it != m_arguments.cend())
        {
          REX_WARN(LogEngine, "Command {} was already passed in. passing the same argument multiple times is not supported. will be skipped", key);
          return;
        }

        m_arguments.push_back({key, value});
      }

      bool verify_args(const Argument* args, count_t argCount) // NOLINT(readability-convert-member-functions-to-static)
      {
        for(count_t i = 0; i < argCount; ++i)
        {
          const Argument& lhs_arg = args[i];
          for(count_t j = 0; j < argCount; ++j)
          {
            if(i == j)
              continue;

            const Argument& rhs_arg = args[j];
            if(rsl::strincmp(lhs_arg.name.data(), rhs_arg.name.data(), lhs_arg.name.length()) == 0)
            {
              REX_ERROR(LogEngine, "This executable already has an argument for {} specified in 'g_command_line_args', please resolve the ambiguity", lhs_arg.name);
              return false;
            }
          }
        }

        return true;
      }

    private:
      rsl::vector<ActiveArgument> m_arguments;
    };

    rsl::unique_ptr<CommandLineArguments> g_cmd_line_args; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

    void init(rsl::string_view cmdLine)
    {
      g_cmd_line_args = rsl::make_unique<CommandLineArguments>(cmdLine);
    }

    void print_args()
    {
      REX_LOG(LogEngine, "Listing Command line arguments");

      rsl::unordered_map<rsl::string_view, rsl::vector<Argument>> project_to_arguments;

      for(Argument cmd: g_command_line_args)
      {
        project_to_arguments[cmd.module].push_back(cmd);
      }

      for(auto [project, cmds]: project_to_arguments)
      {
        REX_LOG(LogEngine, "");
        REX_LOG(LogEngine, "Commandline Arguments For {}", project);
        REX_LOG(LogEngine, "------------------------------");

        for(Argument cmd: cmds)
        {
          REX_LOG(LogEngine, "\"{}\" - {}", cmd.name, cmd.desc);
        }
      }
    }

    // we need to manually shut this down, otherwise it's memory might get tracked
    // by an already deleted memory tracking, causing an assert on shutdown
    void shutdown()
    {
      g_cmd_line_args.reset();
    }

    rsl::optional<rsl::string_view> get_argument(rsl::string_view arg)
    {
      return g_cmd_line_args->get_argument(arg);
    }

  } // namespace cmdline
} // namespace rex