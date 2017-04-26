/*********************************
 * PlaylistEntry - Class representing the entry for a specific video segment
 * in an extended M3U playlist. It should be considered internal to the Playlist
 * class's implementation.
 *********************************/

#ifndef _PLAYLIST_ENTRY_H_
#define _PLAYLIST_ENTRY_H_

#include <string>

class PlaylistEntry {
 public:
  /*********************************
   * Name:    PlaylistEntry, a playlist entry for a specific video segment
   * Purpose: Constructor, constructs a new PlaylistEntry
   * Receive: url - the URL to the video segment
   *          duration - duration of the video segment
   * Return:  None
   *********************************/
  PlaylistEntry(std::string const& url = "", unsigned int duration = 0);

  /*********************************
   * Name:    ~PlaylistEntry
   * Purpose: Destructor of PlaylistEntry objects
   * Receive: None
   * Return:  None
   *********************************/
  ~PlaylistEntry();

  /*********************************
   * Name:    getUrl
   * Purpose: Looks up the URL of this playlist entry
   * Receive: None
   * Return:  The URL of this playlist entry
   *********************************/
  std::string const& getUrl() const;

  /*********************************
   * Name:    getDuration
   * Purpose: Looks up the duration of a playlist entry
   * Receive: None
   * Return:  The duration of this playlist entry
   *********************************/
  unsigned int getDuration() const;

  /*********************************
   * Name:    setUrl
   * Purpose: Sets the URL of this playlist entry
   * Receive: url - the URL to set to the entry
   * Return:  None
   *********************************/
  void setUrl(std::string const& url);

  /*********************************
   * Name:    setDuration
   * Purpose: Sets the duration of this playlist entry
   * Receive: duration - the duration to set to the entry
   * Return:  None
   *********************************/
  void setDuration(unsigned int duration);

private:
  std::string url;
  unsigned duration;
};

#endif // _PLAYLIST_ENTRY_H_
