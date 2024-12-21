#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_std/chrono.h"
#include "rex_std/thread.h"
#include "rex_std/iostream.h"

#include <Windows.h>
#include <ShlObj.h>

TEST_CASE("Path Joining")
{
  REX_CHECK(rex::path::join("D:", "foo", "bar") == "D:/foo/bar");
  REX_CHECK(rex::path::join("foo", "bar") == "foo/bar");
  REX_CHECK(rex::path::join("foo", "bar.txt") == "foo/bar.txt");


}

TEST_CASE("Is Valid Path")
{
  // Valid ones
  REX_CHECK(rex::path::is_valid_path("") == true);
  REX_CHECK(rex::path::is_valid_path("/") == true);
  REX_CHECK(rex::path::is_valid_path("\\") == true);
  REX_CHECK(rex::path::is_valid_path("/file.txt") == true);
  REX_CHECK(rex::path::is_valid_path("/path") == true);
  REX_CHECK(rex::path::is_valid_path("/path/to/file.txt") == true);
  REX_CHECK(rex::path::is_valid_path("!.txt") == true);
  REX_CHECK(rex::path::is_valid_path("/path.to.file.txt") == true);
  REX_CHECK(rex::path::is_valid_path("path.to.file.txt") == true);
  REX_CHECK(rex::path::is_valid_path("d:/path.to.file.txt") == true);
  REX_CHECK(rex::path::is_valid_path("d:\\path/to.file.txt") == true);
  REX_CHECK(rex::path::is_valid_path("d:\\path/to file.txt") == true);

  // AI generated examples
  REX_CHECK(rex::path::is_valid_path("/home/user/documents/file.txt") == true);
  REX_CHECK(rex::path::is_valid_path("/etc/nginx/nginx.conf") == true);
  REX_CHECK(rex::path::is_valid_path("/var/log/system.log") == true);
  REX_CHECK(rex::path::is_valid_path("/usr/local/bin/script.sh") == true);
  REX_CHECK(rex::path::is_valid_path("/tmp/") == true);
  REX_CHECK(rex::path::is_valid_path("documents/file.txt") == true);
  REX_CHECK(rex::path::is_valid_path("../file.txt") == true);
  REX_CHECK(rex::path::is_valid_path("./script.sh") == true);
  REX_CHECK(rex::path::is_valid_path("some/folder/another_file") == true);
  REX_CHECK(rex::path::is_valid_path("C:\\Users\\Alice\\Documents\\file.txt") == true);
  REX_CHECK(rex::path::is_valid_path("D:\\Projects\\code\\main.py") == true);
  REX_CHECK(rex::path::is_valid_path("E:\\Music\\song.mp3") == true);
  REX_CHECK(rex::path::is_valid_path("\\\\NetworkShare\\Folder\\File.docx") == true);
  REX_CHECK(rex::path::is_valid_path("Documents\\file.txt") == true);
  REX_CHECK(rex::path::is_valid_path("..\\config.ini") == true);
  REX_CHECK(rex::path::is_valid_path(".\\run.bat") == true);
  REX_CHECK(rex::path::is_valid_path("some\\folder\\subfolder") == true);

  // Invalid ones
  REX_CHECK(rex::path::is_valid_path("this is a very long path that goes way beyond the limit of what's supported xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx") == false);

  // AI generated examples
  REX_CHECK(rex::path::is_valid_path("/home/user/<documents>/file.txt") == false);
  REX_CHECK(rex::path::is_valid_path("/etc/nginx/nginx|conf") == false);
  REX_CHECK(rex::path::is_valid_path("/tmp/file*.txt") == false);
  REX_CHECK(rex::path::is_valid_path("C:\\Users\\Alice\\Documents\\file?.txt") == false);
  REX_CHECK(rex::path::is_valid_path("C:\\Users\\Alice\\|Documents\\file.txt") == false);
  REX_CHECK(rex::path::is_valid_path("C:\\Users\\Alice\\Con\\file.txt") == false);
  REX_CHECK(rex::path::is_valid_path("C:UsersAliceDocumentsfile.txt") == false);
}
TEST_CASE("Is Valid Filename")
{
  // Valid ones
  REX_CHECK(rex::path::is_valid_filename("file.txt") == true);
  REX_CHECK(rex::path::is_valid_filename("file with space.txt") == true);
  REX_CHECK(rex::path::is_valid_filename("file_with_underscores.txt") == true);
  REX_CHECK(rex::path::is_valid_filename("file-with-dashes.txt") == true);
  REX_CHECK(rex::path::is_valid_filename("file.txt.txt") == true);
  REX_CHECK(rex::path::is_valid_filename("file!.!.txt") == true);
  REX_CHECK(rex::path::is_valid_filename("FILE!.!.txt") == true);
  REX_CHECK(rex::path::is_valid_filename("FILE!.!.TXT") == true);
  REX_CHECK(rex::path::is_valid_filename("file!.!.TXT") == true);

  // AI generated examples
  REX_CHECK(rex::path::is_valid_filename("file@name!#&$~.txt") == true);

  // Invalid ones
  REX_CHECK(rex::path::is_valid_filename("") == false);
  REX_CHECK(rex::path::is_valid_filename("path/file.txt") == false);

  // AI generated examples
  REX_CHECK(rex::path::is_valid_filename("file*name.txt") == false);
  REX_CHECK(rex::path::is_valid_filename("file:name.txt") == false);
  REX_CHECK(rex::path::is_valid_filename("file<name>.txt") == false);
  REX_CHECK(rex::path::is_valid_filename("file|name.txt") == false);
  REX_CHECK(rex::path::is_valid_filename("file?name.txt") == false);
  REX_CHECK(rex::path::is_valid_filename("file\"name\".txt") == false);
  REX_CHECK(rex::path::is_valid_filename("file\\name.txt") == false);

}

