/*
  Copyright 2012 Larry Gritz and the other authors and contributors.
  All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:
  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  * Neither the name of the software's owners nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  (This is the Modified BSD License)
*/


#include "imageio.h"
#include "imagebuf.h"
#include "imagebufalgo.h"
#include "sysutil.h"
#include "unittest.h"

#include <iostream>

OIIO_NAMESPACE_USING;


// Tests ImageBuf construction from application buffer
void ImageBuf_test_appbuffer ()
{
    const int WIDTH = 8;
    const int HEIGHT = 8;
    const int CHANNELS = 1;
    static float buf[HEIGHT][WIDTH] = {
        { 0, 0, 0, 0, 1, 0, 0, 0 }, 
        { 0, 0, 0, 1, 0, 1, 0, 0 }, 
        { 0, 0, 1, 0, 0, 0, 1, 0 }, 
        { 0, 1, 0, 0, 0, 0, 0, 1 }, 
        { 0, 0, 1, 0, 0, 0, 1, 0 }, 
        { 0, 0, 0, 1, 0, 1, 0, 0 }, 
        { 0, 0, 0, 0, 1, 0, 0, 0 }, 
        { 0, 0, 0, 0, 0, 0, 0, 0 }
    };
    ImageSpec spec (WIDTH, HEIGHT, CHANNELS, TypeDesc::FLOAT);
    ImageBuf A ("A", spec, buf);

    // Make sure A now points to the buffer
    OIIO_CHECK_EQUAL ((void *)A.pixeladdr (0, 0, 0), (void *)buf);

    // write it
    A.save ("A.tif");

    // Read it back and make sure it matches the original
    ImageBuf B ("A.tif");
    B.read ();
    for (int y = 0;  y < HEIGHT;  ++y)
        for (int x = 0;  x < WIDTH;  ++x)
            OIIO_CHECK_EQUAL (A.getchannel (x, y, 0),
                              B.getchannel (x, y, 0));
}



// Tests histogram computation, by computing the histogram for a known
// ImageBuf and then comparing the results with what we expect.
void histogram_computation_test ()
{
    const int WIDTH     = 10;
    const int HEIGHT    = 10;
    const int BINS      = 10;
    const int CHANNEL   = 0;

    // Create ImageBuf A for testing.
    static float buf[HEIGHT][WIDTH] = {
        { 0.09f, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0.19f, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0.29f, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0.39f, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0.49f, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0.59f, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0.69f, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0.79f, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0.89f, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.99f },
    };
    ImageSpec spec (WIDTH, HEIGHT, 1, TypeDesc::FLOAT);
    const ImageBuf A ("A", spec, buf);

    // Compute A's histogram.
    std::vector<imagesize_t> hist;
    ImageBufAlgo::histogram (A, CHANNEL, hist, BINS);

    // Are the histogram's values what we expect?
    OIIO_CHECK_EQUAL (hist.size(), BINS);
    OIIO_CHECK_EQUAL (hist[0], 91);
    for (int i = 1; i < BINS; i++) {
        OIIO_CHECK_EQUAL (hist[i], 1);
    }
}



int
main (int argc, char **argv)
{
    ImageBuf_test_appbuffer ();
    histogram_computation_test ();
    
    return unit_test_failures;
}
