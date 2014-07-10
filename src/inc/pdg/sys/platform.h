// -----------------------------------------------
// platform.h
// 
// Written by Ed Zavada, 2001-2012
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

#ifndef PLATFORM_H_INCLUDED
#define PLATFORM_H_INCLUDED

#include "pdg_project.h"

/****************************************************************************************************

    This define follows the gcc conventions for displaying a target
    
        PLATFORM_STR          - hardware-vendor-os, something like:
                                "powerpc-apple-macosx", "intel-unknown-linux"
                                or "intel-microsoft-win32"


    These conditionals specify which microprocessor instruction set is being
    generated.  At most one of these is defined, the rest are not.

        PLATFORM_PPC          - Compiler is generating PowerPC instructions
        PLATFORM_68K          - Compiler is generating 680x0 instructions
        PLATFORM_X86          - Compiler is generating x86 instructions
        PLATFORM_MIPS         - Compiler is generating MIPS instructions
        PLATFORM_SPARC        - Compiler is generating Sparc instructions
        PLATFORM_ALPHA        - Compiler is generating Dec Alpha instructions
        PLATFORM_HPPA         - Compiler is generating HP PA-RISC instructions
        PLATFORM_ARM          - Compiler is generating ARM instructions


    These conditionals specify in which Operating System the generated code will
    run.

        PLATFORM_MACOS         - Generated code will run under Mac OS
        PLATFORM_MACOSX        - Generated code will run under Mac OS X
        PLATFORM_WIN32         - Generated code will run under 32-bit Windows
        PLATFORM_UNIX          - Generated code will run under some unix varient
        PLATFORM_BEOS          - Generated code will run under BeOS
        PLATFORM_PALM          - Generated code will run under Palm OS
        PLATFORM_SONYPS        - Generated code will run under Sony PlayStation
        PLATFORM_SONYPS2       - Generated code will run under Sony PlayStation 2
        PLATFORM_LINUX         - Generated code will run under Linux, the GNU Unix varient
        PLATFORM_BSD           - Generated code will run under BSD Unix varient
        PLATFORM_HPUX          - Generated code will run under HP's Unix varient
        PLATFORM_SOLARIS       - Generated code will run under Sun's Solaris Unix varient
        PLATFORM_DARWIN        - Generated code will run under Apple's BSD Unix varient
        PLATFORM_POSIX         - Generated code will run under a Posix compliant OS
        PLATFORM_BREW          - Generated code will run under Qualcomm Brew
        PLATFORM_IOS           - Generated code will run under Apple iOS
        PLATFORM_ANDROID       - Generated code will run under Google's Android OS

    These conditionals specify in which runtime the generated code will
    run. This is needed when the OS and CPU support more than one runtime
    (e.g. MacOS on 68K supports CFM68K and Classic 68k).

        PLATFORM_LITTLE_ENDIAN - Generated code uses little endian format for integers
        PLATFORM_BIG_ENDIAN    - Generated code uses big endian format for integers
     
        PLATFORM_MAC_CFM       - PLATFORM_MAC is true and CFM68K or PowerPC CFM (TVectors) are used
        PLATFORM_MAC_MACHO     - PLATFORM_MAC is true and Mach-O style runtime
        PLATFORM_MAC_68881     - PLATFORM_MAC is true and 68881 floating point instructions used  
        PLATFORM_MAC_CARBON   - Code is being compiled to run on Mac OS 8 and Mac OS X via CarbonLib

    These conditionals specify the compiler being used in a normalized way.
    
        COMPILER_STR          - string version of compiler version used for the build

        COMPILER_MWERKS       - Metrowerks CodeWarrior compiler
        COMPILER_GCC          - Gnu's Open Source gcc compiler
        COMPILER_SUNSW        - Sun's Sparc Works Pro compiler
        COMPILER_MSVC         - Microsoft's Visual C++ compiler

    These conditionals specify whether the compiler supports particular types.

        COMPILER_TYPE_LONGLONG      - Compiler supports "long long" 64-bit integers
        COMPILER_TYPE_BOOL          - Compiler supports "bool"
        COMPILER_TYPE_EXTENDED      - Compiler supports "extended" 80/96 bit floating point

    These conditionals specify whether the compiler supports particular language extensions
    to function prototypes and definitions.

        COMPILER_FUNCTION_PASCAL         - Compiler supports "pascal void Foo()"
        COMPILER_FUNCTION_DECLSPEC       - Compiler supports "__declspec(xxx) void Foo()"
        COMPILER_FUNCTION_WIN32CC        - Compiler supports "void __cdecl Foo()" and "void __stdcall Foo()"

****************************************************************************************************/

