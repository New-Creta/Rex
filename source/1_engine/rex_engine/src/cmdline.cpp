#include "rex_engine/cmdline/cmdline.h"

#include "rex_engine/cmdline/cmd_args.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_std/bonus/hashtable.h"
#include "rex_std/bonus/types.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/cstring.h"
#include "rex_std/format.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

namespace rex
{
  namespace cmdline
  {
    namespace internal
    {
      StringID create_string_id_for_arg(rsl::wstring_view arg)
      {
        return StringID(rsl::wmedium_stack_string(arg).to_lower());
      }
    }

    Argument::Argument(rsl::wstring_view name, rsl::wstring_view desc, rsl::wstring_view module)
      : name_id(internal::create_string_id_for_arg(name))
      , name(name)
      , desc(desc)
      , module(module)
    {
      

    }

    struct ActiveArgument
    {
      StringID argument_id;
      rsl::wstring_view argument;
      rsl::wstring_view value;
    };

    class Processor
    {
    public:
      explicit Processor(rsl::wstring_view cmdLine)
      {
        // verify the auto generated command line arguments
        REX_ASSERT_X(verify_args(g_command_line_args.data(), g_command_line_args.size()), "You have ambiguous command line arguments");

        if(cmdLine.empty())
        {
          REX_LOG(LogEngine, "No command line arguments to parse");
          return;
        }

        parse_cmd_line(rsl::wstring_view(cmdLine));
      }

      rsl::optional<rsl::wstring_view> get_argument(rsl::wstring_view arg)
      {
        StringID arg_id = internal::create_string_id_for_arg(arg);

        for(const ActiveArgument& active_arg : m_arguments)
        {
          if (arg_id == active_arg.argument_id)
          {
            return active_arg.value;
          }
        }

        return rsl::nullopt;
      }

    private:
      void parse_cmd_line(rsl::wstring_view cmdLine)
      {
        // all we have to do is split the command line based with spaces
        // there's no need to perform any heap allocation for that.
        card32 start_pos = cmdLine.find_first_not_of(' ', 0); // it's possible, although unlikely, that we start with a space

        // all executables should be called with at least 1 argument
        // the first argument should always be the path to the executable itself
        REX_ASSERT_X(start_pos != -1, "No arguments given to your exe, there should always be at least 1 arg.");

        // the module we're launching is always the first argument on the commandline
        // we skip this
        const card32 space_pos = cmdLine.find_first_of(' ', start_pos);

        // if we have no arguments, we just return
        // this is possible when running without a debugger
        if(space_pos == -1)
        {
          return;
        }

        // skip all additional spaces
        start_pos = cmdLine.find_first_not_of(' ', space_pos);

        // Here the actual command line parsing begins
        // It gets a bit tricky but here are some examples of command lines we all support
        // 1. -something -else
        // 2. -something=1 -else
        // 3. -something="path/to/something" -else
        // 4. -something="this has spaces" -else
        // So command line arguments are always split between a key and a possible value
        // Arguments always start with a '-', which is the start token of every key.
        // A key argument ends at the next space or '=' token, whichever comes first.
        // A value argument starts after the '=' sign, or the '"" that comes after the '=' token
        // a value arguments ends at the first space or at '"' if it started with one.
        REX_LOG(LogEngine, "CmdLine: {}", rsl::to_string(cmdLine));

        const rsl::wstring_view arg_prefix = L"-"; // all arguments should start with a '-'
        while(start_pos != -1)
        {
          const rsl::wstring_view full_argument = find_next_full_argument(cmdLine, start_pos);
          if(REX_ERROR_X(LogEngine, full_argument.starts_with(arg_prefix), "argument '{}' doesn't start with '{}'. all arguments should start with '{}'", rsl::to_string(full_argument), rsl::to_string(arg_prefix), rsl::to_string(arg_prefix)) ==
             false) // NOLINT(readability-simplify-boolean-expr, readability-implicit-bool-conversion)
          {
            const rsl::wstring_view argument = full_argument.substr(arg_prefix.size());
            add_argument(argument);
          }

          start_pos = cmdLine.find_first_not_of(L" \"", start_pos + full_argument.length()); // skip all additional spaces
        }

        if(start_pos != -1)
        {
          const rsl::wstring_view full_argument = find_next_full_argument(cmdLine, start_pos);
          if(REX_ERROR_X(LogEngine, full_argument.starts_with(arg_prefix), "argument '{}' doesn't start with '{}'. all arguments should start with '{}'", rsl::to_string(full_argument), rsl::to_string(arg_prefix), rsl::to_string(arg_prefix)) ==
             false) // NOLINT(readability-simplify-boolean-expr, readability-implicit-bool-conversion)
          {
            const rsl::wstring_view argument = full_argument.substr(arg_prefix.size());
            add_argument(argument);
          }
        }
      }

