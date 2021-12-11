# KVR
 The *infamous*, fully GPL'd GNU weapon of mass-destruction, KVR or KVNSL RAT (V as in KVLT) is my personal C coding environment for desktop; this project simply contains the tools I use everywhere else, and thus it is always growing.

### What's in this repo?

 - KVRNEL, C lib for debug utils, some wrappers for stdlib (mostly malloc stuff), my hash table implementation, general purpose functions such as data-type conversions, color/grayscale/unit vector quantization, among other things.
 - ESPECTRO, a *legacy* CLI build system written entirely with python and ANSI escapes.
 - BLKMGK, a small util for packing and unpacking compressed texture data.
 - MAMMOTH, where I implement the PE$O language.
 - SIN, graphics library used here mainly for rendering text. 3D capabilities not fully ported yet (look at /Liebranca/DSM/tree/dev/src/SIN for that).
 - CHASM, SDL window manager.
 - SHB7, a minimalistic custom terminal.
 - FMAT, the dullest code-formatting tool in existence.

### Why write all of this from scratch?

 It's a mix of research and simply handrolling my own of something because no good alternative exists. Hey, I pinky promise this isn't actually an evil 16-bit OS, I would never do such a thing. *wink*

### Dependencies

 - GCC
 - MAKE
 - PYTHON3
 - SDL2
 - GLEW+GLAD
 - LIBPNG16
 - ZLIB1

 Python is a terrible language and I personally detest it; eventually all Python glue in the codebase will be removed. But until that day comes: just deal with it.

 The GLEW+GLAD mambo is a hack. SIN links against GLEW but doesn't actually include it; we use glad.h instead. That is how you open an SDL window with a drawable OpenGL context in pure Cygwin, a method apparently no one but me knows.

 Speaking of Cygwin -- if you're on Windows, you'll need it. If you're on GNU/Linux, you probably don't need the GLAD hack (untested). If you're on a Mac, switch to an actual computer.

### Building

 Assuming I didn't mess up the dependency list and your paths are appropiately set:

```
cd kvr/src
make all
```

 Or just `make <target>`, valid targets are:

 - kvrnel
 - sin
 - chasm
 - shb7
 - mamm

 `make reb` is just a `make all` with a pre-clean step.

 To turn different debug flags on and off (so that you get more accurate logs), look at /src/avtomat/avtomat.py and change the value of KVR_DEBUG. A list of valid flags to set or unset can be found in /src/kvrnel/zjc_evil.h

