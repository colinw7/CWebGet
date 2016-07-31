#include <CUrl.h>
#include <COSFile.h>
#include <CFile.h>
#include <CStrUtil.h>

using std::string;

string CUrl::current_site_ = ".";

CUrl::
CUrl()
{
}

CUrl::
CUrl(const CFile &file)
{
  url_ = "file://" + file.getPath();

  decode();
}

CUrl::
CUrl(const string &url) :
 url_(url)
{
  decode();
}

bool
CUrl::
getSearch(const string &name, bool *value) const
{
  string value1;

  if (! getSearch(name, value1))
    return false;

  if (! CStrUtil::toBool(value1, value))
    return false;

  return true;
}

bool
CUrl::
getSearch(const string &name, int *value) const
{
  string value1;

  if (! getSearch(name, value1))
    return false;

  if (! CStrUtil::toInteger(value1, value))
    return false;

  return true;
}

bool
CUrl::
getSearch(const string &name, double *value) const
{
  string value1;

  if (! getSearch(name, value1))
    return false;

  if (! CStrUtil::toReal(value1, value))
    return false;

  return true;
}

bool
CUrl::
getSearch(const string &name, string &value) const
{
  SearchList::const_iterator p = searches_.find(name);

  if (p == searches_.end())
    return false;

  value = (*p).second;

  return true;
}

void
CUrl::
setSearch(const string &name, bool value)
{
  setSearch(name, CStrUtil::toString(value));
}

void
CUrl::
setSearch(const string &name, int value)
{
  setSearch(name, CStrUtil::toString(value));
}

void
CUrl::
setSearch(const string &name, double value)
{
  setSearch(name, CStrUtil::toString(value));
}

void
CUrl::
setSearch(const string &name, const string &value)
{
  SearchList::iterator p = searches_.find(name);

  if (p == searches_.end())
    searches_[name] = value;
  else
    (*p).second = value;;
}

void
CUrl::
decode()
{
  string url1 = url_;

  string::size_type pos;

  // Remove target

  pos = url1.rfind('#');

  if (pos != string::npos) {
    target_ = url1.substr(pos + 1);

    url1 = url1.substr(0, pos);
  }
  else
    target_ = "";

  //------

  // Remove searches

  pos = url1.rfind('?');

  if (pos != string::npos) {
    string search, name, value;

    string searches = url1.substr(pos + 1);

    url1 = url1.substr(0, pos);

    // split search into parts

    pos = searches.find('&');

    while (pos != string::npos) {
      search = searches.substr(0, pos);

      searches = searches.substr(pos + 1);

      pos = search.find('=');

      if (pos != string::npos) {
        name  = search.substr(0, pos);
        value = search.substr(pos + 1);
      }
      else {
        name  = search;
        value = "";
      }

      searches_[name] = value;

      pos = searches.find('&');
    }

    pos = searches.find('=');

    if (pos != string::npos) {
      name  = searches.substr(0, pos);
      value = searches.substr(pos + 1);
    }
    else {
      name  = searches;
      value = "";
    }

    searches_[name] = value;
  }

  //------

  // Remove extra trailing / on file name

  is_dir_ = false;

  int len = url1.size();

  while (len > 0 && url1[len - 1] == '/') {
    is_dir_ = true;

    url1 = url1.substr(0, len - 1);

    --len;
  }

  //------

  // Get prefix

  pos = url1.find(':');

  if (pos != string::npos) {
    prefix_ = url1.substr(0, pos);

    url1 = url1.substr(pos + 1);

    CStrUtil::toLower(prefix_);
  }
  else
    prefix_ = "";

  //----

  if      (prefix_ == "http" || prefix_ == "https") {
    if (url1.size() > 0 && url1[0] == '/')
      url1 = url1.substr(1);

    if (url1.size() > 0 && url1[0] == '/')
      url1 = url1.substr(1);

    string::size_type file_separator_pos = url1.find('/');

    if (file_separator_pos != string::npos) {
      site_ = url1.substr(0, file_separator_pos);

      file_ = url1.substr(file_separator_pos + 1);
    }
    else {
      site_ = url1;

      file_ = "";
    }
  }
  else if (prefix_ == "file") {
    if (url1.size() > 0 && url1[0] == '/')
      url1 = url1.substr(1);

    if (url1.size() > 0 && url1[0] == '/')
      url1 = url1.substr(1);

    if      (current_site_ != ".")
      site_ = current_site_;
    else if (url1.size() > 0 && url1[0] == '/') {
      string::size_type pos = url1.rfind('/');

      site_ = url1.substr(0, pos);
      url1  = url1.substr(pos + 1);
    }
    else
      site_ = COSFile::getCurrentDir();

    file_ = url1;
  }
  else {
    if (prefix_ == "") {
      if (current_site_ != ".") {
        prefix_ = "http";

        if (url1.size() > 0 && url1[0] == '/')
          url1 = url1.substr(1);
      }
      else
        prefix_ = "file";
    }

    if (current_site_ != ".")
      site_ = current_site_;
    else
      site_ = COSFile::getCurrentDir();

    file_ = url1;
  }

  //------

  file_ = encodeFile(file_);

  //------

  pos = file_.rfind('.');

  if (pos != string::npos)
    suffix_ = file_.substr(pos + 1);
  else
    suffix_ = "";

  suffix_ = CStrUtil::toLower(suffix_);

  full_url_ = prefix_ + "://" + site_ + "/";

  if (file_ != "")
    full_url_ += file_;
  else
    full_url_ += "index.html";

  if (! searches_.empty()) {
    full_url_ += "?";

    SearchList::iterator p1 = searches_.begin();
    SearchList::iterator p2 = searches_.end  ();

    for (uint i = 0; p1 != p2; ++p1, ++i) {
      if (i > 0)
        full_url_ += '&';

      full_url_ += (*p1).first;

      if ((*p1).second != "")
        full_url_ += "=" + (*p1).second;
    }
  }

  if (target_ != "")
    full_url_ += "#" + target_;
}

