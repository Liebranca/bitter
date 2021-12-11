# THE PE$O PROGRAMMING LANGUAGE

 For a quick summary: PE$O is contextual, non-typed and memory-oriented. Let's expand on those terms because I mostly made them up myself:

 - *Contextual*, operations are scope-specific; declarations happen within a declaration scope, instructions happen within an instruction scope.

 - *Non-typed*, or every name is an address, which is just a number, and these numbers may be read in whichever way the programmer sees fit.

 - *Memory-oriented*, how the data is layed out is the whole point; making that layout transparent rather than obscured.

 Interested? Let's look at some code.

### 0. HELLO WORLD

 Programming talk without a program to look at is downright idiocy. Here is hello world in PE$O:

```
$$$$$$$$$$$$$$$$

// hello.pe

entry main;

proc  main;
  wed   str;
  buf   1;

  sow   "Hello, world!",0x0A;
  reap  0b00;

  exit  0;

```

 Let's break it down.

 - `$$$$$$$$$$$$$$$$` or the 16 bytes of money. This is a file signature meant to tell the KVR subsystems that the file is a PE$O program
 - `entry` defines where your program starts executing
 - `proc` denotes an executable memory block; this is an instruction scope
 - `wed` sets the typing mode to string; i.e. it's a global implicit cast
 - `buf` sets a file descriptor to work with. In this case it's 1 or `stdout`
 - `sow` copies a sequence of bytes (interpreted according to `wed`'s current typing mode) to an internal, general purpose buffer
 - `reap` writes the contents of the internal buffer to the current file descriptor
 - `exit` returns a program termination code.

 With a working build of the `mamm` interpreter (see section 'Building' under /docs/readme.md ), you can run this program yourself by invoking:

```
mamm -fhello.pe
```

 To get layout info, you can also add the -ml switch:

```
mamm -fhello.pe -ml
```

 That will run the program and then also print out it's bytecode structure.

 Next up, let's look at how to manage storage by means other than the default buffer.

### 1. REG

 PE$O is unorthodox, not inconsistent. You can always make the following assumptions:

 - At least 16 bytes are allocated per buffer
 - All buffers are aligned to a 16-byte boundary
 - All buffers have a size equal to a power of 2

 Additionally, and keeping up with the non-typed nature of the language, every buffer name translates to an address.

 Once again, let's look at a program to illustrate:

```
$$$$$$$$$$$$$$$$

// my_vars.pe

entry main;

reg my_vars;
  char x "################";

proc main;

  wed str;buf 1;
  sow x,0x0A;

  wed long;
  cpy x+0,0x25262728292A2B2C;
  cpy x+8,0x2D2E2F3031323334;

  wed str;
  sow x,0x0A;

  reap 0b00;
  exit 0;

```

 And let's break down the code:

 - `reg` denotes a readable and writable memory block
 - `char` declares a buffer, followed by a name and optionally a sequence of values
 - `sow` with comma `,` separated arguments copies all passed values to the internal buffer.
 - `wed long` to change the typing mode from string to 8-byte chunks. Even though `x` was declared as a `char` buffer, we can use it as any other type.
 - `cpy` copies the right-hand value to the address computed on the left-hand.

 Running the code like so,

```
mamm -fmy_vars.pe
```

 Yields the following output:

```
################
,+*)('&%43210/.-
```

 As you can see, we wrote the initial values on `x` to the internal buffer, modified those values in 8-byte chunks, and then wrote the modified `x` into the internal buffer -- and *then* printed the result.

 Buffers like `x` may only be declared inside a `reg`. `char` here is simply a hint on how to read the initialization values, but in practice `x` is simply the first address into that buffer, and it can be used in whichever way is needed.

 So PE$O does not have "typical" variables, but rather named buffers that are always of size `least multiple of 16` bytes that can hold the values the buffer is initialized with.

 If we do not provide comma `,` separated initialization values, or desire a block of a size larger than is needed for those initial values, we may do this:

```
reg rdwr;
  char2 x;

```

 `char2` gives you 2 *lines*, or blocks of 16 bytes. `char3` gives 4 lines (64 bytes), 'char4' gives 8 lines (128 bytes), and so on.

 The total buffer size is always guaranteed to be a power of two; any required bounds check is then a bitwise AND.

 Every line within the buffer starts, logically, at an address that is divisible by 16 -- just like the first address on the buffer, and every subsequent buffer we might declare inside the `reg`.

 Merely a convenience during declaration, one might pass as many comma `,` separated values to a buffer *without a specified size* as desired. Doing the following:

```
reg rdwr;
  char x 0,1,2,3,4,5,6,7,8,9,'A',
         'B','C','D','E','F,'G';

```

 Is valid PE$O and will silently promote `x` to a two-line buffer.

 In addition, there are four basic buffer types:

 - `char` (1 byte )
 - `wide` (2 bytes)
 - `word` (4 bytes)
 - `long` (8 bytes)

 And a "type-formula" which goes as follows:

```
[u if unsigned]#type#[2-24 if specifying size]
```

 Again, the `type` element only serves the function of specifying the width of each initialization value being passed; and within a `proc`, `type` of a buffer is only whatever `wed` declares it to be.

### 2. ,($*&) SEC

 On initializing large regions of memory with a given pattern, I'm always annoyed when there's no way to do it programmatically outside the body of a function. SEC is a mini-syntax to mitigate this annoyance. It works as follows:

 - `*` is the 'cursor' or current position
 - `$` is the lower bound of the buffer
 - `&` is the upper bound

 Positions can be shifted:

 - `p>N` increase position N bytes
 - `p<N` decrease position N bytes
 - `p>>` move position to upper bound
 - `p<<` move position to lower bound

 And just like so, assigned to:

 - `p=N` copies N to position
 - `=N` flood fills from lower bound to upper bound with N

 These operations can be chained by separating them with semicolons `;`. Example:

```
reg rdwr;
  char buff ,($>;&<4;=0xFF;*>>;*>;*=E2);

```

 Let's unpack that:

 - `$>` increases lower bound by 1. We are now selecting positions 1 to 15
 - `&<4` decreases upper bound by 4. We are now selecting positions 1 to 11
 - `=8` flood fills the selection {1..11} with FF
 - `*>>` moves cursor to upper bound. Cursor is now at position 11.
 - `*>` increases current position by 1. Cursor is now at position 12.
 - `*=3` sets position 12 to 3

 The resulting bytearray:

```
p:00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
v:00 FF FF FF FF FF FF FF FF FF FF FF E2 00 00 00
```

 Note that PE$O is not Brainfuck. This syntax is provided merely for convenience, it is not meant to be used for anything else but buffer flood-fills.

### 3. PROC

 We've briefly looked at `proc`; let's now go into some details.

 For a quick recap: instructions are placed inside of their own context; whereas `reg` is utilized to define storage blocks, `proc` is used to define an execution block.

 First and foremost, which typing is used by an instruction is determined by the `wed` command. You may think of `wed` as a switch for the compiler; it specifies memory usage for all instructions until another `wed` is used. The following:

```
reg rdwr;
  char x 0x00,0x00,0x00,0x00;

proc exec;
  wed word;
  cpy x,0xFFEEDDCC;

```

 Looks at the address `x` as if it were a four-byte word, and assigns it `0xFFEEDDCC` such that:

 - `[x+0]` is `0xCC`
 - `[x+1]` is `0xDD`
 - `[x+2]` is `0xEE`
 - `[x+3]` is `0xFF`

 However, this:

```
reg rdwr;
  char x 0x00,0x00,0x00,0x00;

proc exec;
  wed wide;
  cpy x,0xFFEEDDCC;

```

 Would look at `x` as a two-byte wide, and do the assignment as such:

 - `[x+0]` is `0xCC`
 - `[x+1]` is `0xDD`
 - `[x+2]` is `0x00`
 - `[x+3]` is `0x00`

 The bits that cannot fit within the size-mask of the given type are discarded. This is the basis of how `wed` works.

 With that out of the way, let's look at some actual instructions. Almost all of them follow this format:

```
op dst,src
```

 Where `op` is operation, `dst` is destination and `src` is source.

 The most basic instructions are as follows:

 - `cpy` assigns `src` to `dst`; `dst` must be an address, `src` is interpreted as a plain value.
 - `mov` assigns value of `src` to `dst` and \*clears\* `src`. Both `src` and `dst` are interpreted as addresses.
 - `wap` swaps the values of `src` and `dst`; again, both are interpreted as addresses.

 In the case of `cpy`, when duplicating values from one address to another, you can do so by dereferencing: `cpy dst,[src]`.

 Let's look at some more code:

```
$$$$$$$$$$$$$$$$

// deref.pe

entry main;

reg vars;
  char x ">Print this!";

  long y x;
  char z 0x0A;

proc main;
  wed long;
  wap z,y;

  call print_zy;
  exit 0;

proc print_zy;
  wed str;buf 1;

  sow [z],y;
  reap 0b00;

  ret;
```

 The break down:

 - We initialize `x` with a string
 - Set `y` to the address of `x`
 - Set `z` to a newline character

 And then...

 - Set the typing to 8-byte `long`
 - Swap the values within addresses `z` and `y`
 - We issue a `call` to `print_zy`. This is an unconditional jump into another `proc`, which executes until it encounters a `ret` and thus returns to the caller.

 Inside of `print_zy`...

 - Set the typing mode and target file
 - To `sow` we pass not the address of `z`, but the value within `z`! Because of the earlier `wap`, this value is now the address of `x`.
 - We also pass the address of `y`, which because of the `wap`, is now a newline character.

 If you run the following program like so, you'll get the expected output:

```
mamm -fderef.pe
>Print this!
```

 Have you come this far? Then take a breather, and experiment with these basic operations. Next up, we'll touch some more on jumps, loops and conditionals.
