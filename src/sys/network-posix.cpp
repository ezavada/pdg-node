// -----------------------------------------------
// network-openplay.cpp
// 
// network functionality implemented using OpenPlay
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

#ifndef PDG_NO_NETWORK

#include "pdg/msvcfix.h"  // fix non-standard MSVC

#include "network-posix.h"

// include the cursor management stuff if we
// are doing a GUI version
#ifndef PDG_NO_GUI
    #include "internals.h"
#endif

// make sure we are using the correct networking API
#if defined( PLATFORM_WIN32 ) && !defined( OP_API_NETWORK_SOCKETS)
	#define OP_API_NETWORK_WINSOCK
#else
	#define OP_API_NETWORK_SOCKETS
#endif

// define the following in your build environment, or uncomment it here to get
// debug output for the network layer
//#define PDG_DEBUG_NETWORKING
// define the following in your build environment, or uncomment it here to get
// full packet dump of everything send and received by the the network layer
//#define PDG_NET_DEBUG_PACKET_DUMP

#ifndef PDG_DEBUG_NETWORKING
  // if the network layer is not being debugged, we ignore all 
  #define NET_DEBUG_ONLY(_expression)
#else
  #define NET_DEBUG_ONLY DEBUG_ONLY
#endif

#define ASYNC_DEBUG_OUT(n, b)   NET_DEBUG_ONLY( OS::_DOUT(n) )
#define ASYNC_ASSERT(x)         DEBUG_ONLY( if (!(x)) { OS::_DOUT("Assertion failed: "#x); OS::exit(-1); } )


