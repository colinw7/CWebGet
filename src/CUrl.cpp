#include <CUrl.h>
#include <COSFile.h>
#include <CFile.h>
#include <CStrUtil.h>

std::string CUrl::current_site_ = ".";

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
CUrl(const std::string &url) :
 url_(url)
{
  decode();
}

bool
CUrl::
getSearch(const std::string &name, bool *value) const
{
  std::string value1;

  if (! getSearch(name, value1))
    return false;

  if (! CStrUtil::toBool(value1, value))
    return false;

  return true;
}

bool
CUrl::
getSearch(const std::string &name, int *value) const
{
  std::string value1;

  if (! getSearch(name, value1))
    return false;

  if (! CStrUtil::toInteger(value1, value))
    return false;

  return true;
}

bool
CUrl::
getSearch(const std::string &name, double *value) const
{
  std::string value1;

  if (! getSearch(name, value1))
    return false;

  if (! CStrUtil::toReal(value1, value))
    return false;

  return true;
}

bool
CUrl::
getSearch(const std::string &name, std::string &value) const
{
  SearchList::const_iterator p = searches_.find(name);

  if (p == searches_.end())
    return false;

  value = (*p).second;

  return true;
}

void
CUrl::
setSearch(const std::string &name, bool value)
{
  setSearch(name, CStrUtil::toString(value));
}

void
CUrl::
setSearch(const std::string &name, int value)
{
  setSearch(name, CStrUtil::toString(value));
}

void
CUrl::
setSearch(const std::string &name, double value)
{
  setSearch(name, CStrUtil::toString(value));
}

void
CUrl::
setSearch(const std::string &name, const std::string &value)
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
  std::string url1 = url_;

  std::string::size_type pos;

  // Remove target

  pos = url1.rfind('#');

  if (pos != std::string::npos) {
    target_ = url1.substr(pos + 1);

    url1 = url1.substr(0, pos);
  }
  else
    target_ = "";

  //------

  // Remove searches

  pos = url1.rfind('?');

  if (pos != std::string::npos) {
    std::string search, name, value;

    std::string searches = url1.substr(pos + 1);

    url1 = url1.substr(0, pos);

    // split search into parts

    pos = searches.find('&');

    while (pos != std::string::npos) {
      search = searches.substr(0, pos);

      searches = searches.substr(pos + 1);

      pos = search.find('=');

      if (pos != std::string::npos) {
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

    if (pos != std::string::npos) {
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

  if (pos != std::string::npos) {
    setPrefix(url1.substr(0, pos));

    url1 = url1.substr(pos + 1);

    CStrUtil::toLower(getPrefix());
  }
  else
    setPrefix("");

  //----

  if      (isHttp()) {
    if (url1.size() > 0 && url1[0] == '/')
      url1 = url1.substr(1);

    if (url1.size() > 0 && url1[0] == '/')
      url1 = url1.substr(1);

    std::string::size_type file_separator_pos = url1.find('/');

    if (file_separator_pos != std::string::npos) {
      site_ = url1.substr(0, file_separator_pos);

      file_ = url1.substr(file_separator_pos + 1);
    }
    else {
      site_ = url1;

      file_ = "";
    }
  }
  else if (isFile()) {
    if (url1.size() > 0 && url1[0] == '/')
      url1 = url1.substr(1);

    if (url1.size() > 0 && url1[0] == '/')
      url1 = url1.substr(1);

    if      (current_site_ != ".")
      site_ = current_site_;
    else if (url1.size() > 0 && url1[0] == '/') {
      std::string::size_type pos = url1.rfind('/');

      site_ = url1.substr(0, pos);
      url1  = url1.substr(pos + 1);
    }
    else
      site_ = COSFile::getCurrentDir();

    file_ = url1;
  }
  else {
    if (getPrefix() == "") {
      if (current_site_ != ".") {
        setPrefix("http");

        if (url1.size() > 0 && url1[0] == '/')
          url1 = url1.substr(1);
      }
      else
        setPrefix("file");
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

  if (pos != std::string::npos)
    suffix_ = file_.substr(pos + 1);
  else
    suffix_ = "";

  suffix_ = CStrUtil::toLower(suffix_);

  full_url_ = getPrefix() + "://" + site_ + "/";

  if (file_ != "") {
    std::string::size_type file_separator_pos = file_.rfind('/');

    if (file_separator_pos != std::string::npos)
      local_file_ = file_.substr(file_separator_pos + 1);
    else
      local_file_ = file_;
  }
  else {
    local_file_ = "index.html";
  }

  full_url_ += local_file_;

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

std::string
CUrl::
encode() const
{
  std::string file = getFile();

  std::string file1 = encodeFile(file);

  std::string url_str = getPrefix() + "://" + getSite() + "/" + file1;

  if (getIsDir())
    url_str += "/";

  return url_str;
}

std::string
CUrl::
encodeFile(const std::string &file) const
{
  // Remove leading/trailing '/'
  std::string file1 = file;

  bool absolute = false;

  while (! file1.empty() && file1[0] == '/') {
    file1 = file1.substr(1);

    absolute = true;
  }

  while (! file1.empty() && file1[file1.size() - 1] == '/') {
    file1 = file1.substr(0, file1.size() - 1);
  }

  // split into parts
  std::vector<std::string> fields;

  CStrUtil::addFields(file1, fields, "/");

  if (fields.empty())
    return file;

  // Remove . and .. parts
  std::vector<std::string> fields1;

  for (uint i = 0; i < fields.size(); ++i) {
    if      (fields[i] == ".") {
      if (fields1.empty())
        return file;
    }
    else if (fields[i] == "..") {
      if (fields1.empty())
        return file;

      fields1.pop_back();
    }
    else
      fields1.push_back(fields[i]);
  }

  // rebuild filename
  std::string file2;

  if (absolute)
    file2 += "/";

  for (uint i = 0; i < fields1.size(); ++i) {
    if (! file2.empty() && file2[file2.size() - 1] != '/')
      file2 += "/";

    file2 += fields1[i];
  }

  return file2;
}

std::string
CUrl::
replaceSpecialChars(const std::string &str)
{
  std::string str1;

  uint len = str.size();

  for (uint i = 0; i < len; ++i) {
    if (str[i] == '%' && i < len - 2 &&
        isxdigit(str[i + 1]) && isxdigit(str[i + 2])) {
      uint hval;

      std::string hstr = str.substr(i + 1, 2);

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
