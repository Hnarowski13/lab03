// Video_Player - Class to play back a video stream in real time.  Allows you
// to feed in successive pieces of the video stream, and have them displayed
// in a pop-up window.
//
// Does not support audio playback, to avoid issues on systems that do not
// have an audio output device (like adriatic or black).

#ifndef _VIDEO_PLAYER_H_
#define _VIDEO_PLAYER_H_

#include <cstdlib>
#include <string>

struct _GstBus;
struct _GstElement;
typedef struct _GstBus GstBus;
typedef struct _GstElement GstElement;

class VideoPlayer {
 public:
  virtual ~VideoPlayer();

  // Creates a new video player object.  There are a number of things
  // that could go wrong while doing this, which is why you have to
  // go through this method rather than allocating the object yourself.
  //
  // Returns - A new video player.  If something does go wrong, this will
  //   return NULL instead.  If that happens, and you're testing on one
  //   of the CSE Linux systems... talk to the TA.
  static VideoPlayer* create();

  // Has the video player get ready to start playing back video.  Call
  // this before you start streaming.
  void start();

  // Feeds the next part of the video stream into the video player.
  //
  // data - The buffer of video data to feed in.
  // length - The number of bytes in the given data buffer.
  bool stream(const char* data, size_t length);

  bool stream(std::string const& data) {
    return stream(data.c_str(), data.size());
  }

  // Waits until the user has closed the video window, or a playback
  // error occurs.  Call this if you don't have any more data to stream,
  // and you want to let the user watch whatever video is still playing.
  void waitForClose();


  // Checks if any playback errors have occurred.  Since the user
  // closing the video window counts as a playback error, this means
  // that if you've already finished streaming the entire video, you can
  // repeatedly poll this to wait for the user to finish watching.
  //
  // Use this if you might want to stop playback early rather than
  // waiting for the user to close the window.  If you don't have
  // anything else to do while the user is watching, you will find it
  // easier to make a single call to Wait_for_close() instead.
  //
  // Returns - true if playback is still okay; false if an error has
  //   occurred.  You will want to exit after errors.
  bool checkStatus();

  // Has the video stop playing permanently.
  void stop();

  // Has the video stop playing temporarily.
  void pause();

  bool isSeekable();

 protected:
  // Internal stuff.
  GstElement* pipeline;
  GstElement* queues[2];
  GstBus* bus;
  int pipeHalves[2];

  VideoPlayer();

  bool createPipeline();
  bool createPipe();

 private:
  static void initialize();
  static bool initialized;
};


#endif //endif _VIDEO_PLAYER_H_