namespace pdg {


extern Semaphore gWakeupSemaphore;  // general semaphore used to wake up the framework if something
                                    // interesting happens, such as a network event
                                    // defined in main-{platform}.cpp
                                    
static long sLastEndpointID = 0;    // this is saved as a global across multiple instances

static void     ListenerCallback(PEndpointRef inEndpoint, void* inContext, 
                                     NMCallbackCode inCode, NMErr inError, 
                                     void* inCookie);  

#define PDG_DEFAULT_GAME_NAME "PDGFTCP"

// =======================================================================================================
// NET ENDPOINT IMPLEMENTATION
// =======================================================================================================


NetEndpoint::NetEndpoint(OpenPlayNetworkManager* mgr, PEndpointRef ref, void* userContext, uint8 connectFlags) 
 : mNetMgr(mgr), mEndpoint(ref), mMaxIncomingPacketSize(DEFAULT_MAX_INCOMING_PACKET_SIZE),
   mInProgressPacket(0), mOffsetInHeader(0), mListener(true), mConnectFlags(connectFlags), 
   mLastError(0), mReceivePending(false) {
    initialize(userContext);
    // setup the new context
    NMErr err = ProtocolSetEndpointContext(mEndpoint, &mContext);
    DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolSetEndpointContext returned %d", err); } )
    // get the name of the remote host
    mName[0] = 0;
    err = ProtocolGetEndpointIdentifier(mEndpoint, mName, PDG_MAX_NET_CONNECTION_NAME_LEN);
    DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolGetEndpointIdentifier returned %d", err); } )
    mName[PDG_MAX_NET_CONNECTION_NAME_LEN-1] = 0;  // make sure this is nul terminated
     // add this endpoint to the maps
    NetManagerData* netDataP = &mNetMgr->getNetData();
    AutoMutex mutex(&(netDataP->dataMutex));
    netDataP->endpointRefs.insert(EndpointRefMap::value_type(mEndpoint, this));
}

NetEndpoint::NetEndpoint(OpenPlayNetworkManager* mgr, void* userContext, uint8 connectFlags) 
 : mNetMgr(mgr), mEndpoint(0), mMaxIncomingPacketSize(DEFAULT_MAX_INCOMING_PACKET_SIZE),
   mInProgressPacket(0), mOffsetInHeader(0), mListener(false), mConnectFlags(connectFlags),
   mLastError(0), mReceivePending(false) {
   initialize(userContext);
}

// base intialization done for both active and passive endpoints
void
NetEndpoint::initialize(void* userContext) {
    // setup the new context
    mContext.magic = PDG_OPEN_PLAY_GAME_TYPE; // a magic number to tag this as a valid context
    mContext.userContext = userContext;
    mContext.endpoint = this;
    // clear the name of the remote host
    mName[0] = 0;
     // add this endpoint to the maps
    NetManagerData* netDataP = &mNetMgr->getNetData();
    AutoMutex mutex(&(netDataP->dataMutex));
    mId = ++sLastEndpointID;
    netDataP->endpointIds.insert(EndpointIdMap::value_type(mId, this));
}

NetEndpoint::~NetEndpoint() {
    // remove this endpoint from the maps
    if (mNetMgr) {
        NetManagerData* netDataP = &mNetMgr->getNetData();
        AutoMutex mutex(&(netDataP->dataMutex));
        netDataP->endpointRefs.erase(EndpointRefMap::key_type(mEndpoint));
        netDataP->endpointIds.erase(EndpointIdMap::key_type(mId));
    }
}

void
NetEndpoint::receive() {
    AutoMutex mutex(&mReceiveMutex);    // don't allow two threads to receive on same endpoint at same time
    if (mEndpoint == NULL) {
        mReceivePending = true;
        NET_DEBUG_ONLY( OS::_DOUT("NetEndpoint receive delayed, not connected"); )
        return;
    }
    mReceivePending = false;
	NMErr err = kNMNoError;
	uint32 dataLen;
    void* dataPtr;
	NMFlags flags = 0; // non-blocking
	
    // repeat until we get an error code (no data left is returned as an error)
	while (err == kNMNoError) {
        // --------------------------------------------
        // Header reassembly and deframing section
        // --------------------------------------------
        if (!mInProgressPacket) {  // there is no partially received packet, so we are starting at the header
            if (mOffsetInHeader > 0) {
                // there is a partially received header
                dataPtr = (void*)( (char*)&mPacketInfo + mOffsetInHeader );
                NET_DEBUG_ONLY( OS::_DOUT("Continuing receipt of partial packet header, dataPtr = %p, offsetInHeader = %d, "
                                        "byteRemainingForHeader = %d", dataPtr, mOffsetInHeader, mBytesRemainingForHeader); )
            } else {
                // no partially received header, fetch the whole thing
                mBytesRemainingForHeader = sizeof(mPacketInfo);
        	    dataPtr = &mPacketInfo;
        	}
            dataLen = mBytesRemainingForHeader;
           	err = ProtocolReceive(mEndpoint, dataPtr, &dataLen, &flags);
            NET_DEBUG_ONLY( OS::_DOUT("OpenPlay ProtocolReceive of [%d] bytes returned [%d]: mPacketInfo.packetLen [%d] "
                                "dataLen [%d] flags [%d]", mBytesRemainingForHeader, err, mPacketInfo.packetLen, 
                                dataLen, flags); )
           	if (err != kNMNoDataErr) {  // don't report when we got no data

                if (dataLen != mBytesRemainingForHeader) {
                    NET_DEBUG_ONLY( OS::_DOUT("NetEndpoint::receive only returned %d bytes, not the %d bytes "
                                            "required for the header", dataLen, mBytesRemainingForHeader); )
                    err = kNMNoDataErr;  // don't try to get more data for this packet now
                }
                mBytesRemainingForHeader -= dataLen;
                mOffsetInHeader += dataLen;
                ASYNC_ASSERT(mBytesRemainingForHeader >= 0);  // just making sure, Open Play shouldn't do this
            } else if (err != kNMNoDataErr) {  // don't report when we got no data
               NET_DEBUG_ONLY( OS::_DOUT("NetEndpoint::Receive ProtocolReceive for packet header returned %d", err); )
               return;
            }
            // Check for packet header complete
            if (mBytesRemainingForHeader == 0) {
                // we need to get the length byte swapped into native format to create the packet
                // but we don't want to byte swap the entire packet yet because we don't have all 
                // of it. PacketType is a one byte value and doesn't need to be swapped.
                uint32 packetLen = BigEndian32_ToNative(mPacketInfo.packetLen);
                int errCode = 0;
                if (packetLen <= (mMaxIncomingPacketSize+sizeof(NetPacket))) {
                    mInProgressPacket = createPacket(packetLen);
                    errCode = NetworkManager::error_OutOfMemory; // memory is the only reason a packet will fail to be allocated
                } else {
                    errCode = NetworkManager::error_OversizePacket; // packet was larger than our max size
                }
//        				ASYNC_DEBUG_OUT("Client CreatePacket returned "<<(void*)mInProgressPacket
//        				            << ": mInProgressPacket->packetType = "<<(int)mInProgressPacket->packetType
//        				            << ", mInProgressPacket->packetLen = "<<mInProgressPacket->packetLen, 
//                                    DEBUG_TRIVIA | DEBUG_PACKETS);
        	    if (!mInProgressPacket) {
        	        // bad packet, drop the connection
        	        handleEndpointDied(errCode);
        	        ProtocolCloseEndpoint(mEndpoint, false);  // hard disconnect
        	        DEBUG_ONLY( OS::_DOUT("NetEndpoint::Receive failed to allocate incoming packet data, len = %d", 
        	                            mPacketInfo.packetLen); )
        	        return;
        	    }
                // CreatePacket populated packetType, packetNum and packetLen based on what we
                // passed in. So replace them with the Big Endian values that came from the network
                // We will byte swap once we have the entire packet
              #ifdef NUMBER_ALL_PACKETS
                mInProgressPacket->packetNum = Native_ToBigEndian32(mPacketInfo.packetNum);
              #endif
                mInProgressPacket->packetLen = Native_ToBigEndian32(packetLen);
        	    mBytesRemainingForPacket = packetLen - sizeof(mPacketInfo);
        	    mOffsetInPacket = PDG_PACKET_DATA_OFFSET;
                mOffsetInHeader = 0;  // reset for header of next packet
           	}
        }
        // --------------------------------------------
        // Packet body reassembly section
        // --------------------------------------------
    	if (err == kNMNoError) {
//        				ASYNC_DEBUG_OUT("Client bytes remaining for packet = " << mBytesRemainingForPacket
//    				                << ", mInProgressPacket->packetLen = "<<mInProgressPacket->packetLen,
//                                    DEBUG_TRIVIA | DEBUG_PACKETS);
    	    if (mBytesRemainingForPacket > 0) {  // handle packets that are just the header
    		    dataLen = mBytesRemainingForPacket;
    	        dataPtr = (void*)( (char*)mInProgressPacket + mOffsetInPacket );
//            				    ASYNC_DEBUG_OUT("Client datPtr = " << dataPtr
//        				                << ": (void*)( (char*)mInProgressPacket + mOffsetInPacket ) ["
//        				                <<(void*)mInProgressPacket << "] + ["
//        				                <<mOffsetInPacket<<"]", DEBUG_TRIVIA | DEBUG_PACKETS);
    		    err = ProtocolReceive(mEndpoint, dataPtr, &dataLen, &flags);
                NET_DEBUG_ONLY( OS::_DOUT("OpenPlay ProtocolReceive of [%d] bytes returned [%d]: dataLen [%d] flags [%d]", 
                                mBytesRemainingForPacket, err, dataLen, flags); )
//            				ASYNC_DEBUG_OUT("Client ProtocolReceive returned "<<err
//            				            << ", dataLen = "<<dataLen <<", flags = "<<(int)flags,
//            				            DEBUG_TRIVIA | DEBUG_PACKETS);
                if (err == kNMNoError) {
                    NET_DEBUG_ONLY( if (dataLen != mBytesRemainingForPacket) {
                        OS::_DOUT("NetEndpoint ProtocolReceive() only returned %d bytes, not the "
                                  "%d bytes required", dataLen, mBytesRemainingForPacket);
                    } )
                    mBytesRemainingForPacket -= dataLen;
                    mOffsetInPacket += dataLen;
                    ASYNC_ASSERT(mBytesRemainingForPacket >= 0);  // just making sure. OpenPlay should never do this
                } else if (err != kNMNoDataErr) {
           	        NET_DEBUG_ONLY( OS::_DOUT("NetEndpoint ProtocolReceive for packet body returned %d",err); )
                    return;
                }
     		}
     		if (mBytesRemainingForPacket == 0) {
     		    byteSwapIncomingPacket(mInProgressPacket);
     		    // we got all the data for the packet, and now its in native endian format
#ifdef NUMBER_ALL_PACKETS
//          NET_DEBUG_ONLY(     OS::_DOUT("NetEndpoint enqueuing "<<mInProgressPacket->GetName()
//                			     << " ["<<mInProgressPacket->packetNum<<"]" << ", length = "
//                			     << mInProgressPacket->packetLen, DEBUG_TRIVIA | DEBUG_PACKETS); )
#endif // NUMBER_ALL_PACKETS
                // queue packet until someone calls GetNextPacket()
//                if (mInProgressPacket->NeedsNulTermination()) {
//                    #warning TODO: add nul termination to packet
//                    // right now we only check to make sure that it is Nul terminated
//                    ASYNC_ASSERT(((char*)(mInProgressPacket))[mInProgressPacket->packetLen] == 0);
//                }
                if (mNetMgr) {
                    mNetMgr->getNetData().packetStats.recordPacketReceived(mInProgressPacket);   // globally for all endpoints
                    mPacketStats.recordPacketReceived(mInProgressPacket);   // locally for this endpoint
    //                bool packetIsGood = ValidatePacket(mInProgressPacket);
    //                if (packetIsGood) {
                    EventManager* evtMgr = mNetMgr->getEventManager();
                    NetData nd;
                    nd.id = mId;
                    nd.context = mContext.userContext;
                    nd.dataLen = mInProgressPacket->packetLen - PDG_PACKET_DATA_OFFSET;
                    nd.dataPtr = (char*)mInProgressPacket + PDG_PACKET_DATA_OFFSET;
                    nd.arrivalMs = OS::getMilliseconds();
                    {
                        AutoMutex mutex2(&mDataPtrMapMutex);
                        mDataPtrMap.insert(PacketDataMap::value_type((void*)nd.dataPtr, mInProgressPacket));
                    }
                    evtMgr->enqueueEvent(eventType_NetData, 
                            UserData::makeUserDataViaCopy(&nd, sizeof(NetData)), mNetMgr );
            	    mInProgressPacket = NULL;
                    gWakeupSemaphore.signal();  // wake up the main event loop, there's events to be had
                }
    	    }
    	}
	}
}


void
NetEndpoint::sendPacket(NetPacket* inPacket) {
    if (mEndpoint == 0) {
        ASYNC_DEBUG_OUT("Endpoint SendPacket failed, not connected", DEBUG_ERROR | DEBUG_PACKETS);
        return;
    }
    if (inPacket == 0) {
        ASYNC_DEBUG_OUT("Endpoint SendPacket failed, NULL packet", DEBUG_ERROR | DEBUG_PACKETS);
        return;
    }
/*    if (inPacket->packetLen >= MAX_PACKET_LEN) {
        ASYNC_DEBUG_OUT("Endpoint SendPacket "<<inPacket->GetName() << " failed, too large, size = " 
                    << inPacket->packetLen, DEBUG_ERROR | DEBUG_PACKETS);
        return;
    }
*/
    if (inPacket->packetLen < sizeof(NetPacket) ) {
        NET_DEBUG_ONLY( OS::_DOUT("Endpoint SendPacket failed, size not set, size = %d", inPacket->packetLen); )
    }
    // always try to send any waiting packets
    sendWaitingPackets();
    // if, after trying to send waiting packets, there are still some in the queue, then queue
    // the packet to be sent
    if (waitingToSend()) {
        ASYNC_DEBUG_OUT("Packets already waiting, queuing outgoing packet", DEBUG_TRIVIA | DEBUG_PACKETS);
        NetPacket* p = clonePacket(inPacket);  // packet will be freed by caller, so we must make a copy
        AutoMutex mutex(&mOutgoingQueueMutex);
        mOutgoingPackets.push(p);        
    } else {
        long packetLen = inPacket->packetLen;
        long sentBytes;
        if ((mConnectFlags & NetworkManager::flag_NoFraming) == 0) {
            // using packet framing
            byteSwapOutgoingPacket(inPacket);   // swap to network byte order
            mLastError = kNMProtocolErr; // in case next call fails
            sentBytes = ProtocolSend(mEndpoint, (void*)inPacket, packetLen, 0);
          #ifdef PDG_NET_DEBUG_PACKET_DUMP
            if (sentBytes > 0) {
                char buf[4096];
                OS::binaryDump(buf, 4096, (const char*)inPacket, sentBytes);
                NET_DEBUG_ONLY( OS::_DOUT("Sent on connection [%ld]:\n%s", mId, buf); )
            }
          #endif
            byteSwapIncomingPacket(inPacket);   // restore the data now that it's been sent
        } else {
            // no packet framing, just send the data
            void* ptr = (void*)((char*)inPacket+PDG_PACKET_DATA_OFFSET);
            sentBytes = ProtocolSend(mEndpoint, ptr, packetLen, 0);
          #ifdef PDG_NET_DEBUG_PACKET_DUMP
            if (sentBytes > 0) {
                char buf[4096];
                OS::binaryDump(buf, 4096, (const char*)ptr, sentBytes);
                NET_DEBUG_ONLY( OS::_DOUT("Sent on connection [%ld]:\n%s", mId, buf); )
            }
          #endif
            if (sentBytes != kNMFlowErr) {
                // we don't want to change the packet length if we are about to 
                // push the packet onto the queue for later transmission
                inPacket->packetLen -= PDG_PACKET_DATA_OFFSET;
            }
        }
        mLastError = 0;
        if (sentBytes < 0) {
            // an error was returned, no bytes sent
            NMErr err = sentBytes;
            if (err == kNMFlowErr) {
                ASYNC_DEBUG_OUT("Flow error, queuing outgoing packet ", DEBUG_TRIVIA | DEBUG_PACKETS);
                NetPacket *p = clonePacket(inPacket);  // packet will be freed by caller, so we must make a copy
                AutoMutex mutex(&mOutgoingQueueMutex);
                mOutgoingPackets.push(p);
            } else {
                mLastError = err;
                DEBUG_ONLY( OS::_DOUT("Endpoint SendPacket Failed! Error %d", err); )
            }
        } else if (sentBytes != (long)inPacket->packetLen) {
            // wrong number of bytes sent
            DEBUG_ONLY( OS::_DOUT("Endpoint SendPacket failed to send entire contents, "
                                  "size = %d, sent = %d", inPacket->packetLen, sentBytes); )
        } else {
            // packet was sent normally
            if (mNetMgr) {
                mNetMgr->getNetData().packetStats.recordPacketSent(inPacket);   // globally for all endpoints
            }
            mPacketStats.recordPacketSent(inPacket);   // locally for this endpoint
            NET_DEBUG_ONLY( OS::_DOUT("Sent Packet to OpenPlay endpoint [%p] connection [%d]", mEndpoint, mId); )
        }
    }
}


bool
NetEndpoint::waitingToSend() {
    AutoMutex mutex(&mOutgoingQueueMutex);
    return !mOutgoingPackets.empty();
}

void
NetEndpoint::sendWaitingPackets() {
    AutoMutex mutex(&mOutgoingQueueMutex);
    while (!mOutgoingPackets.empty()) {
        NetPacket* p = mOutgoingPackets.front();
        long packetLen = p->packetLen;
        long sentBytes;
        if ((mConnectFlags & NetworkManager::flag_NoFraming) == 0) {
            // using packet framing
            byteSwapOutgoingPacket(p);   // swap to network byte order
            mLastError = kNMProtocolErr; // in case send fails
            sentBytes = ProtocolSend(mEndpoint, (void*)p, packetLen, 0);
          #ifdef PDG_NET_DEBUG_PACKET_DUMP
            if (sentBytes > 0) {
                char buf[4096];
                OS::binaryDump(buf, 4096, (const char*)p, sentBytes);
                NET_DEBUG_ONLY( OS::_DOUT("Sent on connection [%ld]:\n%s", mId, buf); )
            }
          #endif
            byteSwapIncomingPacket(p);   // restore the data now that it's been sent
        } else {
            // no packet framing, just send the data
            void* ptr = (void*)((char*)p+PDG_PACKET_DATA_OFFSET);
            sentBytes = ProtocolSend(mEndpoint, ptr, packetLen, 0);
          #ifdef PDG_NET_DEBUG_PACKET_DUMP
            if (sentBytes > 0) {
                char buf[4096];
                OS::binaryDump(buf, 4096, (const char*)ptr, sentBytes);
                NET_DEBUG_ONLY( OS::_DOUT("Sent on connection [%ld]:\n%s", mId, buf); )
            }
          #endif
            p->packetLen -= PDG_PACKET_DATA_OFFSET;
        }
        mLastError = 0;
        if (sentBytes < 0) {
            NMErr err = sentBytes;
            if (err == kNMFlowErr) {
                ASYNC_DEBUG_OUT("Flow error, leaving packet in queue", DEBUG_TRIVIA | DEBUG_PACKETS);
                return;  // don't try to send any more
            } else {
                mLastError = err;
                DEBUG_ONLY( OS::_DOUT("Endpoint SendWaitingPackets Failed! Error %d", err); )
                return; // don't try to send any more
            }
        } else if (sentBytes != (long)p->packetLen) {
            // wrong number of bytes sent
            TODO("Need to handle partial send");
            DEBUG_ONLY( OS::_DOUT("Endpoint SendWaitingPacket failed to send entire contents, "
                                   "size = %d, sent = %d", p->packetLen, sentBytes); )
            return; // don't try to send any more
        } else {
            // packet was sent normally
            mOutgoingPackets.pop();
            if (mNetMgr) {
                mNetMgr->getNetData().packetStats.recordPacketSent(p);   // globally for all endpoints
            }
            mPacketStats.recordPacketSent(p);   // locally for this endpoint
            NET_DEBUG_ONLY( OS::_DOUT("Sent Waiting Packet to OpenPlay endpoint [%p] connection [%d]", mEndpoint, mId); )
            releasePacket(p);
        }
    }
}

void
NetEndpoint::setMaxIncomingSize(uint32 inMaxLen) {
    mMaxIncomingPacketSize = inMaxLen;
}

// packets created with CreatePacket are malloc'd, and must be freed with free()
NetPacket* 
NetEndpoint::createPacket(uint32 inLen) {  
    NetPacket* resultPacket = 0;
    if (inLen >= sizeof(NetPacket)) {
        AutoMutex mutex(&mPacketMemMutex);
        resultPacket = (NetPacket*)std::malloc(inLen);
    }
    if (resultPacket) {
        resultPacket->packetLen = inLen;
        if (mNetMgr) {
          #ifdef NUMBER_ALL_PACKETS
            resultPacket->packetNum = mNetMgr->getNetData().packetStats.packetsCreated;
          #endif
            mNetMgr->getNetData().packetStats.recordPacketCreated(resultPacket); // record globally for all endpoints
        }
        mPacketStats.recordPacketCreated(resultPacket); // record locally for this endpoint
//            DEBUG_OUT("Created Packet " << resultPacket->GetName() <<", size = "<<inLen, DEBUG_TRIVIA | DEBUG_PACKETS);
    } else {
        DEBUG_ONLY( OS::_DOUT("MemAlloc failed for createPacket, endpoint [%s], size = [%d]", getName(), inLen); )
    }
    return resultPacket;
}

void
NetEndpoint::releasePacket(NetPacket* inPacket) {
    if (!inPacket) {
        return;
    }
//    DEBUG_OUT("Freeing Packet " << inPacket->GetName() <<", size = "<<inPacket->packetLen, DEBUG_TRIVIA | DEBUG_PACKETS);
    if (mNetMgr) {
        mNetMgr->getNetData().packetStats.recordPacketFreed(inPacket); // record globally for all endpoints
        mPacketStats.recordPacketFreed(inPacket); // record locally for this endpoint
    }
    AutoMutex mutex(&mPacketMemMutex);
    std::free(inPacket);
}

void
NetEndpoint::releasePacketFromDataPtr(void* dataPtr) {
    // this checks to find the data pointer in the list of allocated packets,
    // and frees it if it finds it
    if (!dataPtr) {
        return;
    }
    AutoMutex mutex(&mDataPtrMapMutex);
    PacketDataMap::iterator it = mDataPtrMap.find(dataPtr);
    if (it != mDataPtrMap.end()) {
        NetPacket* packet = (*it).second;
        releasePacket(packet);
        mDataPtrMap.erase(it);
    } else {
        DEBUG_ONLY( OS::_DOUT("ERROR: Failed free of packet data ptr [%p] for packet received on connection "
                              "[%d] user context [%d]", dataPtr, mId, mContext.userContext); )
    }
}

NetPacket*
NetEndpoint::clonePacket(const NetPacket* inPacket) {
    NetPacket* resultPacket = createPacket(inPacket->packetLen);
    if (resultPacket) { // create could return NULL
        AutoMutex mutex(&mPacketMemMutex);
        std::memcpy((void*)resultPacket,(void*)inPacket, inPacket->packetLen);
    }
    return resultPacket; 
}

void
NetEndpoint::byteSwapIncomingPacket(NetPacket *ioPacket) {
  #ifdef NUMBER_ALL_PACKETS
    ioPacket->packetNum = BigEndian32_ToNative(ioPacket->packetNum);
  #endif
    ioPacket->packetLen = BigEndian32_ToNative(ioPacket->packetLen);
}

void
NetEndpoint::byteSwapOutgoingPacket(NetPacket *ioPacket) {
  #ifdef NUMBER_ALL_PACKETS
    ioPacket->packetNum = Native_ToBigEndian32(ioPacket->packetNum);
  #endif
    ioPacket->packetLen = Native_ToBigEndian32(ioPacket->packetLen);
}

void
NetEndpoint::handleAcceptComplete() {
    if (mNetMgr) {
        NetConnect netConnectEvent;
        netConnectEvent.id = mId;
        netConnectEvent.context = mContext.userContext;
        netConnectEvent.name = getName();
        netConnectEvent.incoming = true;
        EventManager* evtMgr = mNetMgr->getEventManager();
        if (evtMgr) {
            evtMgr->enqueueEvent(eventType_NetConnect, 
                    UserData::makeUserDataViaCopy(&netConnectEvent, sizeof(NetConnect)), mNetMgr ); // enqueue so main thread can pick it up
            gWakeupSemaphore.signal();  // wake up the main event loop
        }
    }
}

void
NetEndpoint::handleEndpointDied(long err) {
    if (mNetMgr) {
        NetError netErrEvent;
        netErrEvent.id = mId;
        netErrEvent.context = mContext.userContext;
        if (err) {
            netErrEvent.error = err;
        } else {
            netErrEvent.error = mLastError;
        }
        if (err != 0) { // don't do a net error message if there was no error
            EventManager* evtMgr = mNetMgr->getEventManager();
            if (evtMgr) {
                evtMgr->enqueueEvent(eventType_NetError, 
                        UserData::makeUserDataViaCopy(&netErrEvent, sizeof(NetError)), mNetMgr ); // enqueue so main thread can pick it up
                gWakeupSemaphore.signal();  // wake up the main event loop
            }
        }
    }
}

void
NetEndpoint::handleCloseComplete() {
	mContext.magic = 0;	// make sure this context is dead and the endpoint callback will ignore it
    if (mNetMgr) {
        NetDisconnect netDiscoEvent;
        netDiscoEvent.id = mId;
        netDiscoEvent.context = mContext.userContext;
        EventManager* evtMgr = mNetMgr->getEventManager();
        if (evtMgr) {
            evtMgr->enqueueEvent(eventType_NetDisconnect, 
                                 UserData::makeUserDataViaCopy(&netDiscoEvent, sizeof(NetDisconnect)), mNetMgr ); // enqueue so main thread can pick it up
            gWakeupSemaphore.signal();  // wake up the main event loop
        }
    }
}

void
NetEndpoint::connected(PEndpointRef ref) {
    // save the OpenPlay endpoint reference
    mEndpoint = ref;
    // get the name of the remote host
    mName[0] = 0;
    NET_DEBUG_ONLY(NMErr err =) ProtocolGetEndpointIdentifier(mEndpoint, mName, PDG_MAX_NET_CONNECTION_NAME_LEN);
    NET_DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolGetEndpointIdentifier returned %d", err); } )
    mName[PDG_MAX_NET_CONNECTION_NAME_LEN-1] = 0;  // make sure this is nul terminated
    // add to the endpoint ref map
    NetManagerData* netDataP = &mNetMgr->getNetData();
    AutoMutex mutex(&(netDataP->dataMutex));
    netDataP->endpointRefs.insert(EndpointRefMap::value_type(ref, this));
}

void 
NetEndpoint::EndpointCallback(PEndpointRef inEndpoint, void* inContext, NMCallbackCode inCode, NMErr inError, void* inCookie) {
    // Everything here happens in the Open Play worker thread, not in our application thread(s)

    NET_DEBUG_ONLY( OS::_DOUT("EndpointCallback endpoint [%p] context [%p] code [%d] err [%d] "
                          "cookie [%p]", inEndpoint, inContext, inCode, inError, inCookie); )
    if (inContext == 0) {
        NET_DEBUG_ONLY( OS::_DOUT("EndpointCallback ignoring callback with NULL context"); )
        return;
    }

    NetEndpointContext* context = static_cast<NetEndpointContext*>(inContext);
    NetEndpoint* ep = context->endpoint;
    TODO("Verify that endpoint matches what is in the map");
    
    // check that the magic number is correct, if not this could be a corrupted endpoint
    if (context->magic != PDG_OPEN_PLAY_GAME_TYPE) {
        DEBUG_ONLY( OS::_DOUT("EndpointCallback ignoring callback with bad magic number [%p]", context->magic); )
        return;
    }

    switch(inCode)
    {
      case kNMDatagramData:
        break;
      case kNMStreamData:
        {
            NET_DEBUG_ONLY( OS::_DOUT("EndpointCallback got data on endpoint id [%d] context [%p] "
                                  "cookie [%p]", context->endpoint->getId(), inContext, inCookie); )
            ep->receive();
        }
        break;
      case kNMFlowClear:
        {
            NET_DEBUG_ONLY( OS::_DOUT("EndpointCallback got FlowClear id [%d] context [%p] "
                                "cookie [%p]", context->endpoint->getId(), inContext, inCookie); )
            ep->sendWaitingPackets();
        }
        break;
      case kNMAcceptComplete:
        {   
            // when a listening port spawns a new connection, this callback
            // is received by the new connection when it is first created
            // generally, the context would be an Endpoint context, but since this endpoint is
            // newly created, we didn't have that context yet
            ListenerEndpointContext* oldContext = static_cast<ListenerEndpointContext*>(inContext);
            NET_DEBUG_ONLY( OS::_DOUT("EndpointCallback got AcceptComplete from port [%d] context [%p] "
                                    "cookie [%p]", oldContext->port, inContext, inCookie); )
            ep = new NetEndpoint(oldContext->manager, inEndpoint, oldContext->userContext, oldContext->connectFlags);
            // send the connection event
            ep->handleAcceptComplete();
        }
        break;
      case kNMEndpointDied:
        {
            // generate a network error event
            NET_DEBUG_ONLY( OS::_DOUT("EndpointCallback got EndpointDied id [%d] context [%p] "
                                    "cookie [%p]", context->endpoint->getId(), inContext, inCookie); )
            ProtocolCloseEndpoint(inEndpoint, false);
            ep->handleEndpointDied(inError); // endpoint will be deleted in kNMCloseComplete
        }
        break;
      case kNMCloseComplete:
        {
            // generate a network close event
            NET_DEBUG_ONLY( OS::_DOUT("EndpointCallback got CloseComplete id [%d] context [%d] "
                                    "cookie [%p]", context->endpoint->getId(), inContext, inCookie); )
            ep->handleCloseComplete();
            delete ep;    // delete the NetEndpoint object
            context = 0;  // the context is now completely dead, since it was held by the NetEndpoint we just deleted
        }
        break;
      case kNMHandoffComplete:
      case kNMConnectRequest:
      default:
         NET_DEBUG_ONLY( OS::_DOUT("EndpointCallback unexpected message [%d] context [%p] cookie [%p]", inCode, inContext, inCookie); )
         break;
    }
}

// =======================================================================================================
// OPEN PLAY NETWORK MANAGER IMPLEMENTATION
// =======================================================================================================

NetworkManager* NetworkManager::createSingletonInstance() {
    return new OpenPlayNetworkManager(EventManager::getSingletonInstance());
}

NetworkManager::~NetworkManager() {
}

OpenPlayNetworkManager::OpenPlayNetworkManager(EventManager* eventMgr)
 : mNetData(), mEventMgr(eventMgr), 
   mConnectionCreator(PDG_OPEN_PLAY_GAME_TYPE),
   mPublicName(PDG_DEFAULT_GAME_NAME),
   mNetworkWait(false),
   mSetWaitCursorMs(0) {
}

OpenPlayNetworkManager::~OpenPlayNetworkManager() {
    ListenerPortMap& listenerPorts = mNetData.listenerPorts;
    ListenerPortMap::iterator lit = listenerPorts.begin();
    PEndpointRef opep;
    while (lit != listenerPorts.end()) {
        opep = (*lit).second;
        if (opep) {
            NET_DEBUG_ONLY( OS::_DOUT("Shutting down server port [%d] endpoint [%p]", (*lit).first, opep); )
            ProtocolStopAdvertising(opep);
//            NET_DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolStopAdvertising returned %d", err); } )
            NMErr err = ProtocolSetEndpointContext(opep, 0);
            NET_DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolSetEndpointContext returned %d", err); } )
            err = ProtocolCloseEndpoint(opep, false); // do a fast shutdown of connection, since app
                                                            // layer should have already closed these
            NET_DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolCloseEndpoint returned %d", err); } )
        }
        lit++;
    }
    AutoMutex mutex(&(mNetData.dataMutex));     // endpoints map could be changed by worker thread
    EndpointRefMap& endpointRefs = mNetData.endpointRefs;
    EndpointRefMap::iterator it = endpointRefs.begin();
    if (it != endpointRefs.end()) {
        // found the endpoint, close the connection
        opep = (*it).first;
        NetEndpoint* ep = (*it).second;
        long id = 0;
        if (ep) {
            id = ep->getId();
        }
        if (opep) {
            NET_DEBUG_ONLY( OS::_DOUT("Shutting down connection [%d] OpenPlay endpoint [%p]", id, opep); )
            NMErr err = ProtocolSetEndpointContext(opep, 0);
            NET_DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolSetEndpointContext returned %d", err); } )
            err = ProtocolCloseEndpoint(opep, false); // do a fast shutdown of connection, since app
                                                            // layer should have already closed these
            NET_DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolCloseEndpoint returned %d", err); } )
        }
        it++;
    }
    mNetData.packetStats.debugDumpStats();
}


void
OpenPlayNetworkManager::idle() {
    // handle any delayed receives
    AutoMutex mutex(&(mNetData.dataMutex));     // endpoints map could be changed by worker thread
    EndpointIdMap& endpointIds = mNetData.endpointIds;
    EndpointIdMap::iterator it = endpointIds.begin();
    if (it != endpointIds.end()) {
        // found the endpoint, close the connection
        NetEndpoint* ep = (*it).second;
        if (ep->receivePending()) {
            NET_DEBUG_ONLY( OS::_DOUT("Doing delayed received for connection [%d]", (*it).first); )
            ep->receive();
        }
        it++;
    }
    // deal with network UI blocking
  #ifndef PDG_NO_GUI
    if (mNetworkWait && mSetWaitCursorMs && (mSetWaitCursorMs < OS::getMilliseconds())) {
        mSetWaitCursorMs = 0;
        // set the OS level cursor to a busy cursor
        platform_setHardwareBusyCursor();
    }
  #endif  
}


// start opening a connection to a remote endpoint.
void 
OpenPlayNetworkManager::openConnection(const char* destination, void* userContext, unsigned char connectFlags) {

    // look for port embedded in hostname
    int len = std::strlen(destination)+1;
    char* dstname = new char[len];
    std::strncpy(dstname, destination, len);
    dstname[len-1] = 0;
    char* hostname = std::strtok(dstname, ":");  // get hostname
    char* token = std::strtok(NULL, ":");  // see if there is anything following a colon separator
    uint16 port = 0;
    if (token) {
        port = std::atoi(token);
    }
    if (port == 0) {
        port = PDG_DEFAULT_PORT_NUM;   // use the default port if none specified
    }
    TODO("Spawn a worker thread to make the connection, currently blocking"); 
    // otherwise we block in ProtocolOpenEndpoint()

    NMErr err;
    bool failed = false;
    PConfigRef config;
    if (std::strlen(hostname) > 512) {  // restrict hostname length to a reasonable size
        hostname[512] = 0;              // to avoid overflowing buffer for config_string
    }
//    std::snprintf(config_string, 1024, "IPaddr=%s\tIPport=%d\tmode=%d", hostname, port, kNMStreamMode);
    // chose mode based on connection flags
    NMEndpointMode mode = kNMNormalMode;
    if (connectFlags & flag_TCPOnly) {
        mode = kNMStreamMode;
    } else if (connectFlags & flag_UDPOnly) {
        mode = kNMDatagramMode;
    }
    err = ProtocolCreateConfig(mConnectionCreator, mPublicName.c_str(), NULL, 0, hostname, port, mode, &config);
	if (err != kNMNoError) {
        DEBUG_ONLY( OS::_DOUT("ERROR: ProtocolCreateConfig returned [%d] for host [%s:%d] mode [%d]", err, hostname, port, mode); )
        failed = true; // this is fatal
    } else { // log success
        NET_DEBUG_ONLY( OS::_DOUT("openConnection made config: host [%s:%d] mode [%d]", hostname, port, mode); )
    }
    PEndpointRef opep;
    NetEndpoint* ep = 0;
    if (!failed) {
        NET_DEBUG_ONLY( OS::_DOUT("Attempting connection to [%s:%d]", hostname, port); )
        ep = new NetEndpoint(static_cast<OpenPlayNetworkManager*>(this), userContext, connectFlags);
        err = ProtocolOpenEndpoint(config, NetEndpoint::EndpointCallback, ep->getContext(), &opep, kOpenActive);
    	if (err != kNMNoError) {
            DEBUG_ONLY( OS::_DOUT("ERROR: ProtocolOpenEndpoint returned [%d]", err); )
            failed = true;  // most definitely fatal
        }
    }
	ProtocolDisposeConfig(config); // ignore error return
    if (!failed) {
        NET_DEBUG_ONLY( OS::_DOUT("Successful connection on OpenPlay endpoint [%d]", opep); )
        ep->connected(opep);
        // send the connection event
        NetConnect netConnectEvent;
        netConnectEvent.id = ep->getId();
        netConnectEvent.context = userContext;
        netConnectEvent.name = ep->getName();
        netConnectEvent.incoming = false;
        mEventMgr->enqueueEvent(eventType_NetConnect, 
                    UserData::makeUserDataViaCopy(&netConnectEvent, sizeof(NetConnect)), this ); // enqueue so main thread can pick it up
    } else {
        NetError netErrEvent;
        netErrEvent.id = 0; // no id established yet
        netErrEvent.context = userContext;
        netErrEvent.error = err;
        mEventMgr->enqueueEvent(eventType_NetError, 
                    UserData::makeUserDataViaCopy(&netErrEvent, sizeof(NetError)), this ); // enqueue so main thread can pick it up
    }
    gWakeupSemaphore.signal();  // wake up the main event loop, we have an event in the queue
}

// close a connection, with an optional error code, clean close if error = 0
void
OpenPlayNetworkManager::closeConnection(long id, int error) {
    AutoMutex mutex(&(mNetData.dataMutex));     // endpoints map could be changed by worker thread
    EndpointIdMap& endpointIds = mNetData.endpointIds;
    EndpointIdMap::iterator it = endpointIds.find(id);
    if (it != endpointIds.end()) {
        // found the endpoint, close the connection
        NetEndpoint* ep = (*it).second;
        if (ep) {
            PEndpointRef opep = ep->getOpenPlayEndpoint();
            bool orderlyShutdown = (error == 0); // orderly shutdown unless we are doing an error close
            mNetData.dataMutex.release();   // release the mutex so we don't get a deadlock, since this might be do callback in same thread
            ProtocolCloseEndpoint(opep, orderlyShutdown);
            // the callback for the endpoint will delete the NetEndpoint and send the NetDisconnect event
        }
    }
}

// send data on a particular established connection
void
OpenPlayNetworkManager::sendData(long id, void* data, long dataLen) {
    AutoMutex mutex(&(mNetData.dataMutex));     // endpoints map could be changed by worker thread
    EndpointIdMap& endpointIds = mNetData.endpointIds;
    EndpointIdMap::iterator it = endpointIds.find(id);
    if (it != endpointIds.end()) {
        // found the endpoint, send it
        long len = dataLen + sizeof(NetPacket);
        NetEndpoint* ep = (*it).second;
        if (ep) {
            NetPacket* packet = ep->createPacket(len);
            char* p = (char*)packet + PDG_PACKET_DATA_OFFSET;
            std::memcpy(p, data, dataLen);
            ep->sendPacket(packet);
            ep->releasePacket(packet);
        } 
        DEBUG_ONLY( 
            else {
                OS::_DOUT("ERROR: no endpoint for connection id [%d]", id); 
            } 
        )
    } 
    DEBUG_ONLY( 
        else {
            OS::_DOUT("ERROR: invalid connection id [%d]", id); 
        } 
    )
}

// free data received when done with it
void    
OpenPlayNetworkManager::releaseNetData(NetData* data) {
    if ( (data != 0) && (data->dataPtr != 0) ) {
        AutoMutex mutex(&(mNetData.dataMutex));     // endpoints map could be changed by worker thread
        EndpointIdMap& endpointIds = mNetData.endpointIds;
        EndpointIdMap::iterator it = endpointIds.find(data->id);
        bool freed = false;
        if (it != endpointIds.end()) {
            // found the endpoint, release the packet
            NetEndpoint* ep = (*it).second;
            if (ep) {
                ep->releasePacketFromDataPtr((void*)data->dataPtr);
                freed = true;
            }
        }
        if (!freed) {
            DEBUG_ONLY( OS::_DOUT("WARNING: Ignoring free of packet data [%p] size [%d] from unknown connection "
                                    "[%d] context [%d]", data, data->dataLen, data->id, data->context); )
        }
    }
}

void
OpenPlayNetworkManager::setConnectionContext(long id, void* userContext) {
    if (id < 0) { // negative, should be a port number
        ListenerPortMap& listenerPorts = mNetData.listenerPorts;
        short portNum = (short)-id;
        ListenerPortMap::iterator lit = listenerPorts.find(portNum);
        if (lit != listenerPorts.end()) {
            // found the listening port, change the context
            ListenerEndpointContext* context = new ListenerEndpointContext;
            context->userContext = userContext;
            context->manager = static_cast<OpenPlayNetworkManager*>(this);
            PEndpointRef opep = (*lit).second;
             // this is going to cause a memory leak of the previous context, don't
             // know of a way to easily avoid it, short of tracking the context for each
             // port in a map. This shouldn't be a common operation though for listening
             // ports, so I'm not too worried about it at the moment
            FIXME("Memory leak of old context here");
            ProtocolSetEndpointContext(opep, &context); 
            return;
        }
        // if we didn't find it, we fall through and check for it being a normal endpoint
        // this lets it keep working even if for some reason endpoints are being assigned
        // negative id numbers
    }
    AutoMutex mutex(&(mNetData.dataMutex));     // endpoints map could be changed by worker thread
    EndpointIdMap& endpointIds = mNetData.endpointIds;
    EndpointIdMap::iterator it = endpointIds.find(id);
    if (it != endpointIds.end()) {
        // found the endpoint, change the context
        NetEndpoint* ep = (*it).second;
        if (ep) {
            NetEndpointContext* context = ep->getContext();
            context->userContext = userContext;
//            ProtocolSetEndpointContext(ep->getOpenPlayEndpoint(), &context);
        }
    }
}

void   
OpenPlayNetworkManager::setMaxIncomingPacketSize(long id, long maxLen) {
    AutoMutex mutex(&(mNetData.dataMutex));     // endpoints map could be changed by worker thread
    EndpointIdMap& endpointIds = mNetData.endpointIds;
    EndpointIdMap::iterator it = endpointIds.find(id);
    if (it != endpointIds.end()) {
        // found the endpoint, set the size
        NetEndpoint* ep = (*it).second;
        if (ep) {
            ep->setMaxIncomingSize(maxLen);
        }
    }
}

// open a listening connection on a particular port that 
// waits for incoming connections
void
OpenPlayNetworkManager::createServerPort(short portNum, void* userContext, unsigned char connectFlags) {
    if (portNum == 0) {
        portNum = PDG_DEFAULT_PORT_NUM;   // use the default port if none specified
    }
    ListenerPortMap& listeners = mNetData.listenerPorts;
    ListenerPortMap::iterator it = listeners.find(portNum);
    if (it != listeners.end()) {
        DEBUG_ONLY( OS::_DOUT("ERROR: attempted to open duplicate Server Port [%d]. Port already in use.\n", portNum); )
        return;
    }
    PConfigRef opconfig = 0;

    const char* hostname = "127.0.0.1";
    NMEndpointMode mode = kNMNormalMode;
    if (connectFlags & flag_TCPOnly) {
        mode = kNMStreamMode;
    } else if (connectFlags & flag_UDPOnly) {
        mode = kNMDatagramMode;
    }
    if (connectFlags & flag_NoHandshake) {
    	TODO("Need to put some kind of handshake flag tracking in here, if we have handshake at all");
    }

    NMErr saveErr = kNMNoError;
    NMErr err = ProtocolCreateConfig(mConnectionCreator, mPublicName.c_str(), NULL, 0, hostname, portNum, mode, &opconfig);
    if (err != kNMNoError) {
        DEBUG_ONLY( OS::_DOUT("ERROR: OpenPlay ProtocolCreateConfig returned [%d] for host [%s:%d] mode [%d]", err, hostname, portNum, mode); )
        saveErr = err;
      #ifdef DEDICATED_SERVER
        mHostLog << util::log::category("HOST") << util::log::fatal
        << "OpenPlay CreateConfig ["<<config_string<<"] failed with error ["<< err <<"]"
        << util::endlog;
        throw err; // a dedicated server can't do anything more here
      #endif
    } else {
        PEndpointRef opep = 0;  // OpenPlay endpoint
        NET_DEBUG_ONLY( OS::_DOUT("Creating Listener with config: host [%s:%d] mode [%d]", hostname, portNum, mode); )
        ListenerEndpointContext* context = new ListenerEndpointContext;
        context->magic = PDG_OPEN_PLAY_GAME_TYPE; // a magic number to tag this as a valid context
        context->userContext = userContext;
        context->manager = static_cast<OpenPlayNetworkManager*>(this);
        context->port = portNum;
        context->connectFlags = connectFlags;
        err = ProtocolOpenEndpoint(opconfig, ListenerCallback, context, &opep, kOpenNone);
        if (err != kNMNoError) {
            saveErr = err;
            NET_DEBUG_ONLY( OS::_DOUT("OpenPlay ProtocolOpenEndpoint returned %d\n", err); )
          #ifdef DEDICATED_SERVER
            mHostLog << util::log::category("HOST") << util::log::fatal
            << "OpenPlay OpenEndpoint ["<<config_string<<"] failed with error ["<< err <<"]"
            << util::endlog;
            throw err; // a dedicated server can't do anything further here
          #endif
        } else {
            { // reduce scope for auto mutex
                // add the new listener to the map
                AutoMutex mutex(&mNetData.dataMutex);
                listeners.insert( ListenerPortMap::value_type(portNum, opep) );
            }
            if (connectFlags & flag_PublicPort) {
                // we need to advertise so others can see our game
                ProtocolStartAdvertising(opep);
            }
            NET_DEBUG_ONLY( OS::_DOUT("opened listening port [%s] OpenPlay endpoint [%p]", mPublicName.c_str(), opep); )
        }
        err = ProtocolDisposeConfig(opconfig);
        if (err != kNMNoError) {
            NET_DEBUG_ONLY( OS::_DOUT("OpenPlay ProtocolDisposeConfig returned [%d]", err); )
        }
/*        // look for IPaddr and IPport in the config string, separated by tabs
        // e.g.:  IPaddr=207.126.114.188   IPport=4429
        std::string theIPAddrStr;
        std::string thePortNumStr;
        char* token;
        token = std::strtok(config_string, "\t");
        while(token != NULL) {
            if (std::strncmp("IPaddr=", token, 7) == 0) {
                theIPAddrStr = &token[7];   // copy in the IP address
            } else if (std::strncmp("IPport=", token, 7) == 0) {
                thePortNumStr = &token[7];  // copy in the port number
            }
            token = std::strtok(NULL, "\t");
        }
        mHostLog << util::log::category("HOST") << util::log::inform
        << "Opened ["<<nameStr<<"] on ["<< theIPAddrStr << ":"<<thePortNumStr<<"]"
        << util::endlog;
     */
    }
    if (saveErr != kNMNoError) {
        NetError netErrEvent;
        netErrEvent.id = -portNum;
        netErrEvent.context = userContext;
        netErrEvent.error = saveErr;
        mEventMgr->enqueueEvent(eventType_NetError, 
                    UserData::makeUserDataViaCopy(&netErrEvent, sizeof(NetError)), this ); // enqueue so main thread can pick it up
        gWakeupSemaphore.signal();  // wake up the main event loop
    }
}

// close a particular listening port
void
OpenPlayNetworkManager::closeServerPort(short portNum) {
    ListenerPortMap& listeners = mNetData.listenerPorts;
    ListenerPortMap::iterator it = listeners.find(portNum);
    if (it != listeners.end()) {
        NET_DEBUG_ONLY( OS::_DOUT("closing server port [%d]\n", portNum); )
        PEndpointRef opep = (*it).second;
        { // reduce scope for automutex
            // remove it from the map
            AutoMutex mutex(&mNetData.dataMutex);
            listeners.erase(it);
        }
        // shut down the listening port
        if (opep) {
            ProtocolStopAdvertising(opep);
            NMErr err = ProtocolCloseEndpoint(opep, true); // do an orderly shutdown of connection
            if (err != kNMNoError) {
                NET_DEBUG_ONLY( OS::_DOUT("OpenPlay ProtocolCloseEndpoint returned %d\n", err); )
            }
//            mHostLog << util::log::category("HOST") << util::log::inform
//            << "Shutdown host" << util::endlog;
        }
    }
}


void ListenerCallback(PEndpointRef inEndpoint, void* inContext, NMCallbackCode inCode, NMErr inError, void* inCookie) {
    // Everything here happens in the Open Play worker thread, not in our application thread(s)
    // we only cover the cases needed for a listener port to spawn a new connection. The rest is handled
    // in the connection's callback, implemented by NetEndpoint::EndpointCallback

    NET_DEBUG_ONLY( OS::_DOUT("ListenerCallback endpoint [%p] context [%p] code [%d] err [%d] "
                            "cookie [%p]", inEndpoint, inContext, inCode, inError, inCookie); )

    if (inContext == 0) {
        NET_DEBUG_ONLY( OS::_DOUT("ListenerCallback ignoring callback with NULL context"); )
        return;
    }
    
    ListenerEndpointContext* context = static_cast<ListenerEndpointContext*>(inContext);
    
    // check that the magic number is correct, if not this could be a corrupted endpoint
    if (context->magic != PDG_OPEN_PLAY_GAME_TYPE) {
        DEBUG_ONLY( OS::_DOUT("ListenerCallback ignoring callback with bad magic number [%p]", context->magic); )
        return;
    }

    switch(inCode)
    {
      case kNMConnectRequest:
        {
            NET_DEBUG_ONLY( OS::_DOUT("ListenerCallback got connect request on port [%d]", context->port); )
            NMErr err = ProtocolAcceptConnection(inEndpoint, inCookie, NetEndpoint::EndpointCallback, inContext);
//            NMErr err = ProtocolAcceptConnection(inEndpoint, inCookie, ListenerCallback, inContext);
            if (err != kNMNoError) {
                    // TODO: make _DOUT thread safe, then enable these messages
                DEBUG_ONLY( OS::_DOUT("ERROR: Server ProtocolAcceptConnection returned [%d] context [%p] "
                                      "cookie [%p]\n", err, inContext, inCookie); )
                return;
            }
            NET_DEBUG_ONLY( OS::_DOUT("Server accepted Connection on port [%d]", context->port); )
        }
        break;
      case kNMHandoffComplete:
         NET_DEBUG_ONLY( OS::_DOUT("ListenerCallback got HandoffComplete for port [%d] context [%p] cookie [%p]", 
                    context->port, inContext, inCookie); )
         break;
      case kNMEndpointDied:
        {
            NET_DEBUG_ONLY( OS::_DOUT("ListenerCallback got EndpointDied for port [%d] context [%p] cookie [%p]",
                        context->port, inContext, inCookie); )
            OpenPlayNetworkManager* netMgr = context->manager;
            ProtocolCloseEndpoint(inEndpoint, false);
            ProtocolStopAdvertising(inEndpoint);
            if (netMgr) {
                NetError netErrEvent;
                netErrEvent.id = -context->port;
                netErrEvent.context = context->userContext;
                netErrEvent.error = inError;
                netMgr->getEventManager()->enqueueEvent(eventType_NetError, 
                            UserData::makeUserDataViaCopy(&netErrEvent, sizeof(NetError)), netMgr ); // enqueue so main thread can pick it up
                gWakeupSemaphore.signal();  // wake up the main event loop
            }
        }
        break;
      case kNMCloseComplete:
        {
            TODO("need to remove this listener from the listeners map");
            NET_DEBUG_ONLY( OS::_DOUT("ListenerCallback got CloseComplete for port [%d] context [%p] cookie [%p]",
                        context->port, inContext, inCookie); )
            // not going to do a disconnect message for a listening port, since there wasn't really a "connection"
            delete context;
            context = 0;    // cleanup the context for this listener
        }
        break;
      case kNMAcceptComplete: // this should only happen on the endpoint callback, not on the listener callback
      case kNMStreamData:
      case kNMFlowClear:
//        NetEndpoint::EndpointCallback(inEndpoint, inContext, inCode, inError, inCookie);
//        break;
      case kNMDatagramData:
      default:
         NET_DEBUG_ONLY( OS::_DOUT("ListenerCallback unexpected message [%d] context [%p] cookie [%p]", inCode, inContext, inCookie); )
         break;
   }   
}

void 
OpenPlayNetworkManager::broadcastData(void* inEventData, long dataLen) {
}

bool 
OpenPlayNetworkManager::hasActiveConnections() {
    AutoMutex mutex(&(mNetData.dataMutex));     // endpoints map could be changed by worker thread
    EndpointIdMap& endpointIds = mNetData.endpointIds;
    return !endpointIds.empty();
}

void 
OpenPlayNetworkManager::doOrderlyShutdown() {
    ListenerPortMap& listenerPorts = mNetData.listenerPorts;
    ListenerPortMap::iterator lit = listenerPorts.begin();
    PEndpointRef opep;
    while (lit != listenerPorts.end()) {
        opep = (*lit).second;
        if (opep) {
            NET_DEBUG_ONLY( OS::_DOUT("Fast shutdown of server port [%d] endpoint [%p]", (*lit).first, opep); )
            ProtocolStopAdvertising(opep);
//            NET_DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolStopAdvertising returned %d", err); } )
            NMErr err = ProtocolSetEndpointContext(opep, 0);
            NET_DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolSetEndpointContext returned %d", err); } )
            err = ProtocolCloseEndpoint(opep, false); // do a fast shutdown of listening port connections
            NET_DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolCloseEndpoint returned %d", err); } )
        }
        lit++;
    }
    AutoMutex mutex(&(mNetData.dataMutex));     // endpoints map could be changed by worker thread
    EndpointRefMap& endpointRefs = mNetData.endpointRefs;
    EndpointRefMap::iterator it = endpointRefs.begin();
    if (it != endpointRefs.end()) {
        // found the endpoint, close the connection
        opep = (*it).first;
        NetEndpoint* ep = (*it).second;
        long id = 0;
        if (ep) {
            id = ep->getId();
        }
        if (opep) {
            NET_DEBUG_ONLY( OS::_DOUT("Orderly shutdown of client connection [%d] OpenPlay endpoint [%p]", id, opep); )
            NMErr err = ProtocolSetEndpointContext(opep, 0);
            NET_DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolSetEndpointContext returned %d", err); } )
            err = ProtocolCloseEndpoint(opep, true); // do a orderly shutdown of connection
            NET_DEBUG_ONLY( if (err) { OS::_DOUT("ProtocolCloseEndpoint returned %d", err); } )
        }
        it++;
    }
}

void
OpenPlayNetworkManager::setPublicInfo(long connectionCreator, const char* publicName) {
    if (connectionCreator == 0) {
        mConnectionCreator = PDG_OPEN_PLAY_GAME_TYPE;
    } else {
        mConnectionCreator = connectionCreator;
    }
    if (publicName == 0) {
        mPublicName.assign(PDG_DEFAULT_GAME_NAME);
    } else {
        mPublicName.assign(publicName);
    }
}

// Put up a network busy cursor and stop accepting user input
// This will log a warning if you were already waiting for the network, since
// there is no reference counting, so the first call to doneWaiting will
// clear the waiting state.
void    
OpenPlayNetworkManager::waitingForNetwork() {
    if (mNetworkWait) {
        DEBUG_ONLY( OS::_DOUT("WARNING: called NetworkManager::waitingForNetwork() while already waiting, IGNORED"); )
        return;
    }
    mNetworkWait = true;
    mSetWaitCursorMs = OS::getMilliseconds() + 500; // delay 1/2 second before changing cursor for network wait
    // disable the actual input handling
//    mEventMgr->blockEvent(eventType_MouseMove); don't block mouse moves for now
    mEventMgr->blockEvent(eventType_MouseDown);
    mEventMgr->blockEvent(eventType_MouseUp);
    mEventMgr->blockEvent(eventType_KeyDown);
    mEventMgr->blockEvent(eventType_KeyUp);
    mEventMgr->blockEvent(eventType_KeyPress);
}

// clear the network busy cursor and allow user input
void    
OpenPlayNetworkManager::doneWaitingForNetwork() {
    mNetworkWait = false;
    mSetWaitCursorMs = 0;
  #ifndef PDG_NO_GUI
    platform_setHardwareNormalCursor();
  #endif // PDG_NO_GUI
    // re-enable input handling
    mEventMgr->unblockEvent(eventType_MouseMove);
    mEventMgr->unblockEvent(eventType_MouseDown);
    mEventMgr->unblockEvent(eventType_MouseUp);
    mEventMgr->unblockEvent(eventType_KeyDown);
    mEventMgr->unblockEvent(eventType_KeyUp);
    mEventMgr->unblockEvent(eventType_KeyPress);
}

// returns true if user input is blocked waiting on completion of a network 
// transaction, false if not
bool
OpenPlayNetworkManager::isWaitingForNetwork() {
    return mNetworkWait;
}

} // end namespace pdg



