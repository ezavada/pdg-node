// -----------------------------------------------
// config-unix.h
// 
// Definitions for reading/writing configuration data
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

#ifndef CONFIGMANAGER_UNIX_H_INCLUDED
#define CONFIGMANAGER_UNIX_H_INCLUDED

#include "pdg/sys/config.h"

#include <vector>

//#include <stl_function.h>

namespace pdg {

class ConfigManagerUnix : public ConfigManager
{ 
private:
	class Record
	{
	public:
		std::string mKey;
		std::string mValue;
		Record()
		{
			mKey = "";
			mValue = "";
		}
		Record(const char* key, const char* value)
		{
			mKey = key;
			mValue = value;
		}
		~Record(){};
	};
public:
	ConfigManagerUnix();
	virtual ~ConfigManagerUnix();
	
	// return false if config not found, unusable or empty
    bool  useConfig(const char* inConfigName);
    
    // getters
    // find a given item by case insensitive name in the config, return false if not found, true and value if found
    bool  getConfigString(const char* configItemName, std::string& outString);
    bool  getConfigLong(const char* configItemName, long& outLongValue);
    bool  getConfigFloat(const char* configItemName, float& outLongValue);
    bool  getConfigBool(const char* configItemName, bool& outBooleanValue);
    
    // setters
    // change a given item by case insensitive name in the config, save the changes immediately
    void  setConfigString(const char* configItemName, std::string outString);
    void  setConfigLong(const char* configItemName, long outLongValue);
    void  setConfigFloat(const char* configItemName, float outLongValue);
    void  setConfigBool(const char* configItemName, bool outBooleanValue);
	
protected:
	std::vector<Record> recordList;
	std::string mFileName;

	bool create(const char* fileName);

	bool createRecord(const char* strKey, const char* value);
	bool deleteRecord(const char* keyName);
	bool getRecord(const char* keyName, Record& data);
	bool recordExists(const char* keyName);

	bool setValue(std::string keyName, std::string value);
	bool save();
	bool load();

	void trim(std::string& str, const std::string & chrsToTrim = " \t\n\r", int trimDir = 0);
	void convertToUpper(std::string& str);
	
	struct RecordKeyIs : std::unary_function<Record, bool>
	{
		std::string key_;		
		RecordKeyIs(const std::string& key): key_(key){}
		bool operator()( const Record& rec ) const
		{
			return (rec.mKey == key_);
		}
	};
};
}  // close namespace pdg

#endif // CONFIGMANAGER_UNIX_H_INCLUDED

