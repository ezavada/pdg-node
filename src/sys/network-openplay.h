// -----------------------------------------------
// network-openplay.h
// 
// network functionality implemented using OpenPlay
//
// everything here is internal to the System Framework 
// layer. If a particular build of the System Framework
// needs to, it can include this file and then
// create an OpenPlayNetworkManager as the network
// manager created in the startup event, or give this
// as a NetworkManager to the Application layer through
// some other means. It shouldn't be necessary to
// give the application layer direct access to these
// implementation classes
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


#ifndef PDG_NETWORK_OPENPLAY_H_INCLUDED
#define PDG_NETWORK_OPENPLAY_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/network.h"
#include "pdg/sys/events.h"
#include "pdg/sys/core.h"
#include "pdg/sys/refcounted.h"
#include "pdg/sys/os.h"
#include "pdg/sys/platform.h"
#include "pdg/sys/global_types.h"
#include "pdg/sys/semaphore.h"
#include "pdg/sys/mutex.h"

#include <map>
#include <queue>
#include <string>


#define PDG_MAX_NET_CONNECTION_NAME_LEN    256
#define PDG_DEFAULT_PORT_NUM               4429
#define PDG_OPEN_PLAY_GAME_TYPE            0x50444746 // 'PDGF'

// =======================================================================================================
// OPEN PLAY DEFINITIONS
// =======================================================================================================

// all this stuff taken from OpenPlay.h

#if PLATFORM_MACOS
	#define OP_DEFINE_API_C(_type) extern "C" _type
#elif PLATFORM_WIN32
	#define OP_DEFINE_API_C(_type) extern "C" _type __cdecl
#else
	#define OP_DEFINE_API_C(_type) extern "C" _type
#endif
    
typedef struct Endpoint *PEndpointRef;
typedef struct ProtocolConfig *PConfigRef;

typedef unsigned char	NMBoolean;
typedef signed short	NMSInt16;
typedef unsigned long	NMUInt32;
typedef signed long		NMSInt32;
typedef NMSInt32		NMType;
typedef NMSInt32    	NMErr;
typedef NMUInt32		NMFlags;

typedef enum {
	kNMConnectRequest	= 1,
	kNMDatagramData		= 2,
	kNMStreamData		= 3,
	kNMFlowClear		= 4,
	kNMAcceptComplete	= 5,
	kNMHandoffComplete	= 6,
	kNMEndpointDied		= 7,
	kNMCloseComplete	= 8
} NMCallbackCode;

typedef enum {
	kNMModeNone = 0,
	kNMDatagramMode = 1,
	kNMStreamMode = 2,
	kNMNormalMode = kNMStreamMode + kNMDatagramMode
} NMEndpointMode;

enum { 
	kNMNoError					= 0,
	kNMNoDataErr				= -4987,
	kNMFlowErr					= -4974,
	kNMProtocolErr              = -4793
};
  
typedef enum {
	kOpenNone	= 0x00,
	kOpenActive	= 0x01 /* open it active (dial for a connection, etc.) */
} NMOpenFlags;

/* Module types for OpenPlay Modules */
enum {
//	kATModuleType = 'Atlk',
	kIPModuleType = 1231971700   // 'Inet'
};

typedef void (*PEndpointCallbackFunction)(PEndpointRef inEndpoint, void *inContext,NMCallbackCode inCode, NMErr inError, void *inCookie);

OP_DEFINE_API_C(NMErr) ProtocolCreateConfig(NMType type, NMType game_id, const char * game_name, const void * enum_data, 
                                            NMUInt32 enum_data_len, char * configuration_string, PConfigRef * inConfig);											
OP_DEFINE_API_C(NMErr) ProtocolDisposeConfig(PConfigRef config);
OP_DEFINE_API_C(NMErr) ProtocolGetConfigString(PConfigRef config, char *config_string, NMSInt16 max_length);									
OP_DEFINE_API_C(void) ProtocolStartAdvertising(PEndpointRef endpoint);
OP_DEFINE_API_C(void) ProtocolStopAdvertising(PEndpointRef endpoint);
OP_DEFINE_API_C(NMErr) ProtocolOpenEndpoint(PConfigRef inConfig, PEndpointCallbackFunction inCallback, void *inContext, 
                                            PEndpointRef *outEndpoint, NMOpenFlags flags);
OP_DEFINE_API_C(NMErr) ProtocolCloseEndpoint(PEndpointRef endpoint, NMBoolean inOrderly);
OP_DEFINE_API_C(NMErr) ProtocolSetEndpointContext(PEndpointRef endpoint, void *newContext);
OP_DEFINE_API_C(NMSInt32) ProtocolSend(PEndpointRef endpoint, void *inData, NMUInt32 inSize, NMFlags inFlags);
OP_DEFINE_API_C(NMErr) ProtocolReceive(PEndpointRef endpoint, void *outData, NMUInt32 *ioSize, NMFlags *outFlags);
OP_DEFINE_API_C(NMErr) ProtocolAcceptConnection(PEndpointRef endpoint, void *inCookie, PEndpointCallbackFunction inNewCallback, void *inNewContext);
OP_DEFINE_API_C(NMErr) ProtocolGetEndpointIdentifier(PEndpointRef endpoint, char *identifier_string, NMSInt16 max_length);
OP_DEFINE_API_C(NMErr) ProtocolIdle(PEndpointRef endpoint);