/* 
 *-------------------------------------------------------------
 * Description:
 *   Functions which handle communication
 *
 *------------------------------------------------------------- 
 * Copyright (c) 1999-2004 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Portions Copyright (c) 1999-2004 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.1 (the "License").  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON- INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
*/


//----------------------------------------------------------------------------------------
// OPEN PLAY STUFF
// -DOP_API_NETWORK_WINSOCK to compile for Windows WinSock API
//----------------------------------------------------------------------------------------

#define op_assert(cond) DEBUG_ASSERT(cond, "op_assert")
#define new_pointer(size)      malloc(size)
#define dispose_pointer(x)  	if (x != NULL) { free(x); }
#define machine_copy_data(srcPtr, destPtr, dataLength)	memcpy(destPtr, srcPtr, dataLength)
#define machine_mem_zero(startLoc, zeroLength)			memset(startLoc, 0, zeroLength)
#define machine_move_data(srcPtr, destPtr, dataLength)	memmove(destPtr, srcPtr, dataLength);

#define PENDPOINT_COOKIE      0x4f504c59  /* hex for "OPLY" */
#define PENDPOINT_BAD_COOKIE  0x62616420  /* hex for "bad " */


#ifdef OP_API_NETWORK_WINSOCK
	#include <winsock.h>
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <errno.h>
#endif

	enum
	{
		kQueryFlag      = 0xFAFBFCFD,
		kReplyFlag      = 0xAFBFCFDF,
		kQuerySize      = 512,
		kMaxGameNameLen = 31
//		kNMEnumDataLen  = 1024		//dair, increase data size from 256
	};
	enum {
	  kModuleID                          = 0x496e6574,  /* "Inet" needs to be same as kTCPIPProtocol */
	  kVersion                           = 0x00000100,
	  config_cookie                      = 0x49506366,  /* "IPcf" */
	  DEFAULT_TIMEOUT                    = 5*1000,      /* 5 seconds */
	  MAXIMUM_CONFIG_LENGTH              = 1024,
	  MAXIMUM_OUTSTANDING_REQUESTS       = 4
	};

	enum {
		_datagram_socket,
		_stream_socket,
		NUMBER_OF_SOCKETS,	
		_uber_connection = ((1 << _datagram_socket) | (1 << _stream_socket))
	};

	enum
	{ /* only used for freeing up the library */
		_state_unknown,
		_state_closing,
		_state_closed
	};

	typedef  unsigned short  	word;
    typedef struct
    {
   		char name[PATH_MAX];
	} FileDesc;

	typedef 	struct NMProtocolConfigPriv	*	NMConfigRef;
	typedef  	struct NMEndpointPriv		*	NMEndpointRef;
	typedef int32		ConnectionRef;
	typedef void 
	(NMEndpointCallbackFunction)(	NMEndpointRef	 			inEndpoint, 
									void* 						inContext,
									NMCallbackCode				inCode, 
									NMErr		 				inError, 
									void* 						inCookie);

	struct ProtocolConfig
	{
		void *configuration_data;
		NMConfigRef NMConfig;

		ConnectionRef connection;
	};
	typedef struct ProtocolConfig ProtocolConfig;
	typedef struct ProtocolConfig *PConfigRef;

	/**Function pointer type passed to \ref ProtocolOpenEndpoint() or ProtocolAcceptConnection() to be called back by OpenPlay when events occur such as data arrival.*/	
	typedef void (*PEndpointCallbackFunction)(
								PEndpointRef	 		inEndpoint, 
								void 					*inContext,
								NMCallbackCode			inCode, 
								NMErr		 			inError, 
								void 					*inCookie);

	struct callback_data
	{
		PEndpointCallbackFunction user_callback;
		void *user_context;
	};


	struct 	NMEndpointPriv {
		NMEndpointRef next; //we're in a linked list
		NMEndpointRef parent; //if we were spawned from a host endpoint
		NMType cookie;
		bool alive; //we're alive until we give the close complete message
		bool dying; //we've given the endpoint died message
		bool needToDie; //socket bit the dust - need to start dying
		uint32 version;
		int32 gameID;
		unsigned long timeout;
		long connectionMode;
		bool		advertising;
		bool 		netSprocketMode;	
		sockaddr_in remoteAddress; //in netsprocket mode we need to store the address for datagram sending	
		long valid_endpoints;
		NMEndpointCallbackFunction *callback;
		void *user_context;
		bool dynamically_assign_remote_udp_port;
		char name[kMaxGameNameLen+1];
		long host;
		word port;
#ifdef OP_API_NETWORK_WINSOCK
		unsigned int sockets[NUMBER_OF_SOCKETS];
#else
		int sockets[NUMBER_OF_SOCKETS];
#endif		
		bool flowBlocked[NUMBER_OF_SOCKETS];
		bool newDataCallbackSent[NUMBER_OF_SOCKETS];
//		status_proc_ptr status_proc;
		bool active;
		bool listener;
		NMErr opening_error;
	};

	struct available_game_data {
		long host;
		word port;
		word flags;
		long ticks_at_last_response;
		char name[kMaxGameNameLen+1];
	};

	#define MAXIMUM_GAMES_ALLOWED_BETWEEN_IDLE (10)

	struct NMProtocolConfigPriv {
		uint32 cookie;
		uint32 version;
		int32 gameID;
		long connectionMode;
		bool netSprocketMode;
		char host_name[256];
	        struct sockaddr_in hostAddr;		/* remote host name */
		char name[kMaxGameNameLen + 1];
		char buffer[MAXIMUM_CONFIG_LENGTH];

	  /* Enumeration Data follows */
		struct available_game_data *games;
		short game_count;
//		NMEnumerationCallbackPtr callback;
		void *user_context;
		int enumeration_socket;
		bool enumerating;
		bool activeEnumeration;
		struct available_game_data new_games[MAXIMUM_GAMES_ALLOWED_BETWEEN_IDLE];
		short new_game_count;
		uint32 ticks_at_last_enumeration_request;
	};

