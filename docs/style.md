 Source files must include this doc-block:

```
//**************************
// FILENAME                *
// descriptor,()           *
//                         *
// NOTICE                  *
// Licenced under          *
// your clever siggy       *
//                         *
// CONTRIBUTORS            *
// name,(=)                *
//**************************
```

 Your line width must be this big:

```
------------------------------------------------------
```

 The indent size is 2. Yes, 2 spaces. You read that right. Tabs are strictly forbidden.

 You'll probably want a charcount per page of around 56x28, at a maximum of 64x32;

 Utilizing the LyCon font (provided in /KVR/data/ ) is the most immersive way of editing the sources.

 For additional style points, though this is entirely a matter of preference, you might utilize my awesome color theme: the palette.txt (provided in  /src/avtomat/ ) contains hex codes and names. And it's only 16 colors! Rejoice, simplicity.

#

 Please, use separators:

```
//   ---     ---     ---     ---     ---

Comments are okay like this // but only if you align
                            // them correctly

  // indent them like you would
  // or you'll have a bad time.
  Or like this

```

 Cuddling braces/parens is fine:

```

long long (
  0xDEADBEEF
 +0xDDDDDDDD

);short x+8;

```

 ^that is just perfect.

```
If a line is too long,
  then break it up like so
  you may indent the breaks,
  I prefer it if you do

when closing, empty line and go back one indent.
```

 DO NOT do this:

```
if(wat)
{

};
```

 I forbid it. Do this:

```
if(wat) {

};

```

 Brace on line, people. That's where it's at.

#

 Cuddling, like I said, is alright.

```

if(wat) {

} else {

};

```

 ^I have no problem with that.

#

 If you get confused:
```
python avtomat/avto-fmat.py <filepath>
```

 That prints out a 'good enough' formatting, with the project's preferences in mind.

 If fmat doesn't bug out, apply the changes like so:
```
python avtomat/avto-fmat.py <filepath> -r
```

 And that'll replace the original with the formatted version. Note that I wrote fmat myself, in less than a week: at the time of this writting, it still requires testing and tweaking.

#

