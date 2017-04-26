// Simple video player client for CSE422 SS17 lab 3
#include "simpleClient.h"
#include "VideoPlayer.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[]) {
  char* filename = NULL;

  if (!parseArgs(argc, argv, &filename)) {
    return 1;
  }

  // Open the video file for playback
  std::ifstream videoIn(filename);
  if (!videoIn.good()) {  // make sure the opening is successful
    std::cout << "Unable to open file " << filename << " for playback."
              << std::endl;
    return 2;
  }

  // Make a video player instance to play the video
  VideoPlayer* player = VideoPlayer::create();
  if (!player) {
    std::cout << "Unable to create video player." << std::endl;
    return 3;
  }

  // Ask the player to get ready for play back
  player->start();

  // the program reads the files piece by piece and feeds the 
  // pieces to the player
  char dataBuffer[BUFFER_SIZE];
  while (videoIn.good()) {
    // read a piece of the file and store the piece in dataBuffer
    size_t bytesRead = videoIn.readsome(dataBuffer, sizeof(dataBuffer));
    if (bytesRead == 0) {  // reads no more from the file
      break;
    }

    // feeds the piece to the player
    player->stream(dataBuffer, bytesRead);
  }

  // The player is playing the video in another thread. The main thread
  // has to wait until it ends or until the user closes the playback window.
  player->waitForClose();

  std::cout << std::endl;
  // wait for the few second before terminating the program
  for (int i = 5; i > 0; i--) {
    std::cout << "Player closing in " << i << " seconds." << std::endl;
    sleep(1);
  }

  // Clean up.
  delete player;
  return 0;
}
