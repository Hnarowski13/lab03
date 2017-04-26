#include "VideoPlayer.h"
#include <cstring>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <unistd.h>

bool VideoPlayer::initialized = false;

namespace {

enum PIPE_HALF {PIPE_OUT = 0, PIPE_IN = 1};

void handleNewDecoderPad(GstElement* decoder, GstPad* new_pad,
    gboolean ignored, gpointer videoHookPtr) {
  GstElement* videoHook = (GstElement*)videoHookPtr;

  // Examine the new output pad created by the decoder, and make sure it's
  // video-related.
  GstCaps* newPadCaps = gst_pad_get_caps(new_pad);
  GstStructure* capsStruct = gst_caps_get_structure(newPadCaps, 0);
  gboolean isVideo =
      g_str_has_prefix(gst_structure_get_name(capsStruct), "video");
  gst_caps_unref(newPadCaps);

  if (!isVideo) {
    return;
  }

  // Try actually connect the new decoder pad.
  GstPad* sinkPad = gst_element_get_static_pad(videoHook, "sink");
  if (sinkPad == NULL) {
    g_printerr("Video sink is bad; could not set up output.\n");
    return;
  }

  GstPadLinkReturn result = gst_pad_link(new_pad, sinkPad);
  if (result != 0) {
    g_printerr("Error linking video output (code %d).\n", result);
  }
}

} // end of namespace


VideoPlayer::VideoPlayer() : pipeline(NULL), bus(NULL) {
  memset(pipeHalves, 0, sizeof(pipeHalves));
  memset(queues, 0, sizeof(queues));
}


VideoPlayer::~VideoPlayer() {
  if (!bus) {
    gst_object_unref(bus);
  }

  if (!pipeline) {
    // Make sure anything playback-related is stopped before we
    // start throwing out data.
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
  }

  if (pipeHalves[PIPE_OUT] != 0) {
    close(pipeHalves[PIPE_OUT]);
  }
  if (pipeHalves[PIPE_IN] != 0) {
    close(pipeHalves[PIPE_IN]);
  }
}


void VideoPlayer::initialize() {
  if (!initialized) {
    // We're not using gstreamer's option parsers. Give it some
    // garbage options. Create some garbage args.
    static const char* EXE_NAME_SRC = "Video Player";
    int fakeStrLen = strlen(EXE_NAME_SRC) + 1;

    char* fakeExeName = new char[fakeStrLen];
    strncpy(fakeExeName, EXE_NAME_SRC, fakeStrLen);
    char** fakeArgv = new char*[2];
    fakeArgv[0] = fakeExeName;
    fakeArgv[1] = NULL;

    int fakeArgc = 1;

    // feed the garbage args to it
    gst_init(&fakeArgc, &fakeArgv);
    initialized = true;

    delete [] fakeExeName;
    delete [] fakeArgv;
  }
}

VideoPlayer* VideoPlayer::create() {
  // Make sure we're initialized before we try to do anything.
  initialize();

  // Make a new video player.  Set up all of its stuff.  Make sure it
  // works.
  VideoPlayer* player = new VideoPlayer;

  if (!player->createPipe())
  {
    delete player;
    return NULL;
  }

  if (!player->createPipeline())
  {
    delete player;
    return NULL;
  }

  return player;
}


void VideoPlayer::start() {
  gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void VideoPlayer::stop() {
  gst_element_set_state(pipeline, GST_STATE_READY);
}

void VideoPlayer::pause() {
  gst_element_set_state(pipeline, GST_STATE_PAUSED);
}

bool VideoPlayer::stream(const char* data, size_t length) {
  if (checkStatus()) {
    write(pipeHalves[PIPE_IN], data, length);
    return true;
  }
  return false;
}

bool VideoPlayer::checkStatus() {
  bool stillLooking = true;
  bool okay = true;
  while (stillLooking && okay) {
    GstMessage* msg = gst_bus_pop(bus);
    if (msg != NULL) {
      if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
        okay = false;
      }
      gst_message_unref(msg);
    } else {
      stillLooking = false;
    }
  }

  return okay;
}

