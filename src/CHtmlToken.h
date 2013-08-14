#ifndef CHtmlToken_H
#define CHtmlToken_H

enum CHtmlTokenType {
  CHTML_TOKEN_COMMENT,
  CHTML_TOKEN_TAG,
  CHTML_TOKEN_TEXT
};

class CHtmlComment;
class CHtmlTag;
class CHtmlText;

class CHtmlToken {
 public:
  CHtmlToken(CHtmlTokenType type);

  virtual ~CHtmlToken();

  CHtmlTokenType getType() const { return type_; }

  bool isComment() const { return type_ == CHTML_TOKEN_COMMENT; }
  bool isTag    () const { return type_ == CHTML_TOKEN_TAG    ; }
  bool isText   () const { return type_ == CHTML_TOKEN_TEXT   ; }

  virtual CHtmlComment *getComment() const { return 0; }
  virtual CHtmlTag     *getTag    () const { return 0; }
  virtual CHtmlText    *getText   () const { return 0; }

 protected:
  CHtmlTokenType type_;
};

class CHtmlCommentToken : public CHtmlToken {
 public:
  CHtmlCommentToken(CHtmlComment *comment);

  CHtmlComment *getComment() const { return comment_; }

 private:
  CHtmlComment *comment_;
};

class CHtmlTagToken : public CHtmlToken {
 public:
  CHtmlTagToken(CHtmlTag *tag);

  CHtmlTag *getTag() const { return tag_; }

 private:
  CHtmlTag *tag_;
};

class CHtmlTextToken : public CHtmlToken {
 public:
  CHtmlTextToken(CHtmlText *text);

  CHtmlText *getText() const { return text_; }

 private:
  CHtmlText *text_;
};

#endif
