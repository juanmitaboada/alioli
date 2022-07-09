#include "alioli.h"

// === SERIAL PORT CONTROLLER ===
// Function that printf and related will use to print
int serial_putchar(char c, FILE* f) {
   if (c == '\n') serial_putchar('\r', f);
   return Serial.write(c) == 1? 0 : 1;
}

// === MEMORY MANAGAMENT ===

// Show available RAM
int availableRAM() {
    return 0;
}

// Memory protection
unsigned short int memprotect(char *string, char *limit, size_t size, const char *key, const char *filename, const unsigned int line) {
    unsigned short int error=0;
    if (string+size>limit) {
        // Trying to read after limit
        print_debug("ALIOLI", stderr, CRED, 0, "\n> ERROR at %s [%s:%d] - The software is trying to read beyond the LIMIT (Probably reason: wrong file format)", key, filename, line);
        error = 1;
    }
    return error;
}

// Function to reallocate memory to concat the source text with block pre-allocation
unsigned short int strcat_realloc(char **target, size_t *targetsize, size_t *allocated, const char *source, size_t sourcesize, const char *file, unsigned int line) {
    char *temprealloc=NULL;
    unsigned short int error=0;
    // Reallocate memory for the join
    if ((*targetsize+sourcesize)>*allocated) {
        // Allocate a bigger block
        (*allocated) = max(*allocated + PREALLOCSIZE, *targetsize+sourcesize);
        temprealloc = (char*) realloc(*target, *allocated);
    } else {
        temprealloc = *target;
    }
    if (temprealloc) {
        // Reasign memory
        *target = temprealloc;
        // Add the new string
        memcpy(*target+*targetsize, source, sourcesize);
        // Recalculate size
        *targetsize += sourcesize;
    } else {
        // No memory available
        print_debug("ALIOLI", stderr, CRED, 0, "%s:%d: ERROR no memory available to strcat_realloc()!", __FILE__, __LINE__);
        // Error happened
        error = 1;
    }
    return !error;
}

// Function to reduce allocated memory for ints relloacted blocks
unsigned short int strcat_realloc_finish(char **target, size_t *targetsize, const char *file, unsigned int line) {
    char *temprealloc=NULL;
    unsigned short int error=0;

    if (targetsize) {
        if (*targetsize) {
            temprealloc = (char*) realloc(*target, sizeof(char) * (*targetsize));
            if (temprealloc) {
                // Reasign memory
                *target = temprealloc;
            } else {
                // No memory available
                print_debug("ALIOLI", stderr, CRED, 0, "%s:%d: ERROR no memory available to strcat_realloc_finish()! (called from %s:%d)", __FILE__, __LINE__, file, line);
                // Error happened
                error = 1;
            }
        } else {
            free(*target);
            (*target) = NULL;
        }
    }
    return !error;
}

unsigned short int strcat_prealloc(char **target, size_t *allocated, size_t toallocate, const char *file, unsigned int line) {
    char *temprealloc=NULL;
    unsigned short int error=0;

    if (allocated) {
        temprealloc = (char*) realloc(*target, sizeof(char) * ((*allocated)+toallocate));
        if (temprealloc) {
            // Reasign memory
            *target = temprealloc;
            // Update allocated
            (*allocated) += toallocate;
        } else {
            // No memory available
            print_debug("ALIOLI", stderr, CRED, 0, "%s:%d: ERROR no memory available to strcat_prealloc()! (called from %s:%d)", __FILE__, __LINE__, file, line);
            // Error happened
            error = 1;
        }
    }
    return !error;
}



// === DEBUG CONTROL ===
const char* get_color(unsigned char car) {
    if (car==244) {         return COLOR_SIMPLE_RED;     // F4
    } else if (car==245) {  return COLOR_SIMPLE_CYAN;    // F5
    } else if (car==246) {  return COLOR_SIMPLE_BLUE;    // F6
    } else if (car==247) {  return COLOR_SIMPLE_PURPLE;  // F7
    } else if (car==248) {  return COLOR_YELLOW;        // F8
    } else if (car==249) {  return COLOR_CYAN;          // F9
    } else if (car==250) {  return COLOR_RED;           // FA
    } else if (car==251) {  return COLOR_PURPLE;        // FB
    } else if (car==252) {  return COLOR_GREEN;         // FC
    } else if (car==253) {  return COLOR_SIMPLE_GREEN;   // FD
    } else if (car==254) {  return COLOR_CLOSE;         // FE <--- FREE
    } else if (car==255) {  return COLOR_WHITE;         // FF
    } else {                return COLOR_CLOSE;         // NO COLOR
    }
}

