#ifndef CTCP_H
#define CTCP_H

#include <string>
#include <sys/types.h>

#define CTCP_READ_BUFFER_SIZE 512

class CTcp {
 public:
  CTcp();
 ~CTcp();

  void setDebug(bool debug);
  bool getDebug() const { return debug_; }

  bool connect(const std::string &hostname, unsigned short port=80);

  bool write(const std::string &str);
  bool putc(char c);

  bool getc(char *c);
  bool gets(std::string &str, int max_len = -1);

  bool read(std::string &str);
  bool read(std::string &str, uint len);

  bool close();

 private:
  bool inetConnect(const std::string &hostname, unsigned short port);

 private:
  int  tcp_socket_;
  bool debug_;
};

#endif
