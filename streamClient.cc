// Example driver/solution for Lab 4.

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "Playlist.h"
#include "URL.h"
//#include "VideoPlayer.h"
#include "streamClient.h"
#include <climits>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string>
#include <unistd.h>

int main(int argc, char* argv[]) {
  char* playlistUrlStr = NULL;
  URL* playlistUrl = NULL;
  HTTPRequest* request = NULL;
  char* filename = NULL;
  HTTPResponse response = NULL;
  if (!parseArgs(argc, argv, &playlistUrlStr)) {
    return 1;
  }
TCPSocket clientSock;
  // The only difference between video streaming in HLS and the simpleClient
  // is that the simpleClient is reading a video file locally. On the other
  // hand, HLS is getting the video from a playlist that specifies a list of
  // video segments. The playlist and the video segments are obtained over
  // HTTP.

  // Both playlist and video segments are sent using default transfer
  // encoding, which means they can share some code.

  // Basically, this is combining the client program in lab 2 and the
  // simpleClient in this skeleton code. For more information, please refer
  // to the handout.

  std::cout << "Attempting to stream video from: " << playlistUrlStr
            << std::endl;

  // Parse the playlistUrlStr as a URL object. Just like what we did in
  // lab 2.
  /***PARSE THE ADDRS RECEIVED TO URL OBJECTS***/
  // Must have a server to get data from
  if (!playlistUrlStr) {
    std::cerr << "You did not specify the host address." << std::endl;
    helpMessage(argv[0], std::cout);
    exit(1);
  }

  playlistUrl = URL::parse(playlistUrlStr);
  if (!playlistUrl) {  // If URL parsing is failed
    std::cerr << "Unable to parse host address: " << playlistUrlStr
              << std::endl;
    helpMessage(argv[0], std::cout);
    exit(1);
  }            
  
  // Attempt to download the playlist through HTTP, as if the playlist is an
  // base HTML file
  // Note:
  //  - Handle 404 Not found and 403 Forbidden are required
  if (playlistUrl == NULL) {
      // response(404);
      // return false;
  } 

    try {
      clientSock.Connect(*playlistUrl);  // Connect to the target server.
    } catch(std::string msg) {
      // Give up if sock is not created correctly.
      std::cerr << msg << std::endl;
      std::cerr << "Unable to connect to server: "
                << playlistUrl->getHost() << std::endl;
      delete playlistUrl;
      exit(1);
    }  
  // If the download succeeded, try to parse the response body as a Playlist 
  // object using Playlist::parse

  // Note:
  //  - remember to delete any object that is no longer needed, for example
  //    we do not need the playlist's URL object now)
  //  - Also, make sure any parsed object is not NULL (URL, Playlist ...)

  // Get a video player. Refer to simpleClient.cc for more information

  // For each video segment in the Playlist object, download the video segment
  // as over HTTP, as if the video segments are HTTP reference objects
  // Note:
  //  - Make sure each segment is downloaded successfully. If not, show some
  //    error messages and exit the program is fine.

  // Stream the video segment (in the response body) to the player using
  // Player::stream
  // Note:
  //  - Make sure Player::stream returns true. (What if a user closes the
  //    VideoPlayer early?)


  // Because the main thread (this thread) is downloading the video and is very
  // likely to end before the playback, which is handled by another thread.
  // If we let the main thread to terminate, the child thread (VideoPlayer)
  // also ends. Wait for the player to finish playback using
  // VideoPlayer::waitForClose

  // Clean up!
}