TEST_CASE("Change Extension")
{
  REX_CHECK(rex::path::change_extension("foo.txt", ".bar") == "foo.bar");
  REX_CHECK(rex::path::change_extension("foo.txt", "") == "foo");
  REX_CHECK(rex::path::change_extension("foo", ".bar") == "foo.bar");
  REX_CHECK(rex::path::change_extension("foo", "") == "foo");

  REX_CHECK(rex::path::change_extension("bar/foo.txt", ".bar") == "bar/foo.bar");
  REX_CHECK(rex::path::change_extension("bar/foo.txt", "") == "bar/foo");
  REX_CHECK(rex::path::change_extension("bar/foo", ".bar") == "bar/foo.bar");
  REX_CHECK(rex::path::change_extension("bar/foo", "") == "bar/foo");
}

TEST_CASE("Dir Name")
{
  REX_CHECK(rex::path::dir_name("foo.txt") == "");
  REX_CHECK(rex::path::dir_name("bar/foo.txt") == "bar");
}

TEST_CASE("Get Extension")
{
  REX_CHECK(rex::path::extension("foo.txt") == ".txt");
  REX_CHECK(rex::path::extension("foo.txt.bar") == ".txt.bar");
  REX_CHECK(rex::path::extension("foo") == "");
  
  REX_CHECK(rex::path::extension("bar/foo.txt") == ".txt");
  REX_CHECK(rex::path::extension("bar/foo.txt.bar") == ".txt.bar");
  REX_CHECK(rex::path::extension("bar/foo") == "");
}

TEST_CASE("Get Stem")
{
  REX_CHECK(rex::path::stem("foo.txt") == "foo");
  REX_CHECK(rex::path::stem("foo.txt.bar") == "foo");
  REX_CHECK(rex::path::stem("foo") == "foo");

  REX_CHECK(rex::path::stem("bar/foo.txt") == "foo");
  REX_CHECK(rex::path::stem("bar/foo.txt.bar") == "foo");
  REX_CHECK(rex::path::stem("bar/foo") == "foo");
}

TEST_CASE("Absolute Path")
{
  rsl::string cwd = rex::path::cwd();

  REX_CHECK(rex::path::abs_path("foo.txt") == rex::path::join(cwd, "foo.txt"));
  REX_CHECK(rex::path::abs_path("foo.txt.bar") == rex::path::join(cwd, "foo.txt.bar"));
  REX_CHECK(rex::path::abs_path("foo") == rex::path::join(cwd, "foo"));

  REX_CHECK(rex::path::abs_path("bar/foo.txt") == rex::path::join(cwd, "bar", "foo.txt"));
  REX_CHECK(rex::path::abs_path("bar/foo.txt.bar") == rex::path::join(cwd, "bar", "foo.txt.bar"));
  REX_CHECK(rex::path::abs_path("bar/foo") == rex::path::join(cwd, "bar", "foo"));

  REX_CHECK(rex::path::abs_path("c:/bar/foo.txt") == "c:/bar/foo.txt");
  REX_CHECK(rex::path::abs_path("c:/bar/foo.txt.bar") == "c:/bar/foo.txt.bar");
  REX_CHECK(rex::path::abs_path("c:/bar/foo") == "c:/bar/foo");
}

TEST_CASE("Path Root")
{
  REX_CHECK(rex::path::path_root("c:/bar/foo.txt") == "c:");
  REX_CHECK(rex::path::path_root("bar/foo") == "");
  REX_CHECK(rex::path::path_root("./bar/foo") == ""); // windows only, so root is empty
  REX_CHECK(rex::path::path_root("/bar/foo") == ""); // windows only, so root is empty
  REX_CHECK(rex::path::path_root("~/bar/foo") == ""); // windows only, so root is empty
}

TEST_CASE("Common Path")
{
  {
    rsl::vector<rsl::string_view> paths =
    {
      "c:/bar/foo.txt",
      "c:/bar/text.txt"
    };

    REX_CHECK(rex::path::common_path(paths) == "c:/bar/");
  }

  {
    rsl::vector<rsl::string_view> paths =
    {
      "c:/foo/foo.txt",
      "c:/bar/text.txt"
    };

    REX_CHECK(rex::path::common_path(paths) == "c:/");
  }

  {
    rsl::vector<rsl::string_view> paths =
    {
      "c:/bar/foo.txt",
      "d:/bar/text.txt"
    };

    REX_CHECK(rex::path::common_path(paths) == "");
  }
}

