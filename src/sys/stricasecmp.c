// -----------------------------------------------
// stricasecmp.c
//
// Written by Ed Zavada, 2004-2012
// Copyright (c) 2012, Dream Rock Studios, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// -----------------------------------------------

#include "pdg_project.h"

#include <ctype.h>

// NOTE: these implementations will only work 100% correctly with UTF8 strings
// if the locale is set to Unicode UTF8. Otherwise they risk false positives
// when tolower converts bytes of a multiple char as if they were single chars

int stricmp(const char *s1, const char *s2);
int strnicmp(const char *s1, const char *s2, int n);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);

int stricmp(const char *s1, const char *s2)			/*- cc 010713 -*/
{
    char c1, c2;
    while (1)
    {
    	c1 = tolower(*s1++);
    	c2 = tolower(*s2++);
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (c1 == 0) return 0;
    }
}

/* Compare lexigraphically two strings up to a max length */

int strnicmp(const char *s1, const char *s2, int n)		/*- cc 010605 -*/
{
    int i;
    char c1, c2;
    for (i=0; i<n; i++)
    {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (!c1) return 0;
    }
    return 0;
}

int strcasecmp(const char *s1, const char *s2){
	return stricmp(s1,s2);
}

/* Case-insensitive string comparison */
int strncasecmp(const char *s1, const char *s2, size_t n){
	return strnicmp(s1,s2, n);	
}
