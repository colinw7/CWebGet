#include <CHtml.h>
#include <CHtmlParser.h>

CHtml::
CHtml()
{
}

bool
CHtml::
read(const std::string &filename, CHtmlParserTokens &tokens)
{
  CHtmlParser parser(*this);

  bool flag = parser.read(filename, tokens);

  return flag;
}

bool
CHtml::
readString(const std::string &str, CHtmlParserTokens &tokens)
{
  CHtmlParser parser(*this);

  bool flag = parser.read(str, tokens);

  return flag;
}