string
CUrl::
encode() const
{
  string file = getFile();

  string file1 = encodeFile(file);

  string url_str = getPrefix() + "://" + getSite() + "/" + file1;

  if (getIsDir())
    url_str += "/";

  return url_str;
}

string
CUrl::
encodeFile(const string &file) const
{
  // Remove .. parts

  CStrWords word_list = CStrUtil::toFields(file, "/");

  CStrWords::iterator word_p1 = word_list.begin();
  CStrWords::iterator word_p2 = word_list.end  ();

  if (word_p1 == word_p2)
    return file;

  string file1;

  string word1 = (*word_p1).getWord();

  if (word1 == "..")
    return file;

  ++word_p1;

  for ( ; word_p1 != word_p2; ++word_p1) {
    string word2 = (*word_p1).getWord();

    if (word2 == "..") {
      ++word_p1;

      if (word_p1 == word_p2) {
        word1 = "";

        break;
      }

      word1 = (*word_p1).getWord();
    }
    else {
      if (file1 != "")
        file1 += "/";

      file1 += word1;

      word1 = word2;
    }
  }

  if (word1 != "") {
    if (file1 != "")
      file1 += "/";

    file1 += word1;
  }

  //------

  // Remove . parts

  word_list = CStrUtil::toFields(file1, "/");

  word_p1 = word_list.begin();
  word_p2 = word_list.end  ();

  if (word_p1 == word_p2)
    return file1;

  string file2;

  for ( ; word_p1 != word_p2; ++word_p1) {
    string word1 = (*word_p1).getWord();

    if (word1 != ".") {
      if (file2 != "")
        file2 += "/";

      file2 += word1;
    }
  }

  return file2;
}

string
CUrl::
replaceSpecialChars(const string &str)
{
  string str1;

  uint len = str.size();

  for (uint i = 0; i < len; ++i) {
    if (str[i] == '%' && i < len - 2 &&
        isxdigit(str[i + 1]) && isxdigit(str[i + 2])) {
      uint hval;

      string hstr = str.substr(i + 1, 2);

      CStrUtil::decodeHexString(hstr, &hval);

      i += 2;

      str1 += char(hval);
    }
    else
      str1 += str[i];
  }

  return str1;
}

void
CUrl::
print() const
{
  print(std::cout);
}

void
CUrl::
print(std::ostream &os) const
{
  os << full_url_ << std::endl;
}

void
CUrl::
setCurrentSite(const CUrl &url)
{
  setCurrentSite(url.getSite());
}