// Print string as a list of integers and hexadecimal
void print_asint(const char *string, size_t string_size ) {
    size_t i=0;
    printf("\n");
    for (i=0; i<string_size; i++) {
        if (i>0) {
            if (!(i % PRINT_AS_LINE)) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
        printf("%s%3d%s", get_color(string[i]), (unsigned char) string[i], get_color(0));
    }
    printf("\nSize: %ld - Pointer: %ld\n", string_size, (size_t) string);
}

void print_ashex(const char *string, size_t string_size ) {
    size_t i=0;
    printf("\n");
    for (i=0; i<string_size; i++) {
        if (i>0) {
            if (!(i % PRINT_AS_LINE)) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
        printf("%s%02X%s", get_color(string[i]), (unsigned char) string[i], get_color(0));
    }
    printf("\nSize: %ld - Pointer: %ld\n", string_size, (size_t) string);
}

void print_asbin(const char *string, size_t string_size ) {
    size_t i=0;
    printf("\n");
    for (i=0; i<string_size; i++) {
        printf("%s%c%s", get_color(string[i]), (unsigned char) string[i], get_color(0));
    }
    printf("\nSize: %ld - Pointer: %ld\n", string_size, (size_t) string);
}

void print_debug(const char *mainname, FILE *output, unsigned short int color, unsigned short int modifier, const char *fmt, ...) {
#if RTC
    DateTime datetime = rtc.now();
#endif
    char header[26]="";
    char tail[2], pcolor[8]="", result[COLOR_MAX_BUFFER]="", name[COLOR_MAX_BUFFER]="", nocolor=0;
    va_list argptr;
    unsigned int wrotten = 0;

    // Start variable arguments
    va_start(argptr, fmt);

    // Prepare result
    wrotten = vsnprintf(result, COLOR_MAX_BUFFER-(strlen(PRINT_DEBUG_OVERFLOW_STRING)+1), fmt, argptr);
    if (wrotten>=COLOR_MAX_BUFFER-(strlen(PRINT_DEBUG_OVERFLOW_STRING)+1)) {
        memcpy(result+COLOR_MAX_BUFFER-(strlen(PRINT_DEBUG_OVERFLOW_STRING)+2), PRINT_DEBUG_OVERFLOW_STRING, strlen(PRINT_DEBUG_OVERFLOW_STRING)+1);
    }

    // End variable arguments
    va_end(argptr);

    // Color open
    if (color) {
        if      (color==CGREY) {          sprintf(pcolor, "%s", COLOR_GREY); }
        else if (color==CRED) {           sprintf(pcolor, "%s", COLOR_RED); }
        else if (color==CGREEN) {         sprintf(pcolor, "%s", COLOR_GREEN); }
        else if (color==CYELLOW) {        sprintf(pcolor, "%s", COLOR_YELLOW); }
        else if (color==CBLUE) {          sprintf(pcolor, "%s", COLOR_BLUE); }
        else if (color==CPURPLE) {        sprintf(pcolor, "%s", COLOR_PURPLE); }
        else if (color==CCYAN) {          sprintf(pcolor, "%s", COLOR_CYAN); }
        else if (color==CWHITE) {         sprintf(pcolor, "%s", COLOR_WHITE); }
        else if (color==CSIMPLEGREY){     sprintf(pcolor, "%s", COLOR_SIMPLE_GREY); }
        else if (color==CSIMPLERED){      sprintf(pcolor, "%s", COLOR_SIMPLE_RED); }
        else if (color==CSIMPLEGREEN){    sprintf(pcolor, "%s", COLOR_SIMPLE_GREEN); }
        else if (color==CSIMPLEYELLOW){   sprintf(pcolor, "%s", COLOR_SIMPLE_YELLOW); }
        else if (color==CSIMPLEBLUE){     sprintf(pcolor, "%s", COLOR_SIMPLE_BLUE); }
        else if (color==CSIMPLEPURPLE){   sprintf(pcolor, "%s", COLOR_SIMPLE_PURPLE); }
        else if (color==CSIMPLECYAN){     sprintf(pcolor, "%s", COLOR_SIMPLE_CYAN); }
        else if (color==CSIMPLEWHITE) {   sprintf(pcolor, "%s", COLOR_SIMPLE_WHITE); }
        else {
            // No color
            nocolor=1;
        }
    } else {
        // No color
        nocolor=1;
    }

    // Define header
    if (modifier==0 || modifier==COLOR_NOTAIL) {
#if RTC
        sprintf(header, "%02d/%02d/%04d %02d:%02d:%02d ", datetime.day(), datetime.month(), datetime.year(), datetime.hour(), datetime.minute(), datetime.second());
#else
        sprintf(header, "%d ", millis());
#endif
    } else {
        header[0] = 0;
        mainname = NULL;
    }

    // Define name
    if (mainname) {
        sprintf(name, "%s: ", mainname);
    } else {
        name[0] = 0;
    }

    // Define tail
    if (modifier==0 || modifier==COLOR_NOHEAD) {
        sprintf(tail, "\n");
    } else {
        tail[0] = 0;
    }

    // Send parsed string
    if (!nocolor) {
        fprintf(output, "%s%s%s%s%s%s", pcolor, header, name, result, tail, COLOR_CLOSE);
    } else {
        fprintf(output, "%s%s%s%s", header, name, result, tail);
    }
    fflush(output);
}

// === TYPE CONVERSORS ===
// Convert any number of characters to a decimal number
unsigned long int chr2dec(char **source, int length) {
    unsigned long int result=0, i;
    unsigned char car;

    // For each char
    for (i=length; i>0; i--) {
        car = **source;
        result += ((int)car) * pow(256, i-1);
        (*source)++;
    }

    // Return result
    return result;
}

// Check if the field is a pure number (0:empty, 1:integer, 2:double, 3:string)
unsigned short int getnumber(char* input, int *integer, double *real, char **string, unsigned short int convert) {
    unsigned short int result=0, i=0, dot=0, character=0;
    unsigned char car;

    // For each character
    for (i=0; i<strlen(input); i++) {
        car = (unsigned char) input[i];
        if ((int)car==46) {
            // The character is a dot
            dot++;
        } else if (((int)car<48) && ((int)car>57)) {
            // The character is not a numeric digit
            character=1;
            break;
        }
    }

    if (character || dot>1) {
        // If the string has a character we don't know or it has more than 2 dots, it should be rendered as a string
        (*string) = (char*) calloc(strlen(input), sizeof(char));
        strcpy((*string), input);
        result = 3;
    } else if (dot) {
        // The string has only numbers and one dot, render as double
        (*real) = atof(input);
        result = 2;
    } else {
        // The string is an integer
        (*integer) = atoi(input);
        result = 1;
    }

    // Check force type
    if (convert==1) {
        if (result==2) {
            // Convert double to integer
            (*integer) = (int) (*real);
        } else if (result==3) {
            // Convert string to integer
            (*integer) = atoi(input);
        }
        result = 1;
    } else if (convert==2) {
        if (result==1) {
            // Convert integer to double
            (*real) = (double) *integer;
        } else if (result==3) {
            // Convert string to double
            (*real) = atof(*string);
        }
        result = 2;
    } else if (convert==3) {
        if (result==1) {
            // Convert integer to string
            (*string) = (char*) calloc(strlen(input), sizeof(char));
            strcpy((*string), input);
        } else if (result==2) {
            // Convert double to string
            (*string) = (char*) calloc(strlen(input), sizeof(char));
            strcpy((*string), input);
        }
        result = 3;
    }

    // Return the final result
    return result;
}


// === STRING MANAGEMENT ===

char* string_dup(const char *src, size_t n) {
    char *dest=NULL;

    if (src) {
        dest = (char*) malloc(n+1);
        if (dest == NULL) {
            return NULL;
        } else {
            ((char*) dest)[n] = 0;
        }

        return (char*) memcpy(dest, src, n);
    } else {
        return NULL;
    }
}

char** bstr_split(char* string, size_t string_size, const char splitter, char *filename, size_t linenumber) {
    char **result = (char**) calloc(1, sizeof(char*)), **result_realloc=NULL;
    size_t i=0, total=0;

    // Check for error
    if (splitter=='\0') {
        print_debug("ALIOLI", stderr, CRED, 0, "%s:%ld: ERROR on splitter(): can not split using '\\0'", filename, linenumber);
        free(result);
        return NULL;
    }

    // If we have a string to split
    if (string) {

        // Add first token
        result_realloc= (char**) realloc(result, (total+2) * sizeof(char*));
        if (!result_realloc) {
            print_debug("ALIOLI", stderr, CRED, 0, "%s:%ld: ERROR on splitter(): realloc() didn't get memory", filename, linenumber);
            free(result);
            return NULL;
        } else {
            result=result_realloc;
            result[total] = string;
            total++;
            result[total] = 0;
        }

        // While not reached the end of the string
        while ((string_size) && (*string!='\0')) {

            // Check splitter
            if (*string==splitter) {

                // Set '\0' on the splitter
                (*string) = 0;
                string++;
                string_size--;

                // Add another token
                result_realloc=(char**) realloc(result, (total+2) * sizeof(char*));
                if (!result_realloc) {
                    print_debug("ALIOLI", stderr, CRED, 0, "%s:%ld: ERROR on splitter(): realloc() didn't get memory", filename, linenumber);
                    for (i=0; i<total; i++) {
                        free(result[i]);
                    }
                    free(result);
                    return NULL;
                } else {
                    result=result_realloc;
                    if (string_size) {
                        result[total] = string;
                    } else {
                        result[total] = NULL;
                    }
                    total++;
                    result[total] = 0;
                }
            } else {
                // Check next character
                string++;
                string_size--;
            }
        }
    }

    return result;
}
char** str_split(char* string, const char splitter, char *filename, size_t linenumber) {
    return bstr_split(string, strlen(string), splitter, filename, linenumber);
}


// You must free the result if result is non-NULL.
char* strreplace(const char *orig, const char *rep, const char *with) {
    char *result=NULL; // the return string
    const char *ins=NULL;    // the next insert point
    char *tmp=NULL;    // varies
    int len_rep=0;  // length of rep (the string to remove)
    int len_with=0; // length of with (the string to replace rep with)
    int len_front=0; // distance between rep and end of last rep
    int count=0;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep) {
        result = NULL;
    } else {
        len_rep = strlen(rep);
        if (len_rep == 0) {
            // empty rep causes infinite loop during count
            result = NULL;
        } else {
            if (!with) {
                with = "";
            }
            len_with = strlen(with);

            // count the number of replacements needed
            ins = orig;
            for (count = 0; ( tmp = strstr(ins, rep) ) ; ++count) {
                ins = tmp + len_rep;
            }

            tmp = result = (char*) malloc(strlen(orig) + (len_with - len_rep) * count + 1);

            if (!result) {
                result = NULL;
            } else {
                // first time through the loop, all the variable are set correctly
                // from here on,
                //    tmp points to the end of the result string
                //    ins points to the next occurrence of rep in orig
                //    orig points to the remainder of orig after "end of rep"
                while (count--) {
                    ins = strstr(orig, rep);
                    len_front = ins - orig;
                    tmp = strncpy(tmp, orig, len_front) + len_front;
                    tmp = strcpy(tmp, with) + len_with;
                    orig += len_front + len_rep; // move to next "end of rep"
                }
                strcpy(tmp, orig);
            }
        }
    }
    return result;
}
// You must free the result if result is non-NULL.
int bstr_replace(const char *orig, int orig_size, const char *rep, int rep_size, const char *with, int with_size, char **result, int *result_size) {
    const char *ins=NULL;    // the next insert point
    char *tmp=NULL;    // varies
    int len_front=0; // distance between rep and end of last rep
    int ins_size=0;
    int count=0;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep) {
        (*result) = NULL;
        (*result_size) = 0;
    } else {
        if (rep_size == 0) {
            // empty rep causes infinite loop during count
            (*result) = NULL;
            (*result_size) = 0;
        } else {
            if (!with) {
                with = "";
            }

            // count the number of replacements needed
            ins = orig;
            ins_size = orig_size;
            for (count = 0; ( tmp = bstrstr(ins, ins_size, rep, rep_size) ) ; ++count) {
                ins_size -= (tmp+rep_size)-ins;
                ins = tmp + rep_size;
            }

            tmp = (*result) = (char*) malloc(orig_size + (with_size - rep_size) * count);

            if (!(*result)) {
                (*result) = NULL;
                (*result_size) = 0;
            } else {
                (*result_size) = orig_size + (with_size - rep_size) * count;
                // first time through the loop, all the variable are set correctly
                // from here on,
                //    tmp points to the end of the result string
                //    ins points to the next occurrence of rep in orig
                //    orig points to the remainder of orig after "end of rep"
                while (count--) {
                    ins = bstrstr(orig, orig_size, rep, rep_size);
                    len_front = ins - orig;
                    memcpy(tmp, orig, len_front);
                    tmp += len_front;
                    memcpy(tmp, with, with_size);
                    tmp += with_size;
                    orig += len_front + rep_size; // move to next "end of rep"
                    orig_size -= len_front + rep_size;
                }
                memcpy(tmp, orig, orig_size);
            }
        }
    }
    return 1;
}


