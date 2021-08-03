# KVR IS A BIG WIP

That first and foremost, and I am trapped on Windows for a variety of reasons. I cannot guarantee anything works on a GNU OS, though it just might -- and I'd be happy to help anyone attempting a port in any way I can.

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

And what's that? You want to help and need detailed logging?

# KVR_DEBUG

Sometimes, you need certain data on the operations themselves rather than just the result. But this means collecting data \*as\* the operations run, which is a certain volume of added operations in and of itself, and additional memory usage to boot.

Therefore, it's necessary to have the ability to simply make builds discard certain pieces of code, so that these appendices to the core function remain 'detachable'.

Certain preprocessor hooks can be used to say, "don't include this chunk on this build" -- that's our very goal here. In `/src/kvrnel/zjc_evil` we declare some flags like so,

```
                                            // logflags. enables/disables certain logmesses
#define KVR_CALOM 0x01                      // MEM
#define KVR_CALOF 0x02                      // BIN
#define KVR_CALOK 0x04                      // PROC

#define KVR_CALOS 0x08                      // report string ops
```

And then set which flags we'd like to use on the build, like so

```
#define KVR_DEBUG (                         /* combined logflags */                          \
    KVR_CALOM | KVR_CALOK | KVR_CALOS                                                        \
                                                                                             \
)
```

It is all fairly simple.

We can then except a block from being called for the current build if the appropiate flags are not set. For a practical example of this, here is skipping the memory logger on `MKMEM` and `DLMEM`

```

MEM* MKMEM(uint size, ID* id)               { void* buff = __kvrmalloc(size);

    if(buff != NULL)
    {
        MEM* m=(MEM*) buff;

        m->id=*id;
        m->fsize=size;
#if KVR_DEBUG & KVR_CALOM
        MEMCNT(m, 'A');
#endif

        return m;

    }; return NULL;                                                                         };

void DLMEM(void* p)                         {

    if(p) {
        MEM* m=(MEM*) p;
#if KVR_DEBUG & KVR_CALOM
        MEMCNT(m, 'D');
#endif
        free(p);

    }                                                                                       };
```

Just like everything else, adding more flags or debug blocks where it might be useful is an ongoing process.