#if defined(__MWERKS__)
    /*
        CodeWarrior compiler from Metrowerks, Inc.
    */
    
    #define API_DEPRECATED 

    #define COMPILER_MWERKS           __MWERKS__
    #define COMPILER_STR "Metrowerks CodeWarrior"
    #define COMPILER_VERSION          (void*)__MWERKS__

    #if (__MWERKS__ >= 0x2110) && __MACH__
        #define PLATFORM_PPC               1
        #define PLATFORM_UNIX              1  
        #define PLATFORM_BSD               1  
        #define PLATFORM_DARWIN            1
        #define PLATFORM_MACOSX            1
        #define PLATFORM_MAC_MACHO         1
        #define PLATFORM_POSIX             1
        #if __option(little_endian)
            #define PLATFORM_LITTLE_ENDIAN 1
        #else
            #define PLATFORM_BIG_ENDIAN    1
        #endif
        #define COMPILER_FUNCTION_PASCAL   1
        #define COMPILER_FUNCTION_DECLSPEC 1
        #define PLATFORM_STR "powerpc-apple-macosx"

    #elif (__MWERKS__ < 0x0900) || macintosh
        #define PLATFORM_MACOS             1
        #define PLATFORM_BIG_ENDIAN        1
        #if powerc
            #define PLATFORM_PPC           1
            #define PLATFORM_MAC_CFM       1
            /* now defaulting to use Carbon for Macintosh, unless PLATFORM_USE_MAC_CLASSIC_API is defined */
            #if !defined( PLATFORM_USE_MAC_CLASSIC_API ) /* this must be predefined */
                #define PLATFORM_MAC_CARBON 1
                #define PLATFORM_STR "powerpc-apple-macos-carbon"
            #else
                 #define PLATFORM_STR "powerpc-apple-macos"
            #endif
        #else
            #define PLATFORM_68K           1
            #if defined(__CFM68K__)
                #define PLATFORM_MAC_CFM   1
            #endif
            #if __MC68881__
                #define PLATFORM_MAC_68881 1
            #endif
            #define PLATFORM_STR "68k-apple-macos"
        #endif
        #define COMPILER_FUNCTION_PASCAL         1
        #if (__MWERKS__ >= 0x2000)
            #define COMPILER_FUNCTION_DECLSPEC   1
        #endif

    #elif (__MWERKS__ >= 0x0900) && __INTEL__
        #define PLATFORM_X86               1
        #define PLATFORM_WIN32             1
        #define PLATFORM_LITTLE_ENDIAN     1
        #ifndef COMPILER_FUNCTION_WIN32CC                    /* allow calling convention to be overriddden */
            #define COMPILER_FUNCTION_WIN32CC    1   
        #endif
        #define PLATFORM_STR "intel-microsoft-win32"

    #elif (__MWERKS__ >= 0x1900) && __MIPS__
        #define PLATFORM_MIPS              1
        #define PLATFORM_UNIX              1
        #if __option(little_endian)
            #define PLATFORM_LITTLE_ENDIAN 1
        #else
            #define PLATFORM_BIG_ENDIAN    1
        #endif
        #define PLATFORM_STR "mips-unknown-unix"

    #else
        #error unknown Metrowerks compiler
    #endif

    #if (__MWERKS__ >= 0x1100)
        #if __option(longlong)
            #define COMPILER_TYPE_LONGLONG 1
        #endif
    #endif
    #if (__MWERKS__ >= 0x1000)
        #if __option(bool)
            #define COMPILER_TYPE_BOOL     1
        #endif
    #endif