// bstrchr(): find first occurrence of character in char sstring[] for length
const char *bstrchr (const char *string, int character, size_t length) {
    char *found=NULL;
    const char ch = character;

    while (length && !found) {
        if (*string == ch) {
            found = (char*) string;
            break;
        } else {
            length--;
            if (length) {
                string++;
            }
        }
    }
    return found;
}
// bistrchr(): find first occurrence of character in char sstring[] for length (CASE INSENSITIVE)
const char *bistrchr (const char *string, int character, size_t length) {
    char *found=NULL;
    const char ch = character;

    while (length && !found) {
        if (toupper(*string) == toupper(ch)) {
            found = (char*) string;
            break;
        } else {
            string++;
            length--;
        }
    }
    return found;
}
int bstrcmp(const char *s1, size_t l1, const char *s2, size_t l2) {
    const char *last1 = s1+l1, *last2 = s2+l2;

    if (!l1) {
        if (!l2) {
            // Nothing to compare, they are equal!
            return 0;
        } else {
            // L1 smaller than L2
            return -1;
        }
    } else {
        if (!l2) {
            // L1 bigger than L2
            return 1;
        } else {
            // Both same size, compare
            while ((s1<last1) && (s2<last2)) {
                if ((*s1)<(*s2)) {
                    return -1;
                } else if ((*s1)>(*s2)) {
                    return 1;
                }
                s1++;
                s2++;
            }
            if (l1<l2) {
                return -1;
            } else if (l1>l2) {
                return 1;
            } else {
                return 0;
            }
        }
    }
}
int bistrcmp(const char *s1, size_t l1, const char *s2, size_t l2) {
    const char *last1 = s1+l1, *last2 = s2+l2;

    if (!l1) {
        if (!l2) {
            // Nothing to compare, they are equal!
            return 0;
        } else {
            // L1 smaller than L2
            return -1;
        }
    } else {
        if (!l2) {
            // L1 bigger than L2
            return 1;
        } else {
            // Both same size, compare
            while ((s1<last1) && (s2<last2)) {
                if (toupper(*s1)<toupper(*s2)) {
                    return -1;
                } else if (toupper(*s1)>toupper(*s2)) {
                    return 1;
                }
                s1++;
                s2++;
            }
            if (l1<l2) {
                return -1;
            } else if (l1>l2) {
                return 1;
            } else {
                return 0;
            }
        }
    }
}
// Find first occurrence of s2[] in s1[] for length l1
char* bstrstr(const char *s1, size_t l1, const char *s2, size_t l2) {
    char *found=NULL, *pivot=NULL;

    // Check string size
    if (s1 && l1 && (l1>l2)) {
        if (!s2 || !l2) {
            found = (char *) s1;
        } else {

            // Find prefix
            pivot = (char*) bstrchr(s1, *s2, l1);
            if (pivot) {
                // Update
                l1 -= (pivot-s1);
                s1 = pivot;
                while ((l1>=0) && (l1>=l2) && pivot) {
                    // Check the rest of the string
                    if (!bstrcmp(pivot+1, min(l1, l2)-1, s2+1, l2-1)) {
                        found = pivot;
                        break;
                    }

                    // Go to character after pivot
                    s1++;
                    l1--;

                    // Look for next pivot
                    pivot = (char*) bstrchr(s1, *s2, l1);
                    if (pivot) {
                        // Update
                        l1 -= (pivot-s1);
                        s1 = pivot;
                    }
                }
            }
        }
    }
    return found;
}
char* bistrstr(const char *s1, size_t l1, const char *s2, size_t l2) {
    char *found=NULL, *pivot=NULL;

    // Check string size
    if (s1 && l1 && (l1>l2)) {
        if (!s2 || !l2) {
            found = (char *) s1;
        } else {

            // Find prefix
            pivot = (char*) bistrchr(s1, *s2, l1);
            if (pivot) {
                // Update
                l1 -= (pivot-s1);
                s1 = pivot;
                while ((l1>=0) && (l1>=l2) && pivot) {
                    // Check the rest of the string
                    if (!bistrcmp(pivot+1, min(l1, l2)-1, s2+1, l2-1)) {
                        found = pivot;
                        break;
                    }

                    // Go to character after pivot
                    s1++;
                    l1--;

                    // Look for next pivot
                    pivot = (char*) bistrchr(s1, *s2, l1);
                    if (pivot) {
                        // Update
                        l1 -= (pivot-s1);
                        s1 = pivot;
                    }
                }
            }
        }
    }
    return found;
}


