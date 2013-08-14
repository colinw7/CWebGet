#include <CHtmlToken.h>

CHtmlToken::
CHtmlToken(CHtmlTokenType type) :
 type_(type)
{
}

CHtmlToken::
~CHtmlToken()
{
}

CHtmlCommentToken::
CHtmlCommentToken(CHtmlComment *comment) :
 CHtmlToken(CHTML_TOKEN_COMMENT), comment_(comment)
{
}

CHtmlTagToken::
CHtmlTagToken(CHtmlTag *tag) :
 CHtmlToken(CHTML_TOKEN_TAG), tag_(tag)
{
}

CHtmlTextToken::
CHtmlTextToken(CHtmlText *text) :
 CHtmlToken(CHTML_TOKEN_TEXT), text_(text)
{
}