#elif defined(__GNUC__) 
    /*
        gcc from GNU
    */

	#define API_DEPRECATED __attribute__((deprecated,visibility("default")))

    #define COMPILER_GCC              __GNUC__
    #define COMPILER_STR "GNU gcc"
    #define COMPILER_VERSION          (void*)__GNUC__

    #if (defined(__APPLE_CPP__) || defined(__APPLE_CC__) || defined(__NEXT_CPP__) || defined(__APPLE__) || defined(__NEXT__))

        #define PLATFORM_MACOSX            1
        #define PLATFORM_UNIX              1
        #define PLATFORM_DARWIN            1
        #define PLATFORM_BSD               1  
        #define PLATFORM_POSIX             1
        #define PLATFORM_MAC_MACHO         1
        #if defined(__ppc__) || defined(powerpc) || defined(ppc)
            #define PLATFORM_PPC           1
            #define PLATFORM_BIG_ENDIAN    1
            #define PLATFORM_STR "powerpc-apple-darwin"
        #elif defined(__i386__) || defined(i386) || defined(intel)
            #define PLATFORM_X86           1
            #define PLATFORM_LITTLE_ENDIAN 1
            #define PLATFORM_STR "i386-apple-darwin"
        #elif defined(__arm__)
            #define PLATFORM_ARM	       1
            #define PLATFORM_LITTLE_ENDIAN 1
            #define PLATFORM_IOS           1
            #define PLATFORM_STR "arm-apple-darwin"
        #elif defined(__x86_64__)
            #define PLATFORM_X86_64	       1
            #define PLATFORM_LITTLE_ENDIAN 1
            #define PLATFORM_64_BIT        1
            #define PLATFORM_STR "x86_64-apple-darwin"
        #else
            #error unrecognized gcc compiler
        #endif
    

    #elif defined(__x86_64__)    

        #define PLATFORM_X86           1
        #define PLATFORM_LITTLE_ENDIAN 1
        #if defined(__linux__)
            #define PLATFORM_UNIX     1
            #define PLATFORM_POSIX    1
            #define PLATFORM_LINUX    1
            #define PLATFORM_STR "x86_64-generic-linux"
        #else
            #error unrecognized gcc compiler
        #endif

 
    #elif ( defined(__i386__) || defined(__intel__) )
    
        #define PLATFORM_X86           1
        #define PLATFORM_LITTLE_ENDIAN 1
        #if defined(__linux__)
            #define PLATFORM_UNIX     1
            #define PLATFORM_POSIX    1
            #define PLATFORM_LINUX    1
            #define PLATFORM_STR "i386-generic-linux"
        #elif defined (_WIN32)
            #define PLATFORM_WIN32    1
            #define PLATFORM_STR "intel-microsoft-win32"
        #elif ( defined (__solaris__) || defined (__sunos__) || defined (__sun__) )
            #define PLATFORM_UNIX     1
            #define PLATFORM_POSIX    1
            #define PLATFORM_SOLARIS  1
            #define PLATFORM_STR "intel-sun-solaris"
        #else
            #error unrecognized gcc compiler
        #endif
    
    
    #elif defined(__arm__)
    
        #define PLATFORM_ARM           1
        #define PLATFORM_BREW          1
        #define PLATFORM_LITTLE_ENDIAN 1  /* just assuming little endian now, should check options */
        #define PLATFORM_STR "arm-qualcomm-brew"
    
           
    #elif defined(__sparc__)
    
        #define PLATFORM_SPARC         1
        #define PLATFORM_UNIX          1
        #define PLATFORM_POSIX         1
        #define PLATFORM_BIG_ENDIAN    1
        #if defined(__linux__)
            #define PLATFORM_LINUX    1
            #define PLATFORM_STR "sparc-unknown-linux"
        #elif defined ( (__solaris__) || defined (__sunos__) || defined (__sun__) )
            #define PLATFORM_SOLARIS  1
            #define PLATFORM_STR "sparc-sun-solaris"
        #else
            #error unrecognized gcc compiler
        #endif
    
           
    #elif defined(__hppa__)
    
        #define PLATFORM_HPPA          1
        #define PLATFORM_UNIX          1
        #define PLATFORM_POSIX         1
        #define PLATFORM_HPUX          1
        #define PLATFORM_BIG_ENDIAN    1
        #define PLATFORM_STR "hppa-hp-hpux"


    #elif ( defined(__powerpc__) || defined(__ppc__) )
    
        #define PLATFORM_PPC           1
        #define PLATFORM_BIG_ENDIAN    1
        #if defined(__linux__)
            #define PLATFORM_UNIX      1
            #define PLATFORM_POSIX     1
            #define PLATFORM_LINUX     1
            #define PLATFORM_STR "powerpc-unknown-linux"
        #elif defined (__macos__)
            #define PLATFORM_MACOS     1
            #define PLATFORM_STR "powerpc-apple-macos"
        #else
            #error unrecognized gcc compiler
        #endif

    #else
        #error unrecognized gcc compiler
    #endif
    
    #if __GNUC__ >= 2
        #define COMPILER_TYPE_LONGLONG           1
    #endif
    #ifdef __cplusplus
        #define COMPILER_TYPE_BOOL               1
    #endif



