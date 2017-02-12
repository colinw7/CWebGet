#ifndef CWEBGET_H
#define CWEBGET_H

#include <CUrl.h>
#include <string>
#include <vector>
#include <map>

class CFile;
class CThreadArray;
class CThreadMutex;
class CWebGetUrl;

class CWebGet {
 private:
  typedef std::vector<std::string>           SiteList;
  typedef std::map<std::string,int>          LoadedUrlsMap;
  typedef std::map<std::string,CWebGetUrl *> WebUrlMap;

 public:
  typedef std::vector<CWebGetUrl *> WebUrlList;

 public:
  CWebGet(const std::string &str);
  CWebGet(const CUrl &url);

 ~CWebGet();

  void setDebug(bool debug);
  void setHttpDebug(bool debug);
  void setTcpDebug(bool debug);

  void setOverwrite(bool overwrite);
  void setListRefs(bool list_refs);
  void setUseThreads(bool use_threads);

  const std::string &getBaseDir() const { return base_dir_; }

  void addSite(const CUrl &url);
  void addSite(const std::string &site);

  void loadSubPage(CWebGetUrl &web_url);

  bool tryLoadPage(CWebGetUrl &web_url);

  bool loadPage(CWebGetUrl &web_url);

  bool getPage(CWebGetUrl &web_url);

  bool processFile(CWebGetUrl &web_url);

  bool getWebUrls(CWebGetUrl &web_url, WebUrlList &web_urls);

  bool isValidSite(CWebGetUrl &web_url);

  bool isLoaded(CWebGetUrl &web_url);

 private:
  void init();

  bool download(CWebGetUrl &web_url, CFile *file);

  void listRef(CWebGetUrl &web_url);

  bool isHigherDir(const std::string &dir);

  bool countDirLevels(const std::string &dir);

  bool isSkipDownload(CWebGetUrl &web_url);

  void setLoaded(CWebGetUrl &web_url);

  bool isHtmlFile(CWebGetUrl &web_url);

 private:
  CUrl          base_url_;
  bool          debug_ { false };
  bool          http_debug_ { false };
  bool          tcp_debug_ { false };
  bool          overwrite_ { false };
  bool          list_refs_ { false };
  bool          use_threads_ { false };
  std::string   base_dir_;
  SiteList      sites_;
  LoadedUrlsMap loaded_urls_;
  CThreadArray* thread_array_ { nullptr };
  CThreadMutex* mutex_ { nullptr };
};

//------

class CWebGetUrl {
 public:
  CWebGetUrl(CWebGet *webget, const CUrl &url);
 ~CWebGetUrl();

  CWebGet *getWebGet() const { return webget_; }

  const CUrl &getUrl() const { return url_; }

  const std::string &getUrlStr() const { return url_str_; }

  const std::string &getFilename() const { return filename_; }

  CFile *getFile() const { return file_; }

  bool getProcess() const { return process_; }

  void setProcess(bool process) { process_ = process; }

  std::string getBaseDir() const;

  std::string encodeUrl(const CUrl &url);

  bool urlToFilename(const CUrl &url, std::string &filename);

  void setCurrentSite() const;

 private:
  CWebGet*    webget_ { nullptr };
  CUrl        url_;
  std::string url_str_;
  std::string filename_;
  CFile*      file_ { nullptr };
  bool        process_ { false };
};

#endif
