#include <CWebGet.h>
#include <CDir.h>
#include <CFile.h>
#include <CHtmlLib.h>
#include <CStrUtil.h>
#include <CHttp.h>
#include <CImageLib.h>
#include <CFileUtil.h>
#include <CUrl.h>
#include <CThrow.h>
#include <CThread.h>

class CWebGetLoadPageProc : public CThreadArrayProc {
 private:
  CWebGetUrl *web_url_;

 public:
  CWebGetLoadPageProc(CThreadArray *array, CWebGetUrl *web_url_) :
   CThreadArrayProc(array), web_url_(web_url_) {
  }

  void *execute();

  bool getProcess() const { return web_url_->getProcess(); }
};

CWebGet::
CWebGet(const std::string &base_url) :
 base_url_   (base_url),
 debug_      (false),
 http_debug_ (false),
 tcp_debug_  (false),
 overwrite_  (false),
 use_threads_(false)
{
  base_dir_ = CDir::getCurrent();

  addSite(base_url_);

  CUrl::setCurrentSite(base_url_);

  thread_array_ = new CThreadArray(10);
  mutex_        = new CThreadMutex();
}

CWebGet::
~CWebGet()
{
  delete thread_array_;
  delete mutex_;
}

void
CWebGet::
setDebug(bool debug)
{
  debug_ = debug;
}

void
CWebGet::
setHttpDebug(bool debug)
{
  http_debug_ = debug;
}

void
CWebGet::
setTcpDebug(bool debug)
{
  tcp_debug_ = debug;
}

void
CWebGet::
setOverwrite(bool overwrite)
{
  overwrite_ = overwrite;
}

void
CWebGet::
setListRefs(bool list_refs)
{
  list_refs_   = list_refs;
  use_threads_ = false;
}

void
CWebGet::
setUseThreads(bool use_threads)
{
  use_threads_ = use_threads;
  list_refs_   = false;
}

void
CWebGet::
addSite(const CUrl &url)
{
  addSite(url.getSite());
}

void
CWebGet::
addSite(const std::string &site)
{
  sites_.push_back(site);
}

void
CWebGet::
loadSubPage(CWebGetUrl &web_url)
{
  if (list_refs_)
    listRef(web_url);
  else
    tryLoadPage(web_url);
}

bool
CWebGet::
tryLoadPage(CWebGetUrl &web_url)
{
  try {
    loadPage(web_url);

    return true;
  }
  catch (...) {
    return false;
  }
}

bool
CWebGet::
loadPage(CWebGetUrl &web_url)
{
  if (! isValidSite(web_url))
    return false;

  web_url.setCurrentSite();

  if (isLoaded(web_url))
    return true;

  setLoaded(web_url);

  std::string base_dir = web_url.getBaseDir();

  if (isHigherDir(base_dir)) {
    std::cerr << "Skipping Higher Directory " << base_dir << std::endl;
    return true;
  }

  std::cout << "Loading Page '" << web_url.getUrlStr() << "'" << std::endl;

  if (debug_)
    std::cerr << "Output File '" << web_url.getFilename() << "'" << std::endl;

  if (! getPage(web_url))
    return false;

  CFile *file = web_url.getFile();

  file->close();

  return true;
}

bool
CWebGet::
getPage(CWebGetUrl &web_url)
{
  if (isSkipDownload(web_url))
    return false;

  CFile *file = web_url.getFile();

  web_url.setProcess(true);

  if (overwrite_ || ! file->exists()) {
    if (use_threads_)
      mutex_->lock();

    CDir::makeDirs(*file);

    std::string path = file->getDir();

    CDir::enter(path);

    if (debug_)
      std::cerr << "Dir " << CDir::getCurrent() << std::endl;

    file->open(CFileBase::WRITE);

    CDir::leave();

    if (debug_)
      std::cerr << "Dir " << CDir::getCurrent() << std::endl;

    if (use_threads_)
      mutex_->unlock();

    //------

    if (! download(web_url, file))
      return false;
  }

  return true;
}

bool
CWebGet::
download(CWebGetUrl &web_url, CFile *file)
{
  if (debug_)
    std::cerr << "Downloading " << web_url.getUrlStr() << std::endl;

  if (use_threads_)
    mutex_->lock();

  CHttp http(web_url.getUrlStr());

  http.setDebug   (http_debug_);
  http.setTcpDebug(tcp_debug_);

  CHttpData data;

  bool flag = http.download(data);

  if (flag && ! http.isRelocated()) {
    file->write(data.data);

    if (! data.isHtmlData())
      web_url.setProcess(false);
  }

  if (use_threads_)
    mutex_->unlock();

  if (flag && http.isRelocated()) {
    std::string new_location = http.getNewLocation();

    if (debug_)
      std::cerr << "Redirected to " << new_location << std::endl;

    CUrl url(new_location);

    CWebGetUrl web_url1(web_url.getWebGet(), url);

    flag = getPage(web_url1);
  }

  return flag;
}

