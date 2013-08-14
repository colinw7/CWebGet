#ifndef CHtmlComment_H
#define CHtmlComment_H

#include <string>
#include <iostream>

class CHtmlComment {
 public:
  CHtmlComment(const std::string &str);

  std::string getString() const { return str_; }

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CHtmlComment &comment);

 private:
  std::string str_;
};

#endif
