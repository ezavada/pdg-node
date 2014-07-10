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

#include "network-openplay.h"

// include the cursor management stuff if we
// are doing a GUI version
#ifndef PDG_NO_GUI
    #include "internals.h"
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
        	    mOffsetInPacket = PACKET_DATA_OFFSET;
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
                    nd.dataLen = mInProgressPacket->packetLen - PACKET_DATA_OFFSET;
                    nd.dataPtr = (char*)mInProgressPacket + PACKET_DATA_OFFSET;
                    nd.arrivalMs = OS::getMilliseconds();
                    {
                        AutoMutex mutex(&mDataPtrMapMutex);
                        mDataPtrMap.insert(PacketDataMap::value_type((void*)nd.dataPtr, mInProgressPacket));
                    }
                    evtMgr->enqueueEvent(eventType_NetData, &nd, sizeof(NetData));
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
            void* ptr = (void*)((char*)inPacket+PACKET_DATA_OFFSET);
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
                inPacket->packetLen -= PACKET_DATA_OFFSET;
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
            void* ptr = (void*)((char*)p+PACKET_DATA_OFFSET);
            sentBytes = ProtocolSend(mEndpoint, ptr, packetLen, 0);
          #ifdef PDG_NET_DEBUG_PACKET_DUMP
            if (sentBytes > 0) {
                char buf[4096];
                OS::binaryDump(buf, 4096, (const char*)ptr, sentBytes);
                NET_DEBUG_ONLY( OS::_DOUT("Sent on connection [%ld]:\n%s", mId, buf); )
            }
          #endif
            p->packetLen -= PACKET_DATA_OFFSET;
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
            FIXME("Need to handle partial send");
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
            evtMgr->enqueueEvent(eventType_NetConnect, &netConnectEvent, sizeof(NetConnect)); // enqueue so main thread can pick it up
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
                evtMgr->enqueueEvent(eventType_NetError, &netErrEvent, sizeof(NetError)); // enqueue so main thread can pick it up
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
            evtMgr->enqueueEvent(eventType_NetDisconnect, &netDiscoEvent, sizeof(NetDisconnect)); // enqueue so main thread can pick it up
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
    NMErr err = ProtocolGetEndpointIdentifier(mEndpoint, mName, PDG_MAX_NET_CONNECTION_NAME_LEN);
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
//        setHardwareBusyCursor();
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
    char config_string[1024];
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
    std::snprintf(config_string, 1024, "IPaddr=%s\tIPport=%d\tmode=%d", hostname, port, mode);
    MAKE_STRING_BUFFER_SAFE(config_string, 1024); 
    NMType connectType = kIPModuleType;
    err = ProtocolCreateConfig(connectType, mConnectionCreator, mPublicName.c_str(), NULL, 0, config_string, &config);
	if (err != kNMNoError) {
        DEBUG_ONLY( OS::_DOUT("ERROR: ProtocolCreateConfig returned [%d] for config [%s]", err, config_string); )
        failed = true; // this is fatal
    } else { // log success
//    	char config_string[1024];
        ProtocolGetConfigString(config, config_string, 1024);
        NET_DEBUG_ONLY( OS::_DOUT("openConnection made config [%s]", config_string); )
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
        mEventMgr->enqueueEvent(eventType_NetConnect, &netConnectEvent, sizeof(NetConnect)); // enqueue so main thread can pick it up
    } else {
        NetError netErrEvent;
        netErrEvent.id = 0; // no id established yet
        netErrEvent.context = userContext;
        netErrEvent.error = err;
        mEventMgr->enqueueEvent(eventType_NetError, &netErrEvent, sizeof(NetError)); // enqueue so main thread can pick it up
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
            char* p = (char*)packet + PACKET_DATA_OFFSET;
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
    char config_string[1024];

    const char* hostname = "127.0.0.1";
    NMEndpointMode mode = kNMNormalMode;
    if (connectFlags & flag_TCPOnly) {
        mode = kNMStreamMode;
    } else if (connectFlags & flag_UDPOnly) {
        mode = kNMDatagramMode;
    }
    if (connectFlags & flag_NoHandshake) {
        std::snprintf(config_string, 1024, "netSprocket=true\tIPaddr=%s\tIPport=%d\tmode=%d", hostname, portNum, mode);
    } else {
        std::snprintf(config_string, 1024, "IPaddr=%s\tIPport=%d\tmode=%d", hostname, portNum, mode);
    }
    MAKE_STRING_BUFFER_SAFE(config_string, 1024); 

    NMErr saveErr = kNMNoError;
    NMType connectType = kIPModuleType;
    NMErr err = ProtocolCreateConfig(connectType, mConnectionCreator, mPublicName.c_str(), NULL, 0, config_string, &opconfig);
    if (err != kNMNoError) {
        DEBUG_ONLY( OS::_DOUT("ERROR: OpenPlay ProtocolCreateConfig returned [%d] for config [%s]\n", err, config_string); )
        saveErr = err;
      #ifdef DEDICATED_SERVER
        mHostLog << util::log::category("HOST") << util::log::fatal
        << "OpenPlay CreateConfig ["<<config_string<<"] failed with error ["<< err <<"]"
        << util::endlog;
        throw err; // a dedicated server can't do anything more here
      #endif
    } else {
        PEndpointRef opep = 0;  // OpenPlay endpoint
        ProtocolGetConfigString(opconfig, config_string, 1024);
        NET_DEBUG_ONLY( OS::_DOUT("Creating Listener with config [%s]\n", config_string); )
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
        mEventMgr->enqueueEvent(eventType_NetError, &netErrEvent, sizeof(NetError)); // enqueue so main thread can pick it up
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
                netMgr->getEventManager()->enqueueEvent(eventType_NetError, &netErrEvent, sizeof(NetError)); // enqueue so main thread can pick it up
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
    setHardwareNormalCursor();
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

#endif // !PDG_NO_NETWORK