bool
CWebGet::
processFile(CWebGetUrl &web_url)
{
  WebUrlList web_urls;

  if (! getWebUrls(web_url, web_urls))
    return false;

  uint num_web_urls = web_urls.size();

  CWebGetLoadPageProc **procs = NULL;

  if (use_threads_)
    procs = new CWebGetLoadPageProc * [num_web_urls];

  for (uint i = 0; i < num_web_urls; ++i) {
    if (! isValidSite(*web_urls[i]))
      continue;

    if (use_threads_) {
      if (isLoaded(*web_urls[i]))
        continue;

      procs[i] = new CWebGetLoadPageProc(thread_array_, web_urls[i]);

      thread_array_->startThread(procs[i]);
    }
    else
      loadSubPage(*web_urls[i]);
  }

  if (use_threads_)
    thread_array_->join(NULL);

  for (uint i = 0; i < num_web_urls; ++i) {
    if (web_urls[i]->getProcess())
      processFile(*web_urls[i]);
  }

  for (uint i = 0; i < num_web_urls; ++i) {
    if (use_threads_)
      delete procs[i];

    delete web_urls[i];
  }

  if (use_threads_)
    delete [] procs;

  return true;
}

bool
CWebGet::
getWebUrls(CWebGetUrl &web_url, WebUrlList &web_urls)
{
  if (! isHtmlFile(web_url))
    return false;

  CFile *file = web_url.getFile();

  std::string path = file->getDir();

  if (debug_)
    std::cerr << "Read Html File " << file->getName() << std::endl;

  CDir::enter(path);

  if (debug_)
    std::cerr << "Dir " << CDir::getCurrent() << std::endl;

  CHtml html;

  CHtmlParser parser(html);

  CHtmlParserTokens tokens;

  if (! parser.read(file->getName(), tokens)) {
    CDir::leave();

    if (debug_)
      std::cerr << "Dir " << CDir::getCurrent() << std::endl;

    return false;
  }

  WebUrlMap web_url_map;

  uint num_tokens = tokens.size();

  for (uint i = 0; i < num_tokens; ++i) {
    if (! tokens[i]->isTag())
      continue;

    CHtmlTag *tag = tokens[i]->getTag();

    const CHtmlTagDef &tag_def = tag->getTagDef();

    CHtmlTagId id = tag_def.getId();

    if (id != CHtmlTagId::A     &&
        id != CHtmlTagId::BODY  &&
        id != CHtmlTagId::IMG   &&
        id != CHtmlTagId::FRAME)
      continue;

    const CHtmlTagOptionArray &options = tag->getOptions();

    int num_options = (int) options.size();

    for (int j = 0; j < num_options; j++) {
      CHtmlTagOption *option = options[j];

      std::string option_name = option->getName();

      if ((id == CHtmlTagId::A     && CStrUtil::casecmp(option_name, "href"      ) == 0) ||
          (id == CHtmlTagId::BODY  && CStrUtil::casecmp(option_name, "background") == 0) ||
          (id == CHtmlTagId::IMG   && CStrUtil::casecmp(option_name, "src"       ) == 0) ||
          (id == CHtmlTagId::FRAME && CStrUtil::casecmp(option_name, "src"       ) == 0)) {
        std::string option_value = option->getValue();

        std::string save_site = CUrl::getCurrentSite();

        std::string site = save_site;

        if (web_url.getUrl().getIsDir())
          site += "/" + web_url.getUrl().getFile();

        CUrl::setCurrentSite(site);

        if (debug_)
          std::cerr << option_value << std::endl;

        CUrl url(option_value);

        CWebGetUrl *web_url1 = new CWebGetUrl(this, url.getFullUrl());

        CUrl::setCurrentSite(save_site);

        if (web_url_map.find(web_url1->getUrlStr()) == web_url_map.end()) {
          web_urls.push_back(web_url1);

          web_url_map[web_url1->getUrlStr()] = web_url1;
        }
        else
          delete web_url1;
      }
    }
  }

  CDir::leave();

  if (debug_)
    std::cerr << "Dir " << CDir::getCurrent() << std::endl;

  return true;
}

void
CWebGet::
listRef(CWebGetUrl &web_url)
{
  if (! isValidSite(web_url))
    return;

  std::cerr << web_url.getUrlStr() << std::endl;

  return;
}

bool
CWebGet::
isValidSite(CWebGetUrl &web_url)
{
  const CUrl &url = web_url.getUrl();

  if (url.getPrefix() != "http" && url.getPrefix() != "https")
    return false;

  const std::string &site = url.getSite();

  for (int i = 0; i < (int) sites_.size(); i++) {
    if (sites_[i] == site)
      return true;
  }

  return false;
}

bool
CWebGet::
isHigherDir(const std::string &dir)
{
  int count1 = countDirLevels(base_dir_);
  int count2 = countDirLevels(dir);

  return count1 > count2;
}

