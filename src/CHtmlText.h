#ifndef CHtmlText_H
#define CHtmlText_H

#include <string>

class CHtmlText {
 public:
  CHtmlText(const std::string &text);

  std::string getText() const { return text_; }

 private:
  std::string text_;
};

#endif
