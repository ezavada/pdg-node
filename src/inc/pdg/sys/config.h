// -----------------------------------------------
// config.h
// 
// retrieve configuration infomation in a platform
// independent fashion
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


#ifndef PDG_CONFIG_H_INCLUDED
#define PDG_CONFIG_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"
#include "pdg/sys/singleton.h"

#include <string>

// -----------------------------------------------------------------------------------
// predefined config items that the framework will track
// -----------------------------------------------------------------------------------

#define PDG_CONFIG_LAST_WINDOW_WIDTH   "LAST_WINDOW_WIDTH"
#define PDG_CONFIG_LAST_WINDOW_HEIGHT  "LAST_WINDOW_HEIGHT"

namespace pdg {

// -----------------------------------------------------------------------------------
// ConfigManager
// used for loading and saving application configuration info
// -----------------------------------------------------------------------------------
/** Interface to Save and Load game config values.
 * This class is a pure virtual (interface) class that provides simple
 * configuration management. It does the right thing on each platform to
 * save and load values that need to persist between game sessions. It is
 * not intended to be used to hold saved games.
 * It is not dependent on any other parts of the Pixel Dust Game Framework.
 * \ingroup Managers
 */
class ConfigManager : public Singleton<ConfigManager> {
friend class Singleton<ConfigManager>;
public:
    /** Specify the name of the configuration to use.
	 * Specify an abstract name for the config to be used. Implementations will use
     * this to build the name of a file on disk, a registry entry, or something else
     * that's appropriate for the platform you are running on. 
	 * @param inConfigName Only alpha characters (uppercase and lowercase) are allowed, name is case insensitive
     * @return false if config not found, unusable or empty
	 */
    virtual bool  useConfig(const char* inConfigName) = 0;
    
    // getters
	
    /** Get a config value as a string.
	 * Search the current configuration for a config entry with the same name
	 * and return the value associated with it as a string
     * returns false if not found, true if found and value returned via outString
	 * @param configItemName name of item to search for in config, case insensitive
	 * @param outString holds config string value if found
     */
    virtual bool  getConfigString(const char* configItemName, std::string& outString) = 0;

    /** Get a config value as a long integer.
	 * Search the current configuration for a config entry with the same name
	 * and return the value associated with it as a long integer value
	 * @param configItemName name of item to search for in config, case insensitive
	 * @param outLongValue holds config long integer value if found
     * @return false if not found, true if found and value returned via outLongValue
     */
    virtual bool  getConfigLong(const char* configItemName, long& outLongValue) = 0;

    /** Get a config value as a floating point number.
	 * Search the current configuration for a config entry with the same name
	 * and return the value associated with it as a floating point value
	 * @param configItemName name of item to search for in config, case insensitive
	 * @param outFloatValue holds config float value if found
     * @return false if not found, true if found and value returned via outFloatValue
     */
    virtual bool  getConfigFloat(const char* configItemName, float& outFloatValue) = 0;

    /** Get a config value as a boolean.
	 * Search the current configuration for a config entry with the same name
	 * and return the value associated with it as a boolean value
	 * @param configItemName name of item to search for in config, case insensitive
	 * @param outBooleanValue holds config boolean value if found
     * @return false if not found, true if found and value returned via outBooleanValue
     */
    virtual bool  getConfigBool(const char* configItemName, bool& outBooleanValue) = 0;
    
    // setters
	
    /** Add or change a config value string.
	 * If no config item with that name exists, one is added. Changes to the config
	 * are saved immediately. You should always use the corresponding getConfigString
	 * call to retrieve any value stored with setConfigString.
	 * @param configItemName name of item in config to change or add, case not preserved
	 * @param inString the new string value for the config item
     */
    virtual void  setConfigString(const char* configItemName, std::string inString) = 0;
	
    /** Add or change a config long integer value.
	 * If no config item with that name exists, one is added. Changes to the config
	 * are saved immediately. You should always use the corresponding getConfigLong
	 * call to retrieve any value stored with setConfigLong.
	 * @param configItemName name of item in config to change or add, case not preserved
	 * @param inLongValue the new long integer value for the config item
     */
    virtual void  setConfigLong(const char* configItemName, long inLongValue) = 0;
	
    /** Add or change a config floating point value.
	 * If no config item with that name exists, one is added. Changes to the config
	 * are saved immediately. You should always use the corresponding getConfigFloat
	 * call to retrieve any value stored with setConfigFloat.
	 * @param configItemName name of item in config to change or add, case not preserved
	 * @param inFloatValue the new floating point value for the config item
     */
    virtual void  setConfigFloat(const char* configItemName, float inFloatValue) = 0;
	
    /** Add or change a config boolean value.
	 * If no config item with that name exists, one is added. Changes to the config
	 * are saved immediately. You should always use the corresponding getConfigBool
	 * call to retrieve any value stored with setConfigBool.
	 * @param configItemName name of item in config to change or add, case not preserved
	 * @param inBooleanValue the new boolean value for the config item
     */
    virtual void  setConfigBool(const char* configItemName, bool inBooleanValue) = 0;
    
// lifecycle
 //! @cond C++
    virtual ~ConfigManager() {}
 //! @endcond
 
protected:
	static ConfigManager* createSingletonInstance();
};
 
}  // close namespace pdg

#endif // PDG_CONFIG_H_INCLUDED

