#ifndef __24_BIN_H__
#define __24_BIN_H__

// ---   *   ---   *   ---
// deps

  #include <iostream>
  #include <fstream>
  #include <string>

  #include "kvrnel/Style.hpp"

// ---   *   ---   *   ---
// info

class Bin {

public:

  VERSION   "2.00.1";
  AUTHOR    "IBN-3DILA";

  cx8       READ   = 0x01;
  cx8       WRITE  = 0x02;
  cx8       TRUNC  = 0x04;
  cx8       APPEND = 0x08;

  cx8       SLIMIT = 0x80;

// ---   *   ---   *   ---

private:

  cxstr     m_fsig="";
  cx64      m_header_sz=0;

  size_t    m_size;
  size_t    m_ptr;

  char*     m_fpath;

  std::ios_base::openmode m_mode;
  std::fstream m_fh;

// ---   *   ---   *   ---

  // reads mode flags
  void set_mode(char mode);

// ---   *   ---   *   ---

public:

  // open file
  Bin(std::string fpath,char mode);

  // implicit close
  ~Bin(void);

  // open, read && close
  static std::unique_ptr<char[]> orc(
    std::string fpath

  );

};

//// ---   *   ---   *   ---
//
//BIN* MKBIN     (char* path      ,
//                uint  mode      ,
//                uint  ft        );          // init bin for read/write
//                                            // -ft determines sign to check
//
//// ---   *   ---   *   ---
//
//void  NTRDBUF  (void            );          // nit read buffer
//void  NTWTBUF  (void            );          // nit write buffer
//void  DLRDBUF  (void            );          // free read buffer
//void  DLWTBUF  (void            );          // free write buffer
//
//void  LDBINTO  (BIN* b          );          // load bin into bin-to
//void  LDBINFR  (BIN* b          );          // load bin into bin-from
//
//BIN** GTBINTO  (void            );          // get bin-to
//BIN** GTBINFR  (void            );          // get bin-from
//MEM*  GTBINRD  (void            );          // get read buff
//MEM*  GTBINWT  (void            );          // get write buff
//
//// ---   *   ---   *   ---
//
//SIG   GTSIG   (uint ft         );
//
//int   RDBIN   (BIN* bin        );           // read from bin
//int   DLBIN   (BIN* bin        );           // closes a bin file
//int   RMBIN   (BIN* bin        );           // removes a bin file from disk
//int   CMPSIG  (SIG* s0, SIG* s1);           // compares two signatures
//void  STBADSIG(int is_bad      );           // alters read-state on inappropriate signature
//char* PTHBIN  (BIN* bin        );           // fetch path to bin
//
//int   BIN2BIN (uint size       );           // write size bytes from src to dst
//                                            // -bin-from is src
//                                            // -bin-to is dst
//                                            // -no rewind
//
//// ---   *   ---   *   ---
//
//#if KVR_DEBUG
//                                            /* catch if error on open                      */
//    #define BINOPEN(bin, path, mode, ft, retx) { retx = 0;                                   \
//        bin=MKBIN(path, mode, ft); ERRCATCH(RDBIN(bin), retx, 0x40,PTHBIN(bin));            }
//
//                                            /* catch if error on close                     */
//    #define BINCLOSE(bin)                   { int retx = 0;                                  \
//        ERRCATCH(DLBIN(bin), retx , 0x41, PTHBIN(bin));                                     }
//
//                                            /* catch if we don't write as many bytes       */
//    #define BINWRITE(bin, wb, type, count, buff){ wb=count;                                  \
//        ERRCATCH(fwrite(buff, sizeof(type), count, bin->file), wb, 0x42, PTHBIN(bin));       \
//        wb *= sizeof(type);                                                                 }
//
//                                            /* catch if we don't write as many bytes       */
//    #define BINWRITE_ATR(bin, wb, type, count, buff, offset){ wb=count;                      \
//        fseek(bin->file, 0, SEEK_CUR); fseek(bin->file, offset, SEEK_CUR);                   \
//        ERRCATCH(fwrite(buff, sizeof(type), count, bin->file), wb, 0x42, PTHBIN(bin));       \
//        fseek(bin->file, 0, SEEK_CUR);                                                       \
//        wb *= sizeof(type);                                                                 }
//
//                                            /* catch if we don't read as many bytes        */
//    #define BINREAD(bin, rb, type, count, buff) { rb = count;                                \
//        ERRCATCH(fread(buff, sizeof(type), count, bin->file), rb, 0x43, PTHBIN(bin));        \
//        rb *= sizeof(type);                                                                 }
//
//                                            /* empty error handle because we dont care     */
//    #define PBINREAD(bin, rb, type, count, buff) { rb = count;                               \
//        fread(buff, sizeof(type), count, bin->file); if(rb!=count && !feof(bin->file)) { ; } \
//        rb *= sizeof(type);                                                                 }
//
//    #define BINREAD_ATR(bin, rb, type, count, buff, offset) { rb = count;                    \
//        fseek(bin->file, 0, SEEK_CUR); fseek(bin->file, offset, SEEK_CUR);                   \
//        ERRCATCH(fread(buff, sizeof(type), count, bin->file), rb, 0x43, PTHBIN(bin));        \
//        fseek(bin->file, 0, SEEK_CUR);                                                       \
//        rb *= sizeof(type);                                                                 }
//
//                                            /* catch if file signatures don't match        */
//    #define BINSIG(bin, sig)                { int retx = 0;                                  \
//        ERRCATCH(CMPSIG(&bin->sign, &sig), retx, 0x44, PTHBIN(bin)); STBADSIG(retx);        }
//
//    #define BINREMOVE(bin)                  { int retx = 0;                                  \
//        ERRCATCH(RMBIN(bin), retx, 0x45, PTHBIN(bin));                                      }
//
//// ---   *   ---   *   ---
//
//#else
//    #define BINOPEN(bin, path, mode, ft, retx)                                               \
//        bin=MKBIN(path, mode, ft); retx=RDBIN(bin)
//
//    #define BINCLOSE(bin) DLBIN(bin)
//
//    #define BINWRITE(bin, wb, type, count, buff)                                             \
//        wb=fwrite(buff, sizeof(type), count, bin->file)
//
//    #define BINWRITE_ATR(bin, wb, type, count, buff, offset)                                 \
//        fseek(bin->file, 0, SEEK_CUR); fseek(bin->file, offset, SEEK_CUR);                   \
//        wb=fwrite(buff, sizeof(type), count, bin->file); fseek(bin->file, 0, SEEK_CUR)
//
//    #define BINREAD(bin, rb, type, count, buff)                                              \
//        rb=fread(buff, sizeof(type), count, bin->file)
//
//    #define BINREAD_ATR(bin, rb, type, count, buff, offset)                                  \
//        fseek(bin->file, 0, SEEK_CUR); fseek(bin->file, offset, SEEK_CUR);                   \
//        rb=fread(buff, sizeof(type), count, bin->file); fseek(bin->file, 0, SEEK_CUR)
//
//    #define PBINREAD(bin, rb, type, count, buff) { rb = count;                               \
//        fread(buff, sizeof(type), count, bin->file); if(rb!=count && !feof(bin->file)) { ; } \
//        rb *= sizeof(type);                                                                 }
//
//    #define BINSIG(bin, sig) STBADSIG(CMPSIG(&bin->sign, &sig));
//
//    #define BINREMOVE(bin) RMBIN(bin)
//
//#endif
//
//// ---   *   ---   *   ---

#endif // __24_BIN_H__
