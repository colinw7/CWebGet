#include <CTcp.h>
#include <COSRead.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <unistd.h>

#define CTCP_WRITE_WAIT 5
#define CTCP_READ_WAIT  5

CTcp::
CTcp() :
 tcp_socket_(-1), debug_(false)
{
}

CTcp::
~CTcp()
{
  close();
}

void
CTcp::
setDebug(bool debug)
{
  debug_ = debug;
}

bool
CTcp::
connect(const std::string &hostname, ushort port /* 80 */)
{
  if (! inetConnect(hostname, port))
    return false;

  return true;
}

bool
CTcp::
getc(char *c)
{
  static char  read_buffer3[CTCP_READ_BUFFER_SIZE + 1];
  static uint  read_buffer3_len;
  static char *read_buffer3_ptr;

  if (read_buffer3_len > 0) {
    --read_buffer3_len;

    *c = *read_buffer3_ptr++;

    return true;
  }

  int num_read = 0;

  while (true) {
    if (! COSRead::wait_read(tcp_socket_, CTCP_READ_WAIT))
      break;

    num_read = ::read(tcp_socket_, read_buffer3, CTCP_READ_BUFFER_SIZE);

    if (num_read == -1 && errno == EINTR)
      continue;

    if (getDebug())
      std::cerr << "Read " << num_read << " bytes" << std::endl;

    if (num_read <= 0)
      break;

    read_buffer3_ptr = read_buffer3;
    read_buffer3_len = num_read;

    --read_buffer3_len;

    *c = *read_buffer3_ptr++;

    return true;
  }

  if (num_read < 0)
    return false;

  *c = '\0';

  return true;
}

bool
CTcp::
gets(std::string &str, int max_len)
{
  char c;

  while (true) {
    if (! getc(&c))
      return false;

    if (c == '\0')
      break;

    if (c == '\r')
      continue;

    if (c == '\n')
      break;

    str += c;

    if (max_len > 0 && (int) str.size() == max_len)
      return true;
  }

  return true;
}

bool
CTcp::
read(std::string &str)
{
  static char read_buffer1[CTCP_READ_BUFFER_SIZE + 1];

  str = "";

  int num_read   = 0;
  int total_read = 0;

  while (true) {
    if (! COSRead::wait_read(tcp_socket_, CTCP_READ_WAIT))
      break;

    num_read = ::read(tcp_socket_, read_buffer1, CTCP_READ_BUFFER_SIZE);

    if (num_read == -1 && errno == EINTR)
      continue;

    if (getDebug())
      std::cerr << "Read " << num_read << " bytes" << std::endl;

    if (num_read <= 0)
      break;

    read_buffer1[num_read] = '\0';

    str += std::string(read_buffer1, num_read);

    total_read += num_read;
  }

  if (getDebug())
    std::cerr << "Total " << total_read << " bytes" << std::endl;

  if (num_read < 0)
    return false;

  return true;
}

bool
CTcp::
read(std::string &str, uint len)
{
  static char *read_buffer2;
  static uint  read_buffer2_max;

  if (len > read_buffer2_max) {
    delete [] read_buffer2;

    read_buffer2_max = len + 256;

    read_buffer2 = new char [read_buffer2_max];
  }

  str = "";

  int num_read   = 0;
  int total_read = 0;

  while (true) {
    if (! COSRead::wait_read(tcp_socket_, CTCP_READ_WAIT))
      break;

    num_read = ::read(tcp_socket_, read_buffer2, len);

    if (num_read == -1 && errno == EINTR)
      continue;

    if (getDebug())
      std::cerr << "Read " << num_read << " bytes" << std::endl;

    if (num_read <= 0)
      break;

    read_buffer2[num_read] = '\0';

    str += std::string(read_buffer2, num_read);

    total_read += num_read;

    len -= num_read;

    if (len <= 0)
      break;
  }

  if (getDebug())
    std::cerr << "Total " << total_read << " bytes" << std::endl;

  if (num_read < 0)
    return false;

  return true;
}

