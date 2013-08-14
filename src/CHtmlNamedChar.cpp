#include <CHtmlNamedChar.h>
#include <cstring>
#include <iostream>

CHtmlNamedChar
CHtmlNamedCharMgr::
named_chars_[] = {
  CHtmlNamedChar("AElig" , 198, "a"),
  CHtmlNamedChar("Aacute", 193, "a"),
  CHtmlNamedChar("Acirc" , 194, "a"),
  CHtmlNamedChar("Agrave", 192, "a"),
  CHtmlNamedChar("Aring" , 197, "a"),
  CHtmlNamedChar("Atilde", 195, "a"),
  CHtmlNamedChar("Auml"  , 196, "a"),
  CHtmlNamedChar("Ccedil", 199, "c"),
  CHtmlNamedChar("ETH"   , 208, "d"),
  CHtmlNamedChar("Eacute", 201, "e"),
  CHtmlNamedChar("Ecirc" , 202, "e"),
  CHtmlNamedChar("Egrave", 200, "e"),
  CHtmlNamedChar("Euml"  , 203, "e"),
  CHtmlNamedChar("Iacute", 205, "i"),
  CHtmlNamedChar("Icirc" , 206, "i"),
  CHtmlNamedChar("Igrave", 204, "i"),
  CHtmlNamedChar("Iuml"  , 207, "i"),
  CHtmlNamedChar("Ntilde", 209, "n"),
  CHtmlNamedChar("Oacute", 211, "o"),
  CHtmlNamedChar("Ocirc" , 212, "o"),
  CHtmlNamedChar("Ograve", 210, "o"),
  CHtmlNamedChar("Oslash", 216, "o"),
  CHtmlNamedChar("Otilde", 213, "o"),
  CHtmlNamedChar("Ouml"  , 214, "o"),
  CHtmlNamedChar("THORN" , 222, "p"),
  CHtmlNamedChar("Uacute", 218, "u"),
  CHtmlNamedChar("Ucirc" , 219, "u"),
  CHtmlNamedChar("Ugrave", 217, "u"),
  CHtmlNamedChar("Uuml"  , 220, "u"),
  CHtmlNamedChar("Yacute", 221, "y"),
  CHtmlNamedChar("aacute", 225, "a"),
  CHtmlNamedChar("acute" , 180, "a"),
  CHtmlNamedChar("acirc" , 226, "a"),
  CHtmlNamedChar("aelig" , 230, "a"),
  CHtmlNamedChar("agrave", 224, "a"),
  CHtmlNamedChar("amp"   ,  38, "&"),
  CHtmlNamedChar("apos"  ,  39, "'"),
  CHtmlNamedChar("aring" , 229, "a"),
  CHtmlNamedChar("atilde", 227, "a"),
  CHtmlNamedChar("auml"  , 228, "a"),
  CHtmlNamedChar("brvbar", 166, "|"),
  CHtmlNamedChar("ccedil", 231, "c"),
  CHtmlNamedChar("cedil" , 184, "c"),
  CHtmlNamedChar("cent"  , 162, "c"),
  CHtmlNamedChar("copy"  , 169, "c"),
  CHtmlNamedChar("curren", 164, "#"),
  CHtmlNamedChar("deg"   , 176, "o"),
  CHtmlNamedChar("divide", 247, "%"),
  CHtmlNamedChar("eacute", 233, "e"),
  CHtmlNamedChar("ecirc" , 234, "e"),
  CHtmlNamedChar("egrave", 232, "e"),
  CHtmlNamedChar("eth"   , 240, "d"),
  CHtmlNamedChar("euml"  , 235, "e"),
  CHtmlNamedChar("frac12", 189, "1/2"),
  CHtmlNamedChar("frac14", 188, "1/4"),
  CHtmlNamedChar("frac34", 190, "3/4"),
  CHtmlNamedChar("gt"    ,  62, ">"),
  CHtmlNamedChar("iacute", 237, "i"),
  CHtmlNamedChar("icirc" , 238, "i"),
  CHtmlNamedChar("iexcl" , 161, "i"),
  CHtmlNamedChar("igrave", 236, "i"),
  CHtmlNamedChar("iuml"  , 239, "i"),
  CHtmlNamedChar("iquest", 191, "?"),
  CHtmlNamedChar("laquo" , 171, "<<"),
  CHtmlNamedChar("lt"    ,  60, "<"),
  CHtmlNamedChar("macr"  , 175, "^"),
  CHtmlNamedChar("micro" , 181, "u"),
  CHtmlNamedChar("middot", 183, "."),
  CHtmlNamedChar("nbsp"  , 160, " "),
  CHtmlNamedChar("not"   , 172, "~"),
  CHtmlNamedChar("ntilde", 241, "n"),
  CHtmlNamedChar("oacute", 243, "o"),
  CHtmlNamedChar("ocirc" , 244, "o"),
  CHtmlNamedChar("ograve", 242, "o"),
  CHtmlNamedChar("ordf"  , 170, "b"),
  CHtmlNamedChar("ordm"  , 186, "o"),
  CHtmlNamedChar("oslash", 248, "o"),
  CHtmlNamedChar("otilde", 245, "o"),
  CHtmlNamedChar("ouml"  , 246, "o"),
  CHtmlNamedChar("para"  , 182, "P"),
  CHtmlNamedChar("plusmn", 177, "+/-"),
  CHtmlNamedChar("pound" , 163, "&"),
  CHtmlNamedChar("quot"  ,  34, "\""),
  CHtmlNamedChar("raquo" , 187, ">>"),
  CHtmlNamedChar("reg"   , 174, "R"),
  CHtmlNamedChar("sect"  , 167, "S"),
  CHtmlNamedChar("shy"   , 173, " "),
  CHtmlNamedChar("sup1"  , 185, "1"),
  CHtmlNamedChar("sup2"  , 178, "2"),
  CHtmlNamedChar("sup3"  , 179, "3"),
  CHtmlNamedChar("szlig" , 223, "B"),
  CHtmlNamedChar("thorn" , 254, "p"),
  CHtmlNamedChar("times" , 215, "x"),
  CHtmlNamedChar("uacute", 250, "u"),
  CHtmlNamedChar("ucirc" , 251, "u"),
  CHtmlNamedChar("ugrave", 249, "u"),
  CHtmlNamedChar("uml"   , 168, ".."),
  CHtmlNamedChar("uuml"  , 252, "u"),
  CHtmlNamedChar("yacute", 253, "y"),
  CHtmlNamedChar("yen"   , 165, "Y"),
  CHtmlNamedChar("yuml"  , 255, "y"),
};

