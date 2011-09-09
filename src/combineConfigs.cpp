/*
File: combineConfigs.cpp
Author: Samuel Barrett
Description: combine a set of json configs into 1 file
Created:  2011-09-09
Modified: 2011-09-09
*/

#include <string>
#include <fstream>
#include <iostream>
#include <common/Util.h>

int main(int argc, const char *argv[])
{
  char usage[] = "Usage: combineConfigs destFilename config1 [config2 ...]";

  if (argc == 2) {
    std::string arg = argv[1];
    if ((arg == "-h") || (arg == "--help")) {
      std::cout << usage << std::endl;
      return 0;
    }
  }

  if (argc < 3) {
    std::cerr << "Incorrect number of arguments" << std::endl;
    std::cerr << usage << std::endl;
    return 1;
  }

  std::ofstream destFile(argv[1]);
  if (! destFile.good()) {
    std::cerr << "Problem opening destination file: " << argv[1] << std::endl;
    return 2;
  }

  Json::Value options;
  for (int i = 2; i < argc; i++) {
    readJson(argv[i],options);
  }
  Json::StyledStreamWriter writer("  ");
  writer.write(destFile,options);
  destFile.close();

  return 0;
}
