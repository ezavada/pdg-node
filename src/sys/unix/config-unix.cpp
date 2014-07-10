// -----------------------------------------------
// config-unix.cpp
// 
// Unix specific definitions for reading/writing configuration data
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

#include "pdg/msvcfix.h"  // fix non-standard MSVC
#include "config-unix.h"

#include <fstream>
#include <cstdlib>
#include <algorithm>

namespace pdg {

ConfigManagerUnix::ConfigManagerUnix()
{
	mFileName = ("");
	recordList.clear();
}
ConfigManagerUnix::~ConfigManagerUnix()
{
	mFileName = ("");
	recordList.clear();
}

bool ConfigManagerUnix::useConfig(const char* inConfigName)
{
	mFileName = inConfigName;
	return load();
}

// getters
// find a given item by case insensitive name in the config, return false if not found, true and value if found
bool ConfigManagerUnix::getConfigString(const char* configItemName, std::string& outString)
{
	Record data;
	if(getRecord(configItemName,data))
	{
		outString = data.mValue;
		return true;
	}
	return false;
}

bool ConfigManagerUnix::getConfigLong(const char* configItemName, long& outLongValue)
{
	Record data;
	if(getRecord(configItemName,data))
	{
		outLongValue = std::atol(data.mValue.c_str());
		return true;
	}
	return false;}

bool ConfigManagerUnix::getConfigFloat(const char* configItemName, float& outLongValue)
{
	Record data;
	if(getRecord(configItemName,data))
	{
		sscanf(data.mValue.c_str(),"%f",&outLongValue);
		return true;
	}
	return false;
}

bool ConfigManagerUnix::getConfigBool(const char* configItemName, bool& outBooleanValue)
{
	Record data;
	if(getRecord(configItemName,data))
	{
		if(data.mValue.at(0) == '0')
            outBooleanValue = false;
		else
			outBooleanValue = true;
		return true;
	}
	return false;
}

// setters
// change a given item by case insensitive name in the config, save the changes immediately
void ConfigManagerUnix::setConfigString(const char* configItemName, std::string outString)
{
	if(NULL == configItemName)
		return;
	if(recordExists(configItemName))
	{
		std::string str = configItemName;
		setValue(str,outString);
		return;
	}
	createRecord(configItemName,outString.c_str());
}

void ConfigManagerUnix::setConfigLong(const char* configItemName, long outLongValue)
{
	if(NULL == configItemName)
		return;
	char buffer[40];
	std::sprintf(buffer,"%ld\n",outLongValue);
	if(recordExists(configItemName))
	{
		std::string str = configItemName;
		std::string value = buffer;
		setValue(str,value);
		return;
	}
	createRecord(configItemName,buffer);
}

void ConfigManagerUnix::setConfigFloat(const char* configItemName, float outLongValue)
{
	if(NULL == configItemName)
		return;
	char buffer[40];
	sprintf(buffer,"%f\n",outLongValue);//long d = std::_ftoa(outLongValue,buffer,10);
	if(recordExists(configItemName))
	{
		std::string str = configItemName;
		std::string value = buffer;
		setValue(str,value);
		return;
	}
	createRecord(configItemName,buffer);
}

void ConfigManagerUnix::setConfigBool(const char* configItemName, bool outBooleanValue)
{
	if(NULL == configItemName)
		return;
	std::string value;
	if(outBooleanValue)
		value = '1';
	else
		value = '0';
	if(recordExists(configItemName))
	{
		std::string str = configItemName;
		setValue(str,value);
		return;
	}
	createRecord(configItemName,value.c_str());
}

bool ConfigManagerUnix::createRecord(const char* strKey, const char* value)
{
	Record r(strKey,value);
	convertToUpper(r.mKey);
	recordList.push_back(r);
	return save();
}

bool ConfigManagerUnix::deleteRecord(const char* keyName)
{
	std::string str = keyName;
	convertToUpper(str);
	std::vector<Record>::iterator iter = std::find_if(recordList.begin(), 
			recordList.end(), 
			RecordKeyIs(str));	// Locate the Section/Key

	if (iter == recordList.end()) return false;				// The Section/Key was not found

	recordList.erase(iter);									// Remove the Record
	return save();											// Save
}

bool ConfigManagerUnix::getRecord(const char* keyName, Record& data)
{
	std::string str = keyName;
	convertToUpper(str);
	std::vector<Record>::iterator iter = std::find_if(recordList.begin(), 
			recordList.end(), 
			RecordKeyIs(str));			// Locate the Record

	if (iter == recordList.end())
	{
		data.mKey = "";
		data.mValue = "";
		return false;								// The Record was not found
	}

	data.mKey = (*iter).mKey;												// The Record was found
	data.mValue = (*iter).mValue;
	return true;															// Return the Record
}

bool ConfigManagerUnix::recordExists(const char* keyName)
{
	std::string str = keyName;
	convertToUpper(str);
	std::vector<Record>::iterator iter = std::find_if(recordList.begin(), 
		recordList.end(), RecordKeyIs(str));			// Locate the Section/Key
	if (iter == recordList.end()) return false;							// The Section/Key was not found

	return true;															// The Section/Key was found
}

bool ConfigManagerUnix::setValue(std::string keyName, std::string value)
{
	convertToUpper(keyName);
	std::vector<Record>::iterator iter = std::find_if(recordList.begin(), 
			recordList.end(), 
			RecordKeyIs(keyName));			// Locate the Record

	if (iter == recordList.end())
		return false;								// The Record was not found

	(*iter).mValue = value;
	return save();	
}

bool ConfigManagerUnix::save()
{
	std::ofstream outFile;
	outFile.open(mFileName.c_str());				// Create an output filestream
	if (!outFile.is_open()) return false;				// If the output file doesn't open, then return
	for (int i=0;i<(int)recordList.size();i++)			// Loop through each vector
	{
		outFile << recordList[i].mKey << "=" 
			<< recordList[i].mValue << std::endl;					// Then format the section
	}
	outFile.close();									// Close the file
	return true;
}

bool ConfigManagerUnix::create(const char* FileName)
{
	return save();
}

bool ConfigManagerUnix::load()
{
	std::string s;									// Holds the current line from the config file
	std::ifstream inFile (mFileName.c_str());		// Create an input filestream

	if (!inFile.is_open()) return false;		// If the input file doesn't open, then return
	
	recordList.clear();						// Clear the content vector

	while(!std::getline(inFile, s).eof())		// Read until the end of the file
	{
		trim(s);								// Trim whitespace from the ends
		if(!s.empty())							// Make sure its not a blank line
		{
			Record r;							// Define a new record
			if(s.find('=')!=std::string::npos)				// Is this line a Key/Value?
			{
				r.mKey = s.substr(0,s.find('='));		// Set the Key value to everything before the = sign
				r.mValue = s.substr(s.find('=')+1);		// Set the Value to everything after the = sign
				convertToUpper(r.mKey);
			}
			recordList.push_back(r);					// Add the record to content
		}
	}
	inFile.close();										// Close the file
	return true;

}

void ConfigManagerUnix::trim(std::string& str, const std::string & ChrsToTrim , int TrimDir )
{
    size_t startIndex = str.find_first_not_of(ChrsToTrim);
    if (startIndex == std::string::npos){str.erase(); return;}
    if (TrimDir < 2) str = str.substr(startIndex, str.size()-startIndex);
    if (TrimDir!=1) str = str.substr(0, str.find_last_not_of(ChrsToTrim) + 1);
}

void ConfigManagerUnix::convertToUpper(std::string& str)
{
	for(int i=0;i < (int)str.size(); i++)
	{
		str[i] = toupper(str.at(i));
	}
}

ConfigManager* ConfigManager::createSingletonInstance()
{
	return new ConfigManagerUnix();
}

}   // close namespace pdg

