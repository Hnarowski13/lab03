#include "PlaylistEntry.h"

PlaylistEntry::PlaylistEntry(std::string const& url, unsigned int duration)
    : url(url), duration(duration) {
}

PlaylistEntry::~PlaylistEntry() {
}

std::string const& PlaylistEntry::getUrl() const {
  return url;
}

unsigned int PlaylistEntry::getDuration() const {
  return duration;
}

void PlaylistEntry::setUrl(std::string const& url) {
  this->url = url;
}

void PlaylistEntry::setDuration(unsigned int duration) {
  this->duration = duration;
}