// Binary String control
void bsprintf(BString bs) {
    printf("STRING(%ld): %.*s\n", bs.size, (int) bs.size, bs.pointer);
}

void bssprintf(char *target, BString bs) {
    sprintf(target, "%.*s", (int) bs.size, bs.pointer);
}

// BString functions
int bsstrcmp(BString bs1, char *st2, size_t st2_size) {
    return bstrcmp(bs1.pointer, bs1.size, st2, st2_size);
}
int bsstrcmp2(BString bs1, char *st2) {
    return bstrcmp(bs1.pointer, bs1.size, st2, strlen(st2));
}
int bsbscmp(BString bs1, BString bs2) {
    return bstrcmp(bs1.pointer, bs1.size, bs2.pointer, bs2.size);
}
int bsstricmp(BString bs1, char *st2, size_t st2_size) {
    return bistrcmp(bs1.pointer, bs1.size, st2, st2_size);
}
int bsstricmp2(BString bs1, char *st2) {
    return bistrcmp(bs1.pointer, bs1.size, st2, strlen(st2));
}
int bsbsicmp(BString bs1, BString bs2) {
    return bistrcmp(bs1.pointer, bs1.size, bs2.pointer, bs2.size);
}


BString bsnew(char *string, size_t size) {
    BString target;
    target.pointer = string;
    target.size = size;
    return target;
}
BString bsnew2(char *string) {
    return bsnew(string, strlen(string));
}
void bsinit(BString *bs) {
    bs->pointer = NULL;
    bs->size = 0;
}
int bsempty(BString bs) {
    return (bs.pointer==NULL || bs.size==0);
}
void bsbsclone(BString bsource, BString *btarget) {
    btarget->pointer = bsource.pointer;
    btarget->size = bsource.size;
}
int bsdup(BString bsorig, BString *bscopy) {
    int error=0;
    // Get memory for the new node
    bscopy->pointer = (char*) malloc(sizeof(char)*(bsorig.size));
    if (bscopy->pointer) {
        // Got memory
        memcpy(bscopy->pointer, bsorig.pointer, bsorig.size);
        bscopy->size = bsorig.size;
    } else {
        // No memory
        print_debug("ALIOLI", stderr, CRED, 0, "%s:%d: ERROR no memory available for bsdup()!", __FILE__, __LINE__);
        error=1;
    }
    return !error;
}
void bsfree(BString *bs) {
    if (bs->pointer) {
        free(bs->pointer);
    }
    bs->pointer = NULL;
    bs->size = 0;
}
int bstoi(BString bs) {
    int result=0;
    char *c = (char*) malloc(sizeof(char)*(bs.size+1));
    memcpy(c, bs.pointer, bs.size);
    c[bs.size]=0;
    result = atoi(c);
    free(c);
    return result;
}
long int bstol(BString bs) {
    long int result=0;
    char *c = (char*) malloc(sizeof(char)*(bs.size+1));
    memcpy(c, bs.pointer, bs.size);
    c[bs.size]=0;
    result = atol(c);
    free(c);
    return result;
}
double bstod(BString bs) {
    double result=0;
    char *c = (char*) malloc(sizeof(char)*(bs.size+1));
    memcpy(c, bs.pointer, bs.size);
    c[bs.size]=0;
    result = atof(c);
    free(c);
    return result;
}
char* bstos(BString bs) {
    char *result = (char*) malloc(sizeof(char)*(bs.size+1));
    memcpy(result, bs.pointer, bs.size);
    result[bs.size]=0;
    return result;
}