struct Endpoint 
{
	uint32					cookie;
	NMType						type;
	int16					state;
	NMOpenFlags					openFlags;

	/* Data */
	ConnectionRef				connection;
	FileDesc					library_file; /* where the code is.. */
	
	/* Parent (NULL if none) */
	Endpoint			*		parent;
	
	/* Callback data */
	callback_data				callback;

	/* Private module specific data */
	NMEndpointRef				module;
	
	Endpoint *					next;
};
typedef struct Endpoint Endpoint;
typedef struct Endpoint *PEndpointRef;

const char* GetOPErrorName(NMErr err);
const char* GetOPErrorName(NMErr err) { return "internal error"; }


NMErr 		NMCreateConfig		(	const char* inHostname, 
								uint16 inPort, 
								NMEndpointMode inMode,  
								NMType	inGameID,
								const	char *		inGameName,
								const	void *		inEnumData,
								uint32	inDataLen,
								NMConfigRef *outConfig);

NMErr		NMDeleteConfig		(	NMConfigRef			inConfig);

NMErr		NMOpen				(	NMConfigRef 		inConfig, 
								NMEndpointCallbackFunction *	inCallback, 
								void *				inContext, 
								NMEndpointRef *		outEndpoint, 
								bool			inActive);

NMErr		NMClose				(	NMEndpointRef		inEndpoint,
								bool			inOrderly);

NMErr		NMSend				(	NMEndpointRef 		inEndpoint, 
								void *				inData, 
								uint32			inSize,
								NMFlags 			inFlags);

NMErr		NMReceive			(	NMEndpointRef		inEndpoint,
								void *				ioData, 
								uint32 *			ioSize,
								NMFlags *			outFlags);

NMErr		NMAcceptConnection	(	NMEndpointRef		inEndpoint,
								void *				inCookie, 
								NMEndpointCallbackFunction *	inCallback, 
								void *				inContext);

NMErr       NMRejectConnection  (   NMEndpointRef       inEndpoint, 
								void *              inCookie);

NMErr		NMGetIdentifier 	(	NMEndpointRef 		inEndpoint, 
								char *				outIdStr,
							    int16            inMaxLen);


static PEndpointRef create_endpoint(NMErr *err);
static void clean_up_endpoint(PEndpointRef endpoint);
static void net_module_callback_function(NMEndpointRef inEndpoint, 
	                   void* inContext, NMCallbackCode inCode, 
                           NMErr inError, void* inCookie);
static bool valid_endpoint(Endpoint *endpoint);
static Endpoint *create_endpoint_for_accept(PEndpointRef endpoint, NMErr *err);


//----------------------------------------------------------------------------------------
// ProtocolCreateConfig
//----------------------------------------------------------------------------------------
/**
	Creates a new \ref PConfigRef of the given type using either default values or a configuration string.
	@brief Create a new \ref PConfigRef using default values or a configuration string.
	@param game_id The game's type, used to locate fellow games on the network when enumerating.
	@param game_name The game's name.
	@param enum_data Custom enumeration data, to be passed to enumeration callbacks, or NULL if not needed.
	@param enum_data_len Length of the enumeration data.  The data is copied and stored in its entirety in the \ref PConfigRef.
	@param configuration_string A configuration string to use when creating the \ref PConfigRef, or NULL if default values are desired.
	@param inConfig Pointer to a \ref PConfigRef, to be filled in by the function.
	@return \ref kNMNoError if the function succeeds.\n
	Otherwise, returns an error code.
	\n\n\n\n
 */
/*
	Binds to the protocol_names library, mallocs a structure (which specifies 
	the FSSpec of the library), calls its internal Config(char *), and assigns 
	its return value to part of its structure, and then returns the conglomerate
	If configuration_string is NULL, creates default...
*/	

NMErr ProtocolCreateConfig(
	NMType game_id, 
	const char *game_name,
	const void *enum_data,
	uint32 enum_data_len,
	const char* hostname, 
	uint16 port, 
	NMEndpointMode mode, 
	PConfigRef *inConfig)
{
	PConfigRef config= (PConfigRef) NULL;
	NMErr err= kNMNoError;

	config= (PConfigRef) new_pointer(sizeof(ProtocolConfig));
	if (config)
	{
		/* Clear it.. */
		machine_mem_zero(config, sizeof(ProtocolConfig));


		err = NMCreateConfig(hostname, port, mode, game_id, game_name, enum_data, enum_data_len, 
                                                     (NMConfigRef *)(&config->configuration_data));
		if(!err)
		{
			*inConfig= config;
		} else {
			ProtocolDisposeConfig(config);
		}
	} else {
		err = pdg::NetworkManager::error_OutOfMemory;
	}
	
	return err;
}

//----------------------------------------------------------------------------------------
// ProtocolDisposeConfig
//----------------------------------------------------------------------------------------
/**
	Dispose of a \ref PConfigRef
	@brief Dispose of a \ref PConfigRef
	@param config the \ref PConfigRef to dispose of.
	@return \ref kNMNoError if the function succeeds.\n
	Otherwise, returns an error code.
	\n\n\n\n
 */
/*
	Rebinds, calls the protocol DisposeConfig, frees internal structures, and returns
*/
NMErr ProtocolDisposeConfig(
	PConfigRef config)
{
	NMErr err= kNMNoError;
	
	if(config)
	{
		if(config->configuration_data) 
		{
			err= NMDeleteConfig((NMProtocolConfigPriv*)config->configuration_data);
			config->configuration_data= NULL;
		}
//		unbind_from_protocol(config->type, true);
		dispose_pointer(config);
	}
	
	return err;
}


//----------------------------------------------------------------------------------------
// ProtocolStartAdvertising
//----------------------------------------------------------------------------------------
/**
	Enable a host endpoint to be seen on the network.
	@brief Enable a host endpoint to be seen on the network.
	@param endpoint The passive(host) endpoint to enable advertising for.
	@return No return value.
	\n\n\n\n
 */

void ProtocolStartAdvertising(PEndpointRef endpoint)
{
	op_assert(valid_endpoint(endpoint));
	if (endpoint && !endpoint->module) {
		endpoint->module->advertising = true;
	}
}

//----------------------------------------------------------------------------------------
// ProtocolStopAdvertising
//----------------------------------------------------------------------------------------
/**
	Prevent a host endpoint from being seen on the network.
	@brief Prevent a host endpoint from being seen on the network.
	@param endpoint The passive(host) endpoint to disable advertising for.
	@return No return value.
	\n\n\n\n
 */

void ProtocolStopAdvertising(PEndpointRef endpoint)
{
	op_assert(valid_endpoint(endpoint));
	if (endpoint && !endpoint->module) {
		endpoint->module->advertising = false;
	}
}


//----------------------------------------------------------------------------------------
// ProtocolOpenEndpoint
//----------------------------------------------------------------------------------------
/**
	Create a new endpoint.
	@brief Creates a new endpoint.
	@param inConfig The \ref PConfigRef configuration to create end endpoint from.
	@param inCallback The callback function OpenPlay will call for events involving this endpoint.
	@param inContext A custom context that will be passed to the endpoint's callback function. 
	@param outEndpoint Pass a pointer to a \ref PEndpointRef which will receive the new endpoint.
	@param flags Flags - passing \ref kOpenNone will create a passive(host) endpoint, while passing \ref kOpenActive will create an active(client) endpoint.
	@return \ref kNMNoError if the function succeeds.\n
	Otherwise, an error code.
	\n\n\n\n
 */

/*
	Takes the config data created by ProtocolConfig, and builds an endpoint out of it.
	active, passive
	listening & spawn (ie multiple endpoints)
	Callback for multiples.
*/

/*
	Binds to the protocol layer, fills in the function pointers, stays bound until
	CloseEndpoint is called, and calls ProtocolOpenEndpoint
*/
NMErr ProtocolOpenEndpoint(
	PConfigRef inConfig, 
	PEndpointCallbackFunction inCallback,
	void *inContext,
	PEndpointRef *outEndpoint, 
	NMOpenFlags flags)
{
	NMErr err= kNMNoError;
	Endpoint *ep;

	ep= create_endpoint(&err);
	if(!err)
	{
		op_assert(ep);
		
		ep->openFlags = flags;
		
		/* Now call it's initialization function */
		/* setup the callback data */
		ep->callback.user_callback= inCallback;
		ep->callback.user_context= inContext;

		/* And open.. */			
		err= NMOpen((NMProtocolConfigPriv*)inConfig->configuration_data, net_module_callback_function, (void *)ep, &ep->module, 
									(flags & kOpenActive) ? true : false);
		if(!err)
		{
			op_assert(ep->module);
			*outEndpoint= ep;
			DEBUG_PRINT("New endpoint created (0X%X) (0X%X to netmodule)",ep, ep->module);
		} else {
			DEBUG_PRINT("Open failed creating endpoint"); 
			/* Close up everything... */
			clean_up_endpoint(ep);
		}
	}

#if (DEBUG)
	if( err )
		DEBUG_PRINT("ProtocolOpenEndpoint returning %s (%d)\n",GetOPErrorName(err),err);
#endif //DEBUG
	
	return err;
}

//----------------------------------------------------------------------------------------
// ProtocolCloseEndpoint
//----------------------------------------------------------------------------------------
/**
	Close an endpoint.  Endpoints must always be closed,
	even if they have received a \ref kNMEndpointDied message.
	After closing an endpoint, one should wait for its \ref kNMCloseComplete message to arrive,
	or else wait until \ref ProtocolIsAlive() returns false for the endpoint.
	Only then can one be certain that the endpoint's callback will no longer be triggered with new messages.
	@brief Close an endpoint.
	@param endpoint The endpoint to close.
	@param inOrderly Whether to attempt an "orderly" disconnect.  This is not yet well-defined...
	@return \ref kNMNoError if the function succeeds.\n
	Otherwise, an error code.
	\n\n\n\n
 */

/*
	closes the endpoint
	puts NULL in all the function pointers,
	unbinds from the shared library,
*/
NMErr ProtocolCloseEndpoint(
	PEndpointRef endpoint,
	bool inOrderly)
{
	NMErr err= kNMNoError;

	/* call as often as possible (anything that is synchronous) */

	DEBUG_PRINT("ProtocolCloseEndpoint called for endpoint (0X%X)",endpoint);
	
	if(valid_endpoint(endpoint) && endpoint->state != _state_closing)
	{
		op_assert(endpoint->cookie==PENDPOINT_COOKIE);
		/* don't try to close it more than once... */
		if(endpoint->state==_state_unknown)
		{
			endpoint->state= _state_closing;
			
			//make sure we're not working with an incomplete endpoint
			op_assert(endpoint->module != NULL);
			err= NMClose(endpoint->module, inOrderly);

		} else if (endpoint->state==_state_closing) { 
				DEBUG_PRINT("endpoint state was %d should have been _state_unknown (%d) or _state_closing (%d) in ProtocolCloseEndpoint", 
				endpoint->state, _state_unknown, _state_closing);
		}
		/* note we clean up everything on the response from the NMClose... */
	} else {
		if (valid_endpoint(endpoint) == false)
			DEBUG_PRINT("invalid endpoint(0X%X) passed to ProtocolCloseEndpoint()",endpoint);
		else if (endpoint->state == _state_closing)
			DEBUG_PRINT("endpoint (0X%X) passed to ProtocolCloseEndpoint() in state _state_closing",endpoint);
		err= kNMParameterErr;
	}
	
#if (DEBUG)
	if( err )
		DEBUG_PRINT("ProtocolCloseEndpoint returning %s (%d) for endpoint (0X%X)",GetOPErrorName(err),err,endpoint);	
#endif //DEBUG
	return err;
}


//----------------------------------------------------------------------------------------
// ProtocolSetEndpointContext
//----------------------------------------------------------------------------------------
/**
	Set an endpoint's associated context to a new value.
	@brief Set an endpoint's associated context to a new value.
	@param endpoint The endpoint whose context is to be set.
	@param newContext The new context for the endpoint.
	@return \ref kNMNoError if the function succeeds.\n
	Otherwise, an error code.
	\n\n\n\n
 */

NMErr ProtocolSetEndpointContext(PEndpointRef endpoint, void* newContext)
{
	NMErr err= kNMNoError;

	op_assert(valid_endpoint(endpoint));

	if (endpoint && (endpoint->cookie == PENDPOINT_COOKIE))
	{
		endpoint->callback.user_context = newContext;
	}
	else
	{
		err = kNMParameterErr;
	}
	return err;
}


//----------------------------------------------------------------------------------------
// ProtocolSend
//----------------------------------------------------------------------------------------
/**
	Send data via an endpoint's reliable(stream) connection.  Stream data is guaranteed to arrive in the order it was sent
	and it generally faster for large amounts of data, but does not arrive in "packets" of any specific size.  Applications must
	be prepared to break the stream back into packets if necessary.
	@brief Send data via an endpoint's reliable(stream) connection.
	@param endpoint The endpoint to send the data to.
	@param inData Pointer to the data to be sent.
	@param inSize Size of the data to be sent.
	@param inFlags Flags.
	@return If positive, the number of bytes actually sent.\n If negative, an error code.\n
	If the number of bytes sent is less than the number requested, the endpoint will receive a \ref kNMFlowClear message once
	more data can be sent.
	\n\n\n\n
*/
int32 ProtocolSend(
	PEndpointRef endpoint, 
	void *inData, 
	uint32 inSize, 
	NMFlags inFlags)
{
	int32 result= kNMNoError;
	if (valid_endpoint(endpoint)) { // don't assert if invalid endpoint, just return an error
		result= NMSend(endpoint->module, inData, inSize, inFlags);
	} else {
		result= kNMParameterErr;
	}
	return result; // is < 0 incase of error...
}

//----------------------------------------------------------------------------------------
// ProtocolReceive
//----------------------------------------------------------------------------------------
/**
	Receive data from an endpoint's reliable(stream) connection.
	@brief Receive data from an endpoint's reliable(stream) connection.
	@param endpoint The endpoint to receive the data from.
	@param outData Pointer to a buffer to receive the stream data.
	@param ioSize In: amount of data requested. Out: amount of data received.
	@param outFlags Flags. Pass \ref kNMBlocking to block until receival of \e ioSize bytes of data.
	@return \ref kNMNoError if the function succeeds.\n
	\ref kNMNoDataErr if there is no more data to be read.\n
	Otherwise, an error code.
	\n\n\n\n
 */
NMErr ProtocolReceive(
	PEndpointRef endpoint, 
	void *outData, 
	uint32 *ioSize, 
	NMFlags *outFlags)
{
	int32 result= kNMNoError;
	if (valid_endpoint(endpoint)) { // don't assert if invalid endpoint, just return an error
		result= NMReceive(endpoint->module, outData, ioSize, outFlags);
	} else {
		result= kNMParameterErr;
	}
	return result; // is < 0 incase of error...
}


//----------------------------------------------------------------------------------------
// ProtocolAcceptConnection
//----------------------------------------------------------------------------------------
/**
	Accept a pending client connection, creating a new \ref PEndpointRef for the client connection.
	@brief Accept a pending client connection.
	@param endpoint The endpoint that received the \ref kNMConnectRequest message.
	@param inCookie The cookie value which accompanied the \ref kNMConnectRequest message to the host endpoint's callback.
	@param inNewCallback The callback function for the new endpoint to be created for the client.
	@param inNewContext The custom context to be assigned to the new endpoint.
	@return \ref kNMNoError if the function succeeds.\n
	Otherwise, an error code.
	\n\n\n\n
 */

