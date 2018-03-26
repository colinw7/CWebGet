#include "std_c++.h"
#include "CTclApp/CTclApp.h"
#include "CWebGet/CWebGet.h"

#include "CTclWebGet.k"

class CTclWebGet : public CTclApp {
 private:
  CWebGet *webget_;

 public:
  CTclWebGet(int argc, char **argv);

  void addUrl(const string &url);

  string getTclStr() { return CTclWebGet_TK; }

 protected:
  void addCommands();
};

#define CTclWebGetAppCommand(N) \
class N : public CTclAppCommand { \
 private: \
  CTclWebGet *webget_; \
\
 public: \
  N(CTclWebGet *webget) : \
    CTclAppCommand(webget, #N), webget_(webget) { \
  } \
\
 protected: \
  bool proc(int argc, const char **argv); \
};

CTclWebGetAppCommand(CTclWebGetAddUrl)
CTclWebGetAppCommand(CTclWebGetExit)

int
main(int argc, char **argv)
{
  CTclWebGet app(argc, argv);
}

CTclWebGet::
CTclWebGet(int argc, char **argv) :
 CTclApp(argc, argv), webget_(NULL)
{
  init();
}

void
CTclWebGet::
addCommands()
{
  new CTclWebGetAddUrl(this);
  new CTclWebGetExit  (this);
}

void
CTclWebGet::
addUrl(const string &url)
{
  delete webget_;

  webget_ = new CWebGet(url);

  CWebGetUrl web_url(webget_, url);

  if (webget_->loadPage(web_url)) {
    vector<CWebGetUrl *> web_urls;

    if (! webget_->getWebUrls(web_url, web_urls))
      return;

    string prefix, site, file, loaded, url;

    int num_web_urls = web_urls.size();

    for (int i = 0; i < num_web_urls; ++i) {
      if (! webget_->isValidSite(*web_urls[i]))
        continue;

      if (webget_->isLoaded(*web_urls[i]))
        loaded = "Yes";
      else
        loaded = "No";

      prefix = web_urls[i]->getUrl().getPrefix();
      site   = web_urls[i]->getUrl().getSite  ();
      file   = web_urls[i]->getUrl().getFile  ();

      if (file == "")
        file = "index.html";

      string path = prefix + "://" + site;

      eval("CTclWebGetAddTreeEntry {%s} {%s} {%s} 1 {%s}",
           path.c_str(), path.c_str(), "", "");

      string::size_type pos = file.find("/");

      while (pos != string::npos) {
        string lhs = file.substr(0, pos);
        string rhs = file.substr(pos + 1);

        path += "|" + lhs;

        eval("CTclWebGetAddTreeEntry {%s} {%s} {%s} 1 {%s}",
             path.c_str(), lhs.c_str(), "", "");

        file = rhs;

        pos = file.find("/");
      }

      path += "|" + file;

      url = web_urls[i]->getUrl().getFullUrl();

      eval("CTclWebGetAddTreeEntry {%s} {%s} {%s} 0 {%s}",
           path.c_str(), file.c_str(), url.c_str(), loaded.c_str());
    }
  }
}

bool
CTclWebGetAddUrl::
proc(int, const char **argv)
{
  webget_->addUrl(argv[1]);

  return true;
}

bool
CTclWebGetExit::
proc(int, const char **)
{
  exit(0);
}
