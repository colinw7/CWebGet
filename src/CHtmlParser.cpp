#include <CHtmlParser.h>
#include <CHtmlComment.h>
#include <CHtmlToken.h>
#include <CHtmlTag.h>
#include <CHtmlTagDef.h>
#include <CHtmlNamedChar.h>
#include <CHtmlText.h>
#include <CRegExp.h>
#include <CStrUtil.h>
#include <CThrow.h>
#include <cstdio>

CHtmlParser::
CHtmlParser(const CHtml &html) :
 html_(html), file_(NULL), tokens_(NULL), debug_(false), line_num_(1), char_num_(0)
{
}

CHtmlParser::
~CHtmlParser()
{
}

bool
CHtmlParser::
read(const std::string &filename, CHtmlParserTokens &tokens)
{
  if (! CFile::exists(filename)) {
    CTHROW("File " + filename + " does not exist");
    return false;
  }

  tokens_ = &tokens;

  file_ = new CFile(filename);

  while (lookChar() != EOF) {
    if      (isComment())
      readComment();
    else if (isTag())
      readTag();
    else
      readText();
  }

  file_ = NULL;

  return true;
}

bool
CHtmlParser::
readString(const std::string &str, CHtmlParserTokens &tokens)
{
  unreadChars(str);

  tokens_ = &tokens;

  file_ = NULL;

  while (lookChar() != EOF) {
    if      (isComment())
      readComment();
    else if (isTag())
      readTag();
    else
      readText();
  }

  return true;
}

bool
CHtmlParser::
isComment()
{
  std::string str;

  if (! matchString("<!--"))
    return false;

  unreadChars("<!--");

  return true;
}

bool
CHtmlParser::
readComment()
{
  std::string str;

  if (! matchString("<!--"))
    return false;

  str += "<!--";

  //------

  int c = lookChar();

  while (c != EOF) {
    if (matchString("-->")) {
      str += "-->";
      break;
    }

    str += readChar();

    c = lookChar();
  }

  if (c == EOF) {
    unreadChars(str);

    return false;
  }

  CHtmlComment *comment = new CHtmlComment(str);

  CHtmlCommentToken *token = new CHtmlCommentToken(comment);

  if (debug_)
    std::cerr << "Comment: " << str << std::endl;

  tokens_->add(token);

  return true;
}

bool
CHtmlParser::
isTag()
{
  int c = lookChar();

  if (c != '<')
    return false;

  std::string str;

  str += readChar();

  c = lookChar();

  if (c == '/')
    str += readChar();

  c = lookChar();

  if (! isalpha(c)) {
    unreadChars(str);

    return false;
  }

  unreadChars(str);

  return true;
}

bool
CHtmlParser::
readTag()
{
  int c = readChar();

  if (c != '<') {
    CTHROW("readTag: internal error");
    return false;
  }

  //------

  bool end_tag = false;

  c = readChar();

  if (c == '/') {
    end_tag = true;

    c = readChar();
  }

  if (c != EOF)
    unreadChar(c);

  //------

  c = readChar();

  if (c != EOF && ! isalpha(c)) {
    CTHROW("readTag: internal error");
    return false;
  }

  std::string name;

  name += c;

  c = readChar();

  while (c != EOF && ! isspace(c) && c != '>') {
    name += c;

    c = readChar();
  }

  if (c != EOF)
    unreadChar(c);

  //------

  skipSpaces();

  std::vector<CHtmlTagOption *> options;

  readTagOptions(options);

  //------

  skipSpaces();

  c = readChar();

  if (c != '>') {
    parseError("Bad close > on tag: " + name);

    while (c != EOF && c != '>')
      c = readChar();
  }

  CHtmlTag *tag = new CHtmlTag(name, options, end_tag);

  CHtmlTagToken *token = new CHtmlTagToken(tag);

  if (debug_)
    parseError(std::string("Tag: ") + (tag->isEndTag() ? "/" : "") + tag->getName() +
               (! tag->getTagDef().isValid() ? " <unknown>" : ""));

  tokens_->add(token);

  return true;
}

void
CHtmlParser::
readTagOptions(std::vector<CHtmlTagOption *> &options)
{
  while (true) {
    skipSpaces();

    //------

    int c = lookChar();

    if (c == '>')
      break;

    //------

    std::string name;

    c = readChar();

    while (c != EOF && ! isspace(c) && c != '=' && c != '>') {
      name += c;

      c = readChar();
    }

    if (c != EOF)
      unreadChar(c);

    if (name == "")
      break;

    //------

    skipSpaces();

    //------

    std::string value;

    c = lookChar();

    if (c == '=') {
      c = readChar();

      skipSpaces();

      c = readChar();

      while (c != EOF && ! isspace(c) && c != '>') {
        if      (c == '\'') {
          c = readChar();

          while (c != EOF) {
            if (c == '\'')
              break;

            value += c;

            c = readChar();
          }

          if (c == '\'')
            c = readChar();
        }
        else if (c == '\"') {
          c = readChar();

          while (c != EOF) {
            if (c == '\"')
              break;

            value += c;

            c = readChar();
          }

          if (c == '\"')
            c = readChar();
        }
        else {
          value += c;

          c = readChar();
        }
      }

      if (c != EOF)
        unreadChar(c);
    }

    //------

    value = replaceNamedChars(value);

    //------

    CHtmlTagOption *option = new CHtmlTagOption(name, value);

    if (debug_)
      std::cerr << "Option: " << name << "," << value << std::endl;

    options.push_back(option);
  }
}

