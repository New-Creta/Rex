#include "projected_filesystem/dirInfo.h"

#include "rex_std/algorithm.h"

//////////////////////////////////////////////////////////////////////////
// See dirInfo.h for descriptions of the routines in this module.
//////////////////////////////////////////////////////////////////////////

namespace proj_fs
{


  // A comparison routine for std::sort that wraps PrjFileNameCompare() so that we can sort our DirInfo
  // the same way the file system would.
  bool FileNameLessThan(const DirEntry& entry1, const DirEntry& entry2)
  {
    return PrjFileNameCompare(entry1.FileName.c_str(), entry2.FileName.c_str()) < 0;
  }

  DirInfo::DirInfo(rsl::wstring_view FilePathName) :
    m_file_path_name(FilePathName),
    m_current_idx(0),
    m_entries_filled(false)
  {}

  void DirInfo::Reset()
  {
    m_current_idx = 0;
    m_entries_filled = false;
    m_entries.clear();
  }

  bool DirInfo::EntriesFilled()
  {
    return m_entries_filled;
  }

  bool DirInfo::CurrentIsValid()
  {
    return m_current_idx < m_entries.size();
  }

  PRJ_FILE_BASIC_INFO DirInfo::CurrentBasicInfo()
  {
    PRJ_FILE_BASIC_INFO basicInfo = { 0 };
    basicInfo.IsDirectory = m_entries[m_current_idx].IsDirectory;
    basicInfo.FileSize = m_entries[m_current_idx].FileSize;

    return basicInfo;
  }

  rsl::wstring_view DirInfo::CurrentFileName()
  {
    return m_entries[m_current_idx].FileName;
  }

  bool DirInfo::MoveNext()
  {
    m_current_idx++;

    if (m_current_idx >= m_entries.size())
    {
      return false;
    }
    return true;
  }

  void DirInfo::FillDirEntry(rsl::wstring_view DirName)
  {
    FillItemEntry(DirName, 0, true);
  }

  void DirInfo::FillFileEntry(rsl::wstring_view FileName, INT64 FileSize)
  {
    FillItemEntry(FileName, FileSize, false);
  }

  void DirInfo::FillItemEntry(rsl::wstring_view FileName, INT64 FileSize, bool IsDirectory)
  {
    DirEntry entry;
    auto nameLen = FileName.length();
    if (nameLen > MAX_PATH)
    {
      return;
    }

    entry.FileName.assign(FileName);
    entry.IsDirectory = IsDirectory;
    entry.FileSize = FileSize;

    m_entries.push_back(entry);
  }

  void DirInfo::SortEntriesAndMarkFilled()
  {
    m_entries_filled = true;

    rsl::sort(m_entries.begin(),
      m_entries.end(),
      FileNameLessThan);
  }

}