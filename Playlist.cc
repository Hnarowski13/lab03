#include "Playlist.h"
#include <cstdlib>
#include <sstream>

namespace {
  const std::string GARBAGE_URL = "";

  const std::string PLAYLIST_HEADER = "#EXTM3U";
  const std::string SEGMENT_TAG = "#EXTINF:";
  const std::string END_TAG = "#EXT-X-ENDLIST";
}

Playlist::Playlist() {
}

Playlist::~Playlist() {
}

Playlist* Playlist::parse(const char* data, unsigned int length) {
  // Make sure there's a proper header in the given data.  If not, don't
  // even try.
  if (!verifyHeader(data, length)) {
    return NULL;
  }

  // Read in playlist information until we can't get any more.  Pool it
  // all together.
  Playlist* playlist = new Playlist;
  bool possiblyMore = true;
  while (possiblyMore) {
    possiblyMore = readNextSegment(data, length, playlist);
  }

  return playlist;
}

Playlist* Playlist::parse(std::string const& data) {
  return parse(data.c_str(), data.length());
}

unsigned int Playlist::getNumSegments() const {
  return segments.size();
}

unsigned int Playlist::getSegmentDuration(unsigned int segment) const {
  if (segment < getNumSegments()) {
    return segments[segment].getDuration();
  } else {
    return 0;
  }
}

std::string const& Playlist::getSegmentUrl(unsigned int segment) const {
  if (segment < getNumSegments()) {
    return segments[segment].getUrl();
  } else {
    return GARBAGE_URL;
  }
}

bool Playlist::verifyHeader(const char*& data, unsigned int& length) {
  std::string headerLine;
  readUpTo(data, length, '\n', headerLine);

  return ((headerLine == PLAYLIST_HEADER) && (length != 0));
}


bool Playlist::readNextSegment(const char*& data, unsigned int& length,
    Playlist* outPlaylist) {
  // Read lines out of the buffer repeatedly.  Keep going until we hit
  // the end of the playlist or we finally read a segment.
  std::string line;
  bool endOfList = false;
  bool foundSegment = false;
  bool expectingUrl = false;
  while (!(endOfList || foundSegment) && (length > 0)) {
    // Pull in the next line.
    readUpTo(data, length, '\n', line);

    // If the line has the end-of-playlist tag on it, don't bother
    // reading anything else.
    if (line.substr(0, END_TAG.length()) == END_TAG) {
      endOfList = true;
    }

    // If the line starts with the segment indicator tag, read it,
    // the duration that follows it, and the URL on the next line.
    // If it looks like something fishy's happening, bail.
    if (line.substr(0, SEGMENT_TAG.length()) == SEGMENT_TAG) {
      // The duration should be between the tag and a comma.
      // Make sure the comma's there.
      line.erase(0, SEGMENT_TAG.length());
      size_t commaPos = line.find(',');
      if (commaPos != std::string::npos) {
        // Grab the duration & URL.  Add them as the
        // next entry in the playlist.  Make sure the
        // URL's kosher first, though.
        unsigned int duration = atoi(line.c_str());

        readUpTo(data, length, '\n', line);
        if ((line.length() > 0) && (line[0] != '#')) {
          PlaylistEntry entry(line, duration);
          outPlaylist->segments.push_back(entry);
          foundSegment = true;
        } else if (line.substr(0, END_TAG.length()) == END_TAG) {
          // Toss in this check why not.
          endOfList = true;
        }
      }
    }

    // Since we don't support any other tags...  don't do anything
    // if the line starts with anything else.
  }

  // Double-check that the loop didn't exit because we hit the end of
  // the file.
  return !endOfList && (length > 0);
}

void Playlist::readUpTo(const char*& data, unsigned int& length,
    char delimiter, std::string& output) {
  // Clear whatever is in the output string.
  output.clear();

  // Nothing to read from
  if (length == 0) {
    return;
  }

  // Read up until we reach the delimiter, or we hit the end of the data,
  // whichever comes first.
  // 
  // Put all characters other than the delimiter to the output std::string.
  // Be sure to read at least one character every time so that it's possible 
  // to advance beyond a sequence of several delimiters in a row.
  const char* dataEnd = data + length;
  char test;
  do {
    test = *data;
    data++;
    length--;

    if (test != delimiter) {
      output += test;
    }
  } while ((test != delimiter) && (data < dataEnd));
}