      void add_argument(rsl::wstring_view arg)
      {
        const card32 equal_pos = arg.find('=');
        rsl::wstring_view key    = L"";
        rsl::wstring_view value  = L"";

        // if the argument is of type -MyArg=Something
        if(equal_pos != -1)
        {
          key   = arg.substr(0, equal_pos);
          value = arg.substr(equal_pos + 1);

          // Remove the quote at the beginning if it starts with one
          if(value.starts_with('"'))
          {
            value = value.substr(1);
          }
        }
        // if the argument is of type -EnableSomething
        else
        {
          key   = arg;
          value = L"1"; // this is so we can easily convert to bool/int
        }

        StringID key_id = internal::create_string_id_for_arg(key);
        auto cmd_it = rsl::find_if(g_command_line_args.cbegin(), g_command_line_args.cend(), [key_id](const Argument& cmdArg) { return key_id == cmdArg.name_id; });

        if(cmd_it == g_command_line_args.cend())
        {
          REX_WARN(LogEngine, "Command '{}' passed in but it's not recognised as a valid command so will be ignored", rsl::to_string(key));
          return;
        }

        auto active_it = rsl::find_if(m_arguments.cbegin(), m_arguments.cend(), [key](const ActiveArgument& activeArg) { return rsl::strincmp(key.data(), activeArg.argument.data(), key.length()) == 0; });

        if(active_it != m_arguments.cend())
        {
          REX_WARN(LogEngine, "Command '{}' was already passed in. passing the same argument multiple times is not supported. will be skipped", rsl::to_string(key));
          return;
        }

        m_arguments.push_back({key_id, key, value});
      }

      bool verify_args(const Argument* args, count_t argCount) // NOLINT(readability-convert-member-functions-to-static)
      {
        for(count_t i = 0; i < argCount; ++i)
        {
          const Argument& lhs_arg = args[i];
          for(count_t j = 0; j < argCount; ++j)
          {
            if(i == j)
            {
              continue;
            }

            const Argument& rhs_arg = args[j];
            if (lhs_arg.name_id == rhs_arg.name_id)
            {
              REX_ERROR(LogEngine, "This executable already has an argument for {} specified in 'g_command_line_args', please resolve the ambiguity by changing the code_generation file resulting in this ambiguity", rsl::to_string(lhs_arg.name));
              return false;
            }
          }
        }

        return true;
      }

      rsl::wstring_view find_next_full_argument(rsl::wstring_view cmdLine, count_t startPos) // NOLINT(readability-convert-member-functions-to-static)
      {
        const count_t space_pos = cmdLine.find_first_of(' ', startPos);
        count_t comma_pos       = cmdLine.find_first_of('"', startPos);
        count_t pos_to_use      = space_pos;
        if(comma_pos < space_pos && comma_pos != -1)
        {
          comma_pos  = cmdLine.find_first_of('"', comma_pos + 1);
          pos_to_use = comma_pos;
        }

        return pos_to_use != -1 ? cmdLine.substr(startPos, pos_to_use - startPos) : cmdLine.substr(startPos);
      }

    private:
      rsl::vector<ActiveArgument> m_arguments;
    };

    rsl::unique_ptr<cmdline::Processor> g_cmd_line_args; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)
    rsl::wstring_view g_cmd_line;                         // Saved as wstring_view, save as string if it's causing lifetime issues. // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

    void init(rsl::wstring_view cmdLine)
    {
      g_cmd_line      = cmdLine;
      g_cmd_line_args = rsl::make_unique<cmdline::Processor>(cmdLine);
    }

    void print_args()
    {
      REX_LOG(LogEngine, "Listing Command line arguments");

      rsl::unordered_map<rsl::wstring_view, rsl::vector<Argument>> project_to_arguments;

      for(const Argument& cmd: g_command_line_args)
      {
        project_to_arguments[cmd.module].push_back(cmd);
      }

      for(auto& [project, cmds]: project_to_arguments)
      {
        REX_LOG(LogEngine, "");
        REX_LOG(LogEngine, "Commandline Arguments For {}", rsl::to_string(project));
        REX_LOG(LogEngine, "------------------------------");

        for(const Argument& cmd: cmds)
        {
          REX_LOG(LogEngine, "\"{}\" - {}", rsl::to_string(cmd.name), rsl::to_string(cmd.desc));
        }
      }
    }

    // we need to manually shut this down, otherwise it's memory might get tracked
    // by an already deleted memory tracking, causing an assert on shutdown
    void shutdown()
    {
      g_cmd_line_args.reset();
    }

    rsl::optional<rsl::wstring_view> get_argument(rsl::wstring_view arg)
    {
      return g_cmd_line_args->get_argument(arg);
    }

    rsl::wstring_view get()
    {
      return g_cmd_line;
    }

  } // namespace cmdline
} // namespace rex