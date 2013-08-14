#include <CHtmlComment.h>

CHtmlComment::
CHtmlComment(const std::string &str) :
 str_(str)
{
}

void
CHtmlComment::
print(std::ostream &os) const
{
  os << "<!--" << str_ << "-->";
}

std::ostream &
operator<<(std::ostream &os, const CHtmlComment &comment)
{
  comment.print(os);

  return os;
}
