// -----------------------------------------------
// config-win32.cpp
// 
// Windows specific definitions for reading/writting configuration data
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

#include "pdg/sys/os.h"

#include "config-win32.h"
#include "configResource.h"

#ifdef LEAK_AND_EXCEPTION_CHECKS
#include "..\LeakCheck\LeakCheck.h"
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define THIS_FILE __FILE__
#endif

//#define PDG_DEBUG_CONFIG_MANAGER

#ifndef PDG_DEBUG_CONFIG_MANAGER
	#define CONFIG_MANAGER_DEBUG_ONLY( ops )
#else
	#define CONFIG_MANAGER_DEBUG_ONLY( ops ) ops
#endif


namespace pdg {

ConfigManagerWin32::ConfigManagerWin32(HINSTANCE appInstance)
{
	mResourceID = IDS_REGKEYSTR;
	mHKEY = 0;
	mAppInstance = appInstance;
	if (mAppInstance == 0) {
		mAppInstance = GetModuleHandle(NULL);
	}
}
ConfigManagerWin32::~ConfigManagerWin32()
{
}

bool ConfigManagerWin32::useConfig(const char* inConfigName)
{
	char buffer[1024];
	LoadStringA(mAppInstance,mResourceID,buffer,1024);
	std::string resString = buffer;
	if(resString.empty()) // If we can't load the key we can't create the key
	{
		resString = "SOFTWARE\\PDG_ENGINE\\";
		CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("couldn't load resource %d, using %s default value", mResourceID, resString.c_str()); )
	}

	if(!resString.empty() && resString.at(resString.length()-1) != '\\') {
        resString += '\\';
    }
    resString += inConfigName;
	CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("Using Registry Key [%s]", resString.c_str()); )
	if(verifyKey(HKEY_CURRENT_USER,resString.c_str()))
	{
		mHKEY = HKEY_CURRENT_USER;
        mKeyPath = resString + '\\';
		CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("Key Found in HKEY_CURRENT_USER [%s]", resString.c_str()); )
		return true;
	}
	else if(verifyKey(HKEY_LOCAL_MACHINE,resString.c_str()))
	{
		mHKEY = HKEY_LOCAL_MACHINE;
		mKeyPath = resString + '\\';
		CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("Key Found in HKEY_LOCAL_MACHINE [%s]", resString.c_str()); )
		return true;
	}
    // do this automatically to make sure that the key is present
	CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("Key Not Found, creating it [%s]", resString.c_str()); )
    createKey(inConfigName);
    // but return false to let them know it wasn't already there
	return false;
}

bool ConfigManagerWin32::verifyKey(HKEY hKeyRoot, const char* strPath)
{
	HKEY hkResult;
	LONG ReturnValue = RegOpenKeyExA (hKeyRoot, strPath, 0L,KEY_ALL_ACCESS, &hkResult);
	if(ReturnValue == ERROR_SUCCESS)
	{
		RegCloseKey (hkResult);
		return TRUE;
	}
	return FALSE;
}


// getters
// find a given item by case insensitive name in the config, return false if not found, true and value if found
bool ConfigManagerWin32::getConfigString(const char* configItemName, std::string& outString)
{
	CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("ConfigManager getConfigString [%s]", configItemName); )
	DWORD dwType = REG_SZ;
	DWORD dwSize = 0;
	HKEY hKey;
	
	//std::string tempString = mKeyPath + '\' + configItemName;
	//int size = GetDataSize(tempString.c_str());
	int size = GetDataSize(configItemName);
	if ( -1 == size)
        return false;

	char* buffer = new char[size + 1];
	dwSize = size;

	//if(RegOpenKeyEx(mHKEY, tempString.c_str(), 0,KEY_READ, &hKey) != ERROR_SUCCESS)
	if(RegOpenKeyExA(mHKEY, mKeyPath.c_str(), 0,KEY_READ, &hKey) != ERROR_SUCCESS)
	{
        delete [] buffer;
		return false;
	}

	//if(RegQueryValueEx(hKey, tempString.c_str(), NULL,&dwType, (LPBYTE)buffer, &dwSize) != ERROR_SUCCESS)
	if(RegQueryValueExA(hKey, configItemName, NULL,&dwType, (LPBYTE)buffer, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		delete [] buffer;
		return false;
	}	
	outString = buffer;
	RegCloseKey(hKey);	
	delete [] buffer;
	return true;
}

