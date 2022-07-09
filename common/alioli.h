#ifndef ALIOLI_H
#define ALIOLI_H

#define ALIOLI_VERSION "1.1"
#define PREALLOCSIZE 256
#define PRINT_AS_LINE 30
#define COLOR_MAX_BUFFER 2048
#define PRINT_DEBUG_OVERFLOW_STRING " [...]"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <stdarg.h>

#include "color.h"
#include "half.h"

// === SERIAL PORT CONTROLLER ===
// Function that printf and related will use to print
int serial_putchar(char c, FILE* f);


// === MEMORY MANAGEMENT ===

// Show available RAM
int availableRAM();

// Memory protection
unsigned short int memprotect(char *string, char *limit, size_t size, const char *key, const char *filename, const int unsigned line);

// Function to reallocate memory to concat the source text
unsigned short int strcat_realloc(char **target, size_t *targetsize, size_t *allocated, const char *source, size_t sourcesize, const char *file, unsigned int line);
unsigned short int strcat_realloc_finish(char **target, size_t *targetsize, const char *file, unsigned int line);
unsigned short int strcat_prealloc(char **target, size_t *allocated, size_t toallocate, const char *file, unsigned int line);


// === DEBUG CONTROL ===
void print_asint( const char *string, size_t string_size );
void print_ashex( const char *string, size_t string_size );
void print_asbin(const char *string, size_t string_size );
void print_debug(const char *mainname, FILE *output, unsigned short int color, unsigned short int modifier, const char *fmt, ...);


#define COLOR_NORMAL 0
#define COLOR_NOHEAD 1
#define COLOR_NOHEADER 1
#define COLOR_NOTAIL 2
#define COLOR_NOHEAD_NOTAIL 3
#define COLOR_NOHEADER_NOTAIL 3
#define COLOR_NORTC_NORMAL 10
#define COLOR_NORTC_NOHEAD 11
#define COLOR_NORTC_NOHEADER 11
#define COLOR_NORTC_NOTAIL 12
#define COLOR_NORTC_NOHEAD_NOTAIL 13
#define COLOR_NORTC_NOHEADER_NOTAIL 13

// === TYPE CONVERSORS ===

// Convert any number of characters to a decimal number and viceversa
unsigned long int chr2dec(char **source, int length);

// Check if the field is a pure number (0:empty, 1:integer, 2:double, 3:string)
unsigned short int getnumber(char* input, int *integer, double *real, char **string, unsigned short int convert);


// === STRING MANAGEMENT ===
// Split a string
char* string_dup(const char *src, size_t n);
char** bstr_split(char* string, size_t string_size, const char splitter, char *filename, size_t linenumber);
char** str_split(char* string, const char splitter, char *filename, size_t linenumber);
void strreplace(char *src, char *str, char *rep);
int bstr_replace(const char *orig, int orig_size, const char *rep, int rep_size, const char *with, int with_size, char **result, int *result_size);
const char *bstrchr (const char *string, int character, size_t length);
char* bstrstr(const char *s1, size_t l1, const char *s2, size_t l2);
char* bistrstr(const char *s1, size_t l1, const char *s2, size_t l2);
int bstrcmp(const char *s1, size_t l1, const char *s2, size_t l2);
int bistrcmp(const char *s1, size_t l1, const char *s2, size_t l2);

// Binary String
struct TBString {
    char *pointer;
    size_t size;
};
typedef struct TBString BString;

void bsprintf(BString bs);
void bssprintf(char *target, BString bs);

int bsstrcmp(BString bs1, char *st2, size_t st2_size);
int bsstrcmp2(BString bs1, char *st2);
int bsbscmp(BString bs1, BString bs2);
int bsstricmp(BString bs1, char *st2, size_t st2_size);
int bsstricmp2(BString bs1, char *st2);
int bsbsicmp(BString bs1, BString bs2);
BString bsnew(char *string, size_t size);
void bsinit(BString *bs);
int bsempty(BString bs);
void bsbsclone(BString bssource, BString *bstarget);
int bsdup(BString bsorig, BString *bscopy);

int bstoi(BString bs);
long int bstol(BString bs);
double bstod(BString bs);
char* bstos(BString bs);

// === DATE/TIME HELPERS ===
long int get_current_time (void);
unsigned long int get_current_time_with_ms (void);
int epoch2date(unsigned long int epoch, unsigned int *year, unsigned short int *month, unsigned short int *day, unsigned short int *hour, unsigned short int *minute, unsigned short int *second, unsigned short int *day_of_week, unsigned short int *day_of_year);
unsigned long int date2epoch(unsigned int year, unsigned short int month, unsigned short int day, unsigned short int hour, unsigned short int minute, unsigned short int second);


// === ENCODERS ===
byte CRC8(const byte *data, size_t dataLength);


// === MATHEMATICAL FUNCTIONS ===

// Define halffloat
typedef uint16_t halffloat;

// Convert halffloat types
float halffloat2float(halffloat hf);
halffloat float2halffloat(float f);

// Extract half float
float gethf(halffloat hf);


// === INTERNAL INFORMATION FROM THIS  LIBRARY ===
const char* alioli_version(void);              // It returns the version of the library

#endif
