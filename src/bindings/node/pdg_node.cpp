// -----------------------------------------------
// pdg_node.cpp
//
// support and bindings for pdg game engine as a
// node.js add-on
//
// Written by Ed Zavada, 2012
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

//#define PDG_STANDALONE_NODE_APP
//#define WANT_TRACE

#include <node.h>
#include <node_version.h>

#ifdef PDG_STANDALONE_NODE_APP
#include <v8_typed_array.h> // used by pdg::Start() replacement for node::Start()
#include <node_string.h>	// used by pdg::installIntoApplication()
#include "pdg_natives.h"  // embedded pdg.js et al directly into the app as strings
#endif

#ifdef PDG_USE_GLFW
#include "GLFW/glfw3.h"
#endif

#ifdef PDG_USE_LIBPNG
#include "png.h"
#endif

#include "internals.h"
#include "pdg-lib.h"
#include "pdg_script_interface.h"

#ifndef WANT_TRACE
  #define TRACE(msg)
  #define TRACEIN
  #define TRACEOUT
#else
  #define TRACE(msg)   std::cerr << "TRACE: in "<<__func__<<" at "__FILE__":" << __LINE__ << " " << msg << "\n"
  #define TRACEIN TRACE("ENTER")
  #define TRACEOUT TRACE("EXIT")
#endif

// #if defined( PLATFORM_MACOSX ) && !defined( PDG_NO_GUI )
// // defined in pdg-lib-macosx.mm to do things that only the NSApp framework can do
// extern "C" {
// 
// void pdg_LibAppInit();
// void pdg_LibPoolInit();
// void pdg_LibPoolCleanup();
// 
// }
// #endif

extern "C" void NODE_EXTERN init(v8::Handle<v8::Object> target);

namespace node {
	extern v8::Persistent<v8::String> process_symbol;
	extern v8::Persistent<v8::String> domain_symbol;
	extern v8::Persistent<v8::Object> binding_cache;
	extern v8::Persistent<v8::Array> module_load_list;
    void RunAtExit();
	v8::Local<v8::Value> ExecuteString(v8::Handle<v8::String> source, v8::Handle<v8::Value> filename);
}

#define NODE_LESS_THAN (!(NODE_VERSION_AT_LEAST(0, 6, 0)))

