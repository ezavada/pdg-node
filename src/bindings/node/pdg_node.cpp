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


#define _VALUE_TYPE_STR(val) \
        (val.IsEmpty() ? "empty" : val->IsArray() ? "array" : val->IsFunction() ? "function" : \
		val->IsStringObject() ? "string (object)" : val->IsString() ? "string" : val->IsNull() ? "null" : \
		val->IsUndefined() ? "undefined" : val->IsNumberObject() ? "number (object)" : \
		val->IsNumber() ? "number" : val->IsBoolean() ? "boolean" : val->IsDate() ? "date" : \
		val->IsRegExp() ? "regexp" : val->IsNativeError() ? "error" : val->IsObject() ? "object" : "unknown")

#define _V8_VALUE_TO_CSTRING(cVar, valVar) \
	v8::String::Utf8Value cVar##_Str(valVar->ToString()); CR \
	const char* cVar = *cVar##_Str;

#define DUMP_STRING(s)   do { \
    _V8_VALUE_TO_CSTRING(cstr, s); \
    std::cerr << _VALUE_TYPE_STR(s) << ": " << cstr << "\n"; \
  } while(0);

#define DUMP_AS_STRING(o)   do { \
    _V8_VALUE_TO_CSTRING(cstr, o->ToString()); \
    std::cerr << _VALUE_TYPE_STR(o) << ": " << cstr << "\n"; \
  } while(0);



#ifdef PDG_STANDALONE_NODE_APP
    // all this was taken from node.cc
    #include "util.h"  // for node::OneByteString
    #include "node_internals.h"   // for node::NodeInstanceData
    #include "env.h"  // for node::Environment
    #include "env-inl.h"
    #include "libplatform/libplatform.h"  // for v8::platform
    #include "v8-profiler.h"  // for v8::HeapProfiler
    #undef NO_RETURN // node_internals defines this
    #include "pdg_natives.h"  // embedded pdg.js et al directly into the app as strings
    #ifdef __APPLE__
        #include "atomic-polyfill.h"  // NOLINT(build/include_order)
        namespace node { template <typename T> using atomic = nonstd::atomic<T>; }
    #else
        #include <atomic>
        namespace node { template <typename T> using atomic = std::atomic<T>; }
    #endif
    namespace node {
        // all these are defined in node.cc but were declared static or inline
        extern Environment* CreateEnvironment(v8::Isolate* isolate, v8::Local<v8::Context> context, NodeInstanceData* instance_data);
        extern void EnableDebug(node::Environment* env);
        extern void StartDebug(node::Environment* env, bool wait);
        extern void PlatformInit();
        extern bool ShouldAbortOnUncaughtException(v8::Isolate* isolate);
        extern bool track_heap_objects;
        extern bool use_debug_agent;
        extern bool debug_wait_connect;
        extern bool trace_sync_io;
        extern node::atomic<v8::Isolate*> node_isolate;
        extern v8::Local<v8::Value> ExecuteString(Environment* env, v8::Local<v8::String> source, v8::Local<v8::String> filename);
	    extern v8::Persistent<v8::Object> binding_cache;
    }
    // end stuff taken from node.cc
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

extern "C" void NODE_EXTERN init(v8::Local<v8::Object> target);

#define NODE_LESS_THAN (!(NODE_VERSION_AT_LEAST(4, 2, 0)))

