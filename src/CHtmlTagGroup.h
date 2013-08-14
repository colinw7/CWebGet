#ifndef CHtmlTagGroup_H
#define CHtmlTagGroup_H

#include <vector>

class CHtmlTagSubGroup;
class CHtmlTokenArray;

class CHtmlTagGroup {
 private:
  CHtmlTagSubGroup *sub_group_;

 public:
  CHtmlTagGroup(const CHtmlTokenArray &tokens, int *i);
};

class CHtmlTagSubGroup {
 private:
  std::vector<CHtmlTagSubGroupValue *> sub_values_;

 public:
  CHtmlTagSubGroup(CHtmlToken *token);
  CHtmlTagSubGroup(const CHtmlTokenArray &tokens, int *i);

  void add(CHtmlToken *token);
  void add(CHtmlTagSubGroup *sub_group);
};

class CHtmlTagSubGroupValue {
};

class CHtmlTagSubGroupToken : public CHtmlTagSubGroupValue {
 private:
  CHtmlToken *token_;

 public:
  CHtmlTagSubGroupToken(CHtmlToken *token);
};

class CHtmlTagSubGroupSubGroup : public CHtmlTagSubGroupValue {
 private:
  CHtmlTagSubGroup *sub_group_;

 public:
  CHtmlTagSubGroupToken(CHtmlTagSubGroup *sub_group);
};

#endif