TEST_CASE("Random Dir")
{
  for (int i = 0; i < 1000; ++i)
  {
    REX_CHECK(!rex::file::exists(rex::path::random_dir()));
  }
}

TEST_CASE("Random Filename")
{
  for (int i = 0; i < 1000; ++i)
  {
    REX_CHECK(!rex::file::exists(rex::path::random_filename()));
  }
}

TEST_CASE("Real Path")
{
  // Make sure that the working directory is set correctly here
  rsl::string real_path = rex::path::real_path("shortcut_to_foo.txt.lnk");
  rsl::to_lower(real_path.cbegin(), real_path.begin(), real_path.size());
  REX_CHECK(real_path.ends_with(rex::path::join("rexengineunittest", "foo.txt")));
}

TEST_CASE("Normalise Path")
{
  REX_CHECK(rex::path::norm_path("/foo/./bar") == rex::path::join("/foo", "bar"));
  REX_CHECK(rex::path::norm_path("/path/to/dir") == rex::path::join("/path","to","dir"));
  REX_CHECK(rex::path::norm_path("path/to/dir") == rex::path::join("path","to","dir"));
  REX_CHECK(rex::path::norm_path("/") == "/");

  REX_CHECK(rex::path::norm_path("/path/./to/./dir") == rex::path::join("/path","to","dir"));
  REX_CHECK(rex::path::norm_path("/path/to/../dir") == rex::path::join("/path","dir"));
  REX_CHECK(rex::path::norm_path("/path/to/../../dir") == "/dir");

  REX_CHECK(rex::path::norm_path("/path/to/dir\\file") == rex::path::join("/path","to","dir","file"));
  REX_CHECK(rex::path::norm_path("\\path\\to\\dir\\file") == rex::path::join("/path", "to", "dir", "file"));
  REX_CHECK(rex::path::norm_path("path/to/dir/file\\") == rex::path::join("path", "to", "dir", "file"));

  REX_CHECK(rex::path::norm_path("//path/to//dir") == rex::path::join("/path","to","dir"));
  REX_CHECK(rex::path::norm_path("/\\path\\to/dir") == rex::path::join("/path","to","dir"));
  REX_CHECK(rex::path::norm_path("/path/to/dir\\") == rex::path::join("/path","to","dir"));

  REX_CHECK(rex::path::norm_path("/path//to//dir") == rex::path::join("/path","to","dir"));
  REX_CHECK(rex::path::norm_path("/path/to//dir") == rex::path::join("/path","to","dir"));
  REX_CHECK(rex::path::norm_path("/path/to/dir//") == rex::path::join("/path","to","dir"));

  REX_CHECK(rex::path::norm_path("/path//to//dir/") == rex::path::join("/path","to","dir"));
  REX_CHECK(rex::path::norm_path("../..") == rex::path::join("..",".."));
  REX_CHECK(rex::path::norm_path("path/../..") == "..");

  REX_CHECK(rex::path::norm_path("C:\\path\\to\\dir") == rex::path::join("C:","path","to","dir"));
  REX_CHECK(rex::path::norm_path("C:\\path\\to\\..\\dir") == rex::path::join("C:","path","dir"));
}

