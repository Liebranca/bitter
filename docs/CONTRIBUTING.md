First and foremost, I am trapped on Windows for a variety of reasons. I cannot guarantee it works for a GNU OS, though it just might -- and I'd be happy to help anyone attempting a port in any way I can.

### So, for now:
- Check that you're also on Windows (7 or later)

### Then, understand that:
- KVR's build system depends on the GNU compiler.
- *For now* I use Mingw-w64 mainly, so producing binaries from other versions could need some fine-tunning.
- The entire interface runs on ANSI escapes. If you don't run it on a terminal capable of understanding those sequences, you'll see weird things.
- You'll have to get something like *ansicon* if you're seeing said weird things.
- Every piece of code must display the following notice in comments, right at the start of the *source* file:

```
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    FILENAME                              *
*                                           *
*     -descriptor,(=)                       *
*                                           */
// *   ---     ---     ---     ---     ---  *
/*    NOTICE                                *
*                                           *
*     Licenced under                        *
*     your clever siggy.                    *
*                                           */
// *   ---     ---     ---     ---     ---  *
/* CONTRIBUTORS                             *
*     name,(=)                              *
*                                           */
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
```

And this final license block cannot be incompatible with GPL3. Needless to say, if you `#include` a GPL'd file, then be a bro and use the same license because it's your inheritance.

### What to look for

Porting is a low priority for now. The main thing is:
- Improving the build system and it's interface
- Developing PE$O lang
- Making C libraries compatible with `zjc_evil`

And therefore, those are the things I would mainly be working on. So if you really want to help, help with those.
