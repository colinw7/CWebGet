#include <CHttp.h>
#include <CTcp.h>
#include <CUrl.h>
#include <CThrow.h>
#include <CStrUtil.h>
#include <cstdio>

namespace {

class Parser {
 public:
  Parser(const std::string &str) :
   str_(str), pos_(0), len_(str.size()) {
  }

  void skipSpace() {
    while (pos_ < len_ && isspace(str_[pos_]))
      ++pos_;
  }

  std::string readWord() {
    skipSpace();

    int j = pos_;

    while (pos_ < len_ && (isalnum(str_[pos_]) || str_[pos_] == '_'))
      ++pos_;

    std::string word = str_.substr(j, pos_ - j);

    return word;
  }

  int readInteger() {
    std::string word = readWord();

    if (! CStrUtil::isInteger(word))
      return -1;

    return CStrUtil::toInteger(word);
  }

  bool skipChar(char c) {
    skipSpace();

    if (pos_ >= len_ || str_[pos_] != c)
      return false;

    ++pos_;

    return true;
  }

 private:
  std::string str_;
  int         pos_ { 0 };
  int         len_ { 0 };
};

int monthNameToInt(const std::string &name) {
  typedef std::vector<std::string> Names;

  static Names months = {{ "jan", "feb", "mar", "apr", "may", "jun",
                           "jul", "aug", "sep", "oct", "nov", "dec" }};

  for (uint i = 0; i < months.size(); ++i)
    if (name == months[i])
      return i;

  return 0;
}

}

//---

CHttp::
CHttp(const std::string &url)
{
  url_ = new CUrl(url);
  tcp_ = new CTcp;
}

CHttp::
~CHttp()
{
  delete tcp_;
  delete url_;
}

void
CHttp::
setDebug(bool debug)
{
  debug_ = debug;
}

void
CHttp::
setTcpDebug(bool debug)
{
  tcp_->setDebug(debug);
}

bool
CHttp::
download(CHttpData &data)
{
  const std::string &site = url_->getSite();

  std::string file = url_->getFile();

  if (url_->getIsDir())
    file += "/";

  if (! tcp_->connect(site)) {
    std::cerr << "Connect failed" << std::endl;
    return false;
  }

  std::string message;

  message += "GET /" + file + " HTTP/1.1\r\n";
  message += "Host: " + site + "\r\n";
  message += "User-Agent: CWebGet/1.0\r\n";
  message += "Accept: text/html,text/plain,image/gif,image/jpg,image/png,*/*\r\n";
  message += "Accept-Language: en-us,en\r\n";
  message += "Accept-Charset: ISO-8859-1,utf-8\r\n";
  message += "\r\n";

  if (debug_)
    std::cerr << message << std::endl;

  if (! tcp_->write(message)) {
    std::cerr << "Write failed" << std::endl;
    return false;
  }

  if (! tcp_->read(data.data)) {
    std::cerr << "Read failed" << std::endl;
    return false;
  }

  std::string line;
  std::string name, value;

  data.data = CStrUtil::readLine(data.data, line);

  line = CStrUtil::stripSpaces(line);

  if (debug_)
    std::cerr << line << std::endl;

  relocated_   = false;
  newLocation_ = "";

  std::vector<std::string> words;

  CStrUtil::addWords(line, words);

  if (words.size() >= 2) {
    int error_code = CStrUtil::toInteger(words[1]);

    if      (error_code == 200)
      ;
    else if (error_code == 301)
      relocated_ = true;
    else if (error_code == 302)
      relocated_ = true;
    else if (error_code == 400)
      return false;
    else if (error_code == 404)
      return false;
    else
      std::cerr << "Unrecognised reply \'" << line << std::endl;
  }
  else
    std::cerr << "Unrecognised reply \'" << line << std::endl;

  data.data = readLine(data.data, line);

  int data_length = 0;

  chunked_ = false;

  while (line.size() > 0) {
    decodeLine(line, name, value);

    if (debug_)
      std::cerr << name << ":" << value << std::endl;

    if      (name == "Transfer-Encoding") {
      if (value == "chunked")
        chunked_ = true;
    }
    else if (name == "Content-Type") {
      decodeContent(value, data.type, data.sub_type);
    }
    else if (name == "Content-Length") {
      data_length = CStrUtil::toInteger(value);
    }
    else if (name == "Location") {
      newLocation_ = value;
    }
    else if (name == "Last-Modified") {
      decodeDate(value, lastModified_);
    }

    data.data = readLine(data.data, line);
  }

  if (relocated_) {
    if (newLocation_ != "") {
      std::cerr << "Moved to " << newLocation_ << std::endl;
      return true;
    }
    else {
      std::cerr << "Moved to unspecified location" << std::endl;
      return false;
    }
  }

  if (chunked_) {
    if (debug_)
      std::cerr << "Chunked" << std::endl;

    std::string data1;

    if (data.data.size() == 0) {
      if (! getMoreData(site, data.data))
        return false;
    }

    data.data = readLine(data.data, line);

    uint len;

    if (sscanf(line.c_str(), "%x", &len) != 1)
      len = 0;

    if (debug_)
      std::cerr << line << "=" << len << std::endl;

    while (len > 0) {
      while (data.data.size() < len) {
        data1 += data.data;

        len -= data.data.size();

        if (! getMoreData(site, data.data))
          return false;
      }

      if (data.data.size() >= len) {
        data1 += data.data.substr(0, len);

        data.data = data.data.substr(len);
      }

      if (data.data.size() == 0) {
        if (! getMoreData(site, data.data))
          return false;
      }

      data.data = readLine(data.data, line);

      if (data.data.size() == 0) {
        if (! getMoreData(site, data.data))
          return false;
      }

      data.data = readLine(data.data, line);

      if (sscanf(line.c_str(), "%x", &len) != 1)
        len = 0;

      if (debug_)
        std::cerr << line << "=" << len << std::endl;
    }

    data.data = data1;
  }
  else {
    std::string more_data;

    int data_length1 = data_length;

    data_length1 -= data.data.size();

    while (data_length1 > 0) {
      if (! getMoreData(site, more_data, data_length1))
        return false;

      int more_len = more_data.size();

      if (more_len == 0)
        break;

      data_length1 -= more_len;

      data.data += more_data;
    }

    if (data_length1 != 0) {
      std::cerr << "Short Read (" << (data_length - data_length1) << " of " <<
                   data_length << ")" << std::endl;
      return true;
    }
  }

  return true;
}

