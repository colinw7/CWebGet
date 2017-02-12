#ifndef CURL_H
#define CURL_H

#include <string>
#include <map>
#include <iostream>

class CFile;

class CUrl {
 public:
  typedef std::map<std::string,std::string> SearchList;

 public:
  CUrl();

  explicit CUrl(const std::string &url);
  explicit CUrl(const CFile &file);

  bool isValid() const { return ! url_.empty(); }

  const std::string &getUrl() const { return url_; }

  const std::string &getPrefix() const { return prefix_; }
  void setPrefix(const std::string &s) { prefix_ = s; }

  const std::string &getSite() const { return site_; }

  const std::string &getFile() const { return file_; }
  void setFile(const std::string &s) { file_ = s; }

  const std::string &getLocalFile() const { return local_file_; }
  const std::string &getSuffix   () const { return suffix_    ; }
  const SearchList  &getSearches () const { return searches_  ; }
  const std::string &getTarget   () const { return target_    ; }
  const std::string &getFullUrl  () const { return full_url_  ; }
  bool               getIsDir    () const { return is_dir_    ; }

  std::string getPath() const { return site_ + "/" + file_; }

  bool getSearch(const std::string &name, bool *value) const;
  bool getSearch(const std::string &name, int *value) const;
  bool getSearch(const std::string &name, double *value) const;
  bool getSearch(const std::string &name, std::string &value) const;

  void setSearch(const std::string &name, bool value);
  void setSearch(const std::string &name, int value);
  void setSearch(const std::string &name, double value);
  void setSearch(const std::string &name, const std::string &value);

  bool isHttp() const { return (prefix_ == "http" || prefix_ == "https"); }
  bool isFile() const { return (prefix_ == "file"); }

  std::string encode() const;

  static std::string replaceSpecialChars(const std::string &str);

  void print() const;
  void print(std::ostream &os) const;

  static std::string &getCurrentSite() { return current_site_; }
  static void setCurrentSite(const std::string &s) { current_site_ = s; }

  static void setCurrentSite(const CUrl &url);

  friend bool operator==(const CUrl &url1, const CUrl &url2) {
    return (url1.url_ == url2.url_);
  }

  friend std::ostream &operator<<(std::ostream &os, const CUrl &url) {
    url.print(os);

    return os;
  }

 private:
  void decode();

  std::string encodeFile(const std::string &file) const;

 private:
  static std::string current_site_;

  std::string url_;
  std::string prefix_;
  std::string site_;
  std::string file_;
  std::string local_file_;
  std::string suffix_;
  SearchList  searches_;
  std::string target_;
  std::string full_url_;
  bool        is_dir_ { false };
};

#endif