#elif ( defined(_MSC_VER) && !defined(__MWERKS__) )
    /*
        Visual Studio C/C++ from Microsoft, Inc.
    */
    
    #define API_DEPRECATED __declspec(deprecated)
    
    #define COMPILER_MSVC             _MSC_VER
    #define COMPILER_STR "Microsoft Visual C++"
    #define COMPILER_VERSION          (void*)_MSC_VER

    #if defined(_M_M68K)    /* Visual C++ with Macintosh 68K target */
        #define PLATFORM_68K               1
        #define PLATFORM_MAC               1
        #define PLATFORM_BIG_ENDIAN        1
        #define PRAGMA_STRUCT_ALIGN        1
        #define PRAGMA_STRUCT_PACK         1
        #define PRAGMA_STRUCT_PACKPUSH     1
        #define COMPILER_FUNCTION_PASCAL   1
        #define PLATFORM_STR "68k-apple-macos"

    #elif defined(_M_MPPC)  /* Visual C++ with Macintosh PowerPC target */
        #define PLATFORM_PPC               1
        #define PLATFORM_MAC               1
        #define PLATFORM_BIG_ENDIAN        1
        #define PLATFORM_MAC_CFM           1
        #define PLATFORM_STR "powerpc-apple-macos"

    #elif defined(_M_IX86)  /* Visual Studio with Intel x86 target */
        #define PLATFORM_X86               1
        #define PLATFORM_WIN32             1
        #define PLATFORM_LITTLE_ENDIAN     1
        #define PRAGMA_STRUCT_PACK         1
        #define PRAGMA_STRUCT_PACKPUSH     1
        #if defined(__cplusplus) && (_MSC_VER >= 1100)
            #define COMPILER_TYPE_BOOL     1
        #endif
        #ifndef COMPILER_FUNCTION_DECLSPEC               /* allow use of __declspec(dllimport) to be overridden */
            #define COMPILER_FUNCTION_DECLSPEC       1  
        #endif
        #ifndef COMPILER_FUNCTION_WIN32CC                /* allow calling convention to be overriddden */
            #define COMPILER_FUNCTION_WIN32CC        1   
        #endif
        #define PLATFORM_STR "intel-microsoft-win32"

    #elif defined(_M_ALPHA)     /* Visual C++ with Dec Alpha target */
        #define PLATFORM_ALPHA             1
        #define PLATFORM_WIN32             1
        #define PLATFORM_LITTLE_ENDIAN     1
        #define PLATFORM_STR "alpha-microsoft-win32"

    #elif defined(_M_PPC)   /* Visual C++ for Windows NT on PowerPC target */
        #define PLATFORM_PPC               1
        #define PLATFORM_WIN32             1
        #define PLATFORM_LITTLE_ENDIAN     1
        #define PLATFORM_STR "powerpc-microsoft-win32"

    #elif defined(_M_MRX000)    /* Visual C++ for Windows NT on MIPS target */
        #define PLATFORM_MIPS              1
        #define PLATFORM_WIN32             1
        #define PLATFORM_LITTLE_ENDIAN     1
        #define PLATFORM_STR "mips-microsoft-win32"

    #else
        #error unrecognized microsoft visual c++ compiler
    #endif


#elif ( defined(__sparc) || defined(__SUNPRO_CC) )
    /*
        SPARCompiler or SPARCWorks C++ compiler from Sun Microsystems Inc.
    */
    
    #define API_DEPRECATED 
    
    #define COMPILER_SUNSW             1
    #define COMPILER_STR "Sun SPARCWorks"
//    #define COMPILER_VERSION           0


    #define PLATFORM_UNIX              1
    #define PLATFORM_POSIX             1
    #define PLATFORM_SOLARIS           1
    #define PLATFORM_BIG_ENDIAN        1
    #if ( defined(__i386) )
        #define PLATFORM_X86           1  
        #define PLATFORM_LITTLE_ENDIAN 1
        #define PLATFORM_STR "intel-sun-solaris"
    #else     
        #define PLATFORM_SPARC         1       
        #define PLATFORM_BIG_ENDIAN 1
        #define PLATFORM_STR "sparc-sun-solaris"
    #endif
    
    

#else
       /*
            NOTE:   If your compiler errors out here then support for your compiler 
                    has not yet been added to platform.h.  
                    
                    platform.h is designed to be plug-and-play.  It auto detects
                    which compiler is being run and configures the PLATFORM_ conditionals
                    appropriately.
        */
        #error platform.h: unknown compiler (see comment above)
#endif

#ifdef RUBY_DOX
  /* used when Doxygen is generating Ruby Documentation */
#endif

//! @cond C++
#ifdef COMPILER_MWERKS
    /* option opt_propagation is defined in CodeWarrior when
       global optimization level is 2 or higher, a good indication
       of a non-debug build. This emulates the behavior of some
       compilers where a debug build has DEBUG defined by the compiler */
    #if !defined(DEBUG) && !__option(opt_propagation)
        #define DEBUG 1
    #endif