bool ConfigManagerWin32::getConfigLong(const char* configItemName, long& outLongValue)
{
	CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("ConfigManager getConfigLong [%s]", configItemName); )
	DWORD dwType = REG_BINARY;
	long d;
	DWORD dwSize = sizeof(d);
	HKEY hKey;
	//std::string tempString = mKeyPath + '\' + configItemName;

	if (RegOpenKeyExA(mHKEY, mKeyPath.c_str(), 0,KEY_READ, &hKey) != ERROR_SUCCESS)
		return false;

	//if (RegQueryValueEx(mHKEY, mKeyPath.c_str(), 0,&dwType, (LPBYTE)&d, &dwSize) != ERROR_SUCCESS)
	if (RegQueryValueExA(hKey, configItemName, 0,&dwType, (LPBYTE)&d, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return false;
	}
	outLongValue = d;
	RegCloseKey(hKey);	
	return true;
}

bool ConfigManagerWin32::getConfigFloat(const char* configItemName, float& outLongValue)
{
	CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("ConfigManager getConfigFloat [%s]", configItemName); )
	DWORD dwType = REG_BINARY;
	float d;
	DWORD dwSize = sizeof(d);
	HKEY hKey;
	//std::string tempString = mKeyPath + '\' + configItemName;

	if (RegOpenKeyExA(mHKEY, mKeyPath.c_str(), 0,KEY_READ, &hKey) != ERROR_SUCCESS)
		return false;

	//if (RegQueryValueEx(mHKEY, mKeyPath.c_str(), 0,&dwType, (LPBYTE)&d, &dwSize) != ERROR_SUCCESS)
	if (RegQueryValueExA(hKey, configItemName, 0,&dwType, (LPBYTE)&d, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return false;
	}
	outLongValue = d;
	RegCloseKey(hKey);	
	return true;
}

bool ConfigManagerWin32::getConfigBool(const char* configItemName, bool& outBooleanValue)
{
	CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("ConfigManager getConfigBool [%s]", configItemName); )
	DWORD dwType = REG_BINARY;
	bool b;
	DWORD dwSize = sizeof(b);
	HKEY hKey;
	//std::string tempString = mKeyPath + '\' + configItemName;
	
	if (RegOpenKeyExA(mHKEY, mKeyPath.c_str(), 0,KEY_READ, &hKey) != ERROR_SUCCESS) 
		return false;

	if (RegQueryValueExA(hKey, configItemName, NULL,&dwType, (LPBYTE)&b, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);	
		return false;
	}    
	outBooleanValue = b;
	RegCloseKey(hKey);	
	return true;
}

// setters
// change a given item by case insensitive name in the config, save the changes immediately
void ConfigManagerWin32::setConfigString(const char* configItemName, std::string outString)
{
	CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("ConfigManager setConfigString [%s]", configItemName); )
	if(NULL == configItemName)
		return;
	HKEY hKey;

	if (RegOpenKeyExA(mHKEY, mKeyPath.c_str(), 0,KEY_WRITE, &hKey) != ERROR_SUCCESS)
		return;
	RegSetValueExA(hKey, configItemName, 0,REG_SZ, (LPBYTE)outString.c_str(), outString.length());
	// if not able to set value, do nothing
	RegCloseKey(hKey);	
}

void ConfigManagerWin32::setConfigLong(const char* configItemName, long outLongValue)
{
	CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("ConfigManager setConfigLong [%s]", configItemName); )
	if(NULL == configItemName)
		return;
	HKEY hKey;

	if (RegOpenKeyExA(mHKEY, mKeyPath.c_str(), 0,KEY_WRITE, &hKey) != ERROR_SUCCESS) 
		return;
	RegSetValueExA(hKey, configItemName, 0,REG_BINARY, (LPBYTE)&outLongValue, sizeof(outLongValue));
	// if not able to set value, do nothing
	RegCloseKey(hKey);
}