// this is declared in pdg_js_classes
namespace pdg {

void addProcessVersions();
//v8::Local<v8::Object> getProcessObject(v8::Isolate* isolate);
void setupThirdPartyMain(node::Environment* env);
void installIntoNodeApplication(node::Environment* env);

// v8::Local<v8::Object> getProcessObject(v8::Isolate* isolate) {
// 	v8::Local<v8::String> process_symbol = v8::String::NewFromUtf8(isolate, "process", v8::String::kInternalizedString);
//     DUMP_AS_STRING(process_symbol);
//     v8::Local<v8::Object> process = isolate->GetCurrentContext()->Global()->Get(process_symbol)->ToObject();
//     std::cerr << "getProcessObject()\n";
//     DUMP_AS_STRING(process);
//     return process;
// }
// 
// 
#ifdef PDG_STANDALONE_NODE_APP

// install a do-nothing "_third_party_main" as a built-in native module to
// bypass the normal startup
void setupThirdPartyMain(node::Environment* env) {
    TRACEIN;
    v8::Isolate* isolate = env->isolate();
    v8::Local<v8::Object> process = env->process_object();
    
	v8::Local<v8::String> binding_symbol = v8::String::NewFromUtf8(isolate, "binding", v8::String::kInternalizedString);
	v8::Local<v8::String> natives_string = v8::String::NewFromUtf8(isolate, "natives");
	
	// call process.binding("natives"). We have to call it through Javascript
	// because the C++ implementation -- Binding() in node.cc -- is declared static.
	v8::Local<v8::Value> binding_v = process->Get(binding_symbol);
	assert(binding_v->IsFunction());
	v8::Local<v8::Function> binding = v8::Local<v8::Function>::Cast(binding_v);
	
	v8::Local<v8::Value> args[] = { natives_string };
	v8::TryCatch try_catch(isolate);
	binding->Call(process, 1, args);
	if (try_catch.HasCaught()) {
		node::FatalException(try_catch);
	}

	// add blank source code for _third_party_main to the bindings_cache's "natives" 
	// object, which holds source code for native modules that are included as strings
	// this tricks Node.js into not calling it's normal node-main
    v8::Local<v8::Object> binding_cache_ = env->binding_cache_object();
	v8::Local<v8::Object> nativesObj = binding_cache_->Get(
			v8::String::NewFromUtf8(isolate, "natives", v8::String::kInternalizedString) )->ToObject();
	v8::Local<v8::String> source = node::OneByteString(isolate, pdg_main_native, sizeof(pdg_main_native)-1);

	nativesObj->Set(v8::String::NewFromUtf8(isolate, "_third_party_main"), source);
    TRACEOUT;
}
#endif //PDG_STANDALONE_NODE_APP

void addProcessVersions() {
    TRACEIN;
    v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::String> process_symbol = v8::String::NewFromUtf8(isolate, "process", v8::String::kInternalizedString);
	v8::Local<v8::String> pdg_symbol = v8::String::NewFromUtf8(isolate, "pdg", v8::String::kInternalizedString);
	v8::Local<v8::String> versions_symbol = v8::String::NewFromUtf8(isolate, "versions", v8::String::kInternalizedString);
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
	v8::Local<v8::String> chipmunk_symbol = v8::String::NewFromUtf8(isolate, "chipmunk", v8::String::kInternalizedString);
  #endif
  #ifdef PDG_USE_GLFW
	v8::Local<v8::String> glfw_symbol = v8::String::NewFromUtf8(isolate, "glfw", v8::String::kInternalizedString);
  #endif
  #ifdef PDG_USE_LIBPNG
	v8::Local<v8::String> libpng_symbol = v8::String::NewFromUtf8(isolate, "libpng", v8::String::kInternalizedString);
  #endif

	// process.pdg
    v8::Local<v8::Object> process = isolate->GetCurrentContext()->Global()->Get(process_symbol)->ToObject();

	v8::Local<v8::Object> versions = process->Get(versions_symbol)->ToObject();
  	versions->Set(pdg_symbol, v8::String::NewFromUtf8(isolate, PDG_VERSION));
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
  	versions->Set(chipmunk_symbol, v8::String::NewFromUtf8(isolate, cpVersionString));
  #endif
  #ifdef PDG_USE_GLFW
  	char glfw_vers_str[256];
  	std::sprintf(glfw_vers_str, "%d.%d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
  	versions->Set(glfw_symbol, v8::String::NewFromUtf8(isolate, glfw_vers_str));
  #endif
  #ifdef PDG_USE_LIBPNG
  	versions->Set(libpng_symbol, v8::String::NewFromUtf8(isolate, PNG_LIBPNG_VER_STRING));
  #endif
    TRACEOUT;
}

#ifdef PDG_STANDALONE_NODE_APP
// install the pdg framework into the node.js application as a built-in
// native module, accessable at process.pdg
void installIntoNodeApplication(node::Environment* env) {
    TRACEIN;
    v8::Isolate* isolate = env->isolate();
	
	// initialize C++ module
	v8::Local<v8::Object> exports = v8::Object::New(isolate);
	init(exports); // pdg C++ module init

	v8::Local<v8::String> process_symbol = v8::String::NewFromUtf8(isolate, "process", v8::String::kInternalizedString);
	v8::Local<v8::String> pdg_symbol = v8::String::NewFromUtf8(isolate, "pdg", v8::String::kInternalizedString);
	v8::Local<v8::String> natives_symbol = v8::String::NewFromUtf8(isolate, "natives", v8::String::kInternalizedString);

	// process.pdg
    v8::Local<v8::Object> process = isolate->GetCurrentContext()->Global()->Get(process_symbol)->ToObject();

	process->Set(pdg_symbol, exports);

	// this next part closely echos what is done in node::Binding() in node.cc
	// and DefineJavaScript() in node_javascript.cc
	// if those change we may have to rewrite this next section

	// go through all the pdg_natives (embedded js files) and add the source code
	// to the bindings_cache's "natives" object, which holds source code
	// for native modules that are included as strings
    v8::Local<v8::Object> binding_cache_ = env->binding_cache_object();
	v8::Local<v8::Object> nativesObj = binding_cache_->Get(natives_symbol)->ToObject();

	for (int i = 0; natives[i].name; i++) {
	    // std::cerr << "installIntoNodeApplication: " << natives[i].name << "\n";
		v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, natives[i].name);
		v8::Local<v8::String> source = node::OneByteString(isolate, natives[i].source, natives[i].source_len);
		if (natives[i].source == dump_native) {
			// dump is just added to the global context immediately,
			// which is slightly different than running as a node add-on,
			// since there console.dump won't be available until after a
			// require('pdg');
            TRACE("executing dump.js");
			node::ExecuteString(env, source, name);
		} else if (natives[i].source == pdg_main_native) {
			// This should happen last, after everything else has been added.
			// It will do final setup of environment and bootstap the user-land
			// javascript main.js
//            TRACE("executing pdg_main.js");
//			node::ExecuteString(env, source, name);
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
    v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::String> process_symbol = v8::String::NewFromUtf8(isolate, "process", v8::String::kInternalizedString);
	v8::Local<v8::String> _pdgScriptClasses_symbol = v8::String::NewFromUtf8(isolate, "_pdgScriptClasses", v8::String::kInternalizedString);

	// process._pdgScriptClasses
    v8::Local<v8::Object> process = isolate->GetCurrentContext()->Global()->Get(process_symbol)->ToObject();

	v8::Local<v8::Object> obj = process->Get(_pdgScriptClasses_symbol)->ToObject();
	v8::Local<v8::Array> array = obj->GetPropertyNames();
	int length = array->Length();
	for (int i = 0; i < length; i++) {
		v8::Local<v8::Value> prop = array->Get(i);
		v8::Local<v8::Object> proto = obj->Get(prop)->ToObject();
 		VALUE_TO_CSTRING(propName, prop);
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
	process->Delete(_pdgScriptClasses_symbol);
    TRACEOUT;
}


// Deal with an exception thrown from inside Javascript callback function
void FatalException(v8::TryCatch &try_catch) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	node::FatalException(isolate, try_catch);
}

// Deal with a fatal error from a Javascript callback function
void FatalError(v8::Local<v8::Function> func, const char* errorMsg) {
	// TODO: implement this and use it!!
	// see node.cc:1923 (node::FatalException) for implementation clues
}

#ifdef PDG_STANDALONE_NODE_APP

static v8::Platform* default_platform;

// a module that does the same thing as node.js's Start call
static char **copy_argv(int argc, char **argv); // helper for start
static void PrintHelp();
void StartNodeInstance(void* arg);
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
//            DEBUG_PRINT("Script name [%s] passed on command line", argv[i]);
            return argv[i];
    	} else if ((std::strcmp(argv[i], "-e") == 0) && (argc > i+1)) {
//            DEBUG_PRINT("Script name [-e %s] passed on command line", argv[i+1]);
    		return argv[i+1];
    	}
    }
    return 0;
}

