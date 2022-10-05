Source files must include this doc-block:

```
// ---   *   ---   *   ---
// FILENAME
// descriptor
//
// NOTICE
// Licenced under
// your clever siggy
//
// CONTRIBUTORS
// name,(=)

// ---   *   ---   *   ---
```

Your line width must be this big:

```
------------------------------------------------------
```

The indent size is 2 spaces; tabs are strictly forbidden; apologies to uncle Linus.

You'll probably want a charcount per page of around 56x28, at a maximum of 64x32;

Utilizing the [LyCon 8x8 font](https://github.com/Liebranca/ft8) is the most immersive way of editing the sources.

# NAMING

The convention is dead simple:

- `local` is all lower case.
- `m_local` if it's an attribute/member var.
- `Static` capitalized if it's visible at file-scope and beyond.
- `static` lower case if it's local, again.
- `CONST` if it's a constant, regardless of scope.

Mixed caps are generally disliked, even by me, but they serve a purpose: `This_Value` is external, and `this_value` is, again, local. Following this convention, class names, namespaces and structs are mixed-caps.

Use pointers when appropiate, declare with star to the side. If you need more than one pointer, write more than one decl.

# GENERAL ETIQUETTE

Please, use separators:

```
// ---   *   ---   *   ---
// with headers under them
// whenever needed

```

Do not write long lines, I hate them.

```

my $what=(
  0xDEADBEEF
+ 0xDDDDDDDD

);

say $what;

```

^that is just perfect.

```
If a line is too long,
  then break it up like so
  you may indent the breaks,
  I prefer it if you do

when closing, empty line and go back one indent.
```

Braces go on the line. Do this:

```
if(wat) {

};

```

Cuddling is fine.

```
if(wat) {
  ...

} else {
  ...

};

```

Ternary operators can get too long. I prefer breaking them up like so:

```
x=(very long expression)
  ? if_so
  : else_this
  ;

```

Longdecls are a mess, and that's just that. Break them up the same.

```
very long specifier oh my god
some more specs type name(

  arg,
  arg...

) {

  ...

};
```

Same pattern for conditionals and loops

```

while(

   this_condition
&& that_condition

) {

  ...

};

```

People get confused about the for. Generally, you wouldn't have to break it up, but sometimes you do. So do it.

```
for(

  very_long_type i=0;

  i!=even_longer_condition;
  i++
  

) {

  ...

};
```

#  CLASSIFY

Use constants. I love magic numbers as much as anybody, but I'd rather not guess what `0b101001101110110110111` or `0xD0921F923FE0CA` means for this particular context when I'm twenty hours into reading code. So I love named constants better.

But better than named constants: putting them inside the classes and namespaces where they're actually being used. If no one else needs to see them, then make it so.

```

class Yes {

  VERSION   "v0.00.0b";
  AUTHOR    "some-dude";

  cx64 MY_VERY_LONG_LONG=0xD046;

};

```

The macros `VERSION`, `AUTHOR` and `cx[8-64]` (also `cxr[32-64]` and `cxstr`) declare constants. Use them.

Another thing you can do inside classes and namespaces is declare typedefs, structures and unions. Yes. Your class has a helper struct? Make it specific to the class.

If the definition of your helper is too involved however, one thing that is perfectly acceptable in my book is writting it to another file and then pasting it in. Need I remind you that `#include` does exactly this?

```
class Paste_Here {

  #include "paste-file"

};

```

#
