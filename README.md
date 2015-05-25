# cleanup_tools

I wanted to play around with c++ 11's regexe's, and I had a code base
being compiled with lots of old school includes.  Some people had even
taken to doing external ifdefs to reduce compile time.  So I wrote a
little tool to change the include guards from the old style:

       #ifndef some-identifier
       #define some-identifier

       /// code.

       #endif


with the more modern, widely supported

     #pragma once

# Compiling
  easy makefile provided.  Should be trivial to adapt to other compilers.
  
# Usage
  is really easy.   Here's how I use it from bash:
     find . -name "*h" | xargs cig

  