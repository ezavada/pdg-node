// -----------------------------------------------
// config-macosx.cpp
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

#include "pdg/sys/os.h"

#include "config-macosx.h"

#include <CoreFoundation/CoreFoundation.h>

namespace pdg {

ConfigManagerMac::ConfigManagerMac()
{
    
}

ConfigManagerMac::~ConfigManagerMac()
{
}

bool ConfigManagerMac::useConfig(const char* inConfigName)
{
    mAppKey = inConfigName; // save the new appKey
    std::string keyStr = mAppKey + '_' + "PDG_ConfigExists";
    CFStringRef key = MacAPI::CFStringCreateWithCString(0, keyStr.c_str(), kCFStringEncodingUTF8);

    CFPropertyListRef ref = MacAPI::CFPreferencesCopyAppValue(key, kCFPreferencesCurrentApplication);
    bool exists = (ref != NULL);
    if (ref) {
        MacAPI::CFRelease(ref);
    } else {
        // the config didn't exist, so create it
        MacAPI::CFPreferencesSetAppValue(key, kCFBooleanTrue, kCFPreferencesCurrentApplication);
        MacAPI::CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);  // Write out the preference data.
    }
    MacAPI::CFRelease(key);
	return exists;
}

// getters
// find a given item by case insensitive name in the config, return false if not found, true and value if found
bool ConfigManagerMac::getConfigString(const char* configItemName, std::string& outString)
{
    std::string keyStr = mAppKey + '_' + configItemName;
    CFStringRef key = MacAPI::CFStringCreateWithCString(0, keyStr.c_str(), kCFStringEncodingUTF8);

    CFStringRef value = (CFStringRef) MacAPI::CFPreferencesCopyAppValue(key, kCFPreferencesCurrentApplication);
    bool exists = (value != NULL);
    if (value) {
        int len = MacAPI::CFStringGetLength(value)*6 + 1; // utf8 chars are up to 6 bytes for each unicode char
        char* buf = (char*) std::malloc(len);
        buf[0] = 0;
        DEBUG_ASSERT( MacAPI::CFStringGetCString(value, buf, len, kCFStringEncodingUTF8), "CFString conversion failed" );
        outString = buf;
        std::free(buf);
        MacAPI::CFRelease(value);
    }
    MacAPI::CFRelease(key);
	return exists;
}


bool ConfigManagerMac::getConfigLong(const char* configItemName, long& outLongValue)
{
    std::string keyStr = mAppKey + '_' + configItemName;
    CFStringRef key = MacAPI::CFStringCreateWithCString(0, keyStr.c_str(), kCFStringEncodingUTF8);

    CFNumberRef value = (CFNumberRef) MacAPI::CFPreferencesCopyAppValue(key, kCFPreferencesCurrentApplication);
    bool exists = (value != NULL);
    if (value) {
        long n = 0;
        DEBUG_ASSERT( MacAPI::CFNumberGetValue(value, kCFNumberLongType, &n), "CFNumberGetValue(long) failed");
        outLongValue = n;
        MacAPI::CFRelease(value);
    }
    MacAPI::CFRelease(key);
	return exists;
}

bool ConfigManagerMac::getConfigFloat(const char* configItemName, float& outFloatValue)
{
    std::string keyStr = mAppKey + '_' + configItemName;
    CFStringRef key = MacAPI::CFStringCreateWithCString(0, keyStr.c_str(), kCFStringEncodingUTF8);

    CFNumberRef value = (CFNumberRef) MacAPI::CFPreferencesCopyAppValue(key, kCFPreferencesCurrentApplication);
    bool exists = (value != NULL);
    if (value) {
        float n = 0;
        DEBUG_ASSERT( MacAPI::CFNumberGetValue(value, kCFNumberFloatType, &n), "CFNumberGetValue(float) failed");
        outFloatValue = n;
        MacAPI::CFRelease(value);
    }
    MacAPI::CFRelease(key);
	return exists;
}

bool ConfigManagerMac::getConfigBool(const char* configItemName, bool& outBooleanValue)
{
    std::string keyStr = mAppKey + '_' + configItemName;
    CFStringRef key = MacAPI::CFStringCreateWithCString(0, keyStr.c_str(), kCFStringEncodingUTF8);

    Boolean existsAndValid = false;
    Boolean value = MacAPI::CFPreferencesGetAppBooleanValue(key, kCFPreferencesCurrentApplication, &existsAndValid);
    outBooleanValue = value;
    MacAPI::CFRelease(key);
    return existsAndValid;
}

// setters
// change a given item by case insensitive name in the config, save the changes immediately
void ConfigManagerMac::setConfigString(const char* configItemName, std::string inString)
{
    std::string keyStr = mAppKey + '_' + configItemName;
    CFStringRef key = MacAPI::CFStringCreateWithCString(0, keyStr.c_str(), kCFStringEncodingUTF8);
    CFStringRef value = MacAPI::CFStringCreateWithCString(0, inString.c_str(), kCFStringEncodingUTF8);
    MacAPI::CFPreferencesSetAppValue(key, value, kCFPreferencesCurrentApplication);
    MacAPI::CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication); // write out the prefs data
}

void ConfigManagerMac::setConfigLong(const char* configItemName, long inLongValue)
{
    std::string keyStr = mAppKey + '_' + configItemName;
    CFStringRef key = MacAPI::CFStringCreateWithCString(0, keyStr.c_str(), kCFStringEncodingUTF8);
    CFNumberRef value = MacAPI::CFNumberCreate(0, kCFNumberLongType, &inLongValue);
    MacAPI::CFPreferencesSetAppValue(key, value, kCFPreferencesCurrentApplication);
    MacAPI::CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication); // write out the prefs data
}

void ConfigManagerMac::setConfigFloat(const char* configItemName, float inFloatValue)
{
    std::string keyStr = mAppKey + '_' + configItemName;
    CFStringRef key = MacAPI::CFStringCreateWithCString(0, keyStr.c_str(), kCFStringEncodingUTF8);
    CFNumberRef value = MacAPI::CFNumberCreate(0, kCFNumberFloatType, &inFloatValue);
    MacAPI::CFPreferencesSetAppValue(key, value, kCFPreferencesCurrentApplication);
    MacAPI::CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication); // write out the prefs data
}

void ConfigManagerMac::setConfigBool(const char* configItemName, bool inBooleanValue)
{
    std::string keyStr = mAppKey + '_' + configItemName;
    CFStringRef key = MacAPI::CFStringCreateWithCString(0, keyStr.c_str(), kCFStringEncodingUTF8);
    CFBooleanRef value = inBooleanValue ? kCFBooleanTrue : kCFBooleanFalse;
    MacAPI::CFPreferencesSetAppValue(key, value, kCFPreferencesCurrentApplication);
    MacAPI::CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication); // write out the prefs data
}

ConfigManager* ConfigManager::createSingletonInstance() {
	return new ConfigManagerMac();
}


} // close namespace pdg

