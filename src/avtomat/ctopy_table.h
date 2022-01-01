#ifndef CTOPY
#define CTOPY

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {

  const char* c_name;
  const char* py_name;

  // TODO: add convertion methods
  const char* pytoc;
  // const char* ctopy;

} TRTOK; static TRTOK trtoks[]={

  "void*",  "voidstar","vstar(",
  "void",   "None", NULL,
  "char**", "star(charstar)","mcstrp(",
  "char*",  "charstar","mcstr(",
  "size_t", "size_t","size_t(",

  "uchar",  "uchar","uchar(",
  "ushort", "ushort","ushort(",
  "uint",   "uint","uint(",
  "ulong",  "ulong","ulong(",

  "char",   "schar","schar(",
  "short",  "sshort","sshort(",
  "int",    "sint","sint(",
  "long",   "slong","slong(",

  "float*", "star(c_float)","mstar(c_float,",
  "float",  "c_float","c_float("

};static const int trtok_len=(
  sizeof(trtoks)/sizeof(TRTOK)

);

#ifdef __cplusplus
}
#endif

#endif // CTOPY