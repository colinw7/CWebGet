#ifndef CHtmlTag_H
#define CHtmlTag_H

#include <CHtmlTypes.h>
#include <string>
#include <vector>
#include <sys/types.h>

class CHtmlTagOption {
 public:
  CHtmlTagOption(const std::string &name, const std::string &value);

  const std::string &getName () const { return name_ ; }
  const std::string &getValue() const { return value_; }

 private:
  std::string name_;
  std::string value_;
};

class CHtmlTag {
 public:
  typedef std::vector<CHtmlTagOption *> OptionArray;

 public:
  CHtmlTag(const std::string &name, const OptionArray &options, bool end_tag);

  const std::string &getName() const { return name_; }

  uint getNumOptions() const { return options_.size(); }

  const OptionArray &getOptions() const { return options_; }

  const CHtmlTagOption *getOption(int i) const { return options_[i]; }

  bool isEndTag  () const { return   end_tag_; }
  bool isStartTag() const { return ! end_tag_; }

  const CHtmlTagDef &getTagDef() const { return tag_def_; }

 private:
  std::string        name_;
  OptionArray        options_;
  bool               end_tag_;
  const CHtmlTagDef &tag_def_;
};

#endif
