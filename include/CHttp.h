#ifndef CHTTP_H
#define CHTTP_H

#include <string>

class CUrl;
class CTcp;

struct CHttpData {
  std::string type;
  std::string sub_type;
  std::string data;

  CHttpData() :
   type    ("unknown"),
   sub_type("unknown") {
  }

  bool isHtmlData() {
   return (type == "text" && sub_type == "html");
  }
};

//---

class CHttp {
 public:
  CHttp(const std::string &url);
 ~CHttp();

  bool getDebug() const { return debug_; }

  void setDebug(bool debug);
  void setTcpDebug(bool debug);

  bool download(CHttpData &data);

  bool isRelocated() const { return relocated_; }

  std::string getNewLocation() const { return newLocation_; }

 private:
  bool getMoreData(const std::string &site, std::string &more_data);
  bool getMoreData(const std::string &site, std::string &more_data, int len);

  std::string readLine(const std::string &data, std::string &line);

  void decodeLine(const std::string &line, std::string &name, std::string &value);

  void decodeContent(const std::string &value, std::string &type, std::string &sub_type);

  bool decodeDate(const std::string &value, time_t &t);

 private:
  CUrl        *url_ { nullptr };
  CTcp        *tcp_ { nullptr };
  bool         debug_ { false };
  bool         chunked_ { false };
  bool         relocated_ { false };
  std::string  newLocation_;
  time_t       lastModified_;
};

#endif