/* -------------- connections */
NMErr 
ProtocolAcceptConnection(
	PEndpointRef endpoint,
	void *inCookie,
	PEndpointCallbackFunction inNewCallback,
	void *inNewContext)
{
	NMErr err= kNMNoError;

	op_assert(valid_endpoint(endpoint));
	if(endpoint)
	{
		op_assert(endpoint->cookie==PENDPOINT_COOKIE);
		Endpoint *new_endpoint;
		
		new_endpoint= create_endpoint_for_accept(endpoint, &err);
		if(!err)
		{
			op_assert(new_endpoint);
			
			new_endpoint->callback.user_callback= inNewCallback;
			new_endpoint->callback.user_context= inNewContext;

/* DEBUG_PRINT("Calling accept connection on endpoint: 0x%x ep->module: 0x%x new_endpoint: 0x%x", endpoint, endpoint->module, new_endpoint); */
			err= NMAcceptConnection(endpoint->module, inCookie, net_module_callback_function, new_endpoint);
			if (err) {
				/* error occured.  Clean up... */
				DEBUG_PRINT("Open failed creating endpoint of type 0x%x for accept Error: %d", endpoint->type, err);
				clean_up_endpoint(new_endpoint);
			}
		}
	} else {
		err= kNMParameterErr;
	}
	
#if (DEBUG)
	if( err )
		DEBUG_PRINT("ProtocolAcceptConnection returning %s (%d)",GetOPErrorName(err),err);	
#endif //DEBUG
	return err;
}


//----------------------------------------------------------------------------------------
// ProtocolGetEndpointIdentifier
//----------------------------------------------------------------------------------------

/* ----------- information functions */
NMErr ProtocolGetEndpointIdentifier (   
    PEndpointRef endpoint,
    char *identifier_string, 
    int16 max_length)
{
	NMErr err= kNMNoError;

	/* Some of this error checking could be ignored.. */	
	op_assert(valid_endpoint(endpoint));
	if(endpoint)
	{
		err= NMGetIdentifier(endpoint->module, identifier_string, max_length);
	} else {
		err= kNMParameterErr;
	}
	return err;
}


//----------------------------------------------------------------------------------------
// net_module_callback_function
//----------------------------------------------------------------------------------------

/* ------------ static code */
static void net_module_callback_function(
	NMEndpointRef inEndpoint, 
	void* inContext,
	NMCallbackCode inCode, 
	NMErr inError, 
	void* inCookie)
{
	PEndpointRef ep= (PEndpointRef) inContext;
	bool callback= true;
	
	op_assert(ep);
	op_assert(ep->callback.user_callback);
//	op_vassert(ep->cookie==PENDPOINT_COOKIE, csprintf(sz_temporary, "inCookie wasn't our PEndpointRef? 0x%x Cookie: 0x%x", ep, ep->cookie));

	/* must always reset it, because it may not be set yet. (trust me) */
	ep->module= inEndpoint;
	op_assert(valid_endpoint(ep));
	switch(inCode)
	{
		case kNMAcceptComplete: /* new endpoint, cookie is the parent endpoint. (easy) */
			/* generate the kNMHandoffComplete code.. */
			op_assert(ep->parent);
			if(ep->parent->callback.user_callback)
			{
				ep->parent->callback.user_callback(ep->parent, ep->parent->callback.user_context, kNMHandoffComplete, 
					inError, ep);
			}
			
			/* now setup for the kNMAcceptComplete */
			inCookie= ep->parent;
			op_assert(inCookie);
			break;
			
		case kNMHandoffComplete:
			/* eat it. */
			callback= false;
			break;
		default:
		break;
	}

	if(callback && ep->callback.user_callback)
		ep->callback.user_callback(ep, ep->callback.user_context, inCode, inError, inCookie);
	if(inCode==kNMCloseComplete)
	{
//		op_vwarn(ep->state==_state_closing, csprintf(sz_temporary, 
//			"endpoint state was %d should have been _state_closing (%d) for kNMCloseComplete", 
//			ep->state, _state_closing));
		clean_up_endpoint(ep); /* Must try to return to the cache, because we can't deallocate memory at interrupt time. */
	}
}

//----------------------------------------------------------------------------------------
// create_endpoint
//----------------------------------------------------------------------------------------

static PEndpointRef create_endpoint(
	NMErr *err)
{
	Endpoint *ep = NULL;
	
	ep= (Endpoint *) new_pointer(sizeof(Endpoint));
	if(ep)
	{
		machine_mem_zero(ep, sizeof(Endpoint));
		/* Load in the endpoint information.. */
		ep->cookie= PENDPOINT_COOKIE;
		ep->state= _state_unknown;
		ep->parent= NULL;
		ep->next= NULL;
	} else {
		*err = pdg::NetworkManager::error_OutOfMemory;
	}
	return ep;
}

//----------------------------------------------------------------------------------------
// clean_up_endpoint
//----------------------------------------------------------------------------------------

static void clean_up_endpoint(
	PEndpointRef endpoint)
{
  if (endpoint) {
  	/* done after the above to make sure everything's okay.. */
  	endpoint->cookie = PENDPOINT_BAD_COOKIE;
  	endpoint->state = _state_closed;
	dispose_pointer(endpoint);
  }
}

//----------------------------------------------------------------------------------------
// valid_endpoint
//----------------------------------------------------------------------------------------
static bool valid_endpoint(Endpoint *endpoint) {
	if (!endpoint) return false;
	if (endpoint->cookie != PENDPOINT_COOKIE) return false;
	if (!endpoint->module) return false;
	return true;
}

//----------------------------------------------------------------------------------------
// create_endpoint_for_accept
//----------------------------------------------------------------------------------------

static Endpoint *create_endpoint_for_accept(
	PEndpointRef endpoint,
	NMErr *err)
{
	Endpoint *new_endpoint= NULL;

	new_endpoint= create_endpoint(err);
	if (new_endpoint) {
		new_endpoint->parent= endpoint;
	}
	return new_endpoint;
}


/* 
 *-------------------------------------------------------------
 * Description:
 *   Functions which handle communication
 *
 *------------------------------------------------------------- 
 *   Author: Kevin Holbrook
 *  Created: June 23, 1999
 *
 * Copyright (c) 1999-2004 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Portions Copyright (c) 1999-2004 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.1 (the "License").  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON- INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
*/

#include <signal.h>
#include <fcntl.h>
#ifndef OP_API_NETWORK_WINSOCK
#include <sys/time.h>
#include <pthread.h>
#endif

#define kDefaultNetSprocketMode false

// this hacks in minimal support for EAGAIN for asynchronous communications
#define HACKY_EAGAIN
#define DEBUG_ENTRY_EXIT(x)
#define DEBUG_NETWORK_API( message,err)
#define op_vassert_return(expr,diag,err) if (!(expr)) {  return err; }

#if defined(PLATFORM_WIN32)
	typedef int 			posix_size_type;
	typedef WORD word;
	#define op_errno WSAGetLastError()
	#define EINPROGRESS WSAEINPROGRESS
	#define EWOULDBLOCK WSAEWOULDBLOCK
	#define EMSGSIZE WSAEMSGSIZE
	#define usleep(a) Sleep(a/1000)
	#define pthread_exit ExitThread
	#define close closesocket
#else
	typedef unsigned int 	posix_size_type;
	#define op_errno errno
#endif

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET (-1)
#endif

// -------------------------------  Private Definitions

bool workerThreadAlive = false;
bool dieWorkerThread = false;

#if defined(OP_API_NETWORK_WINSOCK)
	DWORD	worker_thread;
	HANDLE	worker_thread_handle;
#else 
	pthread_t	worker_thread;
#endif

//since we are multithreaded, we have to use a mutual-exclusion locks for certain items
uint32 endpointListState;
NMEndpointPriv *endpointList;
pdg::Mutex notifierLock;
pdg::Mutex endpointListLock;
pdg::Mutex endpointWaitingListLock;

#define kNMOutOfMemoryErr pdg::NetworkManager::error_OutOfMemory

//for locking callbacks -
#define TRY_ENTER_NOTIFIER() notifierLock.tryAquire(0)
#define ENTER_NOTIFIER() notifierLock.aquire()
#define LEAVE_NOTIFIER() notifierLock.release()

//locks the endpoint list - you must lock this when adding or removing endpoints, and increment endpointListState whenever you change it (while locked of course)
#define TRY_LOCK_ENDPOINT_LIST() endpointListLock.tryAquire(0)
#define TRY_LOCK_ENDPOINT_WAITING_LIST() endpointWaitingListLock.tryAquire(0)
#define LOCK_ENDPOINT_LIST() endpointListLock.aquire()
#define LOCK_ENDPOINT_WAITING_LIST() endpointWaitingListLock.aquire()
#define UNLOCK_ENDPOINT_LIST() endpointListLock.release()
#define UNLOCK_ENDPOINT_WAITING_LIST() endpointWaitingListLock.release()

#define MARK_ENDPOINT_AS_VALID(e, t) ((e)->valid_endpoints |= (1<<(t)))

#define MACHINE_TICKS_PER_SECOND  1000L


//	------------------------------	Private Functions
static bool 	internally_handle_read_data(NMEndpointRef endpoint, int16 type);
bool processEndpoints(bool block);
void processEndPointSocket(NMEndpointPriv *theEndPoint, long socketType, fd_set *input_set, fd_set *output_set, fd_set *exc_set);
void receive_udp_port(NMEndpointRef endpoint);
static bool internally_handled_datagram(NMEndpointRef endpoint);
static long socketReadResult(NMEndpointRef endpoint,int socketType);
void SetNonBlockingMode(int fd);
int createWakeSocket(void);
void disposeWakeSocket(void);
void sendWakeMessage(void);
void createWorkerThread(void);
void killWorkerThread(void);
#ifdef OP_API_NETWORK_WINSOCK
	void initWinsock(void);
	void shutdownWinsock(void);
	DWORD WINAPI worker_thread_func(LPVOID arg);
#else
	void* worker_thread_func(void *arg);
#endif
bool	is_ip_request_packet(char *buffer, uint32 length, NMType gameID);
int32      build_ip_enumeration_response_packet(char *buffer, NMType gameID, 
																uint32 version, 
																uint32 host, 
																uint16 port, 
																char *name, 
																uint32 custom_data_length,
																void *custom_data);

void       byteswap_ip_enumeration_packet(char *buffer);
//static void _build_config_string_into_config_buffer(NMConfigRef config);
//static NMErr _parse_config_string(char *string, uint32 GameID, NMConfigRef config);
//static short _get_default_port(uint32 GameID);
//static bool _build_standard_config_strings(NMConfigRef config);
//static short _generate_default_port(uint32 GameID);
//static NMErr _get_standard_config_strings(char *string, NMConfigRef config);
//static bool get_token( const char * s, const char * token_label, short token_type, void * token_data, long * io_length );
//static bool put_token( char * s, long s_max_length, const char * token_label, short token_type, void * token_data, long data_len );
//static int32 atol_tab(const char * s);
//static int32 strlen_tab(const char * s);
//static bool ascii_to_hex(const char *inAscii, int32 inLen, char *ioData);
//static bool hex_to_ascii(const char *inData, int32 inLen, char *ioAscii);



//  ------------------------------  Private Variables

#define LABEL_TERM          '='
#define TOKEN_SEPARATOR     '\t'
#define MAX_BIN_DATA_LEN    256
#define STRING_DATA     0
#define LONG_DATA       1
#define	BOOLEAN_DATA    2
#define BINARY_DATA     3	/* no greater than 256 bytes */
#define kIPConfigAddress  "IPaddr"
#define kIPConfigPort     "IPport"

const char *kConfigModuleType = "type";
const char *kConfigModuleVersion = "version";
const char *kConfigGameID = "gameID";
const char *kConfigGameName = "gameName";
const char *kConfigEndpointMode = "mode";
const char *kConfigNetSprocketMode = "netSprocket";
const char *kConfigCustomData = "enumData";


#if (OP_API_NETWORK_WINSOCK)
	bool	winSockRunning = false;
#endif

static int wakeSocket;
static int wakeHostSocket;

//for notifier locks
//static long notifierLockCount = 0;

/*stuff for our worker thread*/
#if (USE_WORKER_THREAD)
	bool workerThreadAlive = false;
	bool dieWorkerThread = false;
	
	#ifdef OP_API_NETWORK_SOCKETS
		pthread_t	worker_thread;
	#elif defined(OP_API_NETWORK_WINSOCK)
		DWORD	worker_thread;
		HANDLE	worker_thread_handle;
	#endif
#endif

struct udp_port_struct_from_server {
	word port;
};

typedef struct IPEnumerationResponsePacket
{
	uint32	responseCode;
	NMType		gameID;
	uint32	version;
	uint32	host;
	uint16	port;
	uint16	pad;
	char		name[kMaxGameNameLen+1];
	uint32	customEnumDataLen;
} IPEnumerationResponsePacket;


/* 
 * Static Function: _lookup_machine
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN]  machine = name of machine to resolve
 *  [IN]  default_port = port number to include in final address
 *  [IN/OUT]  hostAddr = structure to contain resolved information  
 *
 * Returns:
 *   0 = failed to get IP address for name
 *   1 = successfully got IP address for name
 *
 * Description:
 *   Function to lookup the IP address of a specified machine name.
 *   Expects and returns in network order.
 *
 *--------------------------------------------------------------------
 */

static int _lookup_machine(char *machine, unsigned short default_port, struct sockaddr_in *hostAddr)
{
  int  gotAddr = 0;
  char trimmedHost[256];
  char *colon_pos;              /* pointer to rightmost colon in name */
  unsigned short  needPort;     /* port we need */
  struct hostent  *hostInfo;
  unsigned long	  hostIPAddr;

	DEBUG_ENTRY_EXIT("_lookup_machine");


  if (!hostAddr)
    return(0);


  /* Take port numbers on the end of host names/numbers
   * This does nothing in the context of OpenPlay, since we always specify the port
   * number separately. This will let you paste in a host name with a colon and
   * safely ignore it.
   */
  needPort = default_port;

  strcpy(trimmedHost, machine);

  /* find first colon */
  colon_pos = strchr(trimmedHost, ':');

  /* everything after that is the port number, if we found it */
  if (colon_pos)
  {
    *colon_pos++ = 0;  /* strip the port spec and pass to next char */
    needPort = htons((short) atoi(colon_pos));
  }
  
  hostInfo = gethostbyname(trimmedHost);

  if (hostInfo)
  {
    /* Got an ip address by resolving name */
    hostIPAddr = *(unsigned long *) (hostInfo->h_addr_list[0]);

    gotAddr = 1;
  }
  else if (INADDR_NONE != (hostIPAddr = inet_addr(trimmedHost)))
    gotAddr = 1;

  if (gotAddr)
  {
    hostAddr->sin_family = AF_INET;
    hostAddr->sin_port = needPort;
    hostAddr->sin_addr.s_addr = hostIPAddr;
  }

  return gotAddr;
} /* _lookup_machine */

//creates a datagram and stream socket on the same port - retries if necessary until success is achieved
static NMErr _create_sockets(int *sockets, word required_port, bool active)
{
	NMErr status = kNMNoError;
	struct sockaddr_in address;
	posix_size_type size = sizeof(address);
	int opt = true;

	DEBUG_ENTRY_EXIT("_create_sockets");
	
	int datagramSockets[64]; //if we dont get it in 64 tries, we dont deserve to get it...
	int streamSocket;
	long counter = 0;
	
	while (true)
	{
		//make a datagram socket, bind it to any port, and try to make a stream socket on that port.
		//if we don't get the same port, leave it bound for now and move to the next (otherwise we might get same one repeatedly)
		datagramSockets[counter] = socket(PF_INET,SOCK_DGRAM,0);
		op_assert(datagramSockets[counter]);
		
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(required_port);
		
		//so we can bind to it even if its just been used
		if (required_port)
		{
			status = setsockopt(datagramSockets[counter], SOL_SOCKET, SO_REUSEADDR,(char*)&opt, sizeof(opt));
			DEBUG_NETWORK_API("setsockopt for reuse address", status);
		}
		
		status = bind(datagramSockets[counter],(sockaddr*)&address,sizeof(address));
		if (!status)
		{	
			status = getsockname(datagramSockets[counter], (sockaddr*) &address, (socklen_t*)&size);
			if (!status)
			{	
				DEBUG_PRINT("bound a datagram socket to port %d; trying stream",ntohs(address.sin_port));
				//weve got a bound datagram socket and its port - now try making a stream socket on the same one
				streamSocket = socket(PF_INET,SOCK_STREAM,0);
				op_assert(streamSocket);
				
				//so we can bind to it even if its just been used
				status = setsockopt(streamSocket, SOL_SOCKET, SO_REUSEADDR,(char*)&opt, sizeof(opt));
				DEBUG_NETWORK_API("setsockopt for reuse address", status);
				
				//why is this only done when not active? just curious...
				if (!active)
				{
					/* set no delay */
					status = setsockopt(streamSocket, IPPROTO_TCP, TCP_NODELAY,(char*)&opt, sizeof(opt));
					DEBUG_NETWORK_API("setsockopt for no delay", status);
				}
				
				//port is already correct from our getsockname call
				address.sin_family = AF_INET;
				address.sin_addr.s_addr = INADDR_ANY;
				
				status = bind(streamSocket,(sockaddr*)&address,sizeof(address));
				if (!status)
				{
					DEBUG_PRINT("bind worked for stream");
					//success! return these two sockets
					sockets[_datagram_socket] = datagramSockets[counter];
					sockets[_stream_socket] = streamSocket;
					//decrement our counter so this most recent one isnt destroyed
					counter--;
					break;
				}
				//if bind failed, kill the stream socket. we only use one of those at once
				else
				{
					DEBUG_NETWORK_API("bind for stream endpoint",status);
					close(streamSocket);
				}
			}			
		}
		//if we required a certain port and didnt get it, we fail
		if (required_port)
			break;

		if (counter > 63)
			break; //cry uncle if it goes this far
			
		//well, that one didnt work. increment the counter and try again
		counter++;
	}
	
	//cleanup time - go backwards through our array, closing down all the sockets we made
	while (counter > 0)
	{
		close(datagramSockets[counter]);
		counter--;
	}
	
	if (status == -1)
		status = kNMOpenFailedErr;
	return status;
}
/* 
 * Static Function: _setup_socket
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN] Endpoint = 
 *  [IN] socket_index = 
 *  [IN] Data = 
 *  [IN] Size = 
 *  [IN] Flags =   
 *
 * Returns:
 *  
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

static NMErr _setup_socket(	NMEndpointRef 			new_endpoint, 
							int						index,
                            struct sockaddr_in *	hostAddr, 
                            bool 				Active, 
                            int *					preparedSockets)
{
	int socket_types[] = {SOCK_DGRAM, SOCK_STREAM};
	int status = kNMNoError;
		
	DEBUG_ENTRY_EXIT("_setup_socket");


	// sjb 19990330 either we aren't active meaning we don't need a remote address
	//              or we better be passed an address
	op_assert((Active == 0) || (0 != hostAddr));

	//use a prepared socket, or make our own
	if (preparedSockets)
		new_endpoint->sockets[index] = preparedSockets[index];
	else
		new_endpoint->sockets[index] = socket(PF_INET, socket_types[index], 0);

	if (new_endpoint->sockets[index] != INVALID_SOCKET)
	{
		//if it wasnt prepared, set up the socket and bind it
		if (preparedSockets == NULL)
		{
			struct sockaddr_in sin;
			int opt = true;
		
			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = INADDR_ANY;
			sin.sin_port = htons(0);
			
			if ((Active == 0) && (index == _stream_socket))
			{
				/* set no delay */
				status = setsockopt(new_endpoint->sockets[index], IPPROTO_TCP, TCP_NODELAY,(char*)&opt, sizeof(opt));
				DEBUG_NETWORK_API("setsockopt for no delay", status);
			}

			if (index == _datagram_socket)
				DEBUG_PRINT("binding datagram socket to port %d",ntohs(sin.sin_port));
			else
				DEBUG_PRINT("binding stream socket to port %d",ntohs(sin.sin_port));
			status = bind(new_endpoint->sockets[index], (sockaddr*)&sin, sizeof(sin));
		}
		if (!status)
		{
			sockaddr_in address;
			posix_size_type size = sizeof(address);
					
			// get the host and port...
			status = getsockname(new_endpoint->sockets[index], (sockaddr*) &address, (socklen_t*)&size);
			if (!status)
			{
				new_endpoint->host= ntohl(address.sin_addr.s_addr);
				new_endpoint->port= ntohs(address.sin_port);
				
				//*required_port = new_endpoint->port;
				if (index == _datagram_socket)
					DEBUG_PRINT("have a legal datagram socket on port %d",new_endpoint->port);
				else
					DEBUG_PRINT("have a legal stream socket on port %d",new_endpoint->port);
			
				//set the endpoint to be non-blocking
				SetNonBlockingMode(new_endpoint->sockets[index]);
			}
			else
				DEBUG_NETWORK_API("getsockname",status);
									
			if (!status)
			{
				//we dont want the datagram socket to connect() in netsprocket mode because
				//it won't be sending and receiving to the same port (and connect locks it to one)
				//(we send data to the port we connect to, but they use a random port to send us data)
				if ((Active) && ((new_endpoint->netSprocketMode == false) || (index == _stream_socket)))
				{
					status = connect(new_endpoint->sockets[index], (sockaddr*)hostAddr, sizeof(*hostAddr));

					//if we get EINPROGRESS, it just means the socket can't be created immediately, but that
					//we can select() for completion by selecting the socket for writing...
					if (status)
					{
						if ((op_errno == EINPROGRESS) || (op_errno == EWOULDBLOCK))
						{
							if (index == _datagram_socket)
								DEBUG_PRINT("got EINPROGRESS from connect() on datagram socket - will need to select() for completion");
							else
								DEBUG_PRINT("got EINPROGRESS from connect() on stream socket - will need to select() for completion");
							status = 0;							
						}
						else
							DEBUG_NETWORK_API("connect()",status);
					}
					else //we can go ahead and mark this socket as valid
					{
						MARK_ENDPOINT_AS_VALID(new_endpoint,index);
						new_endpoint->flowBlocked[index] = false;
					}
				}
				else if (index == _stream_socket)
				{
					status = listen(new_endpoint->sockets[index], MAXIMUM_OUTSTANDING_REQUESTS);  
					if (status != 0) {
						DEBUG_NETWORK_API("listen()",status);
                    }
				}
			}
		}
		else
		{
			DEBUG_NETWORK_API("Bind Socket", status);
			status = kNMOpenFailedErr;
		}
	}
	else
	{
		DEBUG_NETWORK_API("Creating Socket",status);
		
		//standard error codes
		status = kNMOpenFailedErr;
	}
	
    return(status);
}