// this is declared in pdg_js_classes
namespace pdg {

#ifdef PDG_STANDALONE_NODE_APP
void setupThirdPartyMain(v8::Handle<v8::Object> process_l);
void installIntoNodeApplication();
#endif

void addProcessVersions();

#ifdef PDG_STANDALONE_NODE_APP
// install a do-nothing "_third_party_main" as a built-in native module to
// bypass the normal startup
void setupThirdPartyMain(v8::Handle<v8::Object> process_l) {
    TRACEIN;
	v8::HandleScope scope;

	// call process.binding("natives"). We have to call it through Javascript
	// because the C++ implementation -- Binding() in node.cc -- is declared static.
	v8::Local<v8::Value> binding_v = process_l->Get(v8::String::New("binding"));
	assert(binding_v->IsFunction());
	v8::Local<v8::Function> binding = v8::Local<v8::Function>::Cast(binding_v);
	v8::Local<v8::Value> args[] = { v8::String::New("natives") };
	v8::TryCatch try_catch;
	binding->Call(process_l, 1, args);
	if (try_catch.HasCaught()) {
		node::FatalException(try_catch);
	}

	// add blank source code for _third_party_main to the bindings_cache's "natives" 
	// object, which holds source code for native modules that are included as strings
	// this tricks Node.js into not calling it's normal node-main
	v8::Local<v8::Object> nativesObj = node::binding_cache->Get(
			v8::String::NewSymbol("natives") )->ToObject();
	v8::Handle<v8::String> source = BUILTIN_ASCII_ARRAY(pdg_main_native, sizeof(pdg_main_native)-1);
	nativesObj->Set(v8::String::New("_third_party_main"), source);
    TRACEOUT;
}
#endif //PDG_STANDALONE_NODE_APP

void addProcessVersions() {
    TRACEIN;
	v8::HandleScope scope;
	// process.pdg
	v8::Handle<v8::Object> process = v8::Context::GetCurrent()->Global()
		->Get(node::process_symbol)->ToObject();
	v8::Handle<v8::Object> versions = process->Get(v8::String::NewSymbol("versions"))->ToObject();
  	versions->Set(v8::String::NewSymbol("pdg"), v8::String::New(PDG_VERSION));
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
  	versions->Set(v8::String::NewSymbol("chipmunk"), v8::String::New(cpVersionString));
  #endif
  #ifdef PDG_USE_GLFW
  	char glfw_vers_str[256];
  	std::sprintf(glfw_vers_str, "%d.%d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
  	versions->Set(v8::String::NewSymbol("glfw"), v8::String::New(glfw_vers_str));
  #endif
  #ifdef PDG_USE_LIBPNG
  	versions->Set(v8::String::NewSymbol("libpng"), v8::String::New(PNG_LIBPNG_VER_STRING));
  #endif
    TRACEOUT;
}

#ifdef PDG_STANDALONE_NODE_APP
// install the pdg framework into the node.js application as a built-in
// native module, accessable at process.pdg
void installIntoNodeApplication() {
    TRACEIN;
	v8::HandleScope scope;
	
	// initialize C++ module
	v8::Local<v8::Object> exports = v8::Object::New();
	init(exports); // pdg C++ module init

	// process.pdg
	v8::Handle<v8::Object> process = v8::Context::GetCurrent()->Global()
		->Get(node::process_symbol)->ToObject();
	process->Set(v8::String::NewSymbol("pdg"), exports);

	// this next part closely echos what is done in node::Binding() in node.cc
	// and DefineJavaScript() in node_javascript.cc
	// if those change we may have to rewrite this next section

	// go through all the pdg_natives (embedded js files) and add the source code
	// to the bindings_cache's "natives" object, which holds source code
	// for native modules that are included as strings
	v8::Local<v8::Object> nativesObj = node::binding_cache->Get(
			v8::String::NewSymbol("natives") )->ToObject();

	for (int i = 0; natives[i].name; i++) {
		v8::Local<v8::String> name = v8::String::New(natives[i].name);
		v8::Handle<v8::String> source = BUILTIN_ASCII_ARRAY(natives[i].source, natives[i].source_len);
		if (natives[i].source == dump_native) {
			// dump is just added to the global context immediately,
			// which is slightly different than running as a node add-on,
			// since there console.dump won't be available until after a
			// require('pdg');
            TRACE("executing dump.js");
			node::ExecuteString(source, name);
		} else if (natives[i].source == pdg_main_native) {
			// This should happen last, after everything else has been added.
			// It will do final setup of environment and bootstap the user-land
			// javascript main.js
//            TRACE("executing pdg_main.js");
//			node::ExecuteString(source, name);
		} else {
			nativesObj->Set(name, source);
		}
	}
    TRACEOUT;
}
#endif //PDG_STANDALONE_NODE_APP

// grab all the native class prototypes out of the process object and save them
extern "C" void scriptSetupCompleted() {
    TRACEIN;
	// process._pdgScriptClasses
	v8::Handle<v8::Object> process = v8::Context::GetCurrent()->Global()->Get(node::process_symbol)->ToObject();
//	VALUE_TO_CSTRING(processName, process->ToString());
	v8::Handle<v8::Object> obj = process->Get(v8::String::NewSymbol("_pdgScriptClasses"))->ToObject();
	v8::Handle<v8::Array> array = obj->GetPropertyNames();
	int length = array->Length();
	for (int i = 0; i < length; i++) {
		v8::Handle<v8::Value> prop = array->Get(i);
		v8::Handle<v8::Value> val = obj->Get(prop);
 		VALUE_TO_CSTRING(propName, prop);
		v8::Persistent<v8::Object> proto = v8::Persistent<v8::Object>::New(val->ToObject());
		if (std::strcmp("Color", propName) == 0) {
			v8_SetColorPrototype(proto);
		} else if (std::strcmp("Offset", propName) == 0) {
			v8_SetOffsetPrototype(proto);
		} else if (std::strcmp("Point", propName) == 0) {
			v8_SetPointPrototype(proto);
		} else if (std::strcmp("Vector", propName) == 0) {
			v8_SetVectorPrototype(proto);
		} else if (std::strcmp("Rect", propName) == 0) {
			v8_SetRectPrototype(proto);
		} else if (std::strcmp("RotatedRect", propName) == 0) {
			v8_SetRotatedRectPrototype(proto);
		} else if (std::strcmp("Quad", propName) == 0) {
			v8_SetQuadPrototype(proto);
		}
	}
	process->Delete(v8::String::NewSymbol("_pdgScriptClasses"));
    TRACEOUT;
}


// Deal with an exception thrown from inside Javascript callback function
void FatalException(v8::TryCatch &try_catch) {
	node::FatalException(try_catch);
}

// Deal with a fatal error from a Javascript callback function
void FatalError(v8::Handle<v8::Function> func, const char* errorMsg) {
	// TODO: implement this and use it!!
	// see node.cc:1923 (node::FatalException) for implementation clues
}

#ifdef PDG_STANDALONE_NODE_APP

// a module that does the same thing as node.js's Start call
static char **copy_argv(int argc, char **argv); // helper for start
int Start(int argc, char *argv[]);

static char **copy_argv(int argc, char **argv) {
  size_t strlen_sum;
  char **argv_copy;
  char *argv_data;
  size_t len;
  int i;

  strlen_sum = 0;
  for(i = 0; i < argc; i++) {
    strlen_sum += strlen(argv[i]) + 1;
  }

  argv_copy = (char **) malloc(sizeof(char *) * (argc + 1) + strlen_sum);
  if (!argv_copy) {
    return NULL;
  }

  argv_data = (char *) argv_copy + sizeof(char *) * (argc + 1);

  for(i = 0; i < argc; i++) {
	argv_copy[i] = argv_data;
	len = strlen(argv[i]) + 1;
	memcpy(argv_data, argv[i], len);
    argv_data += len;
  }

  argv_copy[argc] = NULL;

  return argv_copy;
}

// look to see if *.js or "-e filename" were passed on command line
// returns null if none passed
const char* getScriptPassedInArgs(int argc, const char* argv[]) {
    for (int i = 1; i<argc; i++) {
    	const char* last3 = argv[i] + std::strlen(argv[i]) - 3;
        if (std::strcmp(last3, ".js") == 0) {
            DEBUG_PRINT("Script name [%s] passed on command line", argv[i]);
            return argv[i];
    	} else if ((std::strcmp(argv[i], "-e") == 0) && (argc > i+1)) {
            DEBUG_PRINT("Script name [-e %s] passed on command line", argv[i+1]);
    		return argv[i+1];
    	}
    }
    return 0;
}


int Start(int argc, char *originalArgv[]) {
    TRACEIN;
// ERZ mod, don't copy -psn_ arg, which Mac OS X generates for
// double-clickable applications
// now make a copy of the args and pretend main.js was requested on the command line
	bool needMainJs = (getScriptPassedInArgs(argc, (const char**)originalArgv) == 0);
    int i = 0;
    int j = 0;
    char* sMainJsStr = (char*) std::malloc(10);
    std::strcpy(sMainJsStr, "main.js");
    char** argv = (char**)std::malloc(sizeof(char*)*(argc+2));
    for (i = 0; i<argc; i++, j++) {
        argv[j] = originalArgv[i];
        if (std::strncmp(originalArgv[i], "-psn_", 4) == 0) {
            j--; // overwrite the -psn arg
		} else if (strcmp(originalArgv[i], "--version") == 0 || strcmp(originalArgv[i], "-v") == 0) {
		  printf("PDG v%s\nNode.js %s\n", PDG_VERSION, NODE_VERSION);
		  exit(0);
		} else if (strcmp(originalArgv[i], "--help") == 0) {
		  printf("Usage: pdg [options] [script.js] [arguments] \n"
				 "\n"
				 "If script is not specified, pdg will search for main.js \n"
				 "\n"
				 "Options:\n"
				 "  -v, --version        print pdg's version\n"
				 "  --no-deprecation     silence deprecation warnings\n"
				 "  --trace-deprecation  show stack traces on deprecations\n"
				 "  --v8-options         print v8 command line options\n"
				 "  --max-stack-size=val set max v8 stack size (bytes)\n"
				 "\n"
				 "Environment variables:\n"
		#ifdef _WIN32
				 "NODE_PATH              ';'-separated list of directories\n"
		#else
				 "NODE_PATH              ':'-separated list of directories\n"
		#endif
				 "                       prefixed to the module search path.\n"
				 "NODE_DEBUG             Set to \"module\" to debug module loading.\n"
				 "NODE_DISABLE_COLORS    Set to 1 to disable colors in the REPL\n"
				 "\n"
				 "PDG Documentation is at http://ezavada.com/pdg/\n"
				 "Node.js Documentation can be found at http://nodejs.org/\n");
		  exit(0);
        }
    }
    argc -= (i-j); // adjust for argument(s) removed
    if (needMainJs) {
		argv[argc] = sMainJsStr;
		argc++;
    }
    argv[argc] = 0;  // guard

    // uv_setup_args() won't like our non-contigous argv memory, so make a copy
  char **argv_clean = copy_argv(argc, argv);

  // Hack aroung with the argv pointer. Used for process.title = "blah".
  argv = uv_setup_args(argc, argv_clean);


  // Logic to duplicate argv as Init() modifies arguments
  // that are passed into it.
  char **argv_copy = copy_argv(argc, argv);

    TRACE("calling node::Init()..");
  // This needs to run *before* V8::Initialize()
  // Use copy here as to not modify the original argv:
  node::Init(argc, argv_copy);

    TRACE("calling v8::Initialize()..");
  v8::V8::Initialize();
  {
    v8::Locker locker;
    v8::HandleScope handle_scope;

    // Create the one and only Context.
    v8::Persistent<v8::Context> context = v8::Context::New();
    v8::Context::Scope context_scope(context);

    node::process_symbol = NODE_PSYMBOL("process");
    node::domain_symbol = NODE_PSYMBOL("domain");

      TRACE("calling node::SetupProcessObject()..");
   // Use original argv, as we're just copying values out of it.
    v8::Handle<v8::Object> process_l = node::SetupProcessObject(argc, argv);
      TRACE("calling v8_typed_array::AttachBindings()..");
    v8_typed_array::AttachBindings(context->Global());

	// bypass the usual bootstrap and use ours instead
    pdg::setupThirdPartyMain(process_l);

    // Create all the objects, load modules, do everything.
    // so your next reading stop should be node::Load()!
      TRACE("calling node::Load()..");
    node::Load(process_l);
    
    // ERZ modification to standard node Start(), to support loading pdg modules
      TRACE("calling pdg::installIntoNodeApplication()..");
      pdg::installIntoNodeApplication();

    // All our arguments are loaded. We've evaluated all of the scripts. We
    // might even have created TCP servers. Now we enter the main eventloop. If
    // there are no watchers on the loop (except for the ones that were
    // uv_unref'd) then this function exits. As long as there are active
    // watchers, it blocks.
      TRACE("calling uv_run()..");
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

      TRACE("calling node::EmitExit()..");
    node::EmitExit(process_l);
      TRACE("calling node::RunAtExit()..");
    node::RunAtExit();

#ifndef NDEBUG
    context.Dispose();
#endif
  }

#ifndef NDEBUG
  // Clean up. Not strictly necessary.
  v8::V8::Dispose();
#endif  // NDEBUG

  // Clean up the copy:
  free(argv_copy);
    TRACEOUT;
  return 0;
}
#endif // PDG_STANDALONE_NODE_APP



} // end pdg namespace


#ifdef PDG_STANDALONE_NODE_APP

int main(int argc, char *argv[]) {
	pdg_LibSaveArgs(argc, (const char**)argv);
	return pdg::Start(argc, argv);
}

#endif //PDG_STANDALONE_NODE_APP

//#define HACK_TEST_SCML_PDG

#ifdef HACK_TEST_SCML_PDG
	#include "SCML_pdg.h"
	using namespace std;
	using namespace SCML_pdg;
	list<Entity*> entities;
    FileSystem fs;
#endif

// bind the pdg module
extern "C" void NODE_EXTERN init(v8::Handle<v8::Object> target) {
  #ifndef PDG_STANDALONE_NODE_APP
    const char* nodestr = "node";
	pdg_LibSaveArgs(1, &nodestr);
  #endif
	pdg::addProcessVersions(); // set up the version flags
	pdg_LibInit();
    pdg::initBindings(target);
};


NODE_MODULE(pdg, init)