TEST_CASE("Relative Path")
{
  // linux style paths
  REX_CHECK(rex::path::is_same(rex::path::rel_path("/path/to/dir", "/path")                   , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("path/to/dir", "path")                     , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("/", "/")                                  , rex::path::join("")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("/path/./to/./dir", "/path")               , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("/path/to/../dir", "/path")                , rex::path::join("dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("/path/to/../../dir", "/path")             , rex::path::join("..", "dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("/path/to/dir\\file", "/path")             , rex::path::join("to", "dir", "file")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("\\path\\to\\dir\\file", "/path")          , rex::path::join("to", "dir", "file")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("path/to/dir/file\\", "path")              , rex::path::join("to", "dir", "file")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("//path/to//dir", "/")                     , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("/\\path\\to/dir", "/")                    , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("/path/to/dir\\", "/")                     , rex::path::join("path", "to", "dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("/path//to//dir", "/")                     , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("/path/to//dir", "/")                      , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("/path/to/dir//", "/")                     , rex::path::join("path", "to", "dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("/path/to/else", "/path/to/dir")           , rex::path::join("..", "else")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("\\path\\to\\else", "/path/to/dir")        , rex::path::join("..", "else")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("path/to/else\\", "path/to/dir")           , rex::path::join("..", "else")));

  rsl::string_view rel_path = "path/to/dir";
  s32 expected_depth = rex::path::abs_depth(rex::path::cwd());
  expected_depth += rex::path::depth(rel_path);
  rsl::string rel_to_root;
  for (s32 i = 0; i < expected_depth; ++i)
  {
    rel_to_root = rex::path::join(rel_to_root, "..");
  }

  rsl::string abs_path = rex::path::abs_path(rel_path);
  rex::path::SplitResult split_res = rex::path::split_origin(abs_path);
  
  REX_CHECK(rex::path::is_same(rex::path::rel_path("/path//to//dir/", "/")                    , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("/", rel_path)                             , rel_to_root));
  REX_CHECK(rex::path::is_same(rex::path::rel_path(rel_path, "/path")                         , rex::path::join("..", split_res.tail)));
  REX_CHECK(rex::path::is_same(rex::path::rel_path(rel_path, "/path/dir")                     , rex::path::join("..", "..", split_res.tail)));

  // windows style paths
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/dir", "d:/path")               , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("path/to/dir", "path")                     , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/", "d:/")                              , rex::path::join("")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/./to/./dir", "d:/path")           , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/../dir", "d:/path")            , rex::path::join("dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/../../dir", "d:/path")         , rex::path::join("..", "dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/./to/./dir", "D:/path")           , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/../dir", "D:/path")            , rex::path::join("dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/../../dir", "D:/path")         , rex::path::join("..", "dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/dir\\file", "d:/path")         , rex::path::join("to", "dir", "file")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("\\path\\to\\dir\\file", "d:/path")        , rex::path::join("to", "dir", "file")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("path/to/dir/file\\", "path")              , rex::path::join("to", "dir", "file")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("d://path/to//dir", "d:/")                 , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/\\path\\to/dir", "d:/")                , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/dir\\", "d:/")                 , rex::path::join("path", "to", "dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path//to//dir", "d:/")                 , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to//dir", "d:/")                  , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/dir//", "d:/")                 , rex::path::join("path", "to", "dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path//to//dir/", "d:/")                , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/", rel_path)                           , rex::path::join(rel_to_root)));
  REX_CHECK(rex::path::is_same(rex::path::rel_path(rel_path, "d:/path")                       , rex::path::join("..", split_res.tail)));
  REX_CHECK(rex::path::is_same(rex::path::rel_path(rel_path, "d:/path/dir")                   , rex::path::join("..", "..", split_res.tail)));

  // linux & windows style paths
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/dir", "/path")                 , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("path/to/dir", "path")                     , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/", "/")                                , rex::path::join("")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/./to/./dir", "/path")             , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/../dir", "/path")              , rex::path::join("dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/../../dir", "/path")           , rex::path::join("..", "dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/dir\\file", "/path")           , rex::path::join("to", "dir", "file")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("\\path\\to\\dir\\file", "/path")          , rex::path::join("to", "dir", "file")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("path/to/dir/file\\", "path")              , rex::path::join("to", "dir", "file")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("d://path/to//dir", "/")                   , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/\\path\\to/dir", "/")                  , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/dir\\", "/")                   , rex::path::join("path", "to", "dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path//to//dir", "/")                   , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to//dir", "/")                    , rex::path::join("path", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path/to/dir//", "/")                   , rex::path::join("path", "to", "dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("d:/path//to//dir/", "/")                  , rex::path::join("path", "to", "dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("C:\\path\\to\\dir", "C:\\path")           , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("C:\\path\\to\\..\\dir", "C:\\path")       , rex::path::join("dir")));

  REX_CHECK(rex::path::is_same(rex::path::rel_path("D:\\path\\to\\dir", "C:\\path\\to\\dir")  , rex::path::join(rex::path::abs_path("D:\\path\\to\\dir"))));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("D:/path/to/dir", "C:/path/to/dir")        , rex::path::join(rex::path::abs_path("D:/path/to/dir"))));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("D:\\", "C:\\")                            , rex::path::join(rex::path::abs_path("D:\\"))));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("D:/", "C:/")                              , rex::path::join(rex::path::abs_path("D:/"))));

  // edge cases
  REX_CHECK(rex::path::is_same(rex::path::rel_path("to/dir/", "path")                         , rex::path::join("..", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("to/dir/", "")                             , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("to/dir/", ".")                            , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("to/dir/", "./")                           , rex::path::join("to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("../to/dir/", "")                          , rex::path::join("..", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("../to/dir/", ".")                         , rex::path::join("..", "to", "dir")));
  REX_CHECK(rex::path::is_same(rex::path::rel_path("../to/dir/", "./")                        , rex::path::join("..", "to", "dir")));
}

TEST_CASE("Is Under Dir")
{
  REX_CHECK(rex::path::is_under_dir("/path/to/dir", "/path") == true);
  REX_CHECK(rex::path::is_under_dir("path/to/dir", "path") == true);
  REX_CHECK(rex::path::is_under_dir("/", "/") == true);

  REX_CHECK(rex::path::is_under_dir("/path/./to/./dir", "/path") == true);
  REX_CHECK(rex::path::is_under_dir("/path/to/../dir", "/path") == true);
  REX_CHECK(rex::path::is_under_dir("/path/to/../../dir", "/path") == false);

  REX_CHECK(rex::path::is_under_dir("/path/to/dir\\file", "/path") == true);
  REX_CHECK(rex::path::is_under_dir("\\path\\to\\dir\\file", "/path") == true);
  REX_CHECK(rex::path::is_under_dir("path/to/dir/file\\", "path") == true);

  REX_CHECK(rex::path::is_under_dir("//path/to//dir", "/") == true);
  REX_CHECK(rex::path::is_under_dir("/\\path\\to/dir", "/") == true);
  REX_CHECK(rex::path::is_under_dir("/path/to/dir\\", "/") == true);

  REX_CHECK(rex::path::is_under_dir("/path//to//dir", "/") == true);
  REX_CHECK(rex::path::is_under_dir("/path/to//dir", "/") == true);
  REX_CHECK(rex::path::is_under_dir("/path/to/dir//", "/") == true);

  REX_CHECK(rex::path::is_under_dir("/path//to//dir/", "/") == true);
  REX_CHECK(rex::path::is_under_dir("/", "path/to/dir") == false);

  REX_CHECK(rex::path::is_under_dir("C:\\path\\to\\dir", "C:\\path") == true);
  REX_CHECK(rex::path::is_under_dir("C:\\path\\to\\..\\dir", "C:\\path") == true);
}

TEST_CASE("Has Extension")
{
  REX_CHECK(rex::path::has_extension("file.txt") == true);
  REX_CHECK(rex::path::has_extension("document.pdf") == true);
  REX_CHECK(rex::path::has_extension("/path/to/file.txt") == true);
  REX_CHECK(rex::path::has_extension("/path/to/document.pdf") == true);

  REX_CHECK(rex::path::has_extension("file") == false);
  REX_CHECK(rex::path::has_extension("/path/to/file") == false);
  REX_CHECK(rex::path::has_extension("no_extension.") == false);
  REX_CHECK(rex::path::has_extension("/path/to/no_extension.") == false);

  REX_CHECK(rex::path::has_extension("") == false);  // Empty string
  REX_CHECK(rex::path::has_extension(".") == false);  // Only a dot, not a valid extension
  REX_CHECK(rex::path::has_extension("..") == false); // Double dots, not a valid extension

  // This is a special edge case, but it caused a bug before
  // We need to have a string view that points to actual data, but has a length of 0
  rsl::string abs_path("C:\\Windows\\System32\\file.dll");
  rsl::string_view abs_path_view(abs_path.data(), 0);
  REX_CHECK(rex::path::has_extension(abs_path_view) == false);
}

TEST_CASE("Is Absolute")
{
    REX_CHECK(rex::path::is_absolute("/absolute/path") == true);
    REX_CHECK(rex::path::is_absolute("/another/absolute/path/file.txt") == true);
    REX_CHECK(rex::path::is_absolute("C:\\Windows\\System32\\file.dll") == true);  // Windows path

    REX_CHECK(rex::path::is_absolute("relative/path") == false);
    REX_CHECK(rex::path::is_absolute("./relative/path") == false);
    REX_CHECK(rex::path::is_absolute("../relative/path/file.txt") == false);

    REX_CHECK(rex::path::is_absolute("") == false);  // Empty string
    REX_CHECK(rex::path::is_absolute(".") == false);  // Current directory
    REX_CHECK(rex::path::is_absolute("..") == false); // Parent directory

    // This is a special edge case, but it caused a bug before
    // We need to have a string view that points to actual data, but has a length of 0
    rsl::string abs_path("C:\\Windows\\System32\\file.dll");
    rsl::string_view abs_path_view(abs_path.data(), 0);
    REX_CHECK(rex::path::is_absolute(abs_path_view) == false);
}

TEST_CASE("Is Relative")
{
    REX_CHECK(rex::path::is_relative("relative/path") == true);
    REX_CHECK(rex::path::is_relative("./relative/path") == true);
    REX_CHECK(rex::path::is_relative("../relative/path/file.txt") == true);

    REX_CHECK(rex::path::is_relative("/absolute/path") == false);
    REX_CHECK(rex::path::is_relative("/another/absolute/path/file.txt") == false);
    REX_CHECK(rex::path::is_relative("C:\\Windows\\System32\\file.dll") == false);  // Windows path

    REX_CHECK(rex::path::is_relative("") == false);  // Empty string
    REX_CHECK(rex::path::is_relative(".") == false);  // Current directory
    REX_CHECK(rex::path::is_relative("..") == false); // Parent directory

    // This is a special edge case, but it caused a bug before
    // We need to have a string view that points to actual data, but has a length of 0
    rsl::string abs_path("../relative/path/file.txt");
    rsl::string_view abs_path_view(abs_path.data(), 0);
    REX_CHECK(rex::path::has_extension(abs_path_view) == false);
}

TEST_CASE("Is Junction")
{
  REX_CHECK(rex::path::is_junction("original_folder") == false);
  REX_CHECK(rex::path::is_junction("original_file.txt") == false);
  //REX_CHECK(rex::path::is_junction("junction_folder") == true); // not testable as junction can't be put on version control
}

TEST_CASE("Is Link")
{
  REX_CHECK(rex::path::is_link("original_folder") == false);
  REX_CHECK(rex::path::is_link("original_file.txt") == false);
  REX_CHECK(rex::path::is_link("junction_folder") == false);
}

TEST_CASE("Is Same File")
{
  // No upper case
  REX_CHECK(rex::path::is_same("same_file.txt", "same_file.txt") == true);
  REX_CHECK(rex::path::is_same("sub_folder_1/../same_file.txt", "same_file.txt") == true);
  REX_CHECK(rex::path::is_same("same_file.txt", rex::path::random_filename()) == false);
  REX_CHECK(rex::path::is_same("same_file.txt", rex::path::random_dir()) == false);


  REX_CHECK(rex::path::is_same("sub_folder_1", "sub_folder_1") == true);
  REX_CHECK(rex::path::is_same("sub_folder_1/../sub_folder_1", "sub_folder_1") == true);
  REX_CHECK(rex::path::is_same("same_file.txt", rex::path::random_filename()) == false);
  REX_CHECK(rex::path::is_same("same_file.txt", rex::path::random_dir()) == false);


  // First upper case
  REX_CHECK(rex::path::is_same("SAME_FILE.TXT", "same_file.txt") == true);
  REX_CHECK(rex::path::is_same("SUB_FOLDER_1/../same_file.txt", "same_file.txt") == true);
  REX_CHECK(rex::path::is_same("SAME_FILE.TXT", rex::path::random_filename()) == false);
  REX_CHECK(rex::path::is_same("SAME_FILE.TXT", rex::path::random_dir()) == false);


  REX_CHECK(rex::path::is_same("SUB_FOLDER_1", "sub_folder_1") == true);
  REX_CHECK(rex::path::is_same("SUB_FOLDER_1/../SUB_FOLDER_1", "sub_folder_1") == true);
  REX_CHECK(rex::path::is_same("SAME_FILE.TXT", rex::path::random_filename()) == false);
  REX_CHECK(rex::path::is_same("SAME_FILE.TXT", rex::path::random_dir()) == false);

  // Second upper case
  REX_CHECK(rex::path::is_same("same_file.txt", "SAME_FILE.TXT") == true);
  REX_CHECK(rex::path::is_same("sub_folder_1/../same_file.txt", "SAME_FILE.TXT") == true);


  REX_CHECK(rex::path::is_same("sub_folder_1", "SUB_FOLDER_1") == true);
  REX_CHECK(rex::path::is_same("sub_folder_1/../sub_folder_1", "SUB_FOLDER_1") == true);

  // All upper case
  REX_CHECK(rex::path::is_same("SAME_FILE.TXT", "SAME_FILE.TXT") == true);
  REX_CHECK(rex::path::is_same("SUB_FOLDER_1/../SAME_FILE.TXT", "SAME_FILE.TXT") == true);


  REX_CHECK(rex::path::is_same("SUB_FOLDER_1", "sub_folder_1") == true);
  REX_CHECK(rex::path::is_same("SUB_FOLDER_1/../sub_folder_1", "sub_folder_1") == true);

  REX_CHECK(rex::path::is_same(rex::path::random_filename(), rex::path::random_filename()) == false);
  REX_CHECK(rex::path::is_same(rex::path::random_filename(), rex::path::random_dir()) == false);
  REX_CHECK(rex::path::is_same(rex::path::random_dir(), rex::path::random_dir()) == false);
}

TEST_CASE("Split")
{
    auto result = rex::path::split("/path/to/file.txt");
    REX_CHECK(result.head == "/path/to");
    REX_CHECK(result.tail == "file.txt");

    result = rex::path::split("C:\\Windows\\System32\\file.dll");
    REX_CHECK(result.head == "C:\\Windows\\System32");
    REX_CHECK(result.tail == "file.dll");

    result = rex::path::split("./relative/path");
    REX_CHECK(result.head == "./relative");
    REX_CHECK(result.tail == "path");

    result = rex::path::split("");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "");

    result = rex::path::split(".");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == ".");

    result = rex::path::split("..");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "..");
}