bool VideoPlayer::isSeekable() {
  GstQuery *query;
  gint64 start, end;
  gboolean seekEnabled;
  query = gst_query_new_seeking (GST_FORMAT_TIME);
  gst_query_unref (query);
  if (gst_element_query (pipeline, query)) {
    gst_query_parse_seeking (query, NULL, &seekEnabled, &start, &end);
    if (seekEnabled) {
      g_print ("Seeking is ENABLED from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n",
            GST_TIME_ARGS (start), GST_TIME_ARGS (end));
      return true;
    } else {
      g_print ("Seeking is DISABLED for this stream.\n");
      return false;
    }
  } else {
    g_printerr ("Seeking query failed.");
    return false;
  }
}

void VideoPlayer::waitForClose() {
  gint64 previous = -1; 
  while (checkStatus()) {
    usleep(100000);
    GstFormat fmt = GST_FORMAT_TIME;
    gint64 current = -1; 
    if (!gst_element_query_position (pipeline, &fmt, &current)) {
        g_printerr ("Could not query current position.\n");
    }
    if (previous != current) {  // the new current time is different from
                                // previous, which means the video is 
                                // still playing
      g_print ("Position %" GST_TIME_FORMAT "\r", GST_TIME_ARGS (current));
    } else {  // the new current time is the same as previous, which
              // means the player has done playing the video
      break;  // break the loop to stop waiting
    }
    previous = current;
  }
}

bool VideoPlayer::createPipeline() {
  // Create elements for each part of the pipeline.  The goal is this:
  //
  // input source -> buffer -> decoder
  // decoder >> buffer -> video format cleanup -> video output
  //
  // Note that the >> links above will need to be set up later, once the
  // decoder has determined that video content is present.
  pipeline = gst_pipeline_new("pipeline");
  GstElement* source = gst_element_factory_make("fdsrc", "source");
  GstElement* mainQueue = gst_element_factory_make("queue",
    "mainQueue");
  GstElement* decoder = gst_element_factory_make("decodebin", "decoder");
  GstElement* videoQueue = gst_element_factory_make("queue",
    "videoQueue");
  GstElement* videoColorFix = gst_element_factory_make(
    "ffmpegcolorspace", "videoColorFix");
  GstElement* videoScaleFix = gst_element_factory_make("videoscale",
    "videoScaleFix");
  GstElement* videoSink = gst_element_factory_make("ximagesink",
    "videoSink");

  // If we were unable to create any of the above elements, give up.
  // Clean up anything that won't be freed in the constructor.
  if (!(pipeline && source && mainQueue && decoder && videoQueue &&
      videoColorFix && videoScaleFix && videoSink)) {
    gst_object_unref(source);
    gst_object_unref(mainQueue);
    gst_object_unref(decoder);
    gst_object_unref(videoQueue);
    gst_object_unref(videoColorFix);
    gst_object_unref(videoScaleFix);
    gst_object_unref(videoSink);
    return false;
  }

  // Set up the input source to read from the input pipe.
  g_object_set(G_OBJECT(source), "fd", pipeHalves[PIPE_OUT], NULL);

  // Set up the main buffer to hold as much data as the client feels like
  // shoving in.
  g_object_set(G_OBJECT(mainQueue), "max-size-bytes", 0, NULL);
  g_object_set(G_OBJECT(mainQueue), "max-size-time", 0ll, NULL);
  g_object_set(G_OBJECT(mainQueue), "max-size-buffers", 0, NULL);

  // Set things up so that when the decoder finds video content, we can
  // have it linked into the pipeline automatically.
  g_signal_connect(decoder, "new-decoded-pad",
      (GCallback)(handleNewDecoderPad), videoQueue);

  // Assemble the non-decoder-dependent parts of the pipeline.
  gst_bin_add_many(GST_BIN(pipeline), source, mainQueue, decoder,
      videoQueue, videoColorFix, videoScaleFix, videoSink,
      NULL);
  bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));

  gboolean linked = gst_element_link(source, mainQueue);
  linked = linked && gst_element_link(mainQueue, decoder);
  linked = linked && gst_element_link(videoQueue, videoColorFix);
  linked = linked && gst_element_link(videoColorFix, videoScaleFix);

  linked = linked && gst_element_link(videoScaleFix, videoSink);

  return linked;
}

bool VideoPlayer::createPipe() {
  return (pipe(pipeHalves) == 0);
}