bool
CTcp::
write(const std::string &str)
{
  int num_written = 0;

  int pos = 0;
  int len = str.size();

  const char *p = str.c_str();

  while (len > 0) {
    if (! COSRead::wait_write(tcp_socket_, CTCP_WRITE_WAIT))
      break;

    num_written = ::write(tcp_socket_, &p[pos], len);

    if (num_written == -1 && errno == EINTR)
      continue;

    if (getDebug())
      std::cerr << "Wrote " << num_written << " bytes" << std::endl;

    if (num_written <= 0)
      break;

    pos += num_written;
    len -= num_written;
  }

  if (len > 0)
    return false;

  return true;
}

bool
CTcp::
putc(char c)
{
  int num_written;

  bool written = false;

  while (! written) {
    if (! COSRead::wait_write(tcp_socket_, CTCP_WRITE_WAIT))
      break;

    num_written = ::write(tcp_socket_, &c, 1);

    if (num_written == -1 && errno == EINTR)
      continue;

    if (getDebug())
      std::cerr << "Wrote " << num_written << " bytes" << std::endl;

    if (num_written <= 0)
      break;

    written = true;
  }

  if (! written)
    return false;

  return true;
}

bool
CTcp::
close()
{
  if (tcp_socket_ != -1)
    ::close(tcp_socket_);

  tcp_socket_ = -1;

  return true;
}

bool
CTcp::
inetConnect(const std::string &hostname, ushort port)
{
#if 0
  // open socket
  tcp_socket_ = ::socket(AF_INET, SOCK_STREAM, 0);

  if (tcp_socket_ == -1) {
    perror("socket");
    return false;
  }

  //----

  // get host
  ulong addr = (ulong) inet_addr(hostname.c_str());

  struct hostent *hostent;

  if ((int) addr != -1)
    hostent = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
  else
    hostent = gethostbyname(hostname.c_str());

  if (! hostent) {
    std::cerr << "gethostbyname failed for " << hostname << " : " <<
                 hstrerror(h_errno) << std::endl;
    return false;
  }

  if (getDebug()) {
    std::cerr << "host name : " << hostent->h_name << std::endl;

    std::cerr << "  aliases :";

    for (char **pp = hostent->h_aliases; *pp != NULL; pp++)
      std::cerr << " " << *pp;

    std::cerr << std::endl;
  }

  //----

  // connect
  for (char **pp = hostent->h_addr_list; *pp != NULL; pp++) {
    struct sockaddr_in sockaddr;

    memset(&sockaddr, 0, sizeof(struct sockaddr_in));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port   = htons(port);

    memcpy(&sockaddr.sin_addr, *pp, hostent->h_length);

    if (::connect(tcp_socket_, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) != -1)
      return true;
  }

  return false;
#else
  // lookup hosts which will accept out connection
  char portStr[32];

  sprintf(portStr, "%d", port);

  struct addrinfo hints;

  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_canonname = NULL;
  hints.ai_addr      = NULL;
  hints.ai_next      = NULL;
  hints.ai_family    = AF_UNSPEC /*AF_INET, AF_INET6*/;
  hints.ai_socktype  = SOCK_STREAM /*can we use SOCK_DGRAM*/;

  struct addrinfo *result;

  int rc = getaddrinfo(hostname.c_str(), portStr, &hints, &result);

  if (rc != 0) {
    std::cerr << gai_strerror(rc) << std::endl;
    return false;
  }

  // loop through hosts and try to connect to each one
  for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
    tcp_socket_ = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    // try next
    if (tcp_socket_ == -1)
      continue;

    // success
    if (::connect(tcp_socket_, rp->ai_addr, rp->ai_addrlen) != -1) {
      if (getDebug()) {
        char host[NI_MAXHOST], service[NI_MAXSERV];

        int rc = getnameinfo(rp->ai_addr, rp->ai_addrlen, host, NI_MAXHOST,
                             service, NI_MAXSERV, NI_NUMERICSERV);

        if (rc != -1)
          std::cerr << "connected to " << host << ":" << service << std::endl;
      }

      break;
    }

    // failed
    ::close(tcp_socket_);

    tcp_socket_ = -1;
  }

  freeaddrinfo(result);

  if (tcp_socket_ == -1)
    return false;

  return true;
#endif
}