std::string
CHtmlParser::
replaceNamedChars(const std::string &value)
{
  static CRegExp re1("#x[0-9a-fA-F][0-9a-fA-F]");
  static CRegExp re2("#[0-9][0-9]*");

  std::string value1;

  uint i   = 0;
  uint len = value.size();

  while (i < len) {
    char c = value[i];

    if (c == '&') {
      uint j = i++;

      while (i < len) {
        if (value[i] == ';')
          break;

        ++i;
      }

      if (i >= len) {
        value1 += value.substr(j);
        break;
      }

      uint len1 = i - j - 1;

      std::string name = value.substr(j + 1, len1);

      if      (re1.find(name)) {
        std::string hstr = name.substr(2);

        uint h;

        CStrUtil::decodeHexString(hstr, &h);

        value1 += char(h);
      }
      else if (re2.find(name)) {
        std::string dstr = name.substr(1);

        long l;

        CStrUtil::toInteger(dstr, &l);

        value1 += char(l);
      }
      else {
        CHtmlNamedChar *named_char;

        if      (! CHtmlNamedCharMgrInst->lookup(name, &named_char))
          value1 += value.substr(j, i - j + 1);
        else if (named_char->value > 255)
          value1 += value.substr(j, i - j + 1);
        else
          value1 += char(named_char->value);
      }

      ++i;
    }
    else {
      value1 += c;

      ++i;
    }
  }

  return value1;
}

void
CHtmlParser::
readText()
{
  int c = readChar();

  if (c == EOF)
    return;

  std::string str;

  str += c;

  while (! isComment() && ! isTag()) {
    c = readChar();

    if (c == EOF)
      break;

    str += c;
  }

  //----

  str = replaceNamedChars(str);

  CHtmlText *text = new CHtmlText(str);

  CHtmlTextToken *token = new CHtmlTextToken(text);

  if (debug_)
    std::cerr << "Text: " << str << std::endl;

  tokens_->add(token);
}

bool
CHtmlParser::
matchString(const std::string &str)
{
  std::string str1;

  int len = str.size();

  for (int i = 0; i < len; ++i) {
    int c = lookChar();

    if (c != str[i]) {
      unreadChars(str1);
      return false;
    }

    str1 += readChar();
  }

  return true;
}

void
CHtmlParser::
skipSpaces()
{
  int c = readChar();

  while (c != EOF && isspace(c))
    c = readChar();

  if (c != EOF)
    unreadChar(c);
}

int
CHtmlParser::
lookChar()
{
  if (buffer_.size() == 0) {
    int c = EOF;

    if (file_ != NULL)
      c = file_->getC();

    if (c == EOF)
      return EOF;

    buffer_.push_back(c);
  }

  return buffer_[buffer_.size() - 1];
}

int
CHtmlParser::
readChar()
{
  if (buffer_.size() == 0) {
    if (file_ != NULL) {
      int c = file_->getC();

      if (c == '\n') {
        ++line_num_;

        char_num_ = 0;
      }
      else
        ++char_num_;

      return c;
    }
    else
      return EOF;
  }

  int c = buffer_[buffer_.size() - 1];

  if (c == '\n') {
    ++line_num_;

    char_num_ = 0;
  }
  else
    ++char_num_;

  buffer_.pop_back();

  return c;
}

void
CHtmlParser::
unreadChars(const std::string &str)
{
  for (int i = str.size() - 1; i >= 0; --i) {
    if (str[i] == '\n') {
      --line_num_;

      char_num_ = 256;
    }
    else
      --char_num_;

    buffer_.push_back(str[i]);
  }
}

void
CHtmlParser::
unreadChar(int c)
{
  if (c == '\n')
    --line_num_;
  else
    --char_num_;

  buffer_.push_back(c);
}

void
CHtmlParser::
parseError(const char *fmt, ...)
{
  va_list vargs;

  va_start(vargs, fmt);

  std::cerr << line_num_ << ":" << char_num_ << "> ";
  std::cerr << CStrUtil::vstrprintf(fmt, &vargs) << std::endl;

  va_end(vargs);
}

void
CHtmlParser::
parseError(const std::string &str)
{
  std::cerr << line_num_ << ":" << char_num_ << "> " << str << std::endl;
}

//--------------

CHtmlParserTokens::
CHtmlParserTokens()
{
}

CHtmlParserTokens::
~CHtmlParserTokens()
{
  clear();
}

void
CHtmlParserTokens::
add(CHtmlToken *token)
{
  tokens_.push_back(token);
}

void
CHtmlParserTokens::
clear()
{
  int num_tokens = tokens_.size();

  for (int i = 0; i < num_tokens; i++)
    delete tokens_[i];

  tokens_.clear();
}
