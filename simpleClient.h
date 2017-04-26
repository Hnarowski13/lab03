#include <iostream>
#include <cstring>

/*********************************
 * Name:    helpMessage
 * Purpose: prints a brief usage string describing how to use the application,
 *          in case the user passes in something that just doesn't work.
 * Receive: exeName - the name of the executable
 *          out - the ostream
 * Return:  None
 *********************************/
void helpMessage(const char* exeName, std::ostream& out) {
  out << "Usage: " << exeName << " -f filename" << std::endl;
  out << "The following options are required:" << std::endl;
  out << "    -f filename" << std::endl;
  out << std::endl;
  out << "Example: " << exeName
      << " -f someVideo.mp4" << std::endl;
}

/*********************************
 * Name:    parseArgs 
 * Purpose: parse the parameters
 * Receive: argv and argc
 *          filename - the file to be played
 * Return:  True if filename is gotten, false otherwise
 *********************************/
bool parseArgs(int argc, char *argv[], char **filename) {
  for (int i = 1; i < argc; i++) {
    if ((!strncmp(argv[i], "-f", 2)) ||
       (!strncmp(argv[i], "-F", 2))) {
      *filename = argv[++i];
    } else if ((!strncmp(argv[i], "-h", 2)) ||
              (!strncmp(argv[i], "-H", 2))) {
      helpMessage(argv[0], std::cout);
      return false;
    } else {
      helpMessage(argv[0], std::cout);
      return false;
    }   
  }

  if (!filename) {
    helpMessage(argv[0], std::cout);
    return false;
  }

  return true;
}