/* 
 * Static Function: _create_endpoint
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN] hostInfo = 
 *  [IN] Callback = 
 *  [IN] Context = 
 *  [OUT] Endpoint = 
 *  [IN] Active =
 *  [IN] create_sockets = 
 *  [IN] connectionMode = 
 *  [IN] version = 
 *  [IN] gameID = 
 *
 * Returns:
 *  
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

static NMErr 
_create_endpoint(
	sockaddr_in *hostInfo, 
	NMEndpointCallbackFunction *Callback,
	void *Context,
	NMEndpointRef *Endpoint,
	bool Active,
	bool create_sockets,
	long connectionMode,
	bool netSprocketMode,
	unsigned long version,
	unsigned long gameID)
{
	NMEndpointRef new_endpoint;
	int index;
	NMErr err = 0;

	DEBUG_ENTRY_EXIT("_create_endpoint");

		
	new_endpoint = (NMEndpointRef)calloc(1, sizeof(struct NMEndpointPriv));


	if (!new_endpoint)
		return(kNMOutOfMemoryErr);

	if (create_sockets == false)
		DEBUG_PRINT("\n\nCREATING JOINER ENDPOINT (0X%X)\n\n",new_endpoint);
	else if (Active)
		DEBUG_PRINT("\n\nCREATING ACTIVE ENDPOINT (0X%X)\n\n",new_endpoint);
	else
		DEBUG_PRINT("\n\nCREATING PASSIVE ENDPOINT (0X%X)\n\n",new_endpoint);

	machine_mem_zero(new_endpoint, sizeof(NMEndpointPriv));
	
	//both joiner and passive endpoints are passed with active == false, so we distinguish here	
	new_endpoint->listener = false;
	if (!Active)
	{
		if (create_sockets == true)
			new_endpoint->listener = true;
	}
	new_endpoint->parent = NULL;
	new_endpoint->alive = false;
	new_endpoint->cookie = kModuleID;
	new_endpoint->dying = false;
	new_endpoint->needToDie = false;
	new_endpoint->connectionMode = connectionMode;
	new_endpoint->netSprocketMode= netSprocketMode;
	new_endpoint->advertising = false;
	new_endpoint->timeout  = DEFAULT_TIMEOUT;
	new_endpoint->callback = Callback;
	new_endpoint->user_context = Context;
	if ((new_endpoint->netSprocketMode) || (new_endpoint->listener == true))
	//if ((new_endpoint->netSprocketMode) || (Active == false))
		new_endpoint->dynamically_assign_remote_udp_port = false;
	else
		new_endpoint->dynamically_assign_remote_udp_port = ((connectionMode & _uber_connection) == _uber_connection);
	new_endpoint->valid_endpoints = 0;
	new_endpoint->version = version;
	new_endpoint->gameID = gameID;
	new_endpoint->active = Active;
	new_endpoint->opening_error = 0;

	for (index = 0; index < NUMBER_OF_SOCKETS; ++index)
	{
		new_endpoint->sockets[index] = INVALID_SOCKET;
		new_endpoint->flowBlocked[index] = true; //gotta wait till it says we can write
		new_endpoint->newDataCallbackSent[index] = false;
	}
	*Endpoint = new_endpoint;

	if (create_sockets)
 	{ 
 		int preparedSockets[NUMBER_OF_SOCKETS];
		int *preparedSocketsPtr;
 		
		//sometimes we need datagram and stream sockets to be on the same port
		//netsprocket mode does because we don't send our our port number to the connected machine so it has to be predictable
		//we also need it when hosting, because enumeration requests are sent to the datagram socket on that port
		//therefore, we prepare the sockets first-off, since we might not be able to get two of a kind on the first try
		if ((new_endpoint->netSprocketMode) || (new_endpoint->listener))
		{
			word required_port;
			if (Active)
				required_port = 0; //we need two of the same, but they can be any port
			else
				required_port = ntohs(hostInfo->sin_port);//gotta get two of a specific port
				
			err = _create_sockets(preparedSockets,required_port,Active);
			preparedSocketsPtr = preparedSockets;
		}
		else
		{
			preparedSocketsPtr = NULL;
		}

		if (!err)
		{
			for (index = 0; !err && index < NUMBER_OF_SOCKETS; ++index)
			{
				if (new_endpoint->connectionMode & (1 << index))
				{
					if (!err)
						err = _setup_socket(new_endpoint, index, hostInfo, Active, preparedSocketsPtr);
				}
			}
		}
	}

	if (err)
	{
		NMClose(new_endpoint, false);
		new_endpoint = NULL;
		*Endpoint = NULL;

		return(err);
	}

	// now we add ourself to our list of live endpoints.
	// the worker thread might be in the middle of a long select() call,
	// so we hop on the waiting list to keep the worker thread from re-acquiring the lock,
	// then attempt to "wake up" out of the select() call to get it to relenquish its current lock,
	// and hopefully grab the lock quickly ourselves

	LOCK_ENDPOINT_WAITING_LIST();
	sendWakeMessage();
	LOCK_ENDPOINT_LIST();
	new_endpoint->next = endpointList;
	endpointList = new_endpoint;
	endpointListState++;
	UNLOCK_ENDPOINT_LIST();
	UNLOCK_ENDPOINT_WAITING_LIST();
	return(kNMNoError);
}

/* 
 * Static Function: _send_data
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN] Endpoint = 
 *  [IN] socket_index = 
 *  [IN] Data = 
 *  [IN] Size = 
 *  [IN] Flags =   
 *
 * Returns:
 *  
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

static NMErr _send_data(	NMEndpointRef 		Endpoint, 
							int					socket_index,
                        	void *				Data, 
                        	unsigned long 		Size, 
                        	NMFlags 			Flags)
{
	int result;
	int done = 0;
	int offset = 0;
	unsigned long bytes_to_send = Size;
	unsigned long total_bytes_sent = 0;

	DEBUG_ENTRY_EXIT("_send_data");


	if (Endpoint->sockets[socket_index] == INVALID_SOCKET)
	return(kNMParameterErr);

	do
	{
		//if we're in netsprocket mode and active, we have to supply the address for each send, since
		//we're not associated with an address via connect().  Doing that would not allow us to receive datagrams from
		//the host, since they dont come from that same port.
		if ((socket_index == _datagram_socket) && (Endpoint->netSprocketMode) && (Endpoint->active))
			result = sendto(Endpoint->sockets[socket_index], ((char*)Data) + offset, bytes_to_send,0,(sockaddr*)&Endpoint->remoteAddress,sizeof(Endpoint->remoteAddress));
		else
			result = send(Endpoint->sockets[socket_index], ((char*)Data) + offset, bytes_to_send, 0);

		/* FIXME - check all result codes */

		if (result == -1)
		{
#ifdef HACKY_EAGAIN
			if ( errno == EAGAIN )
			{
				result = 0;
			}
			else
			{
				DEBUG_NETWORK_API("send",result);
				return(kNMInternalErr);
			}
#else
			DEBUG_NETWORK_API("send",result);
			return(kNMInternalErr);
#endif // HACKY_EAGAIN
		}

		offset += result;
		bytes_to_send -= result;
		total_bytes_sent += result;

/*    if ((Flags & kNMBlocking) && (total_bytes_sent != Size))
		done = 0;
	else */
 		done = 1;

	} while (!done);

	return(total_bytes_sent);

} /* _send_data */


/* 
 * Static Function: _receive_data
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN] Endpoint = 
 *  [IN] the_socket = 
 *  [IN] Data = 
 *  [IN] Size = 
 *  [IN] Flags =   
 *
 * Returns:
 *  
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

static NMErr _receive_data(NMEndpointRef inEndpoint, int which_socket, 
                           void *ioData, unsigned long *ioSize, NMFlags *outFlags)
{
	NMErr		err = kNMNoError;
	int32	result;

	*outFlags = 0;
	op_vassert_return((inEndpoint != NULL),"inEndpoint is NIL!",kNMParameterErr);
    op_vassert_return(inEndpoint->cookie==kModuleID, csprintf(sz_temporary, "cookie: 0x%x != 0x%x", inEndpoint->cookie, kModuleID),kNMParameterErr); 
	op_assert(inEndpoint->sockets[which_socket]!=INVALID_SOCKET);

	if (inEndpoint->needToDie == true)
		return kNMNoDataErr;
#ifdef HACKY_EAGAIN
	SetNonBlockingMode(inEndpoint->sockets[which_socket]);
#endif // HACKY_EAGAIN
	result = recv(inEndpoint->sockets[which_socket], (char *)ioData, *ioSize, 0);

	if (result == 0)
	{
		inEndpoint->needToDie = true;
		return kNMNoDataErr;
	}
	
	//should be more detailed here
	if (result == -1)
	{
		if (op_errno != EWOULDBLOCK) {
			DEBUG_NETWORK_API("recv",result);
        }
		err= kNMNoDataErr;
	}
	else {				
		*ioSize = result;
	}

	return err;
} /* _receive_data */


/* 
 * Function: _send_datagram_socket
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN]  Endpoint = 
 *
 * Returns:
 *  
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

static void _send_datagram_socket(NMEndpointRef endpoint)
{
	struct sockaddr_in  address;
	int    status;
	NMErr  err;
	posix_size_type size;
	
	DEBUG_ENTRY_EXIT("_send_datagram_socket");
	
	size = sizeof(address);
	
	status = getsockname(endpoint->sockets[_datagram_socket], (sockaddr*)&address, (socklen_t*)&size);
	
	if (status == 0)
	{
		struct udp_port_struct_from_server  port_data;
		
		port_data.port = address.sin_port;
		DEBUG_PRINT("sending udp port: %d",ntohs(port_data.port));
		
		err = _send_data(endpoint, _stream_socket, (void *)&port_data, sizeof(port_data), 0);
	}
}

//----------------------------------------------------------------------------------------
// internally_handle_read_data
//----------------------------------------------------------------------------------------

static bool
internally_handle_read_data(NMEndpointRef endpoint, int16 type)
{
bool	handled_internally = false;

	DEBUG_ENTRY_EXIT("internally_handle_read_data");

	//if we're dying, dont bother - but return true so its not passed on
	if (endpoint->needToDie)
		return true;
		
	if (type == _stream_socket)
	{
		// if we are an uber endpoint ASSIGN_SOCKET 
		if (endpoint->dynamically_assign_remote_udp_port)
		{
			endpoint->dynamically_assign_remote_udp_port = false;
			
			receive_udp_port(endpoint);
			handled_internally = true;

		}
	}
	else
	{
		
		handled_internally= internally_handled_datagram(endpoint);
	}

	return handled_internally;
}

//----------------------------------------------------------------------------------------
// internally_handled_datagram
//----------------------------------------------------------------------------------------

static bool
internally_handled_datagram(NMEndpointRef endpoint)
{
	bool	handled_internally = false;
	int32	bytes_read;
	char		packet[kQuerySize + 4]; //add a bit so we know if it goes over the size we're looking for

	bytes_read= recvfrom(endpoint->sockets[_datagram_socket], packet, sizeof (packet), MSG_PEEK, NULL, NULL);

	if (bytes_read == kQuerySize)
	{
		
		if (is_ip_request_packet(packet, bytes_read, endpoint->gameID))
		{
			sockaddr	remote_address;
			char		response_packet[512];
			uint32	bytes_to_send;
			ssize_t result;
			posix_size_type   remote_address_size  = sizeof(remote_address);

			DEBUG_PRINT("got an enumeraion-request object");

			// its ours - read it for real and see who its from
			bytes_read = recvfrom(endpoint->sockets[_datagram_socket], (char *) &packet, sizeof (packet),
				0, (sockaddr*) &remote_address, (socklen_t*)&remote_address_size);

			if (endpoint->advertising)
			{
				DEBUG_PRINT("responding");
				// And respond to it.
				bytes_to_send= build_ip_enumeration_response_packet(response_packet, endpoint->gameID, 
					endpoint->version, endpoint->host, endpoint->port, endpoint->name, 0, NULL);
				op_assert(bytes_to_send<=sizeof (response_packet));

				byteswap_ip_enumeration_packet(response_packet);

				// send!
				result= sendto(endpoint->sockets[_datagram_socket], response_packet, 
					bytes_to_send, 0, (sockaddr*) &remote_address, sizeof (remote_address));

				if (result > 0) {
					op_assert(result==bytes_to_send);
				} else if (result < 0) {
					DEBUG_NETWORK_API("Sendto on enum response",result);
				}
			}
			// we handled it.
			handled_internally= true;
		}

	} 

	return handled_internally;
}

/* 
 * Function: _wait_for_open_complete
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN]  Endpoint = 
 *
 * Returns:
 *  
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

static NMErr _wait_for_open_complete(NMEndpointRef Endpoint)
{
	int done = 0;
	long entry_time;
	long elapsed_time = 0;
	long max_wait_time = 10 * MACHINE_TICKS_PER_SECOND;

	DEBUG_ENTRY_EXIT("_wait_for_open_complete");

	// both types of non-active endpoints have stream sockets that are already valid
	// listener endpoints dont need to wait, and a new handed-off socket is already valid too
	if ( !Endpoint->active && (Endpoint->connectionMode & (1 << _stream_socket)) )
		MARK_ENDPOINT_AS_VALID(Endpoint,_stream_socket);
		
	entry_time = pdg::OS::getMilliseconds();

	// wait here for stream and datagram socket confirmations - and also the remote udp port if we're not in nsp mode
	while (!done && !Endpoint->opening_error && (elapsed_time < max_wait_time))
	{
		if (((Endpoint->connectionMode & Endpoint->valid_endpoints) == Endpoint->connectionMode)
			&& (Endpoint->dynamically_assign_remote_udp_port == false))
			done = 1;
		
		elapsed_time = pdg::OS::getMilliseconds() - entry_time;
	} 
	if (done)
		DEBUG_PRINT("_wait_for_open_complete: endpoint successfully constructed");
		
	if (Endpoint->opening_error)
		DEBUG_PRINT("_wait_for_open_complete: opening_error %d",Endpoint->opening_error);
	if (!done)
	{
		NMErr returnValue;
		if (elapsed_time >= max_wait_time)
			DEBUG_PRINT("timed-out waiting for open complete");
		
		Endpoint->alive = false;

		//this gets set if they close before we get a udp port
		if (Endpoint->opening_error)
			returnValue =  Endpoint->opening_error;
		else
			returnValue =  kNMOpenFailedErr;

		NMClose(Endpoint, false);
		
		return returnValue;
	}
	else
		return Endpoint->opening_error;

	return(kNMNoError);
} /* _wait_for_open_complete */


/* 
 * Function: NMOpen
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN]  Config = 
 *  [IN]  Callback = 
 *  [IN]  Context = 
 *  [OUT] Endpoint = 
 *  [IN]  Active =   
 *
 * Returns:
 *  
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

NMErr NMOpen(NMConfigRef Config,
             NMEndpointCallbackFunction *Callback, void* Context, 
             NMEndpointRef *Endpoint, bool Active)
{
	NMErr  err;
	int    status;

	DEBUG_ENTRY_EXIT("NMOpen");

	if (!Config || !Callback || !Endpoint)
		return(kNMParameterErr);

	if (Config->cookie != config_cookie)
		return(kNMInvalidConfigErr);

	//make sure we've inited winsock (doing this duringDllMain causes problems)
	#ifdef OP_API_NETWORK_WINSOCK
		initWinsock();
	#endif

	//make sure a worker-thread is running if need-be (doing this in _init can cause problems)
	createWorkerThread();

	*Endpoint = NULL;

	if (Active)
	{
		if (Config->host_name[0])
		{
			struct sockaddr_in host_info;

			status = _lookup_machine(Config->host_name, Config->hostAddr.sin_port, &host_info);

			if (status)
				memcpy(&(Config->hostAddr), &host_info, sizeof(struct sockaddr_in));
			else
				return(kNMAddressNotFound);
		}
	}

	err = _create_endpoint(&(Config->hostAddr), Callback, Context, Endpoint,
		Active, true, Config->connectionMode, Config->netSprocketMode,
		Config->version, Config->gameID);

	if (!err)
	{
		/* copy the name */
		strcpy((*Endpoint)->name, Config->name);

		err = _wait_for_open_complete(*Endpoint);
	}

	/* FIXME : check that Endpoint can be set NULL here after other calls */
	if (err)
	{
		*Endpoint = NULL;
		return(err);
	}

	//unleash the dogs.  this lets messages start hitting the callback
	DEBUG_PRINT("endpoint 0x%x is now alive",*Endpoint);
	(*Endpoint)->alive = true;

	return(kNMNoError);
} /* NMOpen */


