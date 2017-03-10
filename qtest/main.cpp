#include <CQApp.h>
#include <CQWebGet.h>
#include <iostream>

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  QWebGet webget;

  std::vector<std::string> args;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg[0] == '-') {
      std::string opt = arg.substr(1);

      if (opt == "debug")
        webget.setDebug(true);
      else
        std::cerr << "Invalid option '" << opt << "'" << std::endl;
    }
    else
      args.push_back(arg);
  }

  if (args.size() > 0)
    webget.setText(args[0].c_str());

  return app.exec();
}