static void PrintHelp() {
  printf("Usage: pdg [options] [ -e script | script.js ] [arguments] \n"
         "       pdg debug script.js [arguments] \n"
         "\n"
         "If script is not specified, pdg will search for main.js \n"
         "\n"
         "Options:\n"
         "  -v, --version         print pdg's version\n"
         "  -e, --eval script     evaluate script\n"
         "  -p, --print           evaluate script and print result\n"
         "  -c, --check           syntax check script without executing\n"
         "  -i, --interactive     always enter the REPL even if stdin\n"
         "                        does not appear to be a terminal\n"
         "  -r, --require         module to preload (option can be repeated)\n"
         "  --no-deprecation      silence deprecation warnings\n"
         "  --throw-deprecation   throw an exception anytime a deprecated "
         "function is used\n"
         "  --trace-deprecation   show stack traces on deprecations\n"
         "  --trace-sync-io       show stack trace when use of sync IO\n"
         "                        is detected after the first tick\n"
         "  --track-heap-objects  track heap object allocations for heap "
         "snapshots\n"
         "  --v8-options          print v8 command line options\n"
#if HAVE_OPENSSL
         "  --tls-cipher-list=val use an alternative default TLS cipher list\n"
#endif
#if defined(NODE_HAVE_I18N_SUPPORT)
         "  --icu-data-dir=dir    set ICU data load path to dir\n"
         "                        (overrides NODE_ICU_DATA)\n"
#if !defined(NODE_HAVE_SMALL_ICU)
         "                        note: linked-in ICU data is\n"
         "                        present.\n"
#endif
#endif
         "\n"
         "Environment variables:\n"
#ifdef _WIN32
         "NODE_PATH               ';'-separated list of directories\n"
#else
         "NODE_PATH               ':'-separated list of directories\n"
#endif
         "                        prefixed to the module search path.\n"
         "NODE_DISABLE_COLORS     set to 1 to disable colors in the REPL\n"
#if defined(NODE_HAVE_I18N_SUPPORT)
         "NODE_ICU_DATA           data path for ICU (Intl object) data\n"
#if !defined(NODE_HAVE_SMALL_ICU)
         "                        (will extend linked-in data)\n"
#endif
#endif
         "NODE_REPL_HISTORY       path to the persistent REPL history file\n"
         "\n"
		 "PDG Documentation is at http://ezavada.com/pdg/javascript/html/\n"
         "Documentation can be found at https://nodejs.org/\n");
}

