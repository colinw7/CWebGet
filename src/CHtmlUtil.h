#ifndef CHtmlUtil_H
#define CHtmlUtil_H

#include <CFileType.h>
#include <string>

class CHtmlFileMgr {
 public:
  virtual ~CHtmlFileMgr() { }

  virtual std::string getTypeName (CFileType file_type);
  virtual std::string getTypeImage(CFileType file_type);
};

class CDir;
class CFile;

class CHtmlUtil {
 public:
  static bool listDirectory(CDir &dir, CFile &output, CHtmlFileMgr &file_mgr);

  static bool listImage(const std::string &filename, CFile &output);

  static bool listTextFile(const std::string &filename, CFile &output);

  static bool listBinaryFile(const std::string &filename, CFile &output);

  static bool listScriptFile(const std::string &filename, CFile &output);
};

#endif
