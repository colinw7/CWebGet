#include <CHtmlTagGroup.h>

CHtmlTagGroup::
CHtmlTagGroup(const CHtmlTokenArray &tokens, int *i)
{
  sub_group_ = new CHtmlTagSubGroup(tokens, i);
}

CHtmlTagSubGroup::
CHtmlTagSubGroup(CHtmlToken *token)
{
  CHtmlTagSubGroupToken *sub_value = new CHtmlTagSubGroupToken(token);

  sub_values_.push_back(sub_value);
}

CHtmlTagSubGroup::
CHtmlTagSubGroup(const CHtmlTokenArray &tokens, int *i)
{
  int num_tokens = tokens.size();

  if (*i >= num_tokens)
    return;

  CHtmlToken *token = tokens[(*i)++];

  if (! token->isTag()) {
    CHtmlTagSubGroupToken *sub_value = new CHtmlTagSubGroupToken(token);

    sub_values_.push_back(sub_value);

    return;
  }

  CHtmlTag *tag = token->getTag()->getTag();

  if (tag->getIsEndTag()) {
    CHtmlTagSubGroupToken *sub_value = new CHtmlTagSubGroupToken(token);

    sub_values_.push_back(sub_value);

    return;
  }

  CHtmlTagSubGroup *sub_group = new CHtmlTagSubGroup(token);

  while (*i < num_tokens) {
    token = tokens[(*i)++];

    if (! token->isTag()) {
      sub_group->add(token);

      continue;
    }

    CHtmlTag *tag1 = token->getTag()->getTag();

    if (tag->getName() != tag1->getName() || ! tag1->getIsEndTag())
      CHtmlTagSubGroup *sub_group1 = CHtmlTagSubGroup(tokens, i);

      sub_group->add(sub_group1);

      continue;
    }

    break;
  }

  CHtmlTagSubGroupSubGroup *sub_value = new CHtmlTagSubGroupSubGroup(sub_group);

  sub_values_.push_back(sub_value);
}