// Entry point for new node instances, also called directly for the main
// node instance.
void StartNodeInstance(void* arg) {
  node::NodeInstanceData* instance_data = static_cast<node::NodeInstanceData*>(arg);
  v8::Isolate::CreateParams params;
  node::ArrayBufferAllocator* array_buffer_allocator = new node::ArrayBufferAllocator();
  params.array_buffer_allocator = array_buffer_allocator;
  v8::Isolate* isolate = v8::Isolate::New(params);
  if (node::track_heap_objects) {
    isolate->GetHeapProfiler()->StartTrackingHeapObjects(true);
  }

  // Fetch a reference to the main isolate, so we have a reference to it
  // even when we need it to access it from another (debugger) thread.
  if (instance_data->is_main())
    CHECK_EQ(nullptr, node::node_isolate.exchange(isolate));

  {
    v8::Locker locker(isolate);
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    
//    std::cerr << "call node::CreateEnvironment()\n";
    node::Environment* env = node::CreateEnvironment(isolate, context, instance_data);
//    std::cerr << "done node::CreateEnvironment()\n";
    array_buffer_allocator->set_env(env);
    v8::Context::Scope context_scope(context);

    // ERZ mod: do some additional pdg specific setup of the process object
    pdg::setupThirdPartyMain(env);
    // end ERZ mod

    isolate->SetAbortOnUncaughtExceptionCallback(
        node::ShouldAbortOnUncaughtException);

    // Start debug agent when argv has --debug
    if (instance_data->use_debug_agent())
      node::StartDebug(env, node::debug_wait_connect);

//    std::cerr << "call node::LoadEnvironment()\n";
    node::LoadEnvironment(env);
//    std::cerr << "done node::LoadEnvironment()\n";

    // ERZ mod: added to support loading pdg modules
//    std::cerr << "calling installIntoNodeApplication()\n";
    TRACE("calling pdg::installIntoNodeApplication()..");
    pdg::installIntoNodeApplication(env);
    // end ERZ mod

//    std::cerr << "set_trace_sync()\n";
    env->set_trace_sync_io(node::trace_sync_io);

    // Enable debugger
    if (instance_data->use_debug_agent())
      node::EnableDebug(env);

    {
      v8::SealHandleScope seal(isolate);
      bool more;
      do {
        v8::platform::PumpMessageLoop(default_platform, isolate);
        more = uv_run(env->event_loop(), UV_RUN_ONCE);

        if (more == false) {
          v8::platform::PumpMessageLoop(default_platform, isolate);
          node::EmitBeforeExit(env);

          // Emit `beforeExit` if the loop became alive either after emitting
          // event, or after running some callbacks.
          more = uv_loop_alive(env->event_loop());
          if (uv_run(env->event_loop(), UV_RUN_NOWAIT) != 0)
            more = true;
        }
      } while (more == true);
    }

    env->set_trace_sync_io(false);

    int exit_code = node::EmitExit(env);
    if (instance_data->is_main())
      instance_data->set_exit_code(exit_code);
    node::RunAtExit(env);

#if defined(LEAK_SANITIZER)
    __lsan_do_leak_check();
#endif

    array_buffer_allocator->set_env(nullptr);
    env->Dispose();
    env = nullptr;
  }

  if (instance_data->is_main()) {
    // Synchronize with signal handler, see TryStartDebugger.
    while (isolate != node::node_isolate.exchange(nullptr));  // NOLINT
  }

  CHECK_NE(isolate, nullptr);
  isolate->Dispose();
  isolate = nullptr;
  delete array_buffer_allocator;
}



