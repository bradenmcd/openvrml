//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//

# ifndef IMAGE_H
#   define IMAGE_H
//
//  Image document class
//

# include <stddef.h>

class Doc;
class Doc2;
class VrmlMFString;

class Image {

public:

  Image(const char *url = 0, Doc *relative = 0);
  ~Image();

  bool setURL(const char *url, Doc *relative = 0);
  bool setURL(const char *url, Doc2 *relative = 0);

  bool tryURLs(size_t nUrls, char const * const * urls, Doc *relative = 0);
  bool tryURLs(size_t nUrls, char const * const * urls, Doc2 *relative = 0);
  bool tryURLs(const VrmlMFString & urls, Doc2 * relative = 0);

  const char *url();

  int w()			{ return d_w; }
  int h()			{ return d_h; }
  int nc()			{ return d_nc; }
  int nFrames()			{ return d_nFrames; }
  unsigned char *pixels()	{ return d_pixels; }
  unsigned char *pixels(int frame);

  void setSize(int w, int h)	{ d_w = w; d_h = h; }

protected:

  Doc * d_url;
  int d_w, d_h, d_nc, d_nFrames;
  unsigned char *d_pixels;
  unsigned char **d_frame;

};

#endif // IMAGE_H