/* 
 * Function: NMClose
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN/OUT] Endpoint = 
 *  [IN] Orderly  =  
 *
 * Returns:
 *  
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

NMErr NMClose(NMEndpointRef Endpoint, bool Orderly)
{
	int index;
	int status;
	struct linger linger_option;

	DEBUG_ENTRY_EXIT("NMClose");

	if (!Endpoint)
	{
		DEBUG_PRINT("NULL Endpoint in NMClose");
		return(kNMParameterErr);
	}

	if (Endpoint->cookie != kModuleID)
	{
		DEBUG_PRINT("Invalid endpoint cookie detected in NMClose");
		return(kNMInternalErr);
	}
	linger_option.l_onoff = 1;
	linger_option.l_linger = 0;

	DEBUG_PRINT("Searching for theEndpoint in NMClose");

	//search for this endpoint on the list, and if its there, remove it
	{
		bool found = false;
		NMEndpointPriv *theEndpoint;

		theEndpoint = endpointList;

		if (theEndpoint == Endpoint) //we're first on the list
		{
			found = true;
			endpointList = Endpoint->next;
		}
		else while (theEndpoint != NULL)
		{
			if (theEndpoint->next == Endpoint) //its us
			{
				found = true;
				theEndpoint->next = Endpoint->next;
				break;
			}
			theEndpoint = theEndpoint->next;
		}
		if (found)
			endpointListState++;
	}

    	DEBUG_PRINT("Done searching for theEndpoint in NMClose");

	for (index = 0; index < NUMBER_OF_SOCKETS; ++index)
	{
		if (Endpoint->sockets[index] != INVALID_SOCKET)
		{
			if (!Orderly && index != _datagram_socket)
			{
				/* cause socket to discard remaining data and reset on close */
				status = setsockopt(Endpoint->sockets[index], SOL_SOCKET, SO_LINGER,
					(char*)&linger_option, sizeof(struct linger));
			}

    		DEBUG_PRINT("Shutting down a socket in NMClose...");
			/* shutdown the socket, not allowing any further send/receives */
			status = shutdown(Endpoint->sockets[index], 2);

    		DEBUG_PRINT("Closing a socket in NMClose...");
			status = close(Endpoint->sockets[index]);
		} 
	} /* for (index) */

	// notify that it is closed, if necessary
	if (Endpoint->alive)
	{
		Endpoint->alive = false;
    	DEBUG_PRINT("Notifying about closure in NMClose...");
		Endpoint->callback(Endpoint, Endpoint->user_context, kNMCloseComplete, 0, NULL);
	}

	Endpoint->cookie = PENDPOINT_BAD_COOKIE;


	/* FIX ME - why free the endpoint pointer here ? */
	DEBUG_PRINT("Freeing the Endpoint in NMClose...");
	free(Endpoint);

	return(kNMNoError);
} /* NMClose */


/* 
 * Function: NMAcceptConnection
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN] Endpoint
 *  [IN] Cookie
 *  [IN] Callback
 *  [IN] Context
 *
 * Returns:
 *  
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

NMErr
NMAcceptConnection(
	NMEndpointRef				inEndpoint, 
	void						*inCookie, 
	NMEndpointCallbackFunction	*inCallback, 
	void						*inContext)
{
	DEBUG_ENTRY_EXIT("NMAcceptConnection");
	NMErr			err;
	NMEndpointRef	new_endpoint;

	op_vassert_return((inCallback != NULL),"Callback is NULL!",kNMParameterErr);
	op_vassert_return((inEndpoint != NULL),"inEndpoint is NIL!",kNMParameterErr);
    op_vassert_return(inEndpoint->cookie==kModuleID, csprintf(sz_temporary, "cookie: 0x%x != 0x%x", inEndpoint->cookie, kModuleID),kNMParameterErr); 

    // create all the data...
    
    err = _create_endpoint(0, inCallback, inContext, &new_endpoint, false, false, inEndpoint->connectionMode,
			  inEndpoint->netSprocketMode, inEndpoint->version, inEndpoint->gameID);

	if (! err)
	{
		new_endpoint->parent = inEndpoint;
		
   		// create the sockets..
		sockaddr_in	remote_address;
		posix_size_type	remote_length  = sizeof(remote_address);

 		// make the accept call...
		DEBUG_PRINT("calling accept()");
		new_endpoint->sockets[_stream_socket]= accept(inEndpoint->sockets[_stream_socket], (sockaddr*) &remote_address, 
													   (socklen_t*)&remote_length);

		DEBUG_PRINT("the remote address is %d",ntohs(remote_address.sin_port));
		if (new_endpoint->sockets[_stream_socket] != INVALID_SOCKET)
		{
			sockaddr_in	address;
			uint16	required_port;
			posix_size_type     size  = sizeof(address);

			err = getpeername(new_endpoint->sockets[_stream_socket], (sockaddr*) &address, (socklen_t*)&size);

	  		if (!err)
			{
		 		required_port = 0;

				     // if we need a datagram socket
		      	if (inEndpoint->connectionMode & 1)
		 		{
					// create the datagram socket with the default port and host set..
					err= _setup_socket(new_endpoint, _datagram_socket, &address, true,NULL);
		     	}
			}
	  		else
	      		DEBUG_NETWORK_API("Getsockname", err);
		}
		else
			DEBUG_NETWORK_API("Accept (for Accept)", err);

		if (err)
		{
			//we're not really alive - we dont wanna send a close complete
			new_endpoint->alive = false;
			NMClose(new_endpoint, false);
			new_endpoint= NULL;
		}
		else
		{
			// copy whatever we need from the original endpoint...
			new_endpoint->host= inEndpoint->host;
			new_endpoint->port= inEndpoint->port;
			strcpy(new_endpoint->name, inEndpoint->name);
//			new_endpoint->status_proc= inEndpoint->status_proc;

			// if we are uber, send our datagram port..
			if ( ((new_endpoint->connectionMode & _uber_connection) == _uber_connection) && 
					(!new_endpoint->netSprocketMode) )
			{
				// uber connection.
			// passive and active send the datagram port.
				_send_datagram_socket(new_endpoint);
			}
		}

		// and the async calls...
		// This calls back the original endpoints callback proc, which may or may not be what you are expecting.
		// (It wasn't what I was expecting the first time)

		// CRT, Sep 24, 2000
		// There are 2 async calls here:  (1)  The kNMAcceptComplete callback is to be called to the new endpoint,
		// and (2) the kNMHandoffComplete is called to the original endpoint, which is the listener...

		
		
	}
	else
		DEBUG_NETWORK_API("Create Endpoint (for Accept)", err);

	return err;

} // NMAcceptConnection


/* 
 * Function: NMRejectConnection
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN] Endpoint =
 *  [IN] Cookie   =  
 *
 * Returns:
 *  
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

NMErr NMRejectConnection(NMEndpointRef Endpoint, void *Cookie)
{
	int    closing_socket;
	struct sockaddr_in remote_address;
	posix_size_type remote_length = sizeof(remote_address);
	
	DEBUG_ENTRY_EXIT("NMRejectConnection");
	
	if (!Endpoint)
		return(kNMParameterErr);
	
	if (Endpoint->cookie != kModuleID)
		return(kNMInternalErr);
	
	DEBUG_PRINT("calling accept()");
	closing_socket = accept(Endpoint->sockets[_stream_socket], 
		(sockaddr*)&remote_address, (socklen_t*)&remote_length);
	
	if (closing_socket != INVALID_SOCKET)
	{
		struct linger linger_option;
		int status;
		
		linger_option.l_onoff = 1;
		linger_option.l_linger = 0;
		
		status = setsockopt(closing_socket, SOL_SOCKET, SO_LINGER,
			(char*)&linger_option, sizeof(struct linger)); 
		
		if (status != 0)
			return(op_errno);
		
		status = shutdown(closing_socket, 2);
		
		if (status != 0)
			return(op_errno);
		
		status = close(closing_socket);
		
		if (status != 0)
			return(op_errno);
	}
	
	return(kNMNoError);
} /* NMRejectConnection */



/* 
 * Function: NMGetIdentifier
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN] Endpoint = 
 *  [OUT] IdStr =  
 *  [IN] MaxLen = 
 *
 * Returns:
 *  identifier for remote endpoint (dotted IP address) in outIdStr
 *
 *--------------------------------------------------------------------
 */

NMErr
NMGetIdentifier(NMEndpointRef inEndpoint,  char * outIdStr, int16 inMaxLen)
{
	int the_socket;

	DEBUG_ENTRY_EXIT("NMGetIdentifier");

	if (!inEndpoint)
		return(kNMParameterErr);
	
	if (!outIdStr)
		return(kNMParameterErr);

	if (inMaxLen < 1)
		return(kNMParameterErr);

	if (inEndpoint->cookie != kModuleID)
		return(kNMInternalErr);
	
    the_socket = inEndpoint->sockets[_stream_socket];
    sockaddr_in   remote_address;
    int			remote_length = sizeof(remote_address);
    char result[256];
    
    getpeername(the_socket, (sockaddr*)&remote_address, (socklen_t*)&remote_length);
    
    in_addr addr = remote_address.sin_addr;

    sprintf(result, "%s", inet_ntoa(addr));
	
    strncpy(outIdStr, result, inMaxLen - 1);
    outIdStr[inMaxLen - 1] = 0;

	return (kNMNoError);
}



/* 
 * Function: NMSend
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN] Endpoint = 
 *  [IN/OUT] Data = 
 *  [IN/OUT] Size = 
 *  [IN/OUT] Flags = 
 *
 * Returns:
 *   See _send_data(). 
 *
 * Description:
 *   Function to send a stream packet.
 *
 *--------------------------------------------------------------------
 */

NMErr NMSend(NMEndpointRef Endpoint, void *Data, unsigned long Size, NMFlags Flags)
{
	DEBUG_ENTRY_EXIT("NMSend");

	unsigned long result;

	if (!Endpoint || !Data)
		return(kNMParameterErr);

	if (Endpoint->cookie != kModuleID)
		return(kNMInternalErr);

	result = _send_data(Endpoint, _stream_socket, Data, Size, Flags);

	//if its a positive return value (not an error) and not the same as they requested,
	//we're flow blocked - start looking for a flow clear
	if ((result != Size) && (result > 0))
		Endpoint->flowBlocked[_stream_socket] = true; //let em know when they can go again

	return(result);
} /* NMSend */


/* 
 * Function: NMReceive
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN] Endpoint = 
 *  [IN/OUT] Data = 
 *  [IN/OUT] Size = 
 *  [IN/OUT] Flags = 
 *
 * Returns:
 *   See _receive_data().
 *
 * Description:
 *   Function to receive a stream packet.
 *
 *--------------------------------------------------------------------
 */

NMErr NMReceive(NMEndpointRef Endpoint, void *Data, unsigned long *Size, NMFlags *Flags)
{

	DEBUG_ENTRY_EXIT("NMReceive");

	Endpoint->newDataCallbackSent[_stream_socket] = false; //we should start telling them of incoming data again
	
	NMErr err;

	if (!Endpoint || !Data || !Size || !Flags)
		return(kNMParameterErr);

	if (Endpoint->cookie != kModuleID)
		return(kNMInternalErr);

	err = _receive_data(Endpoint, _stream_socket, Data, Size, Flags);

	return(err);
} /* NMReceive */


/* 
 * Function: NMCreateConfig
 *--------------------------------------------------------------------
 * Parameters:
 *  [IN]  ConfigStr   =
 *  [IN]  GameID      =
 *  [IN]  GameName    =
 *  [IN]  EnumData    =
 *  [IN]  EnumDataLen =
 *  [OUT] Config      =
 *
 * Returns:
 *   Network module error
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

NMErr NMCreateConfig(const char* inHostname, 
					uint16 inPort, 
					NMEndpointMode inMode,  
	             	NMType GameID, 
                    const char *GameName, 
	             	const void *EnumData, 
                    uint32 EnumDataLen,
	             	NMConfigRef *Config)
{
	DEBUG_ENTRY_EXIT("NMCreateConfig");

	//make sure we've inited winsock (doing this duringDllMain causes problems)
	#ifdef OP_API_NETWORK_WINSOCK
		initWinsock();
	#endif

	//make sure a worker-thread is running if need-be (doing this in _init can cause problems)
	createWorkerThread();

	NMConfigRef _config = (struct NMProtocolConfigPriv *) new_pointer(sizeof(struct NMProtocolConfigPriv));

	if (_config)
	{
		_config->cookie  = config_cookie;
		_config->version = kVersion;
		_config->gameID  = GameID;

		_config->hostAddr.sin_family = AF_INET;
		_config->hostAddr.sin_port = htons( inPort );
		_config->hostAddr.sin_addr.s_addr = INADDR_NONE;

		_config->enumeration_socket = INVALID_SOCKET;
		_config->enumerating = false;
		_config->connectionMode = inMode; // stream and/or datagram.
		_config->netSprocketMode = false;
		_config->games = NULL;
		_config->game_count = 0;
		_config->new_game_count = 0;
	}
	else
	{
		*Config = NULL;
		return(kNMOutOfMemoryErr);
	}

	std::strncpy(_config->host_name, inHostname, 256);
	MAKE_STRING_BUFFER_SAFE(_config->host_name, 256);
	
	if (GameName) {
		strncpy(_config->name, GameName, kMaxGameNameLen);
		_config->name[kMaxGameNameLen] = '\0';
	} else {
		_config->name[0] = '\0';
	}

	*Config = _config;
	return(kNMNoError);

} // NMCreateConfig




/* 
 * Function: NMDeleteConfig
 *--------------------------------------------------------------------
 * Parameters:
 *  
 *
 * Returns:
 *  
 *
 * Description:
 *   Function 
 *
 *--------------------------------------------------------------------
 */

NMErr NMDeleteConfig(NMConfigRef inConfig)
{
	DEBUG_ENTRY_EXIT("NMDeleteConfig");

	op_vassert_return((inConfig != NULL),"Config ref is NULL!",kNMParameterErr);
	op_vassert_return((inConfig->cookie==config_cookie),"inConfig->cookie==config_cookie",kNMParameterErr);

	inConfig->cookie= 'bad ';
	dispose_pointer(inConfig);
	
	return kNMNoError;
} /* NMDeleteConfig */



int createWakeSocket(void)
{
	struct sockaddr Server_Address;
	int result;
	posix_size_type nameLen = sizeof(Server_Address);
	
		
	wakeSocket = 0;
	wakeHostSocket = 0;
	
	machine_mem_zero((char *) &Server_Address, sizeof(Server_Address));	
	((sockaddr_in *) &Server_Address)->sin_family = AF_INET;
	((sockaddr_in *) &Server_Address)->sin_addr.s_addr = INADDR_ANY;
	((sockaddr_in *) &Server_Address)->sin_port = 0;
	
	wakeHostSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (!wakeHostSocket)	return false;
	result = bind(wakeHostSocket, (struct sockaddr *)&Server_Address, sizeof(Server_Address));
	if (result == -1)	return false;
	wakeSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (!wakeSocket)	return false;
	result = bind(wakeSocket, (struct sockaddr *)&Server_Address, sizeof(Server_Address));
	if (result == -1)	return false;
	
	result = getsockname(wakeHostSocket, (struct sockaddr *)&Server_Address, (socklen_t*)&nameLen);
	if (result == -1)	return false;
	result = connect(wakeSocket,(sockaddr*)&Server_Address,sizeof(Server_Address));
	if (result == -1)	return false;
	
//	result = send(wakeSocket,buffer,1,0);
//	if (result == -1)	return false;
		
//	long amountIn = recv(wakeHostSocket,buffer,sizeof(buffer),0);
//	printf("in buffer: %d\n",amountIn);
	return true;
}

void disposeWakeSocket(void)
{
	if (wakeSocket)
		close(wakeSocket);
	if (wakeHostSocket)
		close(wakeHostSocket);
}


//sends a small datagram to our "wake" socket to try and break out of a select() call
void sendWakeMessage(void)
{
	char buffer[10];
	//DEBUG_PRINT("sending wake message");
	send(wakeSocket,buffer,1,0);
	//DEBUG_PRINT("sendWakeMessage result: %d",result);
}


void SetNonBlockingMode(int fd)
{

	DEBUG_ENTRY_EXIT("SetNonBlockingMode");

#ifdef OP_API_NETWORK_SOCKETS
	int val = fcntl(fd, F_GETFL,0); //get current file descriptor flags
	if (val < 0)
	{
		DEBUG_PRINT("error: fcntl() failed to get flags for fd %d: err %d",fd,op_errno);
		return;
	}

	val |= O_NONBLOCK; //turn non-blocking on
	if (fcntl(fd, F_SETFL, val) < 0)
	{
		DEBUG_PRINT("error: fcntl() failed to set flags for fd %d: err %d",fd,op_errno);
	}
#elif defined(OP_API_NETWORK_WINSOCK)
	unsigned long DataVal = 1;
	long result = ioctlsocket(fd,FIONBIO,&DataVal);
	op_assert(result == 0);
#endif
}


//process any events that have happened with the endpoints
bool processEndpoints(bool block)
{
	struct timeval timeout;
	long nfds = 0;
	long numEvents;
	bool gotEvent = false;
	NMEndpointPriv *theEndPoint;
	fd_set input_set, output_set, exc_set; //create input,output,and error sets for the select function
	uint32 listStartState;
			
	// if block is true, we wait one second - otherwise not at all
	// we wait a max of one second because we have to check every once in a while to see if we need to terminate our thread
	// or add new endpoints to watch
	// ECF - changed this to 10 seconds in the debug version so as to identify unnecessary delays
	// (sufficient machinery is in place now so we never should need to spin waiting for the duration of a select() call)
	if (block)
	{
		#if (DEBUG)
			timeout.tv_sec = 10;
		#else
			timeout.tv_sec = 1;
		#endif
		timeout.tv_usec = 0;
	}
	else
	{
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
	}
	
	//set up the sets
	FD_ZERO(&input_set);
	FD_ZERO(&output_set);
	FD_ZERO(&exc_set);

	//so we can abort if the list changes while we're using it
	listStartState = endpointListState;
	
	//dont hog the proc if we cant get the list right now
	if (TRY_LOCK_ENDPOINT_WAITING_LIST() == false)
	{
		if (!block)
			return false;
		usleep(10000); //sleep for 10 millisecs
		return false;
	}

	if (TRY_LOCK_ENDPOINT_LIST() == false)
	{
		UNLOCK_ENDPOINT_WAITING_LIST();
		if (!block)
			return false;
		usleep(10000); //sleep for 10 millisecs
		return false;
	}
	else
		UNLOCK_ENDPOINT_WAITING_LIST();
	
	//if we have a wake endpoint, add it
	if (wakeHostSocket)
		FD_SET(wakeHostSocket,&input_set);
	
	//add all endpoints to our lists to check
	theEndPoint = endpointList;
			
	while (theEndPoint != NULL)
	{
		//first off, if the endpoint needs to die, start it dying.  
		if ((theEndPoint->needToDie == true) && (theEndPoint->dying == false))
		{
			theEndPoint->dying = true;

			//only inform them of its demise if its been fully formed
			if (theEndPoint->alive == true)
			{
				UNLOCK_ENDPOINT_LIST(); //they'll probably kill the endpoint (and thus modify the list) as a result
				//tell the player the endpoint died and make sure we don't do anything else with it
				theEndPoint->callback(theEndPoint,theEndPoint->user_context,kNMEndpointDied,0,NULL);
				LOCK_ENDPOINT_LIST();
			}
		}
		
		//if the list has changed, we need to abort and do it next time
		if (listStartState != endpointListState)
		{
			UNLOCK_ENDPOINT_LIST();
			return true;
		}
		
		//we always check for waiting input and errors
		//we only look for output ability if our flow is blocked and we therefore need to see when we can send again
		if (theEndPoint->connectionMode & (1 << _datagram_socket)){
			FD_SET(theEndPoint->sockets[_datagram_socket],&input_set);
			if (theEndPoint->flowBlocked[_datagram_socket])
				FD_SET(theEndPoint->sockets[_datagram_socket],&output_set);
			FD_SET(theEndPoint->sockets[_datagram_socket],&exc_set);
			if (theEndPoint->sockets[_datagram_socket] + 1 > nfds)
    			nfds = theEndPoint->sockets[_datagram_socket] + 1;			
		}
		if (theEndPoint->connectionMode & (1 << _stream_socket)){
			FD_SET(theEndPoint->sockets[_stream_socket],&input_set);
			//only look for output if our flow is blocked
			if (theEndPoint->flowBlocked[_stream_socket])
				FD_SET(theEndPoint->sockets[_stream_socket],&output_set);
			FD_SET(theEndPoint->sockets[_stream_socket],&exc_set);
			if (theEndPoint->sockets[_stream_socket] + 1 > nfds)
    			nfds = theEndPoint->sockets[_stream_socket] + 1;						
		}
		theEndPoint = theEndPoint->next;
	}
	
	//check for events
	if (endpointList)
		numEvents = select(nfds,&input_set,&output_set,&exc_set,&timeout);
	else
		numEvents = 0; //save ourselves some time with good ol' common sense!

	// Look Through all the endpoints to see which ones have news for us
	if (numEvents > 0)
    {
    	gotEvent = true; //something came in
		theEndPoint = endpointList;
		
		//first check our wake socket
		if (wakeHostSocket){
			char buffer[64];
			if (FD_ISSET(wakeHostSocket,&input_set)){
				long amountIn = recv(wakeHostSocket,buffer,sizeof(buffer),0);
				DEBUG_PRINT("wakeHostSocket amountIn: %d",amountIn);
			}
		}
		while (theEndPoint)
		{
			//this endpoint has a datagram socket
			if (theEndPoint->connectionMode & (1 << _datagram_socket))
				processEndPointSocket(theEndPoint,_datagram_socket,&input_set,&output_set,&exc_set);			

			//abort if the list has been changed since theEndPoint may no longer exist for all we know...
			if (endpointListState != listStartState)
			{
				break;
			}
				
			//if it has a stream socket
			if (theEndPoint->connectionMode & (1 << _stream_socket))
				processEndPointSocket(theEndPoint,_stream_socket,&input_set,&output_set,&exc_set);
			
			//abort if the list has been changed since theEndPoint may no longer exist for all we know...
			if (endpointListState != listStartState)
				break;

			theEndPoint = theEndPoint->next;
		}
	}
	
	UNLOCK_ENDPOINT_LIST();
	
	//if we arent getting events, we dont wanna sit in a spin-lock, hogging the endpoint list
	if (gotEvent == false)
	{
		usleep(10000); //sleep for 10 millisecs
	}
	return gotEvent;
}

