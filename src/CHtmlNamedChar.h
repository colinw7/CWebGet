#ifndef CHtmlNamedChar_H
#define CHtmlNamedChar_H

#include <string>
#include <map>
#include <sys/types.h>

#define CHtmlNamedCharMgrInst CHtmlNamedCharMgr::getInstance()

struct CHtmlNamedChar {
  const char *name;
  uint        value;
  const char *str;

  CHtmlNamedChar(const char *name, uint value, const char *str) :
   name(name), value(value), str(str) {
  }
};

class CHtmlNamedCharMgr {
 private:
  typedef std::map<std::string,CHtmlNamedChar *> NameValueMap;
  typedef std::map<int        ,CHtmlNamedChar *> ValueNameMap;

 public:
  static CHtmlNamedCharMgr *getInstance();

  CHtmlNamedCharMgr();

  bool lookup(const std::string &name, CHtmlNamedChar **named_char) const;
  bool lookup(int value, CHtmlNamedChar **named_char) const;

  std::string encodeString(const std::string &str) const;
  bool        decodeString(const std::string &istr, std::string &ostr) const;

 private:
  static CHtmlNamedChar named_chars_[];
  static uint           num_named_chars_;

  NameValueMap name_value_map_;
  ValueNameMap value_name_map_;
};

#endif