namespace pdg {


// =======================================================================================================
// PACKET
// =======================================================================================================

struct NetPacket {
    uint32  packetLen;
    uint32  packetNum;
};
#define PACKET_DATA_OFFSET  sizeof(NetPacket)
#define NUMBER_ALL_PACKETS
#define DEFAULT_MAX_INCOMING_PACKET_SIZE 1024


// =======================================================================================================
// PACKET STATS
// =======================================================================================================

struct PacketStats {
    uint32 packetsCreated;
    uint32 packetsFreed;
    uint32 packetsSent;
    uint32 packetsReceived;
    uint32 bytesAllocated;
    uint32 bytesFreed;
    uint32 bytesSent;
    uint32 bytesReceived;
    uint32 invalidPacketsReceived;
    Mutex  mPacketStatsMutex;
    void reset() {  packetsCreated = 0; packetsFreed = 0; packetsSent = 0; packetsReceived = 0; 
                    bytesAllocated = 0; bytesFreed = 0; bytesSent = 0; bytesReceived = 0; 
                    invalidPacketsReceived = 0; }
    void recordPacketSent(NetPacket* inPacket)     { if (!inPacket) return; 
                                                  AutoMutex mutex(&mPacketStatsMutex); 
                                                  ++packetsSent; bytesSent += inPacket->packetLen; }
    void recordPacketReceived(NetPacket* inPacket) { if (!inPacket) return; 
                                                  AutoMutex mutex(&mPacketStatsMutex); 
                                                  ++packetsReceived; bytesReceived += inPacket->packetLen; }
    void recordPacketCreated(NetPacket* inPacket)  { if (!inPacket) return; 
                                                  AutoMutex mutex(&mPacketStatsMutex); 
                                                  ++packetsCreated; bytesAllocated += inPacket->packetLen; }
    void recordPacketFreed(NetPacket* inPacket)    { if (!inPacket) return; 
                                                  AutoMutex mutex(&mPacketStatsMutex); 
                                                  ++packetsFreed; bytesFreed += inPacket->packetLen; }
    void debugDumpStats() {
      #ifdef DEBUG
        AutoMutex mutex(&mPacketStatsMutex); 
        OS::_DOUT("\nPacket Statistics Report");
        OS::_DOUT("=========================================================");
        OS::_DOUT("Packets: %d sent, %d received, %d created, %d freed", packetsSent, packetsReceived, packetsCreated, packetsFreed);
        OS::_DOUT("Invalid Packets: [%d] (%d%%) received", invalidPacketsReceived, (packetsReceived) ? (invalidPacketsReceived*100)/packetsReceived : 0); 
        if (packetsCreated != packetsFreed) {
            OS::_DOUT("LEAKED %d Packet(s)!", packetsCreated - packetsFreed); 
        }
        OS::_DOUT("Byte Totals: %d sent, %d received, %d allocated, %d freed", bytesSent, bytesReceived, bytesAllocated, bytesFreed);
        if (bytesAllocated != bytesFreed) {
            OS::_DOUT("LEAKED %d bytes!", bytesAllocated - bytesFreed);
        }
        OS::_DOUT("=========================================================");
      #endif
    }
    PacketStats() { reset(); }
};


// =======================================================================================================
// NET ENDPOINT
// =======================================================================================================

class OpenPlayNetworkManager;
class NetEndpoint;

struct NetEndpointContext {
    uint32          magic;  // the magic number PDG_OPEN_PLAY_GAME_TYPE, that identifies this as a valid context
    void*           userContext;
    NetEndpoint*    endpoint;
};

typedef std::map<void*, NetPacket*> PacketDataMap;

class NetEndpoint {
public:
			    NetEndpoint(OpenPlayNetworkManager* mgr, void* userContext, uint8 connectFlags);   // used for active connections, along with connected() call
			    NetEndpoint(OpenPlayNetworkManager* mgr, PEndpointRef ref, void* userContext, uint8 connectFlags);  // used for passive connections
	virtual		~NetEndpoint();	// stub destructor