TEST_CASE("Split Origin")
{
    auto result = rex::path::split_origin("C:\\Windows\\System32\\file.dll");
    REX_CHECK(result.head == "C:");
    REX_CHECK(result.tail == "Windows\\System32\\file.dll");

    result = rex::path::split_origin("\\Windows\\System32\\file.dll");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "\\Windows\\System32\\file.dll");

    result = rex::path::split_origin("/path/to/file.txt");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "/path/to/file.txt");

    result = rex::path::split_origin("");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "");

    result = rex::path::split_origin("./relative/path");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "./relative/path");
}

TEST_CASE("Split Extension")
{
    auto result = rex::path::split_ext("/path/to/file.txt");
    REX_CHECK(result.head == "/path/to/file");
    REX_CHECK(result.tail == ".txt");

    result = rex::path::split_ext("C:\\Windows\\System32\\file.dll");
    REX_CHECK(result.head == "C:\\Windows\\System32\\file");
    REX_CHECK(result.tail == ".dll");

    result = rex::path::split_ext("./relative/path/file.txt");
    REX_CHECK(result.head == "./relative/path/file");
    REX_CHECK(result.tail == ".txt");

    result = rex::path::split_ext("/path/to/file");
    REX_CHECK(result.head == "/path/to/file");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("C:\\Windows\\System32\\file");
    REX_CHECK(result.head == "C:\\Windows\\System32\\file");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("./relative/path/file");
    REX_CHECK(result.head == "./relative/path/file");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("");
    REX_CHECK(result.head == "");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext(".");
    REX_CHECK(result.head == ".");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("..");
    REX_CHECK(result.head == "..");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("./.");
    REX_CHECK(result.head == "./.");
    REX_CHECK(result.tail == "");

    result = rex::path::split_ext("../..");
    REX_CHECK(result.head == "../..");
    REX_CHECK(result.tail == "");
}

