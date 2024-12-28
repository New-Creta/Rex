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

}

TEST_CASE("TEST - Text Processing - is null terminated")
{

}

TEST_CASE("TEST - Text Processing - day nr to name")
{

}

TEST_CASE("TEST - Text Processing - day nr to full name")
{

}

TEST_CASE("TEST - Text Processing - day name to nr")
{

}

TEST_CASE("TEST - Text Processing - month nr to name")
{

}

TEST_CASE("TEST - Text Processing - month nr to full name")
{

}

TEST_CASE("TEST - Text Processing - month name to nr")
{

}