int Start(int argc, char *originalArgv[]) {
    TRACEIN;

// ERZ mod, don't copy -psn_ arg, which Mac OS X generates for
// double-clickable applications
// now make a copy of the args and pretend main.js was requested on the command line
	bool needMainJs = false; //(getScriptPassedInArgs(argc, (const char**)originalArgv) == 0);
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
		  PrintHelp();
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

// begin standard Node start
  node::PlatformInit();

  CHECK_GT(argc, 0);

  // Hack around with the argv pointer. Used for process.title = "blah".
  argv = uv_setup_args(argc, argv_clean);

  // This needs to run *before* V8::Initialize().  The const_cast is not
  // optional, in case you're wondering.
  int exec_argc;
  const char** exec_argv;
  node::Init(&argc, const_cast<const char**>(argv), &exec_argc, &exec_argv);

#if HAVE_OPENSSL
  // V8 on Windows doesn't have a good source of entropy. Seed it from
  // OpenSSL's pool.
  v8::V8::SetEntropySource(crypto::EntropySource);
#endif

  const int thread_pool_size = 4;
  default_platform = v8::platform::CreateDefaultPlatform(thread_pool_size);
  v8::V8::InitializePlatform(default_platform);
  v8::V8::Initialize();

  int exit_code = 1;
  {
    node::NodeInstanceData instance_data(node::NodeInstanceType::MAIN,
                                   uv_default_loop(),
                                   argc,
                                   const_cast<const char**>(argv),
                                   exec_argc,
                                   exec_argv,
                                   node::use_debug_agent);
    pdg::StartNodeInstance(&instance_data);
    exit_code = instance_data.exit_code();
  }
  v8::V8::Dispose();

  delete default_platform;
  default_platform = nullptr;

  delete[] exec_argv;
  exec_argv = nullptr;

  return exit_code;

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
extern "C" void NODE_EXTERN init(v8::Local<v8::Object> target) {
  #ifndef PDG_STANDALONE_NODE_APP
    const char* nodestr = "node";
	pdg_LibSaveArgs(1, &nodestr);
  #endif
	pdg::addProcessVersions(); // set up the version flags
	pdg_LibInit();
    pdg::initBindings(target);
};


NODE_MODULE(pdg, init)