uint CHtmlNamedCharMgr::num_named_chars_ =
  sizeof(CHtmlNamedCharMgr::named_chars_)/sizeof(CHtmlNamedCharMgr::named_chars_[0]);

CHtmlNamedCharMgr *
CHtmlNamedCharMgr::
getInstance()
{
  static CHtmlNamedCharMgr *instance_ = NULL;

  if (instance_ == NULL)
    instance_ = new CHtmlNamedCharMgr();

  return instance_;
}

CHtmlNamedCharMgr::
CHtmlNamedCharMgr() :
 name_value_map_(), value_name_map_()
{
  for (uint i = 1; i < num_named_chars_; ++i) {
    name_value_map_[named_chars_[i].name ] = &named_chars_[i];
    value_name_map_[named_chars_[i].value] = &named_chars_[i];
  }
}

bool
CHtmlNamedCharMgr::
lookup(const std::string &name, CHtmlNamedChar **named_char) const
{
  NameValueMap::const_iterator p = name_value_map_.find(name);

  if (p != name_value_map_.end()) {
    *named_char = p->second;

    return true;
  }

  return false;
}

bool
CHtmlNamedCharMgr::
lookup(int value, CHtmlNamedChar **named_char) const
{
  ValueNameMap::const_iterator p = value_name_map_.find(value);

  if (p != value_name_map_.end()) {
    *named_char = p->second;

    return true;
  }

  return false;
}

std::string
CHtmlNamedCharMgr::
encodeString(const std::string &str) const
{
  static const char *encode_chars = "\'\"&";

  CHtmlNamedChar *named_char;

  std::string str1;

  uint len = str.size();

  for (uint i = 0; i < len; ++i) {
    if (strchr(encode_chars, str[i]) != NULL) {
      lookup(int(str[i]), &named_char);

      str1 += "&";
      str1 += named_char->name;
      str1 += ";";
    }
    else
      str1 += str[i];
  }

  return str1;
}

bool
CHtmlNamedCharMgr::
decodeString(const std::string &istr, std::string &ostr) const
{
  ostr = "";

  uint len = istr.size();

  for (uint i = 0; i < len; ++i) {
    if (istr[i] == '&') {
      uint j1 = i + 1;
      uint j2 = j1;

      while (j2 < len && istr[j2] != ';')
        ++j2;

      if (istr[j2] == ';') {
        std::string name = istr.substr(j1, j2 - j1);

        CHtmlNamedChar *named_char;

        if (lookup(name, &named_char))
          ostr += (*named_char).str;
        else
          ostr += istr.substr(j1 - 1, j2 - j1 + 2);
      }
      else
        ostr += istr.substr(i);

      i = j2;
    }
    else {
      int c1 = istr[i];

      if (c1 > 127)
        std::cerr << "Invalid char value " << c1 << std::endl;

      ostr += istr[i];
    }
  }

  return true;
}