TEST_CASE("Split Root")
{
    auto result = rex::path::split_root("C:\\Windows\\System32\\file.dll");
    REX_CHECK(result.drive == "C:");
    REX_CHECK(result.root == "\\");
    REX_CHECK(result.tail == "Windows\\System32\\file.dll");

    result = rex::path::split_root("\\Windows\\System32\\file.dll");
    REX_CHECK(result.drive == "");
    REX_CHECK(result.root == "\\");
    REX_CHECK(result.tail == "Windows\\System32\\file.dll");

    result = rex::path::split_root("/path/to/file.txt");
    REX_CHECK(result.drive == "");
    REX_CHECK(result.root == "/");
    REX_CHECK(result.tail == "path/to/file.txt");

    result = rex::path::split_root("relative/path/file.txt");
    REX_CHECK(result.drive == "");
    REX_CHECK(result.root == "");
    REX_CHECK(result.tail == "relative/path/file.txt");

    result = rex::path::split_root("");
    REX_CHECK(result.drive == "");
    REX_CHECK(result.root == "");
    REX_CHECK(result.tail == "");

    result = rex::path::split_root("./relative/path");
    REX_CHECK(result.drive == "");
    REX_CHECK(result.root == "");
    REX_CHECK(result.tail == "./relative/path");
}

