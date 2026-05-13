#ifndef WEBUTIL_H
#define WEBUTIL_H

int getText(const char *srcAddr, char *buffer, int bufSize);
int getLink(const char *srcAddr, char *link, int maxLinkLength);

#endif
