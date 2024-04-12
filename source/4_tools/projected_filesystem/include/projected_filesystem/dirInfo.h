#pragma once

#include "rex_std/string.h"
#include "rex_std/vector.h"

#include <Windows.h>
#include <projectedfslib.h>

namespace proj_fs
{
  // This holds the information the RegFS provider will return for a single directory entry.
  //
  // Note that RegFS does not supply any timestamps.  This is because the only timestamp the registry
  // maintains is the last write time for a key.  It does not maintain creation, last-access, or change
  // times for keys, and it does not maintain any timestamps at all for values.  When RegFS calls
  // PrjFillDirEntryBuffer(), ProjFS sees that the timestamp values are 0 and uses the current time
  // instead.
  struct DirEntry 
  {
    rsl::wstring FileName;
    bool IsDirectory;
    int64 FileSize;
  };

  // RegFS uses a DirInfo object to hold directory entries.  When RegFS receives enumeration callbacks
  // it populates the DirInfo with a vector of DirEntry structs, one for each key and value in the
  // registry key being enumerated.
  //
  // Refer to RegfsProvider::StartDirEnum, RegfsProvider::GetDirEnum, and RegfsProvider::EndDirEnum
  // to see how this class is used.
  class DirInfo {

  public:

    // Constructs a new empty DirInfo, initializing it with the name of the directory it represents.
    DirInfo(rsl::wstring_view FilePathName);

    // Adds a DirEntry to the list using the given name.  The entry gets marked as a directory.
    void FillDirEntry(rsl::wstring_view DirName);

    // Adds a DirEntry to the list, using the given name and size.  The entry gets marked as a file.
    void FillFileEntry(rsl::wstring_view FileName, int64 FileSize);

    // Sorts the entries in the DirInfo object and marks the object as being fully populated.
    void SortEntriesAndMarkFilled();

    // Returns true if the DirInfo object has been populated with entries.
    bool EntriesFilled();

    // Returns true if CurrentBasicInfo() and CurrentFileName() will return valid values. 
    bool CurrentIsValid();

    // Returns a PRJ_FILE_BASIC_INFO populated with the information for the current item.
    PRJ_FILE_BASIC_INFO CurrentBasicInfo();

    // Returns the file name for the current item.
    rsl::wstring_view CurrentFileName();

    // Moves the internal index to the next DirEntry item.  Returns false if there are no more items.
    bool MoveNext();

    // Deletes all the DirEntry items in the DirInfo object.
    void Reset();

  private:

    // Adds a DirEntry to the list, using the given name, size.  Allows the caller to specify whether
    // it should be marked as a directory or not.
    void FillItemEntry(rsl::wstring_view FileName, int64 FileSize, bool IsDirectory);

    // Stores the name of the directory this DirInfo represents.
    rsl::wstring m_file_path_name;

    // The index of the item in m_entries that CurrentBasicInfo() and CurrentFileName() will return.
    int m_current_idx;

    // Marks whether or not this DirInfo has been filled with entries.
    bool m_entries_filled;

    // The list of entries in the directory this DirInfo represents.
    rsl::vector<DirEntry> m_entries;
  };

}