TEST_CASE("Depth")
{
  // Testing from working directory
  REX_CHECK(rex::path::depth("") == 0);
  REX_CHECK(rex::path::depth("path") == 1);
  REX_CHECK(rex::path::depth("path/to/dir") == 3);
  REX_CHECK(rex::path::depth("path/./to/dir") == 3);
  REX_CHECK(rex::path::depth("path/../to/dir") == 2);

  // Testing from other directory
  rsl::string_view dummy_dir = "dummy_dir";
  REX_CHECK(rex::path::depth("", dummy_dir) == 0);
  REX_CHECK(rex::path::depth("path", dummy_dir) == 0);
  REX_CHECK(rex::path::depth("path/to/dir", dummy_dir) == 0);
  REX_CHECK(rex::path::depth("path/./to/dir", dummy_dir) == 0);
  REX_CHECK(rex::path::depth("path/../to/dir", dummy_dir) == 0);

  // Linux absolute path
  REX_CHECK(rex::path::depth("/") == 0);
  REX_CHECK(rex::path::depth("\\") == 0);
  REX_CHECK(rex::path::depth("/path") == 0);
  REX_CHECK(rex::path::depth("/path/to/dir") == 0);
  REX_CHECK(rex::path::depth("/path/./to/dir") == 0);
  REX_CHECK(rex::path::depth("/path/../to/dir") == 0);

  // Windows absolute path
  REX_CHECK(rex::path::depth("d:") == 0);
  REX_CHECK(rex::path::depth("d:/") == 0);
  REX_CHECK(rex::path::depth("d:/path") == 0);
  REX_CHECK(rex::path::depth("d:/path/to/dir") == 0);
  REX_CHECK(rex::path::depth("d:/path/../to/dir") == 0);

  // Testing absolute depth

  rsl::string cwd = rex::path::cwd();
  s32 cwd_depth = rex::path::abs_depth(cwd);

  REX_CHECK(rex::path::abs_depth("") == cwd_depth);
  REX_CHECK(rex::path::abs_depth("path") == cwd_depth + 1);
  REX_CHECK(rex::path::abs_depth("path/to/dir") == cwd_depth + 3);
  REX_CHECK(rex::path::abs_depth("path/./to/dir") == cwd_depth + 3);
  REX_CHECK(rex::path::abs_depth("path/../to/dir") == cwd_depth + 2);

  // Linux absolute path
  REX_CHECK(rex::path::abs_depth("/") == 0);
  REX_CHECK(rex::path::abs_depth("\\") == 0);
  REX_CHECK(rex::path::abs_depth("/path") == 1);
  REX_CHECK(rex::path::abs_depth("/path/to/dir") == 3);
  REX_CHECK(rex::path::abs_depth("/path/./to/dir") == 3);
  REX_CHECK(rex::path::abs_depth("/path/../to/dir") == 2);

  // Windows absolute path
  REX_CHECK(rex::path::abs_depth("d:") == 0);
  REX_CHECK(rex::path::abs_depth("d:/") == 0);
  REX_CHECK(rex::path::abs_depth("d:/path") == 1);
  REX_CHECK(rex::path::abs_depth("d:/path/to/dir") == 3);
  REX_CHECK(rex::path::abs_depth("d:/path/./to/dir") == 3);
  REX_CHECK(rex::path::abs_depth("d:/path/../to/dir") == 2);
}