void ConfigManagerWin32::setConfigFloat(const char* configItemName, float outLongValue)
{
	CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("ConfigManager setConfigFloat [%s]", configItemName); )
	if(NULL == configItemName)
		return;
	HKEY hKey;

	if (RegOpenKeyExA(mHKEY, mKeyPath.c_str(), 0,KEY_WRITE, &hKey) != ERROR_SUCCESS) 
		return;
	RegSetValueExA(hKey, configItemName, 0,REG_BINARY, (LPBYTE)&outLongValue, sizeof(outLongValue));
	// if not able to set value, do nothing
	RegCloseKey(hKey);
}

void ConfigManagerWin32::setConfigBool(const char* configItemName, bool outBooleanValue)
{
	CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("ConfigManager setConfigBool [%s]", configItemName); )
	if(NULL == configItemName)
		return;
	HKEY hKey;

	if (RegOpenKeyExA(mHKEY, mKeyPath.c_str(), 0,KEY_WRITE, &hKey) != ERROR_SUCCESS) 
		return;
	
	RegSetValueExA(hKey, configItemName, 0,	REG_BINARY, (LPBYTE)&outBooleanValue, sizeof(outBooleanValue));
	// if not able to set value, do nothing
	RegCloseKey(hKey);	
}

int ConfigManagerWin32::GetDataSize(const char* strValueName)
{
	CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("ConfigManager GetDataSize [%s]", strValueName); )
	/* Call GetDataSize to determine the size, in bytes, of 
	a data value associated with the current key. ValueName 
	is a string containing the name of the data value to query.
	On success, GetDataSize returns the size of the data value. 
	On failure, GetDataSize returns -1. */

	HKEY hKey;
	LONG lResult;
	//std::string tempString = mKeyPath + '\' + strValueName;

	//if (RegOpenKeyEx(mHKEY, mKeyPath.c_str(), 0,KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) 
	if (RegOpenKeyExA(mHKEY, mKeyPath.c_str(), 0,KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) 
		return -1;

	DWORD dwSize = 1;
	//lResult = RegQueryValueEx(hKey, tempString.c_str(),NULL, NULL, NULL, &dwSize);
	lResult = RegQueryValueExA(hKey, strValueName,NULL, NULL, NULL, &dwSize);
		RegCloseKey(hKey);

	if (lResult != ERROR_SUCCESS) 
		return -1;
	return (int)dwSize;
}

bool ConfigManagerWin32::createKey(const char* strKey)
{
	char buffer[1024];
	LoadStringA(mAppInstance,mResourceID,buffer,1024);
	std::string resString = buffer;
	if(resString.empty()) // If we can't load the key we can't create the key
	{
		resString = "SOFTWARE\\PDG_ENGINE\\";
		CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("couldn't load resource %d, using %s default value", mResourceID, resString.c_str()); )
	}
	if(resString.at(resString.length()-1) != '\\')
        resString += '\\';
	if(verifyKey(HKEY_LOCAL_MACHINE,resString.c_str()))
	{
		CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("Create in HKEY_LOCAL_MACHINE [%s]", resString.c_str()); )
		mHKEY = HKEY_LOCAL_MACHINE;
	} 
	else //if(verifyKey(HKEY_CURRENT_USER,resString.c_str()))
	{
		CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("Create in HKEY_CURRENT_USER [%s]", resString.c_str()); )
		mHKEY = HKEY_CURRENT_USER;
	}

	std::string Key = strKey;
	//if(Key.at(0) == '\\')
	//	Key = Key.substr(1,Key.length() - 1);		
	 Key = resString + Key;
	HKEY hKey;

	DWORD dwDisposition = 0;

	if (RegCreateKeyExA(mHKEY, Key.c_str(), 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,
			&dwDisposition)	!= ERROR_SUCCESS) {
			CONFIG_MANAGER_DEBUG_ONLY( OS::_DOUT("Create key failed [%s]", Key.c_str()); )
			return FALSE;
	}
	
	RegCloseKey(hKey);
	return TRUE;
}

ConfigManager* ConfigManager::createSingletonInstance()
{
	return new ConfigManagerWin32();
}

}   // close namespace pdg

