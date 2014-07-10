# -----------------------------------------------
# binding.gyp (node.js makefile)
#
# Written by Ed Zavada, 2014
# Copyright (c) 2014, Dream Rock Studios, LLC
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to permit
# persons to whom the Software is furnished to do so, subject to the
# following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
# NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
# USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# -----------------------------------------------

# -*- mode: python -*-
{

  'variables': {
    'v8_use_snapshot%': 'true',
    'srcdir': '.',
    'blddir': 'build',
    'VERSION': '0.1.0',
    'macosx_min_vers': '10.6',
    'node_version': '<!(node --version)',
    'chipmunk_dir': './deps/chipmunk',
    'scmlpp_dir': './deps/scml-pp',
    'linux_files': [
      'src/sys/glfw/platform-events-glfw.cpp',
      'src/sys/unix/image-unix.cpp',
      'src/sys/unix/config-unix.cpp',
      'src/sys/unix/platform-unix.cpp',
      'src/sys/unix/os-unix.cpp',
    ],
    'macosx_files': [
      'src/sys/macosx/config-macosx.cpp',
      'src/sys/macosx/platform-dirs-macosx.cpp',
      'src/sys/macosx/platform-image-macosx.mm',
      'src/sys/macosx/platform-events-macosx.cpp',
      'src/sys/unix/os-unix.cpp',
    ],
    'win32_files': [
      'src/sys/glfw/platform-events-glfw.cpp',
      'src/sys/win32/os-win32.cpp',
      'src/sys/win32/config-win32.cpp',
      'src/sys/win32/platform-win32.cpp',
    ],
    'png_files': [
      'src/sys/image-png.cpp',
      'deps/png/png.c',
      'deps/png/pngerror.c',
      'deps/png/pngget.c',
      'deps/png/pngmem.c',
      'deps/png/pngpread.c',
      'deps/png/pngread.c',
      'deps/png/pngrio.c',
      'deps/png/pngrtran.c',
      'deps/png/pngrutil.c',
      'deps/png/pngset.c',
      'deps/png/pngtrans.c',
    ],
  },

  'targets': [
    {
      'target_name': 'pdg',
      'include_dirs': [ 
        './src/inc',
        './src/sys', 
        './deps/glfw/include',
        './deps',
        './deps/png',
        './deps/zlib',
        './deps/zlib/contrib/minizip',
    	'./src/bindings/javascript',
    	'./src/bindings/javascript/v8', 
    	'./src/bindings/node', 
    	'./src/sys/unix', 
    	'./src/sys/macosx', 
    	'./src/sys/unix', 
    	'./src/sys/win32', 
    	'./src/sys/glfw', 
    	'<(chipmunk_dir)/include', 
    	'<(scmlpp_dir)/source', 
    	'<(scmlpp_dir)/source/libraries', 
    	'<(scmlpp_dir)/source/renderers',
      ],
      'libraries': [
      ],
      'defines': [
      	'DEBUG', 
  		#'PDG_DEBUG_SOUND', 
  		'PDG_DEBUG_OUT_TO_LOG', 
  		'PDG_NO_DEBUG_TO_CONSOLE', 
  		#'-DPDG_DEBUG_JAVASCRIPT',
  		'PDG_USE_CHIPMUNK_PHYSICS', 
  		'PDG_SCML_SUPPORT',
  		'PDG_NO_GUI',
  		'PDG_NO_SOUND',
  		'PDG_NO_APP_FRAMEWORK',
  		'PDG_NO_NETWORK', 
  		'PDG_LIBRARY', 
  		'PDG_NO_SLEEP',
  		'PDG_INTERNAL_LIB', 
  		'PDG_NODE_MODULE', 
  		'PDG_COMPILING_FOR_JAVASCRIPT',
      ],
      'cflags': [ 
      	'-g', 
      	#'-Wno-parentheses-equality', # breaks older Gcc
      	#'-O0', 
      	#'-fno-default-inline', 
      	#'-fno-inline',
      ],
      'cflags_c': [ '-std=c99' ],
      'cflags_cc!': [ '-fno-exceptions', '-fno-rtti' ],
      'sources': [
        # extra zip files
        'deps/zlib/contrib/minizip/unzip.c',
        'deps/zlib/contrib/minizip/ioapi.c',
        # chipmunk physics
        'deps/chipmunk/src/chipmunk.c',
        'deps/chipmunk/src/constraints/cpConstraint.c',
        'deps/chipmunk/src/constraints/cpDampedRotarySpring.c',
        'deps/chipmunk/src/constraints/cpDampedSpring.c',
        'deps/chipmunk/src/constraints/cpGearJoint.c',
        'deps/chipmunk/src/constraints/cpGrooveJoint.c',
        'deps/chipmunk/src/constraints/cpPinJoint.c',
        'deps/chipmunk/src/constraints/cpPivotJoint.c',
        'deps/chipmunk/src/constraints/cpRatchetJoint.c',
        'deps/chipmunk/src/constraints/cpRotaryLimitJoint.c',
        'deps/chipmunk/src/constraints/cpSimpleMotor.c',
        'deps/chipmunk/src/constraints/cpSlideJoint.c',
        'deps/chipmunk/src/cpArbiter.c',
        'deps/chipmunk/src/cpArray.c',
        'deps/chipmunk/src/cpBB.c',
        'deps/chipmunk/src/cpBBTree.c',
        'deps/chipmunk/src/cpBody.c',
        'deps/chipmunk/src/cpCollision.c',
        'deps/chipmunk/src/cpHashSet.c',
        'deps/chipmunk/src/cpPolyShape.c',
        'deps/chipmunk/src/cpShape.c',
        'deps/chipmunk/src/cpSpace.c',
        'deps/chipmunk/src/cpSpaceComponent.c',
        'deps/chipmunk/src/cpSpaceHash.c',
        'deps/chipmunk/src/cpSpaceQuery.c',
        'deps/chipmunk/src/cpSpaceStep.c',
        'deps/chipmunk/src/cpSpatialIndex.c',
        'deps/chipmunk/src/cpSweep1D.c',
        'deps/chipmunk/src/cpVect.c',
        # Spriter support
        'deps/scml-pp/source/SCMLpp.cpp',
        'deps/scml-pp/source/libraries/tinystr.cpp',
        'deps/scml-pp/source/libraries/tinyxml.cpp',
        'deps/scml-pp/source/libraries/tinyxmlerror.cpp',
        'deps/scml-pp/source/libraries/tinyxmlparser.cpp',
        'deps/scml-pp/source/libraries/XML_Helpers.cpp',
        'deps/scml-pp/source/renderers/SCML_pdg.cpp',
        # core pdg library
        'src/sys/animated.cpp',
        'src/sys/ConvertUTF.c',
        'src/sys/collisiondetection.cpp',
        'src/sys/deserializer.cpp',
        'src/sys/eventemitter.cpp',
        'src/sys/eventmanager.cpp',
        'src/sys/image.cpp',
        'src/sys/log.cpp',
        'src/sys/os.cpp',
        'src/sys/pdg-lib.cpp',
        'src/sys/pdg-main.cpp',
        'src/sys/resource.cpp',
        'src/sys/serializer.cpp',
        #'src/sys/spline.cpp',
        'src/sys/sprite.cpp',
        'src/sys/spritelayer.cpp',
        'src/sys/spritemanager.cpp',
        'src/sys/tilelayer.cpp',
        'src/sys/timermanager.cpp',
        'src/sys/userdata.cpp',
        # pdg javascript and node bindings
        'src/bindings/javascript/v8/pdg_v8_support.cpp',
        'src/bindings/javascript/v8/pdg_script_impl.cpp',
        'src/bindings/javascript/v8/pdg_interfaces.cpp',
        'src/bindings/node/pdg_node.cpp',
      ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_RTTI': 'YES',
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'OTHER_CFLAGS': [
          '-std=c99',
      	  #'-Wno-parentheses-equality', # breaks older gcc (Mac OS X 10.6)
        ],
        'OTHER_CPLUSPLUSFLAGS': [
        ],
        'OTHER_LDFLAGS': [
          #'-macosx-version-min=<@(macosx_min_vers)',  # breaks XCode 5+
        ],
      },
      'conditions': [
         [ 'OS=="linux"', {
             'sources': ['<@(png_files)>', '<@(linux_files)'],
           }
         ],
         [ 'OS=="mac"', {
             'sources': ['<@(macosx_files)'],
             'link_settings': {
                            'libraries': [
                                '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
                                '$(SDKROOT)/System/Library/Frameworks/AppKit.framework',
                            ]
             },
             #'cflags' doesn't work with gyp on Xcode, have to use OTHER_CFLAGS above
           }
         ],
         [ 'OS=="win"', {
             'msvs_settings': {
               'VCCLCompilerTool': {
                 'CompileAsCpp': 1,
               }
             },
             'sources': ['<@(png_files)>', '<@(win32_files)>'],
           }
         ],
      ],
    },
  ],
}