TEST_CASE("Has Drive")
{
  REX_CHECK(rex::path::has_drive("") == false);
  REX_CHECK(rex::path::has_drive("/") == false);
  REX_CHECK(rex::path::has_drive("\\") == false);
  REX_CHECK(rex::path::has_drive("D") == false);
  REX_CHECK(rex::path::has_drive("D:") == false);
  REX_CHECK(rex::path::has_drive("d:a") == false);
  REX_CHECK(rex::path::has_drive("d:/") == true);
  REX_CHECK(rex::path::has_drive("d:\\") == true);
  REX_CHECK(rex::path::has_drive("d:\\something") == true);
}
TEST_CASE("Is Drive")
{
  REX_CHECK(rex::path::is_drive("D:") == true);
  REX_CHECK(rex::path::is_drive("D:/") == false);
  REX_CHECK(rex::path::is_drive("D:/path") == false);
  REX_CHECK(rex::path::is_drive("D/") == false);
  REX_CHECK(rex::path::is_drive("D") == false);
  REX_CHECK(rex::path::is_drive("path") == false);
}
TEST_CASE("Is Root")
{
  REX_CHECK(rex::path::is_root("") == false);
  REX_CHECK(rex::path::is_root("d:") == false);
  REX_CHECK(rex::path::is_root("path") == false);
  REX_CHECK(rex::path::is_root("/path") == false);
  REX_CHECK(rex::path::is_root("\\path") == false);
  REX_CHECK(rex::path::is_root("/") == true);
  REX_CHECK(rex::path::is_root("\\") == true);
  REX_CHECK(rex::path::is_root("d:/") == true);
  REX_CHECK(rex::path::is_root("d:\\") == true);
}
TEST_CASE("Has Same Root")
{
  REX_CHECK(rex::path::has_same_root("", "") == true);
  REX_CHECK(rex::path::has_same_root("/", "/") == true);
  REX_CHECK(rex::path::has_same_root("\\", "\\") == true);
  REX_CHECK(rex::path::has_same_root("some path", "some path") == true);
  REX_CHECK(rex::path::has_same_root("some path", "some other path") == true);
  REX_CHECK(rex::path::has_same_root("c:/", "c:/") == true);
  REX_CHECK(rex::path::has_same_root("c:/some path", "c:/some path") == true);
  REX_CHECK(rex::path::has_same_root("c:\\", "c:\\") == true);
  REX_CHECK(rex::path::has_same_root("c:/", "c:\\") == true);
  REX_CHECK(rex::path::has_same_root("c:\\", "c:/") == true);
  REX_CHECK(rex::path::has_same_root("c:\\", "C:/") == true);

  REX_CHECK(rex::path::has_same_root("c:", "c:/") == false);
  REX_CHECK(rex::path::has_same_root("c:/", "c:") == false);
  REX_CHECK(rex::path::has_same_root("c:/", "d:") == false);
  REX_CHECK(rex::path::has_same_root("d:/", "c:") == false);
  REX_CHECK(rex::path::has_same_root("d:/", "c:/") == false);
}
TEST_CASE("Set Working Directory")
{
  rsl::string cwd = rex::path::cwd();

  rsl::string old_wd = rex::path::set_cwd("");
  REX_CHECK(rex::path::is_same(old_wd, cwd));
  REX_CHECK(rex::path::is_same(rex::path::cwd(), cwd));

  rsl::string_view new_wd1 = "c:\\";
  old_wd = rex::path::set_cwd(new_wd1);
  REX_CHECK(rex::path::is_same(new_wd1, rex::path::cwd()));
  REX_CHECK(rex::path::is_same(old_wd, cwd));

  rsl::string_view new_wd2 = "c:\\Users";
  old_wd = rex::path::set_cwd(new_wd2);
  REX_CHECK(rex::path::is_same(new_wd2, rex::path::cwd()));
  REX_CHECK(rex::path::is_same(old_wd, new_wd1));

  old_wd = rex::path::set_cwd(cwd);
  REX_CHECK(rex::path::is_same(cwd, rex::path::cwd()));
  REX_CHECK(rex::path::is_same(old_wd, new_wd2));
}