// === DATE/TIME HELPERS ===
long int get_current_time (void) {
    DateTime datetime = rtc.now();
    long int now = (long int) datetime.unixtime();
#ifdef DEBUG
    // print_debug("ALIOLI", stdout, "blue", 0, "get_current_time(): %ld", now);
#endif
    return now;
}

unsigned long int get_current_time_with_ms (void) {
    DateTime datetime = rtc.now();
    long int nows = (long int) datetime.unixtime();
    long int nowms = (long int) millis();
    unsigned long int now = (nows*1000) + (nowms - rtc_millis_offset) % 1000;
    Serial.println(nows);
    Serial.println(nows*1000);
    Serial.println(now);
#ifdef DEBUG
    // print_debug("ALIOLI", stdout, "blue", 0, "get_current_time_with_ms(): %ld", now);
#endif
    return now;
}


/*
 * =========================================================================================================================================================
 *
 * EPOCH to human readable time and date converter Low-Level Algorithm
 * Author: Juanmi Taboada <juanmi@juanmitaboada.com>
 * Ver: 2.0
 * Date: March 12, 2020
 *
 * Techniques used:
 * ================
 *
 * EPOCH to human readable time and date converter for microcontrollers (PIC, AVR, Arduino, STM32) by Siddharth Singh <sidsingh78@gmail.com>
 *
 * Chrono-Compatible Low-Level Date Algorithms by Author: Howard Hinnant <howard.hinnant@gmail.com> - http://howardhinnant.github.io/date_algorithms.html#civil_from_days
 *
 * NOTES:
 *      - Daylight saving time must be taken care of outside this function
 *      - Remember to add or substract time zone to epoch before submiting it to this function (Example: epoch+=19800 ; //GMT +5:30 = +19800 seconds )
 *
 */