bool
CHttp::
getMoreData(const std::string &, std::string &more_data)
{
  if (! tcp_->read(more_data)) {
    std::cerr << "Read failed" << std::endl;
    return false;
  }

  return true;
}

bool
CHttp::
getMoreData(const std::string &, std::string &more_data, int len)
{
  if (! tcp_->read(more_data, len)) {
    std::cerr << "Read failed" << std::endl;
    return false;
  }

  return true;
}

std::string
CHttp::
readLine(const std::string &data, std::string &line)
{
  std::string data1 = CStrUtil::readLine(data, line);

  int len = line.size();

  if (line[len - 1] == '\r')
    line = line.substr(0, len - 1);

  if (debug_)
    std::cerr << "'" << line << "'" << std::endl;

  return data1;
}

void
CHttp::
decodeLine(const std::string &line, std::string &name, std::string &value)
{
  std::string::size_type pos = line.find(':');

  if (pos == std::string::npos) {
    name  = "";
    value = "";
    return;
  }

  name  = line.substr(0, pos);
  value = line.substr(pos + 1);

  name  = CStrUtil::stripSpaces(name);
  value = CStrUtil::stripSpaces(value);
}

void
CHttp::
decodeContent(const std::string &value, std::string &type, std::string &sub_type)
{
  std::string::size_type pos = value.find('/');

  if (pos == std::string::npos)
    return;

  type     = value.substr(0, pos);
  sub_type = value.substr(pos + 1);

  type     = CStrUtil::stripSpaces(type);
  sub_type = CStrUtil::stripSpaces(sub_type);
}

bool
CHttp::
decodeDate(const std::string &value, time_t &t)
{
  Parser parser(value);

  std::string dayName = parser.readWord();

  parser.skipChar(',');

  int dayNum   = parser.readInteger();
  int monthNum = monthNameToInt(CStrUtil::toLower(parser.readWord()));
  int yearNum  = parser.readInteger();

  int hourNum = parser.readInteger();
  parser.skipChar(':');
  int minNum  = parser.readInteger();
  parser.skipChar(':');
  int secNum  = parser.readInteger();

  (void) parser.readWord(); // GMT

  //---

  struct tm tm;

  tm.tm_year  = yearNum - 1900;
  tm.tm_mon   = monthNum;
  tm.tm_mday  = dayNum;
  tm.tm_hour  = hourNum;
  tm.tm_min   = minNum;
  tm.tm_sec   = secNum;
  tm.tm_wday  = 0;
  tm.tm_yday  = 0;
  tm.tm_isdst = -1; // auto DST

  t = mktime(&tm);

  //---

  return true;
}
