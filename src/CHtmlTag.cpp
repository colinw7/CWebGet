#include <CHtmlTag.h>
#include <CHtmlTagDef.h>

CHtmlTag::
CHtmlTag(const std::string &name, const CHtmlTagOptionArray &options, bool end_tag) :
 name_(name), options_(options), end_tag_(end_tag),
 tag_def_(CHtmlTagDef::lookup(name_))
{
}

CHtmlTagOption::
CHtmlTagOption(const std::string &name, const std::string &value) :
 name_(name), value_(value)
{
}
