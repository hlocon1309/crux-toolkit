#ifndef WINCRUX_H
#define WINCRUX_H

#ifdef WIN32
#include <direct.h>
#include <io.h>
#include <string.h>
#include <time.h>
#include <WinSock2.h>
#include <windows.h>
#include "windirent.h"

// Rename some functions to the windows version
#define access _access
#define drand48 rand
#define isnan _isnan
#define pclose _pclose
#define popen _popen
#define random rand
#define srandom srand
#define chdir _chdir
#define getcwd _getcwd
#define mkdir(a, b) _mkdir(a)
#define mkstemp _mktemp_s
#define sleep(x) Sleep(1000 * (x))
#define snprintf _snprintf

#undef NO_ERROR

#define R_OK 04
#define W_OK 02
#define F_OK 00

#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)

int gettimeofday(struct timeval *tv, struct timezone *tz);
char *realpath(const char * file_name, char * resolved_name);

typedef struct 
{ 
   HANDLE f; 
   HANDLE m; 
   void *p; 
} SIMPLE_UNMMAP; 

// map 'filename' and return a pointer to it.
void *stub_mmap(const char *filename, SIMPLE_UNMMAP *un);

// Release memory mapped file
void stub_unmmap(SIMPLE_UNMMAP *un);

int scandir(
  const char *dirname, 
  struct dirent ***namelist,
  int (*select)(struct dirent *),
  int (*compar)(const void *, const void *)
 );

int alphasort(const void *d1, const void *d2);

char *mkdtemp(char *temp);

#endif
#endif