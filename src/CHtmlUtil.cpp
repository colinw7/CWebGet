#include <CHtmlUtil.h>
#include <CFile.h>
#include <CDir.h>
#include <COSTime.h>
#include <COSProcess.h>
#include <CStrUtil.h>
#include <cstdio>

bool
CHtmlUtil::
listDirectory(CDir &dir, CFile &output, CHtmlFileMgr &file_mgr)
{
  std::string path = dir.getName();

  output.printf("<html>\n");
  output.printf("<head>\n");
  output.printf("<title>Directory listing of %s</title>\n", path.c_str());
  output.printf("</head>\n");
  output.printf("<body>\n");
  output.printf("<h1>Directory listing of %s</h1>\n", path.c_str());
  output.printf("<pre>\n");

  size_t pos = path.rfind('/');

  std::string path1;

  if (pos != std::string::npos)
    path1 = path.substr(0, pos - 1);
  else
    path1 = path;

  output.printf("<a href='%s'>Up to higher level directory</a>\n",
                path1.c_str());

  /*------------*/

  std::string type  = "";
  std::string image = "normal.gif";

  std::vector<std::string> files;

  dir.getSortedFilenames(files);

  int num = files.size();

  int max_len = 0;

  for (int i = 0; i < num; i++) {
    int len = files[i].size();

    max_len = std::max(max_len, len);
  }

  for (int i = 0; i < num; ++i) {
    CFile file(files[i]);

    CFileType file_type = file.getType();

    type  = file_mgr.getTypeName (file_type);
    image = file_mgr.getTypeImage(file_type);

    int len = files[i].size();

    std::string time_string = COSTime::getTimeString(CFile::getMTime(files[i]));

    output.printf(" <a href='file:%s/%s'><img src='%s'>%s</a>",
                  path .c_str(), files[i].c_str(),
                  image.c_str(), files[i].c_str());

    output.printf("%*.*s %8lu %24.24s %s\n",
                  max_len - len, max_len - len, " ",
                  CFile::getSize(files[i]),
                  time_string.c_str(), type.c_str());
  }

  /*------------*/

  output.printf("<br>\n");
  output.printf("</pre>\n");
  output.printf("</body>\n");
  output.printf("</html>\n");

  return true;
}

bool
CHtmlUtil::
listImage(const std::string &filename, CFile &output)
{
  output.printf("<html>\n");
  output.printf("<head>\n");
  output.printf("<title>Image %s</title>\n", filename.c_str());
  output.printf("</head>\n");
  output.printf("<body>\n");
  output.printf("<img src='%s'>\n", filename.c_str());
  output.printf("</body>\n");
  output.printf("</html>\n");

  return true;
}

bool
CHtmlUtil::
listTextFile(const std::string &filename, CFile &output)
{
  output.printf("<html>\n");
  output.printf("<head>\n");
  output.printf("<title>%s</title>\n", filename.c_str());
  output.printf("</head>\n");
  output.printf("<body>\n");
  output.printf("<xmp>\n");

  /*------------*/

  std::vector<std::string> lines;

  CFile file(filename);

  file.toLines(lines);

  int num_lines = lines.size();

  for (int i = 0; i < num_lines; i++)
    output.printf("%s\n", lines[i].c_str());

  /*------------*/

  output.printf("</xmp>\n");
  output.printf("</body>\n");
  output.printf("</html>\n");

  return true;
}

bool
CHtmlUtil::
listBinaryFile(const std::string &filename, CFile &output)
{
  output.printf("<html>\n");
  output.printf("<head>\n");
  output.printf("<title>%s</title>\n", filename.c_str());
  output.printf("</head>\n");
  output.printf("<body>\n");
  output.printf("<xmp>\n");

  /*------------*/

  CFile file(filename);

  char chars[16];

  int num_chars = 0;

  int c;

  while ((c = file.getC()) == EOF) {
    if ((num_chars > 0 && (num_chars % 16) == 0)) {
      int i = 0;

      for ( ; i < num_chars; i++)
        output.printf("%02x ", chars[i] & 0x00FF);

      for ( ; i < 16; i++)
        output.printf("   ");

      for (i = 0; i < num_chars; i++)
        if (isprint(chars[i]))
          output.printf("%c", chars[i]);
        else
          output.printf(".");

      output.printf("\n");

      num_chars = 0;
    }

    chars[num_chars++] = c;
  }

  if (num_chars > 0) {
    int i = 0;

    for ( ; i < num_chars; i++)
      output.printf("%02x ", chars[i] & 0x00FF);

    for ( ; i < 16; i++)
      output.printf("   ");

    for (i = 0; i < num_chars; i++)
      if (isprint(chars[i]))
        output.printf("%c", chars[i]);
      else
        output.printf(".");

    output.printf("\n");
  }

  /*------------*/

  output.printf("</xmp>\n");
  output.printf("</body>\n");
  output.printf("</html>\n");

  /*------------*/

  return true;
}

bool
CHtmlUtil::
listScriptFile(const std::string &filename, CFile &output)
{
  std::string command_output;

  if (! COSProcess::executeCommand(filename, command_output))
    return false;

  /*------------*/

  std::string line;

  std::string command_output1 = CStrUtil::readLine(command_output, line);

  std::vector<std::string> words;

  CStrUtil::addWords(line, words);

  int num_words = words.size();

  CFileType type = CFILE_TYPE_TEXT_PLAIN;

  if (num_words > 0 && words[0] == "Content-type:") {
    if      (num_words > 1 && words[1] == "text/plain")
      type = CFILE_TYPE_TEXT_PLAIN;
    else if (num_words > 1 && words[1] == "text/html")
      type = CFILE_TYPE_TEXT_HTML;
  }

  /*------------*/

  std::vector<std::string> lines;

  CStrUtil::addLines(command_output1, lines);

  int num_lines = lines.size();

  /*------------*/

  output.printf("<html>\n");
  output.printf("<head>\n");
  output.printf("<title>%s</title>\n", filename.c_str());
  output.printf("</head>\n");
  output.printf("<body>\n");

  if (type == CFILE_TYPE_TEXT_PLAIN)
    output.printf("<xmp>\n");

  for (int i = 0; i < num_lines; i++)
    output.printf("%s\n", lines[i].c_str());

  if (type == CFILE_TYPE_TEXT_PLAIN)
    output.printf("</xmp>\n");

  output.printf("</body>\n");
  output.printf("</html>\n");

  /*------------*/

  return true;
}

std::string
CHtmlFileMgr::
getTypeName(CFileType file_type)
{
  if (file_type & CFILE_TYPE_IMAGE)
    return "Image";

  switch (file_type) {
    case CFILE_TYPE_INODE_DIR:
      return "Directory";
    case CFILE_TYPE_TEXT_HTML:
      return "Html Document";
    case CFILE_TYPE_APP_SH:
      return "Script";
    default:
      break;
  }

  return "";
}

std::string
CHtmlFileMgr::
getTypeImage(CFileType file_type)
{
  if (file_type & CFILE_TYPE_IMAGE)
    return "image.gif";

  switch (file_type) {
    case CFILE_TYPE_INODE_DIR:
      return "dir.gif";
    case CFILE_TYPE_TEXT_HTML:
      return "html.gif";
    case CFILE_TYPE_APP_SH:
      return "script.gif";
    default:
      break;
  }

  return "normal.gif";
}
