#include <CHtmlTagDef.h>
#include <CStrUtil.h>

CHtmlTagDef
CHtmlTagDef::
tag_defs_[] = {
  CHtmlTagDef(CHTML_TAG_ID_NONE      , ""          , CHTML_TEXT_TYPE_NONE   ),
  CHtmlTagDef(CHTML_TAG_ID_A         , "a"         , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_ADDRESS   , "address"   , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_APPLET    , "applet"    , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_AREA      , "area"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_B         , "b"         , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_BASE      , "base"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_BASEFONT  , "basefont"  , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_BGSOUND   , "bgsound"   , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_BIG       , "big"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_BLINK     , "blink"     , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_BLOCKQUOTE, "blockquote", CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_BODY      , "body"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_BR        , "br"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_CAPTION   , "caption"   , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_CENTER    , "center"    , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_CITE      , "cite"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_CODE      , "code"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_DD        , "dd"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_DFN       , "dfn"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_DIR       , "dir"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_DIV       , "div"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_DL        , "dl"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_DT        , "dt"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_EM        , "em"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_FONT      , "font"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_FORM      , "form"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_FRAME     , "frame"     , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_FRAMESET  , "frameset"  , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_H1        , "h1"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_H2        , "h2"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_H3        , "h3"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_H4        , "h4"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_H5        , "h5"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_H6        , "h6"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_HEAD      , "head"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_HR        , "hr"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_HTML      , "html"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_I         , "i"         , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_IMG       , "img"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_INPUT     , "input"     , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_ISINDEX   , "isindex"   , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_KBD       , "kbd"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_LI        , "li"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_LINK      , "link"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_LISTING   , "listing"   , CHTML_TEXT_TYPE_LITERAL),
  CHtmlTagDef(CHTML_TAG_ID_MAP       , "map"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_MARQUEE   , "marquee"   , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_MENU      , "menu"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_META      , "meta"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_NEXTID    , "nextid"    , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_NOBR      , "nobr"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_NOFRAMES  , "noframes"  , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_NOSCRIPT  , "noscript"  , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_OL        , "ol"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_OPTION    , "option"    , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_P         , "p"         , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_PARAM     , "param"     , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_PRE       , "pre"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_PLAINTEXT , "plaintext" , CHTML_TEXT_TYPE_RAW    ),
  CHtmlTagDef(CHTML_TAG_ID_S         , "s"         , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_SAMP      , "samp"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_SCRIPT    , "script"    , CHTML_TEXT_TYPE_STYLE  ),
  CHtmlTagDef(CHTML_TAG_ID_SELECT    , "select"    , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_SMALL     , "small"     , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_SPAN      , "span"      , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_STRIKE    , "strike"    , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_STRONG    , "strong"    , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_STYLE     , "style"     , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_SUB       , "sub"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_SUP       , "sup"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_TABLE     , "table"     , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_TD        , "td"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_TEXTAREA  , "textarea"  , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_TH        , "th"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_TITLE     , "title"     , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_TR        , "tr"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_TT        , "tt"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_U         , "u"         , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_UL        , "ul"        , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_VAR       , "var"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_WBR       , "wbr"       , CHTML_TEXT_TYPE_PLAIN  ),
  CHtmlTagDef(CHTML_TAG_ID_XMP       , "xmp"       , CHTML_TEXT_TYPE_LITERAL),
  CHtmlTagDef(CHTML_TAG_ID_COMMENT   , "!--"       , CHTML_TEXT_TYPE_PLAIN  ),
};

uint CHtmlTagDef::num_tag_defs_ =
  sizeof(tag_defs_)/sizeof(CHtmlTagDef);

const CHtmlTagDef &
CHtmlTagDef::
lookup(const std::string &name)
{
  for (uint i = 1; i < num_tag_defs_; i++)
    if (CStrUtil::casecmp(name, tag_defs_[i].name_) == 0)
      return tag_defs_[i];

  return tag_defs_[0];
}

const CHtmlTagDef &
CHtmlTagDef::
lookup(CHtmlTagId id)
{
  for (uint i = 1; i < num_tag_defs_; i++)
    if (id == tag_defs_[i].id_)
      return tag_defs_[i];

  return tag_defs_[0];
}