bool
CWebGet::
countDirLevels(const std::string &dir)
{
  int len = dir.size();

  int count = 0;

  for (int i = 0; i < len; i++)
    if (dir[i] == '/')
      count++;

  return count;
}

bool
CWebGet::
isSkipDownload(CWebGetUrl &web_url)
{
  const CUrl &url = web_url.getUrl();

  const std::string &suffix = url.getSuffix();

  if (suffix == "mp3")
    return true;

  return false;
}

bool
CWebGet::
isLoaded(CWebGetUrl &web_url)
{
  if (use_threads_)
    mutex_->lock();

  bool flag = false;

  const std::string &url_str = web_url.getUrlStr();

  if (loaded_urls_.find(url_str) != loaded_urls_.end())
    flag = true;

  if (use_threads_)
    mutex_->unlock();

  return flag;
}

void
CWebGet::
setLoaded(CWebGetUrl &web_url)
{
  if (use_threads_)
    mutex_->lock();

  std::string url_str = web_url.getUrlStr();

  loaded_urls_[url_str]++;

  if (use_threads_)
    mutex_->unlock();
}

bool
CWebGet::
isHtmlFile(CWebGetUrl &web_url)
{
  const CUrl &url = web_url.getUrl();

  const std::string &suffix = url.getSuffix();

  if (suffix == "jpg"  || suffix == "jpeg" ||
      suffix == "gif"  || suffix == "mp3"  ||
      suffix == "png"  || suffix == "zip"  ||
      suffix == "tar"  || suffix == "gz"   ||
      suffix == "3dml" || suffix == "exe"  ||
      suffix == "bmf"  || suffix == "c"    ||
      suffix == "pdf"  || suffix == "xpm"  ||
      suffix == "xbm"  || suffix == "bmp"  ||
      suffix == "wav"  || suffix == "mov"  ||
      suffix == "tif"  || suffix == "mpg"  ||
      suffix == "inc"  || suffix == "tiff" ||
      suffix == "pov"  || suffix == "cpp"  ||
      suffix == "pl"   || suffix == "h"    ||
      suffix == "txt"  || suffix == "java" ||
      suffix == "f"    || suffix == "hpp"  ||
      suffix == "ppm"  || suffix == "ps"   ||
      suffix == "eps"  || suffix == "z"    ||
      suffix == "avi"  || suffix == "ma"   ||
      suffix == "rar")
    return false;

  CFile *file = web_url.getFile();

  if (CFileUtil::checkJPG(file) || CFileUtil::checkGIF(file))
    return false;

  return true;
}

void *
CWebGetLoadPageProc::
execute()
{
  CWebGet *webget = web_url_->getWebGet();

  webget->loadSubPage(*web_url_);

  return NULL;
}

CWebGetUrl::
CWebGetUrl(CWebGet *webget, CUrl url) :
 webget_(webget), url_(url)
{
  url_str_ = encodeUrl(url_);

  urlToFilename(url, filename_);

  file_ = new CFile(filename_);

  process_ = false;
}

CWebGetUrl::
~CWebGetUrl()
{
  delete file_;
}

std::string
CWebGetUrl::
getBaseDir() const
{
  std::string base_dir = webget_->getBaseDir() + "/" + url_.getFile();

  return base_dir;
}

std::string
CWebGetUrl::
encodeUrl(const CUrl &url)
{
  std::string url_str = url.getPrefix() + std::string("://") + url.getSite() + "/" + url.getFile();

  CStrWords word_list = CStrUtil::toFields(url_str, "/");

  CStrWords::iterator word_p1 = word_list.begin();
  CStrWords::iterator word_p2 = word_list.end  ();

  std::string url_str1;

  std::string word1 = (*word_p1).getWord();

  ++word_p1;

  for ( ; word_p1 != word_p2; ++word_p1) {
    std::string word2 = (*word_p1).getWord();

    if (word2 != ".." && word1 != "..") {
      if (url_str1 != "")
        url_str1 += "/";

      url_str1 += word1;
    }

    word1 = word2;
  }

  if (url_str1 != "")
    url_str1 += "/";

  url_str1 += word1;

  if (url.getIsDir())
    url_str1 += "/";

  return url_str1;
}

bool
CWebGetUrl::
urlToFilename(const CUrl &url, std::string &filename)
{
  filename = webget_->getBaseDir() + "/" + url.getFile();

  if (! url.getIsDir()) {
    CFile file(filename);

    if (file.exists() && file.isDirectory()) {
      if (filename[filename.size() - 1] != '/')
        filename += "/";

      filename += "index.html";

      CFile file1(filename);

      filename = file1.getPath();
    }
    else
      filename = file.getPath();
  }
  else {
    filename += "/dummy.html";

    CFile file(filename);

    filename = file.getPath();
  }

  return true;
}

void
CWebGetUrl::
setCurrentSite() const
{
  CUrl::setCurrentSite(url_.getSite());
}
