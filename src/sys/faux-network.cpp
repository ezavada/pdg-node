// -----------------------------------------------
// faux-network.cpp
// 
// fake network functionality
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

#include "pdg/sys/network.h"
#include "pdg/sys/events.h"
#include "pdg/sys/core.h"
#include "pdg/sys/refcounted.h"
#include "pdg/sys/os.h"
#include "pdg/sys/singleton.h"

namespace pdg {
    
typedef pdg::Singleton<pdg::EventManager> ServerEventManager;

// the idea with these faux timers is that they will set a timer and then
// when the timer fires will generate the event that would be expected in
// reply from a real network layer. The ServerEventManager is used to
// provide an additional event path for the server

// so an application that wanted to simulate having a server should reference
// ServerEventManager and register it's server event handlers with it.

#define TIMER_NET_CONNECT 10000
#define TIMER_NET_DATA    10001
#define TIMER_NET_CLOSE   10002

#define SERVER_ID_BASE 10000

static int connectID = 1;

static int lowestServerPacketNum = 0;
static int lowestClientPacketNum = 0;
static int serverPacketNum = 0;
static int clientPacketNum = 0;


class FauxConnect : public pdg::RefCountedImpl< pdg::IEventHandler > {
public:
    bool handleEvent(EventEmitter*, long, void* inEventData) throw() {
        pdg::TimerInfo* ti = static_cast<pdg::TimerInfo*>(inEventData);
        if (ti->id == TIMER_NET_CONNECT) {
            pdg::NetConnect nc;
 
            // send the server a connect event
            nc.id = connectID++;
            nc.name = "faux.client.local";
            ServerEventManager::instance().postEvent(pdg::eventType_NetConnect, &nc);

            // send the client a connection event
            nc.id = connectID + SERVER_ID_BASE;
            nc.name = destName.c_str();
            ServerEventManager::instance().postEvent(eventType_NetConnect, &nc);
            
            EventManager::getSingletonInstance()->removeHandler(this, eventType_Timer);
            return true;
        }
        return false;
    }
    FauxConnect(const char* destination) {destName = destination;}
    std::string destName;
};

class FauxDisconnect : public pdg::RefCountedImpl< pdg::IEventHandler > {
public:
    bool handleEvent(EventEmitter*, long, void* inEventData) throw() {
        pdg::TimerInfo* ti = static_cast<pdg::TimerInfo*>(inEventData);
        if (ti->id == TIMER_NET_CLOSE) {
            pdg::NetDisconnect ndc;
 
            // send the server a disconnect event
            ndc.id = mid;
            ServerEventManager::instance().postEvent(pdg::eventType_NetDisconnect, &ndc);

            // send the client a disconnect event
            EventManager::getSingletonInstance()->postEvent(pdg::eventType_NetDisconnect, &ndc);

            EventManager::getSingletonInstance()->removeHandler(this, pdg::eventType_Timer);
            return true;
        }
        return false;
    }
    FauxDisconnect(long id, long error) {mid = id; merror = error;}
    long mid;
    long merror;
};

class FauxData : public pdg::RefCountedImpl< pdg::IEventHandler > {
public:
    bool handleEvent(EventEmitter*, long, void* inEventData) throw() {
        pdg::TimerInfo* ti = static_cast<pdg::TimerInfo*>(inEventData);
        if (ti->id == TIMER_NET_DATA) {
            pdg::NetData nd;
 
            nd.dataLen = mlen;
            nd.dataPtr = mdata;
            if (mid > SERVER_ID_BASE) {
                if (packetNum > lowestServerPacketNum) {    // skip this if it isn't in order
                    return false;
                }
                lowestServerPacketNum++;    // we can now sent the next packet in order
                nd.id = mid - SERVER_ID_BASE;  // this is a server connection, data goes to client
                EventManager::getSingletonInstance()->postEvent(pdg::eventType_NetData, &nd);
            } else {
                if (packetNum > lowestClientPacketNum) {    // skip this if it isn't in order
                    return false;
                }
                lowestClientPacketNum++;    // we can now sent the next packet in order
                nd.id = mid;    // client connection, data goes to server
                ServerEventManager::instance().postEvent(pdg::eventType_NetData, &nd);
            }
            delete[] mdata;
            mdata = 0;

            EventManager::getSingletonInstance()->removeHandler(this, pdg::eventType_Timer);
            return true;
        }
        return false;
    }
    FauxData(long id, char* data, long len) {
        mid = id; 
        mdata = new char[len]; 
        std::memcpy(mdata, data, len); 
        if (id > SERVER_ID_BASE) {
            packetNum = serverPacketNum++;
        } else {
            packetNum = clientPacketNum++;
        }
    }
    long packetNum;
    long mid;
    char* mdata;
    long mlen;
};

// lifecycle

NetworkManager::~NetworkManager() {
}


// start opening a connection to a remote endpoint.
void 
NetworkManager::openConnection(const char* destination, void* userContext, unsigned char connectFlags) {
    // start a timer to generate a faux connect event
    FauxConnect* faux = new FauxConnect(destination);
    EventManager::getSingletonInstance()->addHandler(faux, eventType_Timer);
    TimerManager::getSingletonInstance()->startTimer(TIMER_NET_CONNECT, (OS::rand() % 1000) + 500);   // delay 500 to 1500 ms for connect
}

// close a connection, with an optional error code, clean close if error = 0
void
NetworkManager::closeConnection(long id, int error) {
    // start a timer to generate a faux disconnect event
    if (id > SERVER_ID_BASE) {  // if this is the server asking us to disconnect, adjust to get the id the client will use
        id = id - SERVER_ID_BASE;
    }
    FauxDisconnect* faux = new FauxDisconnect(id, error);
    EventManager::getSingletonInstance()->addHandler(faux, eventType_Timer);
    TimerManager::getSingletonInstance()->startTimer(TIMER_NET_CONNECT, (OS::rand() % 500) + 100);   // delay 100 to 600 ms for disconnect
}

// send data on a particular established connection
void
NetworkManager::sendData(long id, void* data, long dataLen) {
    FauxData* faux = new FauxData(id, (char*)data, dataLen);
    EventManager::getSingletonInstance()->addHandler(faux, eventType_Timer);
    TimerManager::getSingletonInstance()->startTimer(TIMER_NET_DATA, (OS::rand() % 300) + 20);   // delay 20 to 320 ms for data
}

// open a listening connection on a particular port that 
// waits for incoming connections
void
NetworkManager::createServerPort(short portNum, void* userContext, unsigned char connectFlags) {
    // do nothing, should generate NetError if something fails
}

// close a particular listening port
void
NetworkManager::closeServerPort(short portNum) {
    // do nothing, should generate NetDisconnect event
}

}

