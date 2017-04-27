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
  URL* segmentUrl = NULL;
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

  //std::cout << "Attempting connect to client socket..." << std::endl;

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

  //std::cout << "Attempting send the request to client socket..." << std::endl;
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

  //delete request;  // We do not need it anymore
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
  //std::cout << "Attempting to receive the response header " << std::endl;
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
 // std::cout << "Checking status codes..." << std::endl;
  int status_code = response->getStatusCode();
  if (status_code == 404)
  {
	  std::cout << "Request failed: " << std::endl;
    std::cout << "404 NOT FOUND" << std::endl;
        std::cout << "Aborting download " << std::endl;
           std::cout << "Unable to download/parse playlist at " << playlistUrlStr<< std::endl;
        delete response;
    delete playlistUrl;
    exit(1);
  }
  else if (status_code == 403)
  {
	    std::cout << "Request failed: " << std::endl;
    std::cout << "403 FORBIDDEN " << std::endl;
    std::cout << "Aborting download " << std::endl;
std::cout << "Unable to download/parse playlist at " << playlistUrlStr<< std::endl;
    
        delete response;
    delete playlistUrl;
    exit(1);
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
 // std::cout << "Attempting to parse playlist object from responseBody" << std::endl;
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

  // Make a video player instance to play the video
  VideoPlayer* player = VideoPlayer::create();
  if (!player) {
    std::cout << "Unable to create video player." << std::endl;
    return 3;
  }


  // For each video segment in the Playlist object, download the video segment
  // as over HTTP, as if the video segments are HTTP reference objects
  // Note:
  //  - Make sure each segment is downloaded successfully. If not, show some
  //    error messages and exit the program is fine.
  unsigned int segments = plist->getNumSegments();
  std::string segmentUrlStr;
  std::string printBuffer;
  response = NULL;
  request = NULL;
  // loop through segments
std::cout << "Playlist has " << segments << " segments."
 << std::endl;
 player->start();
  for (unsigned int c = 0; c < plist->getNumSegments(); c++)
  {
    responseBody = "";
    responseHeader = "";
    std::cout << "Fetching segment "<< c+1 << std::endl;
    segmentUrlStr = plist->getSegmentUrl(c);
    segmentUrl = URL::parse(segmentUrlStr);
   // std::cout << "Parsing segment URL: " << segmentUrlStr << std::endl;
    if (!segmentUrl) {  // If URL parsing is failed
      std::cerr << "Unable to parse segment host address: " << segmentUrlStr
                << std::endl;
      helpMessage(argv[0], std::cout);
      exit(1);
    }


  //std::cout << "Attempting connect to client socket..." << std::endl;

    try {
      clientSock.Connect(*segmentUrl);  // Connect to the target server.
    } catch(std::string msg) {
      // Give up if sock is not created correctly.
      std::cerr << msg << std::endl;
      std::cerr << "Unable to connect to server: "
                << segmentUrl->getHost() << std::endl;
      delete segmentUrl;
      exit(1);

    }  

    /***SEND THE REQUEST TO THE SERVER***/
    // Send a GET request for the specified file.
    // No matter connecting to the server or the proxy, the request is
    // alwasy destined to the server.
    request = HTTPRequest::createGetRequest(segmentUrl->getPath());
    request->setHost(segmentUrl->getHost());
    // set this request to non-persistent.
    request->setHeaderField("Connection", "close");
    // For real browsers, If-Modified-Since field is always set.
    // if the local object is the latest copy, the browser does not
    // respond the object.
    request->setHeaderField("If-Modified-Since", "0");
   // std::cout << "Sending segment request to client socket... " << std::endl;
    try {  // send the request to the sock
      request->send(clientSock);
    } catch(std::string msg) {  // something is wrong, send failed
      std::cerr << msg << std::endl;
      exit(1);
    }

  //delete request;  // We do not need it anymore
  /***END OF SENDING REQUEST***/  
  //std::cout << "Attempting to receive response header from downloaded segment..." << std::endl;
    try {
      response->receiveHeader(clientSock, responseHeader, responseBody);
    } catch (std::string msg) {
      std::cerr << msg << std::endl;
      delete segmentUrl;
      delete response;
      exit(1);
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
      delete segmentUrl;
      exit(1);
    }

	
   // response->print(printBuffer);

    // output the response header
  //  std::cout << std::endl << "Response header received" << std::endl;
  //  std::cout << "=========================================================="
  //            << std::endl;
  //  std::cout << printBuffer.substr(0, printBuffer.length() - 4) << std::endl;
  //  std::cout << "=========================================================="
   //           << std::endl;
    // get the response as a std::string
    // response->print(printBuffer);
    int status_code = response->getStatusCode();
    if (status_code != 200)
    {
      std::cerr << "Unable to properly download segment" << std::endl;
      std::cerr << "Status code: " << status_code << std::endl;
      delete response;
      delete playlistUrl;
      exit(1);      
    }
    int bytesWritten = 0, bytesLeft;
    std::string fullContent="";
    
    response->setHeaderField("Transfer-encoding", "default");
  //std::cout << "Default transfer encoding" << std::endl;
//std::cout << "Content-length: " << response->getContentLen() << std::endl;
    bytesLeft = response->getContentLen();
   //  std::cout << "Content String size before: "<<fullContent.length() << std::endl;
    do {
      // If we got a piece of the file in our buffer for the headers,
      // have that piece written out, so we don't lose it.
      fullContent += responseBody;
      bytesWritten += responseBody.length();
      bytesLeft -= responseBody.length();

    //  std::cout << "bytes written:" <<  bytesWritten << std::endl;
    //  std::cout << "data gotten:" <<  responseBody.length() << std::endl;

      responseBody.clear();
      try {
        // Keeps receiving until it gets the amount it expects.
        response->receiveBody(clientSock, responseBody, bytesLeft);
      } catch(std::string msg) {
        // something bad happend
        std::cerr << msg << std::endl;
        // clean up
        delete response;
        delete segmentUrl;
        //return false;
       // clientSock.Close();
        exit(1);
      }
    } while (bytesLeft > 0);
  //  std::cout << "Content String size after: "<<fullContent.length() << std::endl;
    response->setContent(fullContent);
    
 // }
     
    
    if (!player->stream(fullContent))
    {
      std::cerr << "Unable to properly play segment" << std::endl;
      delete response;
      delete playlistUrl;
      exit(1);             
    }

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
  delete playlistUrl;
  delete segmentUrl;
  return 0;
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