//this function is always called with access to a locked endpoint-list, remember. And it should always return a locked list.
void processEndPointSocket(NMEndpointPriv *theEndPoint, long socketType, fd_set *input_set, fd_set *output_set, fd_set *exc_set)
{
	//cant do nothing if they've called ProtocolEnterNotifier
	if (TRY_ENTER_NOTIFIER() == false)
		return;

	if (FD_ISSET(theEndPoint->sockets[socketType],exc_set)) // There was an Error?
	{
		uint32 listStartState = endpointListState;
		//if we're already dying, theres nothing to be done...
		if (theEndPoint->dying == false)
		{
			DEBUG_PRINT("got an exception for EP 0x%x",theEndPoint);
			if (theEndPoint->alive == true)
			{
				//todo: is there something that would end up here that's not fatal?
			
				//tell the player the endpoint died and make sure we don't do anything else with it
				theEndPoint->needToDie = true;
				theEndPoint->dying = true;
				UNLOCK_ENDPOINT_LIST();
				theEndPoint->callback(theEndPoint,theEndPoint->user_context,kNMEndpointDied,0,NULL);
				LOCK_ENDPOINT_LIST();
				
				//if an endpoint was added or removed, we can't go on (it might have been us)
				if (listStartState != endpointListState){
					LEAVE_NOTIFIER();
					return;
				}
			}
			else//if the endpoint is just being created, set it up to return an error.
			{
				theEndPoint->opening_error = kNMOpenFailedErr;
				theEndPoint->needToDie = true;
				LEAVE_NOTIFIER();
				return;
			}
		}
	}

	if ((FD_ISSET(theEndPoint->sockets[socketType],input_set))) // Data has arrived
	{
		uint32 listStartState = endpointListState;
			
		//if the endpoint is dying, we don't care....
		//(we might, though, in the future, if we're doing "graceful" disconnects or something)
		if (theEndPoint->dying == false)
		{
			NMCallbackCode code;
			
			//if this is a listener-endpoint, we need to deal with the new connection
			//ask the user to accept or reject
			if ((socketType == _stream_socket) && (theEndPoint->listener == true))
			{
				//only dish this out if we're alive
				if (theEndPoint->alive == true)
				{
					DEBUG_PRINT("sending user a connect request");
					UNLOCK_ENDPOINT_LIST();
					theEndPoint->callback(theEndPoint,theEndPoint->user_context,kNMConnectRequest,0,NULL);
					LOCK_ENDPOINT_LIST();
					//if an endpoint was added or removed, we can't go on (it might have been us)
					if (listStartState != endpointListState){
						LEAVE_NOTIFIER();
						return;
					}
				}
			}
			//looks like its data coming in on a normal endpoint. see if we need to handle it internally
			//(non-nspmode connections use a "confirmation" packet at the beginning of a connection)
			else if (internally_handle_read_data(theEndPoint,socketType) == false)
			{
				//only dish out if we're alive
				if (theEndPoint->alive)
				{
					//do a test-read without actually pulling data out
					long readResult = socketReadResult(theEndPoint,socketType);
					
					//first off we check the data to see if its of zero length - this implies
					//that the endpoint has died, in which case we deliver that news instead of the new-data
					if ( readResult <= 0)
					{
						if (theEndPoint->dying == false) //if we havn't delivered the news, do it
						{
							//tell the player the endpoint died and make sure we don't do anything else with it
							theEndPoint->needToDie = true;
							theEndPoint->dying = true;
							UNLOCK_ENDPOINT_LIST();
							DEBUG_PRINT("sending kNMEndpointDied for ep 0x%x",theEndPoint);
							theEndPoint->callback(theEndPoint,theEndPoint->user_context,kNMEndpointDied,0,NULL);
							LOCK_ENDPOINT_LIST();
							
							LEAVE_NOTIFIER();
							return;
						}					
					}
					
					//we only tell them of new data once until they retrieve it
					if (theEndPoint->newDataCallbackSent[socketType] == false)
					{
						theEndPoint->newDataCallbackSent[socketType] = true;
					
						//tell the user that data has arrived
						if (socketType == _stream_socket)
						{
							DEBUG_PRINT("sending kNMStreamData callback to 0x%x",theEndPoint);
							code = kNMStreamData;
						}
						else
						{
							DEBUG_PRINT("sending kNMDatagramData callback to 0x%x",theEndPoint);
							code = kNMDatagramData;
						}
							
						//hand it off to the user to read or whatever
						UNLOCK_ENDPOINT_LIST();
						theEndPoint->callback(theEndPoint,theEndPoint->user_context,code,0,NULL);
						LOCK_ENDPOINT_LIST();
						
						//if an endpoint was added or removed, we can't go on (it might have been us)
						if (listStartState != endpointListState){
							LEAVE_NOTIFIER();
							return;
						}
					}
				}
				//if we're not yet alive, we at least see if the endpoint has died so we can fail in opening
				//otherwise we'd wind up spinning until the open timed out
				else
				{
					//if they killed us off before we could finish opening, just assume it was a rejection
					if (socketReadResult(theEndPoint,_stream_socket) <= 0)
					{
						theEndPoint->opening_error = kNMAcceptFailedErr;
						theEndPoint->needToDie = true;
						LEAVE_NOTIFIER();
						return;
					}
				}
			}
		}
	}
	
	if (FD_ISSET(theEndPoint->sockets[socketType],output_set)) // Socket is ready to write
	{
		uint32 listStartState = endpointListState;
	
		//if its dying, we dont care
		if (theEndPoint->dying == false)
		{
			//if this socket hasnt been declared valid yet, we do so
			//if it already was, there must have been a flow control problem and we need to send
			//out a flow clear message (unless its a listener, which doesnt send)
			theEndPoint->flowBlocked[socketType] = false;
			UNLOCK_ENDPOINT_LIST();
			if (theEndPoint->valid_endpoints & (1 << socketType)) //this endpoint's been marked valid already - this must be flow clear
			{
				DEBUG_PRINT("sending 0x%x a flowclear for socketType %d.",theEndPoint,socketType);
				DEBUG_PRINT("datagram blocked: %d",theEndPoint->flowBlocked[_datagram_socket]);
				DEBUG_PRINT("datagram valid: %d",theEndPoint->valid_endpoints & (1 << _datagram_socket));
				theEndPoint->callback(theEndPoint,theEndPoint->user_context,kNMFlowClear,0,NULL);
			}
			else
			{
				bool setValid = true;

				//if we're active and in netsprocket mode, we set our remote datagram address based on the stream socket.  We have to use it every time we
				//send a datagram, since we can't just connect() to it (because we send and receive to different ports and that wouldnt work)
				//if we're an active, non-netsprocket-mode stream socket coming online, we send our datagram address
				if ((socketType == _stream_socket) && (theEndPoint->active == true))
				{
					{
						NMErr err;
						posix_size_type	size  = sizeof(theEndPoint->remoteAddress);

						DEBUG_PRINT("setting remote datagram address");
						err = getpeername(theEndPoint->sockets[_stream_socket], (sockaddr*) &theEndPoint->remoteAddress,(socklen_t*) &size);
						
						//if we got a not-connected error, it means they dumped us real quick. we die.
						if (err)
						{
							setValid = false;
							DEBUG_NETWORK_API("getpeername",err);
						}
						//if there was no error and we're not in nspmode, ship off our datagram port
						else if (!theEndPoint->netSprocketMode)
						{
							_send_datagram_socket(theEndPoint);
						}
					}
				}
				
				//if its a remote-client connection and the stream-socket is ready to go, we connect our datagram socket and
				//send off our "ready" callbacks.  If we're in NSpMode, we use this opportunity to assign our datagram port too
				if ((socketType == _stream_socket) && (theEndPoint->active == false) && (theEndPoint->listener == false))
				{
					op_assert(theEndPoint->parent != NULL);
					
					//this signals that messages can start flowin' in
					theEndPoint->alive = true;		
			
					// And aim our datagrams at their port
					if (theEndPoint->netSprocketMode)
					{
						NMErr result;
						sockaddr_in address;
						posix_size_type	address_size= sizeof (address);
					
						getpeername(theEndPoint->sockets[_stream_socket], (sockaddr*) &address, (socklen_t*)&address_size);
						DEBUG_PRINT("connecting datagram socket to port %d",ntohs(address.sin_port));						
						result = connect(theEndPoint->sockets[_datagram_socket],(sockaddr*)&address,sizeof(address));
						DEBUG_NETWORK_API("connect",result);
					}
					//send a handoff-complete to its parent and an accept-complete to it
					theEndPoint->callback(theEndPoint, theEndPoint->user_context, kNMAcceptComplete, 0, theEndPoint->parent);
					theEndPoint->parent->callback(theEndPoint->parent, theEndPoint->parent->user_context, kNMHandoffComplete, 0, theEndPoint);
				}
				
				if (setValid)
				{
					if (socketType == _datagram_socket)
						DEBUG_PRINT("datagram socket ready to write for 0x%x",theEndPoint);
					else	
						DEBUG_PRINT("stream socket ready to write for 0x%x",theEndPoint);
											
					MARK_ENDPOINT_AS_VALID(theEndPoint,socketType);
				}
				else
				{
					if (socketType == _datagram_socket)
						DEBUG_PRINT("datagram socket completion failed for 0x%x",theEndPoint);
					else	
						DEBUG_PRINT("stream socket completion failed for 0x%x",theEndPoint);				
				}
			}
			LOCK_ENDPOINT_LIST();

			//if an endpoint was added or removed, we can't go on (it might have been us)
			if (listStartState != endpointListState){
				LEAVE_NOTIFIER();
				return;
			}
		}
	}
	LEAVE_NOTIFIER();
	return;
}


void createWorkerThread(void)
{
	dieWorkerThread = false;
	
	//if we've already got a worker thread...
	if (workerThreadAlive)
		return;
		
	DEBUG_PRINT("creating network worker-thread...");	

	#ifdef OP_API_NETWORK_SOCKETS
		long pThreadResult = pthread_create(&worker_thread,NULL,worker_thread_func,NULL);
		op_assert(pThreadResult == 0);
	#elif defined(OP_API_NETWORK_WINSOCK)
		worker_thread_handle = CreateThread((_SECURITY_ATTRIBUTES*)NULL,
								0,
								worker_thread_func,
								0, //context
								0,
								&worker_thread);
		op_assert(worker_thread_handle != NULL);
	#endif
	
	workerThreadAlive = true;
}

void killWorkerThread(void)
{
	if (workerThreadAlive == false)
		return;
	
	DEBUG_PRINT("terminating worker-thread...");	
	
	// on windows, we simply annihilate the thread in its tracks,
	// as it seems unable to execute, and thus shut itself down, during DllMain().
	// this is a rather unelegant way to do it...
	#ifdef OP_API_NETWORK_WINSOCK
		BOOL result = 	TerminateThread(worker_thread_handle,0);
		op_assert(result != 0);
		dieWorkerThread = true;
		workerThreadAlive = false;
		return;
	#endif
			
	dieWorkerThread = true;
	
	//snap the worker thread out of its select() call
	sendWakeMessage();
	
	//wait while it dies
	while (workerThreadAlive == true)
	{
		usleep(10000); //sleep for 10 millisecs
	}
	DEBUG_PRINT("...worker thread terminated.");			
}

// the main function for our worker thread, which just sits and waits for socket events to happen
#if defined(OP_API_NETWORK_WINSOCK)
	DWORD WINAPI worker_thread_func(LPVOID arg)
#else
	void* worker_thread_func(void *arg)
#endif
{

	bool done = false;

	DEBUG_PRINT("worker_thread is now running");
	workerThreadAlive = true;
	
	//sit in a loop blocking until new stuff happens
	while (!done)
	{
		processEndpoints(true);
		if (dieWorkerThread)
			done = true;
	}
	
	DEBUG_PRINT("worker-thread shutting down");	
	workerThreadAlive = false;
    pthread_exit(0);
	return NULL;
}

//----------------------------------------------------------------------------------------
// receive_udp_port
//----------------------------------------------------------------------------------------

void
receive_udp_port(NMEndpointRef endpoint)
{
	udp_port_struct_from_server	port_data;
	int32	err;
	uint32	size= sizeof(port_data);
	uint32	flags;

	DEBUG_ENTRY_EXIT("receive_udp_port");
	
	//first off, theres a chance that they disconnected before sending
	if (socketReadResult(endpoint,_stream_socket) <= 0)
	{
		endpoint->opening_error = kNMAcceptFailedErr;
		endpoint->needToDie = true;
		return;
	}
	// Read the port...
	err = _receive_data(endpoint, _stream_socket, (unsigned char *)&port_data, &size, &flags);
	if (!err && size==sizeof (port_data))
	{
		int16 	old_port;
		sockaddr_in address;
		posix_size_type	address_size= sizeof (address);

		op_assert(size==sizeof (port_data)); // or else
		
		// And change our local port to match theirs..
		getpeername(endpoint->sockets[_datagram_socket], (sockaddr*) &address, (socklen_t*)&address_size);
		old_port= address.sin_port;
		address.sin_port= port_data.port; // already in network order
		{
			long port = ntohs(address.sin_port);
			DEBUG_PRINT("received remote udp port: %d",port);
		}
		
		// make the connect call.. (sets the udp destination)
		err= connect(endpoint->sockets[_datagram_socket], (sockaddr*) &address, sizeof (address));
		if (!err)
		{
			// and mark the connection as having been completed...
			MARK_ENDPOINT_AS_VALID(endpoint, _datagram_socket);
		}
		else
			DEBUG_NETWORK_API("connect()", err);
	}
	//if we got a read of size 0, they disconnected before sending it (most likely a rejection)
	else
	{
		DEBUG_PRINT("error receiving udp port");	
		endpoint->opening_error = kNMAcceptFailedErr;
		endpoint->needToDie = true;
	}
}

long socketReadResult(NMEndpointRef endpoint, int socketType)
{
	int socket = endpoint->sockets[socketType];
	char buffer[4];
	long result;
	
	//we read the first bit of data in the socket (in peek mode) to determine if the socket has died
	result = recvfrom(socket,(char*)buffer,sizeof(buffer),MSG_PEEK,NULL,NULL);
	
	//if its an error, go ahead and read it for real
	if (result == -1)
	{
		//windows complains if the buffer isnt big enough to fit the datagram. i dont think any unixes do.
		//this of course is not a problem
#ifdef HACKY_EAGAIN
		if ( (op_errno == EMSGSIZE) || (op_errno == EAGAIN) )
#else
		if (op_errno == EMSGSIZE)
#endif // HACKY_EAGAIN
			return 4; //pretend we read it and go home happy
		DEBUG_NETWORK_API("socketReadResult",result);
		return recvfrom(socket,(char*)buffer,sizeof(buffer),0,NULL,NULL);
	}
	return result;
}

#ifdef OP_API_NETWORK_WINSOCK
void initWinsock(void)
{
	DEBUG_ENTRY_EXIT("initWinsock");
	NMErr err;
	// Here we make connection with the Winsock library for the windows build
	// this can cause problems if we do it when loading the library
	if (winSockRunning == false)
	{
		WSADATA		aWsaData;
		int16	aVersionRequested= MAKEWORD(1, 1);
		err = WSAStartup(aVersionRequested, &aWsaData);
		if (! err)
		{
			// Make sure they support our version (will not fail with newer versions!)
			if (LOBYTE(aWsaData.wVersion) != LOBYTE(aVersionRequested) || HIBYTE(aWsaData.wVersion) != HIBYTE(aVersionRequested))
			{
				WSACleanup();
				DEBUG_PRINT("Version bad! (%d.%d) != %d.%d", HIBYTE(aWsaData.wVersion), LOBYTE(aWsaData.wVersion),
								HIBYTE(aVersionRequested), LOBYTE(aVersionRequested));
			}
			else
				winSockRunning = true;
		}
		else
		{
			DEBUG_PRINT("Unable to startup winsock!");
		}
	}
}

void shutdownWinsock(void)
{
	DEBUG_ENTRY_EXIT("shutdownWinsock");

	if (winSockRunning)
	{
		winSockRunning = false;
		WSACleanup();
	}
}
#endif

//----------------------------------------------------------------------------------------
// is_ip_request_packet
//----------------------------------------------------------------------------------------

bool
is_ip_request_packet(
	char		*buffer, 
	uint32	length,
	NMType		gameID)
{
  bool  is_request = false;

	if (length == kQuerySize)
	{
	uint16		index;
	uint32	*query_flag = (uint32 *) buffer;
	uint32	query_value = kQueryFlag;
	
		query_value = BigEndian32_ToNative(query_value);
		
		for (index = 0, query_flag = (uint32 *) buffer; 
			index<kQuerySize / sizeof (uint32); 
			++index, query_flag++)
		{
			if (*query_flag != query_value)
				break;
		}
		
		if (index == kQuerySize / sizeof (uint32))
			is_request = true;
	}
	
	return is_request;
}

//----------------------------------------------------------------------------------------
// build_ip_enumeration_response_packet
//----------------------------------------------------------------------------------------

int32
build_ip_enumeration_response_packet(
	char		*buffer, 
	NMType		gameID, 
	uint32	version, 
	uint32	host, 
	uint16	port, 
	char		*name, 
	uint32	custom_data_length,
	void		*custom_data)
{
IPEnumerationResponsePacket	*packet = (IPEnumerationResponsePacket *) buffer;
int32  				length = sizeof (IPEnumerationResponsePacket);
	
	packet->responseCode = kReplyFlag;
	packet->gameID = gameID;
	packet->host = host;
	packet->port = port;
	packet->version = version;
	machine_copy_data(name, packet->name, kMaxGameNameLen + 1);
	packet->name[kMaxGameNameLen] = 0;
	packet->customEnumDataLen = custom_data_length;

	if (custom_data_length)
	{
		machine_copy_data(custom_data,(char *) packet + sizeof (IPEnumerationResponsePacket), custom_data_length);

		length += custom_data_length;
	}
	
	return length;
}

//----------------------------------------------------------------------------------------
// byteswap_ip_enumeration_packet
//----------------------------------------------------------------------------------------

void
byteswap_ip_enumeration_packet(char *buffer)
{
	IPEnumerationResponsePacket	*packet = (IPEnumerationResponsePacket *) buffer;
	
	packet->responseCode = BigEndian32_ToNative(packet->responseCode);
	packet->gameID = BigEndian32_ToNative(packet->gameID);
	packet->version = BigEndian16_ToNative(packet->version);
	packet->host = BigEndian32_ToNative(packet->host);
	packet->port = BigEndian16_ToNative(packet->port);
	packet->pad = BigEndian16_ToNative(packet->pad);
	packet->customEnumDataLen = BigEndian32_ToNative(packet->customEnumDataLen);
}



static short _get_default_port(uint32 GameID)
{
	DEBUG_ENTRY_EXIT("_get_default_port");

  return((GameID % (32760 - 1024)) + 1024);
}



#endif // !PDG_NO_NETWORK