int epoch2date(unsigned long int epoch, unsigned int *year, unsigned short int *month, unsigned short int *day, unsigned short int *hour, unsigned short int *minute, unsigned short int *second, unsigned short int *day_of_week, unsigned short int *day_of_year) {

    static unsigned char week_days[7] = {4,5,6,0,1,2,3}; //Thu=4, Fri=5, Sat=6, Sun=0, Mon=1, Tue=2, Wed=3

    unsigned char ntp_hour=0, ntp_minute=0, ntp_second=0, ntp_week_day=0, ntp_day=0, ntp_month=0, leap_days=0;
    unsigned int ntp_year=0, days_since_epoch=0, ntp_day_of_year=0;
    int z=0, era=0;
    unsigned int doe=0, yoe=0, doy=0, mp=0;

    // Calculate hour, minute & second
    ntp_second = epoch%60;
    epoch /= 60.0;
    ntp_minute = epoch%60;
    epoch /= 60;
    ntp_hour  = epoch%24;
    epoch /= 24;

    // Number of days since epoch
    days_since_epoch = epoch;

    // Calculating WeekDay
    ntp_week_day = week_days[days_since_epoch%7];

    // Chrono-Compatible Low-Level Date Algorithms for day/moth/year
    z = days_since_epoch + 719468;
    era = (z >= 0 ? z : z - 146096) / 146097;
    doe = (unsigned) (z - era * 146097);                        // [0, 146096]
    yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;      // [0, 399]
    ntp_year = (int) yoe + era * 400;
    doy = doe - (365*yoe + yoe/4 - yoe/100);                    // [0, 365]
    mp = (5*doy + 2)/153;                                       // [0, 11]
    ntp_day = doy - (153*mp+2)/5 + 1;                           // [0, 31]
    ntp_month = mp + (mp < 10 ? 3 : -9);                        // [1, 12]
    ntp_year += (ntp_month <= 2);

    // Calculating accurate number of days of the year
    leap_days = (((ntp_year-1) / 4) - 493) - (((ntp_year-1) / 100) - 19) + (((ntp_year-1) / 400) - 4) + 1;
    ntp_day_of_year = (days_since_epoch-(ntp_year-1970)*365-leap_days)%366+1;

    // Asign results
    (*year) = ntp_year;
    (*month) = ntp_month;
    (*day) = ntp_day;
    (*hour) = ntp_hour;
    (*minute) = ntp_minute;
    (*second) = ntp_second;
    (*day_of_week) = ntp_week_day;
    (*day_of_year) = ntp_day_of_year;

    return 0;
}