#endif


/* name demangling 
   demangleSymbol(str) with a mangled name to demangle it
   typeid_name(symbol) to get a demangled name (C++ only) */
#ifndef NO_DEMANGLE
#if __cplusplus
extern "C" {
#endif

/* Use this to demangle a symbol name into a human readable name.
   Can be called from straight C, even for C++ symbols. */
const char* demangleSymbol(const char* mangled_name);

#if __cplusplus
}
#endif
#if __cplusplus
#define typeid_name(sym) demangleSymbol(typeid(sym).name()) 
#else
#define typeid_name(sym) typeid(sym.name())
#endif
#endif


/* cheezy hack to allow me to embed development notes into the code */
/* only tested with CodeWarrior */
#ifdef DEBUG
    #define TODO(message )              int _TODO; " TODO: "message
    #define FIXME(message )             int _FIXME; " FIXME: "message
    #define CRITICAL(message )          int _CRITICAL; " CRITICAL: "message
    #define DEBUG_ONLY( operations )    operations
#else
    #define TODO(message)
    #define FIXME(message)              int _FIXME; " FIXME: "message
    #define CRITICAL(message)           $% " CRITICAL: "message
    #define DEBUG_ONLY( operations )
#endif


/* Macros used to tag which API a call comes from */
/* if you tag your code with these, then you will get clear errors that
   "WinAPI" is undefined if you try to compile Win32 specific code on the
   Mac, for example. Plus, just reading the code it's easy to see where
   your OS dependencies are. */
/* Examples of use:

    WinAPI HANDLE;
    WinAPI::MessageBox("this is a Windows API call);
    
    MacAPI CriticalSection;
    MacAPI::DrawString("this is a Mac OS call);
    
    PosixAPI::pthread_mutex_create();
*/

#ifdef PLATFORM_POSIX
    #define PosixAPI
#endif
#ifdef PLATFORM_WIN32
    #define WinAPI
#endif
#if defined( PLATFORM_MACOS ) || defined( PLATFORM_MACOSX )
    #define MacAPI
#endif

#ifdef PLATFORM_ANDROID
	#define DroidAPI
#endif

#if defined( PLATFORM_IOS ) || defined( PLATFORM_ANDROID )
    #define PLATFORM_OPENGLES 1
#endif

#define Endian16_Swizzle(n)                         \
            (((((unsigned short)n)<<8) & 0xff00)  | \
             ((((unsigned short)n)>>8) & 0x00ff))

#define Endian32_Swizzle(n)                             \
            (((((unsigned long)n)<<24) & 0xff000000)  | \
             ((((unsigned long)n)<<8)  & 0x00ff0000)  | \
             ((((unsigned long)n)>>8)  & 0x0000ff00)  | \
             ((((unsigned long)n)>>24) & 0x000000ff))

float Float_Swizzle(float f);

inline float Float_Swizzle(float f) {
    unsigned long n = *(unsigned long*)&f;
    n = Endian32_Swizzle(n);
    return *(float*)&n;
}

#if PLATFORM_LITTLE_ENDIAN
    #define BigEndian16_ToNative(n) Endian16_Swizzle(n)
    #define BigEndian32_ToNative(n) Endian32_Swizzle(n)
    #define Native_ToBigEndian16(n) Endian16_Swizzle(n)
    #define Native_ToBigEndian32(n) Endian32_Swizzle(n)
    #define Native_ToBigEndianFloat(n) Float_Swizzle(n)
    #define LittleEndian16_ToNative(n) n
    #define LittleEndian32_toNative(n) n
    #define Native_ToLittleEndian16(n) n
    #define Native_ToLittleEndian32(n) n
    #define Native_ToLittleEndianFloat(n) n
#else
    #define BigEndian16_ToNative(n) n
    #define BigEndian32_ToNative(n) n
    #define Native_ToBigEndian16(n) n
    #define Native_ToBigEndian32(n) n
    #define Native_ToBigEndianFloat(n) n
    #define LittleEndian16_ToNative(n) Endian16_Swizzle(n)
    #define LittleEndian32_toNative(n) Endian32_Swizzle(n)
    #define Native_ToLittleEndian16(n) Endian16_Swizzle(n)
    #define Native_ToLittleEndian32(n) Endian32_Swizzle(n)
    #define Native_ToLittleEndianFloat(n) Float_Swizzle(n)
#endif

//! @endcond

#endif /* PLATFORM_H_INCLUDED */

