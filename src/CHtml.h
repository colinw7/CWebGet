#ifndef CHtml_H
#define CHtml_H

#include <string>

class CHtmlParserTokens;

class CHtml {
 public:
  CHtml();

  bool read(const std::string &filename, CHtmlParserTokens &tokens);

  bool readString(const std::string &str, CHtmlParserTokens &tokens);
};

#endif
