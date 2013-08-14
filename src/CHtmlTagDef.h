#ifndef CHtmlTagDef_H
#define CHtmlTagDef_H

#include <CHtmlTypes.h>
#include <string>
#include <sys/types.h>

class CHtmlTagDef {
 public:
  static const CHtmlTagDef &lookup(const std::string &name);
  static const CHtmlTagDef &lookup(CHtmlTagId type);

  CHtmlTagDef(CHtmlTagId id, const std::string &name, CHtmlTextType text_type) :
   id_(id), name_(name), text_type_(text_type) {
  }

  const std::string &getName    () const { return name_; }
  CHtmlTagId         getId      () const { return id_; }
  CHtmlTextType      getTextType() const { return text_type_; }

  bool isValid() const { return id_ != CHTML_TAG_ID_NONE; }

 private:
  static CHtmlTagDef tag_defs_[];
  static uint        num_tag_defs_;

  CHtmlTagId    id_;
  std::string   name_;
  CHtmlTextType text_type_;
};

#endif
