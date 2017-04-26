/*********************************
 * Playlist - Represents an extended M3U playlist, as described in Apple's HTTP
 * Live Streaming specification.  By design, this leaves out many of the
 * features of the full spec, and instead provides a convenient way to parse
 * such playlist files and see which media files they reference.
 *
 * Conceptually, a playlist is formed from a sequence of segments.  To play
 * back the playlist, each segment should be downloaded and streamed in order,
 * starting with segment 0.
 *********************************/

#ifndef _PLAYLIST_H_
#define _PLAYLIST_H_

#include "PlaylistEntry.h"
#include <string>
#include <vector>

class Playlist {
 public:
  ~Playlist();

  /*********************************
   * Name:    parse
   * Purpose: Parses the extended M3U playlist file stored in the given data
   *          buffer.
   * Receive: data - The buffer in which the playlist file is stored.
   *          length - The length of the given buffer.
   * Return:  A new Playlist that represents the buffer's contents.
   *          Returns NULL if the buffered playlist cannot be parsed.
   *********************************/
  static Playlist* parse(const char* data, unsigned length);

  /*********************************
   * Name:    parse
   * Purpose: Parses the extended M3U playlist file stored in the given string.
   * Receive: data - The string in which the playlist file is stored.
   * Return:  A new Playlist object that represents the string's contents.
   *          Returns NULL if the buffered playlist cannot be parsed.
   *********************************/
  static Playlist* parse(std::string const& data);

  /*********************************
   * Name:    getNumSegments
   * Purpose: Looks up the length of the current playlist.
   * Receive: None 
   * Returns: The number of segments in the playlist.
   *********************************/
  unsigned int getNumSegments() const;

  /*********************************
   * Name:    getSegmentDuration
   * Purpose: Gets the length of the segment at the given index in the playlist.
   * Receive: None
   * Return:  The approximate length of the given segment, in seconds.
   *          The exact length of the segment may differ slightly.
   *********************************/
  unsigned int getSegmentDuration(unsigned int segment) const;

  /*********************************
   * Name:    getSegmentUrl
   * Purpose: Gets the URL of the segment at the given index in the playlist.
   * Receive: None
   * Return:  The URL of the media file for the requested segment.
   *********************************/
  std::string const& getSegmentUrl(unsigned int segment) const;

 protected:
  /*********************************
   * Name:    Playlist
   * Purpose: Constructor to the Playlist class
   * Receive: None
   * Return:  None
   *********************************/
  Playlist();

 private:
  std::vector<PlaylistEntry> segments;

  /*********************************
   * Name:    verifyHeader
   * Purpose: Make sure the data received has the correct header of an extended
   *          m3u file
   * Receive: data - a char* to be checked if it contains athe correct header
   *          length - length of the char*
   * Return:  true if the data cotains the header, false otherwise
   *********************************/
  static bool verifyHeader(const char*& data, unsigned int& length);

  
  /*********************************
   * Name:    readNextSegment
   * Purpose: reads the next segment from the give data
   * Receive: data - a char* to be read
   *          length - length of the char*
   *          outPlaylist - the playlist to store the next segment
   * Return:  true if a next segment is found, false otherwise
   *********************************/
  static bool readNextSegment(const char*& data, unsigned int& length,
      Playlist* outPlaylist);

  /*********************************
   * Name:    readUpTo
   * Purpose: read the data until we find the delimiter
   * Receive: data  - a char* to be read
   *          length - length of the char*
   *          output - to store the chars before the delimiter
   * Return:  None
   *********************************/
  static void readUpTo(const char*& data, unsigned int& length,
      char delimiter, std::string& output);
};

#endif  // _PLAYLIST_H_