unsigned long int date2epoch(unsigned int year, unsigned short int month, unsigned short int day, unsigned short int hour, unsigned short int minute, unsigned short int second) {
    static unsigned char month_days[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    unsigned int day_of_year=0;
    unsigned short int i;
    unsigned long int epoch=0;

    // Calculate last day of february
    if(((year%4==0) && (year%100!=0)) || (year%400==0)) {
        month_days[1]=29;
    } else {
        // Required because it is an static algorithm
        month_days[1]=28;
    }

    // Calculate day of the year
    for (i=0; i<month-1; i++) {
        day_of_year += month_days[i];
    }
    day_of_year += day - 1;
    
    year -= 1900;

    // POSIX formula to convert to seconds since the epoch
    // https://stackoverflow.com/questions/9076494/how-to-convert-from-utc-to-local-time-in-c
    // All elements must be unsigned long int to avoid overflow on int or signed elements
    epoch = (unsigned long int) second + (unsigned long int)minute * 60 + (unsigned long int)hour * 3600 + (unsigned long int)day_of_year * 86400 + ((unsigned long int)year - 70) * 31536000 + ((((unsigned long int)year - 69) / 4)) * 86400 - ((((unsigned long int)year - 1) / 100)) * 86400 + (((unsigned long int)year + 299 ) / 400) * 86400;
    return epoch;
}

/*
void sleep(unsigned int total, unsigned int msstep) {
    unsigned int start = millis(), now=millis(), left=0;

    // Set defaults if no other specified
    if (!total) {
        total = 1;
    }

    // Left first time is total
    left = total;
    while (left>0) {

        // Execute priority functions
        priority();

        // Get our time now
        now = millis();

        // Calculate how must time is left to wait still
        left = start+total-now;

        // Do a msstep if some time left
        if (left>msstep) {
            // Sleep a normal step
            delay(msstep);
        } else if (left>0) {
            // Sleep whatever is left
            delay(left);
            // Reset left
            left = 0;
        }

    }
}
void sleep(unsigned int total) {
    sleep(total, DEFAULT_SERIAL_STEP);
}
*/

// === ENCODERS ===

//CRC-8 - based on the CRC8 formulas by Dallas/Maxim
//code released under the therms of the GNU GPL 3.0 license
byte CRC8(const byte *data, size_t dataLength) {
    byte crc = 0x00;
    while (dataLength--) {
        byte extract = *data++;
        for (byte tempI = 8; tempI; tempI--) {
            byte sum = (crc ^ extract) & 0x01;
            crc >>= 1;
            if (sum) {
                crc ^= 0x8C;
            }
            extract >>= 1;
        }
    }
    return crc;
}



// === MATHEMATICAL FUNCTIONS ===
// Convert halffloat types
float halffloat2float(halffloat hf) {
    int b = half_to_float(hf);
    float c=0;
    memcpy(&c, &b, sizeof(c));
    return c;
}
halffloat float2halffloat(float f) {
    halffloat hf=0.0;
    int32_t c=0;
    memcpy(&c, &f, sizeof(c));
    hf = half_from_float(c);
    return hf;
}

// Extract half float
float gethf(halffloat hf) {
    return halffloat2float(hf);
}

const char * alioli_version(void) { return ALIOLI_VERSION; }