    void            receive();
    void            sendPacket(NetPacket* inPacket);
    bool            waitingToSend();
    void            sendWaitingPackets();
    void            setMaxIncomingSize(uint32 inMaxLen);
    NetEndpointContext* getContext() { return &mContext; }
    long            getId() { return mId; }
    NMErr           getLastError() { return mLastError; }
    void            netManagerDied() { mNetMgr = 0; }
    PacketStats&    getStatistics();
    const char*     getName() {return mName;}
    PEndpointRef    getOpenPlayEndpoint() {return mEndpoint;}
// packets created with createPacket must be disposed of with releasePacket
    NetPacket*      createPacket(uint32 inLen);  
    NetPacket*      clonePacket(const NetPacket* inPacket);
    void            releasePacket(NetPacket* inPacket);
    void            releasePacketFromDataPtr(void* dataPtr);
    void            handleAcceptComplete();
    void            handleEndpointDied(long err);
    void            handleCloseComplete();
    bool            receivePending() { return mReceivePending; }
    void            connected(PEndpointRef ref);    // called for an active connection when the connection succeeds
    static void     EndpointCallback(PEndpointRef inEndpoint, void* inContext, 
                                         NMCallbackCode inCode, NMErr inError, 
                                         void* inCookie);
protected: 
    void            initialize(void* userContext);
    void            byteSwapIncomingPacket(NetPacket *ioPacket);
    void            byteSwapOutgoingPacket(NetPacket *ioPacket);

    OpenPlayNetworkManager* mNetMgr; 
    char                mName[PDG_MAX_NET_CONNECTION_NAME_LEN];   
	PEndpointRef        mEndpoint;
    uint32              mMaxIncomingPacketSize;
	std::queue<NetPacket*> mOutgoingPackets;
    NetPacket*          mInProgressPacket;
    uint32              mBytesRemainingForPacket;
    uint32              mOffsetInPacket;
    uint32              mBytesRemainingForHeader;
    uint32              mOffsetInHeader;
    NetPacket           mPacketInfo;
    Mutex               mOutgoingQueueMutex;
    Mutex               mPacketMemMutex;
    Mutex               mReceiveMutex;
    bool                mListener;
    NetEndpointContext  mContext;
    uint8               mConnectFlags;
    long                mId;
    NMErr               mLastError;
    bool                mReceivePending;
    // statistics for this endpoint
    PacketStats     mPacketStats;
    Mutex           mDataPtrMapMutex;
    PacketDataMap   mDataPtrMap;
};

typedef std::map<PEndpointRef, NetEndpoint*>    EndpointRefMap;
typedef std::map<long, NetEndpoint*>            EndpointIdMap;
typedef std::map<short, PEndpointRef>           ListenerPortMap;

struct NetManagerData {
    EndpointRefMap  endpointRefs;   // a map that looks up NetEndpoints by OpenPlay Endpoint Ref
    EndpointIdMap   endpointIds;    // a map that looks up NetEndpoints by Network Manager Endpoint ID
    ListenerPortMap listenerPorts;  // a map that looks up Open Play Listener Endpoint Refs by port number
    Mutex           dataMutex;      // lock for all these fields
    PacketStats     packetStats;
};

// associates a user for a lister port context with a net manager and port
struct ListenerEndpointContext {
    uint32                  magic;  // the magic number PDG_OPEN_PLAY_GAME_TYPE, that identifies this as a valid context
    void*                   userContext;
    OpenPlayNetworkManager* manager;
    short                   port;
    uint8                   connectFlags;
};


// =======================================================================================================
// OPENPLAY NET MANAGER
// =======================================================================================================

class OpenPlayNetworkManager : public NetworkManager {
public:
    OpenPlayNetworkManager(EventManager* eventMgr);
    virtual ~OpenPlayNetworkManager();
    virtual void    idle();
    NetManagerData& getNetData() { return mNetData; }
    EventManager*   getEventManager() { return mEventMgr; }
    virtual void    openConnection(const char* destination, void* userContext = 0, uint8 connectFlags = 0);
    virtual void    closeConnection(long id, int error = 0);
    virtual void    sendData(long id, void* data, long dataLen);
    virtual void    releaseNetData(NetData* data);
    virtual void    setConnectionContext(long id, void* userContext);
    virtual void    setMaxIncomingPacketSize(long id, long maxLen);
    virtual void    createServerPort(short portNum, void* userContext = 0, uint8 connectFlags = flag_PublicPort);
    virtual void    closeServerPort(short portNum);
	virtual void	broadcastData(void* inEventData, long dataLen);
    virtual void    setPublicInfo(long connectionCreator, const char* publicName);
	virtual void	addIncomingPacketGatekeeper(GatekeeperFunc func, void* context) {};
	virtual void	removeIncomingPacketGatekeeper(GatekeeperFunc func) {};
	virtual void    waitingForNetwork();
	virtual void    doneWaitingForNetwork();
	virtual bool    isWaitingForNetwork();
	static OpenPlayNetworkManager* createOpenPlayNetworkManager(EventManager* eventMgr) { return new OpenPlayNetworkManager(eventMgr); }
	
	bool    hasActiveConnections();
	void    doOrderlyShutdown();
protected:
    OpenPlayNetworkManager() {};    // disallow default construction
    NetManagerData  mNetData;
    EventManager*   mEventMgr;
    long            mConnectionCreator;
    std::string     mPublicName;
    bool            mNetworkWait;
    unsigned long   mSetWaitCursorMs;   // when to set the network wait cursor
};

} // end namespace pdg

#endif // PDG_NETWORK_OPENPLAY_H_INCLUDED


