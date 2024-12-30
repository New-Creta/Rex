#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/text_processing/text_processing.h"

TEST_CASE("TEST - Text Processing - strip - whitespace")
{
  // nothing
  {
    rsl::string_view string = "something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  // spaces
  {
    rsl::string_view string = "  something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  {
    rsl::string_view string = "something  ";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  {
    rsl::string_view string = "  something  ";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  {
    rsl::string_view string = "    ";
    rsl::string_view expected = "";

     REX_CHECK(rex::strip(string) == expected);
  }

  // tabs
  {
    rsl::string_view string = "something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  {
    rsl::string_view string = "\t\tsomething";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  {
    rsl::string_view string = "something\t\t";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  {
    rsl::string_view string = "\t\tsomething\t\t";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  {
    rsl::string_view string = "\t\t\t\t";
    rsl::string_view expected = "";

    REX_CHECK(rex::strip(string) == expected);
  }

  // spaces and tabs
  {
    rsl::string_view string = "something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  {
    rsl::string_view string = "\t\t  something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  {
    rsl::string_view string = "something\t\t  ";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  {
    rsl::string_view string = "\t\t  something\t\t  ";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  {
    rsl::string_view string = "\t\t    \t\t";
    rsl::string_view expected = "";

    REX_CHECK(rex::strip(string) == expected);
  }
}

TEST_CASE("TEST - Text Processing - lstrip - whitespace")
{
  // nothing
  {
    rsl::string_view string = "something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  // spaces
  {
    rsl::string_view string = "  something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  {
    rsl::string_view string = "something  ";
    rsl::string_view expected = "something  ";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  {
    rsl::string_view string = "  something  ";
    rsl::string_view expected = "something  ";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  {
    rsl::string_view string = "    ";
    rsl::string_view expected = "";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  // tabs
  {
    rsl::string_view string = "\t\tsomething";
    rsl::string_view expected = "something";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  {
    rsl::string_view string = "something\t\t";
    rsl::string_view expected = "something\t\t";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  {
    rsl::string_view string = "\t\tsomething\t\t";
    rsl::string_view expected = "something\t\t";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  {
    rsl::string_view string = "\t\t\t\t";
    rsl::string_view expected = "";

    REX_CHECK(rex::lstrip(string) == expected);
  }
  
  // spaces and tabs
  {
    rsl::string_view string = "\t\t  something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  {
    rsl::string_view string = "something\t\t  ";
    rsl::string_view expected = "something\t\t  ";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  {
    rsl::string_view string = "\t\t  something\t\t  ";
    rsl::string_view expected = "something\t\t  ";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  {
    rsl::string_view string = "\t\t    \t\t";
    rsl::string_view expected = "";

    REX_CHECK(rex::lstrip(string) == expected);
  }
}

TEST_CASE("TEST - Text Processing - rstrip - whitespace")
{
  // nothing
  {
    rsl::string_view string = "something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  // spaces
  {
    rsl::string_view string = "  something";
    rsl::string_view expected = "  something";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  {
    rsl::string_view string = "something  ";
    rsl::string_view expected = "something";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  {
    rsl::string_view string = "  something  ";
    rsl::string_view expected = "  something";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  {
    rsl::string_view string = "    ";
    rsl::string_view expected = "";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  // tabs
  {
    rsl::string_view string = "\t\tsomething";
    rsl::string_view expected = "\t\tsomething";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  {
    rsl::string_view string = "something\t\t";
    rsl::string_view expected = "something";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  {
    rsl::string_view string = "\t\tsomething\t\t";
    rsl::string_view expected = "\t\tsomething";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  {
    rsl::string_view string = "\t\t\t\t";
    rsl::string_view expected = "";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  // spaces and tabs
  {
    rsl::string_view string = "\t\t  something";
    rsl::string_view expected = "\t\t  something";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  {
    rsl::string_view string = "something\t\t  ";
    rsl::string_view expected = "something";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  {
    rsl::string_view string = "\t\t  something\t\t  ";
    rsl::string_view expected = "\t\t  something";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  {
    rsl::string_view string = "\t\t    \t\t";
    rsl::string_view expected = "";

    REX_CHECK(rex::rstrip(string) == expected);
  }
}

TEST_CASE("TEST - Text Processing - strip - characters")
{
  // nothing
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string) == expected);
  }

  // strip left
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111  something";
    rsl::string_view expected = "  something";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012  something";
    rsl::string_view expected = "  something";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111something  ";
    rsl::string_view expected = "something  ";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012something  ";
    rsl::string_view expected = "something  ";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  // strip right
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something  111";
    rsl::string_view expected = "something  ";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something  012";
    rsl::string_view expected = "something  ";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something111";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something012";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "  something111";
    rsl::string_view expected = "  something";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "  something012";
    rsl::string_view expected = "  something";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  // strip left and right
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111  something  111";
    rsl::string_view expected = "  something  ";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012  something  012";
    rsl::string_view expected = "  something  ";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111something111";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012something012";
    rsl::string_view expected = "something";

    REX_CHECK(rex::strip(string, characters) == expected);
  }
}

TEST_CASE("TEST - Text Processing - lstrip - characters")
{
  // nothing
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::lstrip(string) == expected);
  }

  // strip left
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111  something";
    rsl::string_view expected = "  something";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012  something";
    rsl::string_view expected = "  something";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111something  ";
    rsl::string_view expected = "something  ";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012something  ";
    rsl::string_view expected = "something  ";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  // strip right
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something  111";
    rsl::string_view expected = "something  111";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something  012";
    rsl::string_view expected = "something  012";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something111";
    rsl::string_view expected = "something111";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something012";
    rsl::string_view expected = "something012";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "  something111";
    rsl::string_view expected = "  something111";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "  something012";
    rsl::string_view expected = "  something012";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  // strip left and right
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111  something  111";
    rsl::string_view expected = "  something  111";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012  something  012";
    rsl::string_view expected = "  something  012";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111something111";
    rsl::string_view expected = "something111";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012something012";
    rsl::string_view expected = "something012";

    REX_CHECK(rex::lstrip(string, characters) == expected);
  }
}

TEST_CASE("TEST - Text Processing - rstrip - characters")
{
  // nothing
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::rstrip(string) == expected);
  }

  // strip left
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111  something";
    rsl::string_view expected = "111  something";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012  something";
    rsl::string_view expected = "012  something";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111something";
    rsl::string_view expected = "111something";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012something";
    rsl::string_view expected = "012something";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111something  ";
    rsl::string_view expected = "111something  ";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012something  ";
    rsl::string_view expected = "012something  ";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  // strip right
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something  111";
    rsl::string_view expected = "something  ";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something  012";
    rsl::string_view expected = "something  ";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something111";
    rsl::string_view expected = "something";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "something012";
    rsl::string_view expected = "something";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "  something111";
    rsl::string_view expected = "  something";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "  something012";
    rsl::string_view expected = "  something";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  // strip left and right
  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111  something  111";
    rsl::string_view expected = "111  something  ";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012  something  012";
    rsl::string_view expected = "012  something  ";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "111something111";
    rsl::string_view expected = "111something";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }

  {
    rsl::string_view characters = "012";
    rsl::string_view string = "012something012";
    rsl::string_view expected = "012something";

    REX_CHECK(rex::rstrip(string, characters) == expected);
  }
}

TEST_CASE("TEST - Text Processing - quoted")
{
  {
    rsl::string_view string = "something";
    rsl::string_view expected = "\"something\"";

    REX_CHECK(rex::quoted(string) == expected);
  }

  {
    rsl::string_view string = "\"something";
    rsl::string_view expected = "\"\"something\"";

    REX_CHECK(rex::quoted(string) == expected);
  }

  {
    rsl::string_view string = "something\"";
    rsl::string_view expected = "\"something\"\"";

    REX_CHECK(rex::quoted(string) == expected);
  }

  {
    rsl::string_view string = "something";
    rsl::string_view expected = "\"something\"";

    REX_CHECK(rex::quoted(string) == expected);
  }
}

TEST_CASE("TEST - Text Processing - remove spaces")
{
  {
    rsl::string string("something else");
    rsl::string_view expected = "somethingelse";

    REX_CHECK(rex::remove_spaces(string) == expected);
  }

  {
    rsl::string string("something");
    rsl::string_view expected = "something";

    REX_CHECK(rex::remove_spaces(string) == expected);
  }

  {
    rsl::string string("something  ");
    rsl::string_view expected = "something";

    REX_CHECK(rex::remove_spaces(string) == expected);
  }

  {
    rsl::string string("  something");
    rsl::string_view expected = "something";

    REX_CHECK(rex::remove_spaces(string) == expected);
  }

  {
    rsl::string string("  something  ");
    rsl::string_view expected = "something";

    REX_CHECK(rex::remove_spaces(string) == expected);
  }
}

TEST_CASE("TEST - Text Processing - remove quotes")
{
  {
    rsl::string_view string = "something";
    rsl::string_view expected = "something";

    REX_CHECK(rex::remove_quotes(string) == expected);
  }

  {
    rsl::string_view string = "\"something";
    rsl::string_view expected = "\"something";

    REX_CHECK(rex::remove_quotes(string) == expected);
  }

  {
    rsl::string_view string = "something\"";
    rsl::string_view expected = "something\"";

    REX_CHECK(rex::remove_quotes(string) == expected);
  }

  {
    rsl::string_view string = "\"something\"";
    rsl::string_view expected = "something";

    REX_CHECK(rex::remove_quotes(string) == expected);
  }
}

TEST_CASE("TEST - Text Processing - to multibyte")
{
  const tchar wide_str[] = L"This is a wide string";
  REX_CHECK(rex::to_multibyte(wide_str, sizeof(wide_str) / 2) == rsl::string("This is a wide string"));
  REX_CHECK(rex::to_multibyte(L"", 0) == rsl::string(""));
}

TEST_CASE("TEST - Text Processing - is null terminated")
{
  rsl::string_view str = "this is a normal string";
  rsl::string_view substr = str.substr(0, 5);

  REX_CHECK(rex::is_null_terminated(str) == true);
  REX_CHECK(rex::is_null_terminated("") == true);
  REX_CHECK(rex::is_null_terminated(substr) == false);
  REX_CHECK(rex::is_null_terminated(rsl::string_view()) == false);
}

TEST_CASE("TEST - Text Processing - day nr to name")
{
  REX_CHECK(rex::day_nr_to_name(1) == "Mon");
  REX_CHECK(rex::day_nr_to_name(2) == "Tue");
  REX_CHECK(rex::day_nr_to_name(3) == "Wed");
  REX_CHECK(rex::day_nr_to_name(4) == "Thu");
  REX_CHECK(rex::day_nr_to_name(5) == "Fri");
  REX_CHECK(rex::day_nr_to_name(6) == "Sat");
  REX_CHECK(rex::day_nr_to_name(7) == "Sun");

  REX_CHECK(rex::day_nr_to_name(-1) == "Invalid input");
  REX_CHECK(rex::day_nr_to_name(0) == "Invalid input");
  REX_CHECK(rex::day_nr_to_name(9) == "Invalid input");
}

TEST_CASE("TEST - Text Processing - day nr to full name")
{
  REX_CHECK(rex::day_nr_to_full_name(1) == "Monday");
  REX_CHECK(rex::day_nr_to_full_name(2) == "Tuesday");
  REX_CHECK(rex::day_nr_to_full_name(3) == "Wednesday");
  REX_CHECK(rex::day_nr_to_full_name(4) == "Thursday");
  REX_CHECK(rex::day_nr_to_full_name(5) == "Friday");
  REX_CHECK(rex::day_nr_to_full_name(6) == "Saturday");
  REX_CHECK(rex::day_nr_to_full_name(7) == "Sunday");

  REX_CHECK(rex::day_nr_to_full_name(-1) == "Invalid input");
  REX_CHECK(rex::day_nr_to_full_name(0) == "Invalid input");
  REX_CHECK(rex::day_nr_to_full_name(9) == "Invalid input");
}

TEST_CASE("TEST - Text Processing - day name to nr")
{
  REX_CHECK(rex::day_name_to_nr("monday") == 1);
  REX_CHECK(rex::day_name_to_nr("tuesday") == 2);
  REX_CHECK(rex::day_name_to_nr("wednesday") == 3);
  REX_CHECK(rex::day_name_to_nr("thursday") == 4);
  REX_CHECK(rex::day_name_to_nr("friday") == 5);
  REX_CHECK(rex::day_name_to_nr("saturday") == 6);
  REX_CHECK(rex::day_name_to_nr("sunday") == 7);

  REX_CHECK(rex::day_name_to_nr("mon") == 1);
  REX_CHECK(rex::day_name_to_nr("tue") == 2);
  REX_CHECK(rex::day_name_to_nr("wed") == 3);
  REX_CHECK(rex::day_name_to_nr("thu") == 4);
  REX_CHECK(rex::day_name_to_nr("fri") == 5);
  REX_CHECK(rex::day_name_to_nr("sat") == 6);
  REX_CHECK(rex::day_name_to_nr("sun") == 7);

  REX_CHECK(rex::day_name_to_nr("MONDAY") == 1);
  REX_CHECK(rex::day_name_to_nr("TUESDAY") == 2);
  REX_CHECK(rex::day_name_to_nr("WEDNESDAY") == 3);
  REX_CHECK(rex::day_name_to_nr("THURSDAY") == 4);
  REX_CHECK(rex::day_name_to_nr("FRIDAY") == 5);
  REX_CHECK(rex::day_name_to_nr("SATURDAY") == 6);
  REX_CHECK(rex::day_name_to_nr("SUNDAY") == 7);

  REX_CHECK(rex::day_name_to_nr("MON") == 1);
  REX_CHECK(rex::day_name_to_nr("TUE") == 2);
  REX_CHECK(rex::day_name_to_nr("WED") == 3);
  REX_CHECK(rex::day_name_to_nr("THU") == 4);
  REX_CHECK(rex::day_name_to_nr("FRI") == 5);
  REX_CHECK(rex::day_name_to_nr("SAT") == 6);
  REX_CHECK(rex::day_name_to_nr("SUN") == 7);

  // Invalid input
  REX_CHECK(rex::day_name_to_nr("giberish") == -1);
}

TEST_CASE("TEST - Text Processing - month nr to name")
{
  REX_CHECK(rex::month_nr_to_name(1) == "Jan");
  REX_CHECK(rex::month_nr_to_name(2) == "Feb");
  REX_CHECK(rex::month_nr_to_name(3) == "Mar");
  REX_CHECK(rex::month_nr_to_name(4) == "Apr");
  REX_CHECK(rex::month_nr_to_name(5) == "May");
  REX_CHECK(rex::month_nr_to_name(6) == "Jun");
  REX_CHECK(rex::month_nr_to_name(7) == "Jul");
  REX_CHECK(rex::month_nr_to_name(8) == "Aug");
  REX_CHECK(rex::month_nr_to_name(9) == "Sep");
  REX_CHECK(rex::month_nr_to_name(10) == "Oct");
  REX_CHECK(rex::month_nr_to_name(11) == "Nov");
  REX_CHECK(rex::month_nr_to_name(12) == "Dec");

  // invalid input
  REX_CHECK(rex::month_nr_to_name(-1) == "Invalid input");
  REX_CHECK(rex::month_nr_to_name(0) == "Invalid input");
  REX_CHECK(rex::month_nr_to_name(13) == "Invalid input");
}

TEST_CASE("TEST - Text Processing - month nr to full name")
{
  REX_CHECK(rex::month_nr_to_full_name(1) == "January");
  REX_CHECK(rex::month_nr_to_full_name(2) == "February");
  REX_CHECK(rex::month_nr_to_full_name(3) == "March");
  REX_CHECK(rex::month_nr_to_full_name(4) == "April");
  REX_CHECK(rex::month_nr_to_full_name(5) == "May");
  REX_CHECK(rex::month_nr_to_full_name(6) == "June");
  REX_CHECK(rex::month_nr_to_full_name(7) == "July");
  REX_CHECK(rex::month_nr_to_full_name(8) == "August");
  REX_CHECK(rex::month_nr_to_full_name(9) == "September");
  REX_CHECK(rex::month_nr_to_full_name(10) == "October");
  REX_CHECK(rex::month_nr_to_full_name(11) == "November");
  REX_CHECK(rex::month_nr_to_full_name(12) == "December");

  // invalid input
  REX_CHECK(rex::month_nr_to_full_name(-1) == "Invalid input");
  REX_CHECK(rex::month_nr_to_full_name(0) == "Invalid input");
  REX_CHECK(rex::month_nr_to_full_name(13) == "Invalid input");
}

TEST_CASE("TEST - Text Processing - month name to nr")
{
  REX_CHECK(rex::month_name_to_nr("january") == 1);
  REX_CHECK(rex::month_name_to_nr("february") == 2);
  REX_CHECK(rex::month_name_to_nr("march") == 3);
  REX_CHECK(rex::month_name_to_nr("april") == 4);
  REX_CHECK(rex::month_name_to_nr("may") == 5);
  REX_CHECK(rex::month_name_to_nr("june") == 6);
  REX_CHECK(rex::month_name_to_nr("july") == 7);
  REX_CHECK(rex::month_name_to_nr("august") == 8);
  REX_CHECK(rex::month_name_to_nr("september") == 9);
  REX_CHECK(rex::month_name_to_nr("october") == 10);
  REX_CHECK(rex::month_name_to_nr("november") == 11);
  REX_CHECK(rex::month_name_to_nr("december") == 12);

  REX_CHECK(rex::month_name_to_nr("jan") == 1);
  REX_CHECK(rex::month_name_to_nr("feb") == 2);
  REX_CHECK(rex::month_name_to_nr("mar") == 3);
  REX_CHECK(rex::month_name_to_nr("apr") == 4);
  REX_CHECK(rex::month_name_to_nr("may") == 5);
  REX_CHECK(rex::month_name_to_nr("jun") == 6);
  REX_CHECK(rex::month_name_to_nr("jul") == 7);
  REX_CHECK(rex::month_name_to_nr("aug") == 8);
  REX_CHECK(rex::month_name_to_nr("sep") == 9);
  REX_CHECK(rex::month_name_to_nr("oct") == 10);
  REX_CHECK(rex::month_name_to_nr("nov") == 11);
  REX_CHECK(rex::month_name_to_nr("dec") == 12);

  REX_CHECK(rex::month_name_to_nr("JANUARY") == 1);
  REX_CHECK(rex::month_name_to_nr("FEBRUARY") == 2);
  REX_CHECK(rex::month_name_to_nr("MARCH") == 3);
  REX_CHECK(rex::month_name_to_nr("APRIL") == 4);
  REX_CHECK(rex::month_name_to_nr("MAY") == 5);
  REX_CHECK(rex::month_name_to_nr("JUNE") == 6);
  REX_CHECK(rex::month_name_to_nr("JULY") == 7);
  REX_CHECK(rex::month_name_to_nr("AUGUST") == 8);
  REX_CHECK(rex::month_name_to_nr("SEPTEMBER") == 9);
  REX_CHECK(rex::month_name_to_nr("OCTOBER") == 10);
  REX_CHECK(rex::month_name_to_nr("NOVEMBER") == 11);
  REX_CHECK(rex::month_name_to_nr("DECEMBER") == 12);

  REX_CHECK(rex::month_name_to_nr("JAN") == 1);
  REX_CHECK(rex::month_name_to_nr("FEB") == 2);
  REX_CHECK(rex::month_name_to_nr("MAR") == 3);
  REX_CHECK(rex::month_name_to_nr("APR") == 4);
  REX_CHECK(rex::month_name_to_nr("MAY") == 5);
  REX_CHECK(rex::month_name_to_nr("JUN") == 6);
  REX_CHECK(rex::month_name_to_nr("JUL") == 7);
  REX_CHECK(rex::month_name_to_nr("AUG") == 8);
  REX_CHECK(rex::month_name_to_nr("SEP") == 9);
  REX_CHECK(rex::month_name_to_nr("OCT") == 10);
  REX_CHECK(rex::month_name_to_nr("NOV") == 11);
  REX_CHECK(rex::month_name_to_nr("DEC") == 12);

  // Invalid input
  REX_CHECK(rex::month_name_to_nr("giberish") == -1);
}