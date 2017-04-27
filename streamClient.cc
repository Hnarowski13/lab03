// Example driver/solution for Lab 4.

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "Playlist.h"
#include "URL.h"
#include "VideoPlayer.h"
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
  HTTPResponse* response = NULL;
  Playlist *plist = NULL;
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

  /***SEND THE REQUEST TO THE SERVER***/
  // Send a GET request for the specified file.
  // No matter connecting to the server or the proxy, the request is
  // alwasy destined to the server.
  request = HTTPRequest::createGetRequest(playlistUrl->getPath());
  request->setHost(playlistUrl->getHost());
  // set this request to non-persistent.
  request->setHeaderField("Connection", "close");
  // For real browsers, If-Modified-Since field is always set.
  // if the local object is the latest copy, the browser does not
  // respond the object.
  request->setHeaderField("If-Modified-Since", "0");

  try {  // send the request to the sock
    request->send(clientSock);
  } catch(std::string msg) {  // something is wrong, send failed
    std::cerr << msg << std::endl;
    exit(1);
  }

      // get the request as a std::string
  // std::string printBuffer;
  // request->print(printBuffer);

  // output the request
  // std::cout << "Request sent..." << std::endl;
  // std::cout << "=========================================================="
  //           << std::endl;
  // std::cout << printBuffer.substr(0, printBuffer.size() - 4) << std::endl;
  // std::cout << "=========================================================="
  //           << std::endl;

  delete request;  // We do not need it anymore
  /***END OF SENDING REQUEST***/






  /***RECEIVE RESPONSE HEADER FROM THE SERVER***/
  // The server response is a stream starts with a header and then
  // the body/data. A blank line separates the header and the body/data.
  //
  // Read enough of the server's response to get all of the headers,
  // then have that response interpreted so we at least know what
  // happened.
  //
  // We create two std::strings to hold the incoming data. As described in the
  // hanout, a HTTP message is composed of two portions, a header and a body.
  std::string responseHeader, responseBody;

  // The client receives the response stream. Check if the data it has
  // contains the whole header.
  // read_header separates the header and data by finding the blank line.
  try {
    response->receiveHeader(clientSock, responseHeader, responseBody);
  } catch (std::string msg) {
    std::cerr << msg << std::endl;
  }

  // The HTTPResponse::parse construct a response object. and check if
  // the response is constructed correctly. Also it tries to determine
  // if the response is chunked transfer encoding or not.
  response = HTTPResponse::parse(responseHeader.c_str(),
                                 responseHeader.length());

  // The response is illegal.
  if (!response) {
    std::cerr << "Client: Unable to parse the response header." << std::endl;
    // clean up if there's something wrong
    delete response;
    delete playlistUrl;
    exit(1);
  }

  // get the response as a std::string
  // response->print(printBuffer);
  int status_code = response->getStatusCode();
  if (status_code == 404)
  {
    std::cout << "404 NOT FOUND BISH" << std::endl;
  }
  else if (status_code == 403)
  {
    std::cout << "403 FORBIDDEN BISH " << std::endl;
  }

  // output the response header
  // std::cout << std::endl << "Response header received" << std::endl;
  // std::cout << "=========================================================="
  //           << std::endl;
  // std::cout << printBuffer.substr(0, printBuffer.length() - 4) << std::endl;
  // std::cout << "=========================================================="
  //           << std::endl;







  // If the download succeeded, try to parse the response body as a Playlist 
  // object using Playlist::parse

  try {
    plist = Playlist::parse(responseBody);

  }
  catch(std::string msg) {  // something is wrong, send failed
    std::cerr << msg << std::endl;
    exit(1);
  }

  if (plist == NULL)
  {
    std::cout << "p list is null " << std::endl;
    delete plist;
    delete playlistUrl;
    exit(1);
  }

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

  delete playlistUrl;
  // Because the main thread (this thread) is downloading the video and is very
  // likely to end before the playback, which is handled by another thread.
  // If we let the main thread to terminate, the child thread (VideoPlayer)
  // also ends. Wait for the player to finish playback using
  // VideoPlayer::waitForClose

  // Clean up!
}
