#include <CWebGet.h>
#include <CStrUtil.h>
#include <CThrow.h>
#include <CArgs.h>
#include <cmath>

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

static const char *opts = "\
-overwrite:f \
-debug:f \
-follow_links:f \
-list_refs:f \
-site:sm \
-iterate:f";

static void loadUrl(const string &url_str, bool overwrite, bool follow_links,
                    bool debug, bool list_refs, vector<string> &sites);

int
main(int argc, char **argv)
{
  CArgs args(opts);

  args.parse(&argc, argv);

  bool           overwrite    = args.getBooleanArg   ("-overwrite");
  bool           debug        = args.getBooleanArg   ("-debug");
  bool           list_refs    = args.getBooleanArg   ("-list_refs");
  bool           iterate      = args.getBooleanArg   ("-iterate");
  bool           follow_links = args.getBooleanArg   ("-follow_links");
  vector<string> sites        = args.getStringListArg("-site");

  if (argc != 2) {
    if (argc == 1) cerr << "No URL specified" << endl;
    else           cerr << "Multiple URLs specified" << endl;
    exit(1);
  }

  string url = argv[1];

  if (iterate) {
    string::size_type pos1 = url.find('#');

    if (pos1 == string::npos)
      return 1;

    string url1 = url.substr(0, pos1);
    string url2 = url.substr(pos1);

    int count = 0;

    while (url2.size() > 0 && url2[0] == '#') {
      count++;

      url2 = url2.substr(1);
    }

    int num = (int) pow(10, count);

    for (int i = 0; i < num; i++) {
      string istr = CStrUtil::toString(i);

      while ((int) istr.size() < count)
        istr = "0" + istr;

      url = url1 + istr + url2;

      cout << "Loading " << url << endl;

      loadUrl(url, overwrite, follow_links, debug, list_refs, sites);
    }

    return 0;
  }
  else
    loadUrl(url, overwrite, follow_links, debug, list_refs, sites);

  return 0;
}

static void
loadUrl(const string &url_str, bool overwrite, bool follow_links,
        bool debug, bool list_refs, vector<string> &sites)
{
  CWebGet webget(url_str);

  webget.setOverwrite(overwrite);
  webget.setDebug    (debug);
  webget.setHttpDebug(debug);
  webget.setTcpDebug (debug);
  webget.setListRefs (list_refs);

  for (uint i = 0; i < sites.size(); i++)
    webget.addSite(sites[i]);

  try {
    CWebGetUrl web_url(&webget, url_str);

    if (webget.loadPage(web_url)) {
      if (follow_links && web_url.getProcess())
        webget.processFile(web_url);
    }
  }
  catch (const char *message) {
    cerr << message << endl;
  }
  catch (CThrow *error) {
    cerr << error->message << endl;
  }
}
