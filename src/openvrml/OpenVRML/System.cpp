// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  System.cpp
//  A class to contain system-dependent/non-standard utilities
//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "System.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

# ifndef NDEBUG
#   define SYSTEM_DEBUG
# endif

using namespace OpenVRML;

// A default System object
static System defaultSystem;

// The global System object
System * OpenVRML::theSystem = &defaultSystem;


// Should make these iostream objects...

void System::error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

void System::warn(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr,"Warning: ");
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}


// Write to the browser status line...

void System::inform(const char *fmt, ...)
{
    static char lastbuf[1024] = { 0 };
    char buf[1024];

    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    if (strcmp(lastbuf, buf)) {
        fprintf(stderr,"%s", buf);
        putc('\n', stderr);
        strcpy(lastbuf, buf);
    }
}

#ifdef SYSTEM_DEBUG
void System::debug(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
#else
void System::debug(const char *, ...)
{
#endif
}

#ifdef macintosh

#include <Types.h>
#include <InternetConfig.h>

bool System::loadUrl(char *url, int np, char **parameters )
{
    // We ignore np and parameters and just pass the URL

    static OSStatus err = noErr;
    ICInstance inst;
    long startSelection;
    long endSelection;

    if ( err != noErr || url == NULL )
      return false;

    err = ICStart(&inst, 'vrVW');  // vrVW is MacLookAt's creator code...
    if (err == noErr) {
        err = ICFindConfigFile(inst, 0, nil);
        if (err == noErr) {
            startSelection = 0;
            endSelection   = (long)strlen(url);
            err = ICLaunchURL(inst, "\p", url,
                    endSelection, &startSelection, &endSelection);
        }
        else
          theSystem->warn("Could not locate Internet configuration file\n");

        (void)ICStop(inst);
    }
    else
      theSystem->warn("InternetConfig 2.x or the Internet control panel are missing\n");
    return (err == noErr ? true : false);
}
#else

// This won't work under windows or if netscape isn't running...

bool System::loadUrl(const std::string & url, const MFString & parameters)
{
    return false;
}

// added for working under windows (and is not needed for mac os)...
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <sys/types.h>
#include <winsock2.h>
#include <string.h>
#include <ctype.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>        // memset
#include <ctype.h>
#endif // _WIN32

#endif // macintosh

int System::connectSocket(const char * host, int port)
{
#ifdef macintosh
    return -1;
#else
    hostent * he;

#if defined(_WIN32) && !defined(__CYGWIN__)
    WSADATA wsaData;
    WORD wVersionRequested;
#endif

    int sockfd = -1;

#if defined(_WIN32) && !defined(__CYGWIN__)
    wVersionRequested = MAKEWORD(1, 0);
    if (WSAStartup(wVersionRequested,&wsaData) == SOCKET_ERROR) {
        theSystem->error("WSAStartup failed with error %d\n",WSAGetLastError());
        WSACleanup();
        return -1;
    }
#endif
    sockaddr_in sin = { AF_INET, htons(port) };

    // Check for dotted number format
    char *s;
    for (s=(char*)host; *s; ++s) {
        if (!(isdigit(*s) || *s == '.')) { break; }
    }

    if (*s) { // Not dotted number
        he = gethostbyname(host);
    } else {
        u_long addr = inet_addr(host);
        he = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
    }

    if (he) {
        std::copy(he->h_addr, he->h_addr + he->h_length, (char*)&sin.sin_addr);
        sockfd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
        if (sockfd != -1) {
              if (connect(sockfd, (sockaddr *)&sin, sizeof(sin)) == -1) {
#if defined(_WIN32) && !defined(__CYGWIN__)
                  closesocket(sockfd);
                  WSACleanup();
#else
                  close(sockfd);
#endif
                  sockfd = -1;
               }
        }
    }

    return sockfd;
#endif
}


#include <fcntl.h>        // open() modes
#if defined(_WIN32) && !defined(__CYGWIN__)
# include <io.h>
#else
# include <unistd.h>
#endif

#include <errno.h>

const char * System::httpHost(const char * url, int * port)
{
    static char hostname[256];
    const char *s = strstr(url,"//");
    char *p = hostname;

    if (s) {
        for (s+=2; *s && *s != '/' && *s != ':'; *p++ = *s++) {}
        if (*s == ':' && isdigit(*(s + 1))) { *port = atoi(s + 1); }
    }
    *p = '\0';
    return hostname;
}

// This isn't particularly robust or complete...

#ifndef macintosh

const char *System::httpFetch(const char * url)
{
    int port = 80;
    const char *hostname = httpHost(url, &port);

    if (port == 80) {
        theSystem->inform("Connecting to %s ...", hostname);
    } else {
        theSystem->inform("Connecting to %s:%d ...", hostname, port);
    }

    int sockfd;
    if ((sockfd = System::connectSocket( hostname, port )) != -1) {
        theSystem->inform("connected.");
    } else {
#if defined(_WIN32) && !defined(__CYGWIN__)
        theSystem->warn("Connect failed:  (errno %d)\n",
        WSAGetLastError());
        WSACleanup();
# else
        theSystem->warn("Connect failed: %s (errno %d).\n",
                        strerror(errno), errno);
#endif
    }
    // Copy to a local temp file
    char *result = 0;
    if (sockfd != -1 && (result = tmpnam("VR"))) {
#if defined(_WIN32) && !defined(__CYGWIN__)
        int fd = _open(result, _O_BINARY | _O_RDWR | _O_CREAT, 0777);
#else
        int fd = open(result, O_RDWR|O_CREAT, 0777);
#endif
        if (fd != -1) {
            char *abspath = strstr((char *)url, "//");
            if (abspath) { abspath = strchr(abspath+2, '/'); }
            if (!abspath) { abspath = (char*) url; }

            char request[1024];
            sprintf(request,"GET %s HTTP/1.0\nAccept: */*\n\r\n", abspath);

            int nbytes = strlen(request);
#if defined(_WIN32) && !defined(__CYGWIN__)
            if (send(sockfd, request, nbytes, 0) != nbytes) {
#else
            if (write(sockfd, request, nbytes) != nbytes) {
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
                theSystem->warn("http GET failed:  (errno %d)\n",
                WSAGetLastError());
                WSACleanup();
#else
                theSystem->warn("http GET failed: %s (errno %d)\n",
                                strerror(errno), errno);
#endif
            } else {
                int gothdr = 0, nread = 0, nwrote = 0, nmore;
                char *start;
#if defined(_WIN32) && !defined(__CYGWIN__)
                while ((nmore = recv(sockfd, request, sizeof(request)-1,0)) > 0) {
#else
                while ((nmore = read(sockfd, request, sizeof(request)-1)) > 0) {
#endif
                    nread += nmore;

                    // Skip header (should read return code, content-type...)
                    if (gothdr) {
                        start = request;
                    } else {
                        start = strstr(request, "\r\n\r\n");
                        if (start) {
                            start += 4;
                        } else {
                            start = strstr(request, "\n\n");
                            if (start) { start += 2; }
                        }
                        if (!start) { continue; }
                        gothdr = 1;
                    }

                    nmore -= (start - request);
# if defined(_WIN32) && !defined(__CYGWIN__)
                    if (_write(fd, start, nmore) != nmore) {
# else
                    if (write(fd, start, nmore) != nmore) {
# endif
                        theSystem->warn("http: temp file write error\n");
                        break;
                    }
                    nwrote += nmore;
                }

                theSystem->inform("Read %dk from %s", (nread+1023)/1024, url);
            }

# if defined(_WIN32) && !defined(__CYGWIN__)
            _close(fd);
# else
            close(fd);
# endif
        }
    }

    if (sockfd != -1) {
#if defined(_WIN32) && !defined(__CYGWIN__)
        closesocket(sockfd);
        WSACleanup();
    }
#else
        close(sockfd);
    }
#endif
    return result;
}

#else // Mac code for using OpenTransport to retrieve a URL

// This code contains (unrestricted) sample code from Apple

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <Files.h>
#include <TextUtils.h>
#include <OpenTransport.h>
#include <OpenTptInternet.h>
#include <Threads.h>

#ifdef __MWERKS__
#include <sioux.h>
#endif

static UInt32 gLastUpdated = 0;
static bool   gAbortFetch  = false;

extern pascal void OTDebugStr(const char* str);

enum {
      kTransferBufferSize = 4096
};

enum {
      kDownloadSimple,
      kDownloadFaster
};

#define kWatchCursor 1007
#define kAnimatedBusyCursor 30000
#define kNumberOfAnimatedBusyCursors 7


char * strdup(const char * string)
{
      char *newstr = (char*)malloc(strlen(string) + 1);
      if (!newstr) { return 0; }
      strcpy(newstr, string);
      return newstr;
}

inline void c2str255(const char * cs, Str255 ps)
{
    int i = 255;
    *ps++ = strlen(cs);
    while(*cs && i--) { *ps++ = *cs++; }
}

static pascal void YieldingNotifier(void * contextPtr,
                                    OTEventCode code,
                                    OTResult result,
                                    void* cookie)
{
#pragma unused(contextPtr)
#pragma unused(result)
#pragma unused(cookie)
    switch (code) {
    case kOTSyncIdleEvent:
        YieldToAnyThread();
        break;
    default:
        break;
    }
}

static pascal OSStatus ProgressThread(void * junkParam)
{
#pragma unused(junkParam)

    // This function assumes that either a cursor resource with id kWatchCursor
    // or a set of kNumberOfAnimatedBusyCursors cursor ids from kAnimatedBusyCursor
    // exist:

    Boolean gotEvent;
#ifdef MAC_ANIMATED_BUSY_CURSOR
    int i = 0;
#endif
    while (true) {
        if (TickCount() > (gLastUpdated + 15)) {
#ifdef MAC_ANIMATED_BUSY_CURSOR
            SetCursor(*(GetCursor(kAnimatedBusyCursor + i)));
            i++; if (i == kNumberOfAnimatedBusyCursors) i = 0;
#else
            SetCursor(*GetCursor(kWatchCursor));
#endif
            gLastUpdated = TickCount();
        }
        // Handle events:
        EventRecord event;
        gotEvent = WaitNextEvent(everyEvent & ~highLevelEventMask, &event, 0, nil);

        // To do: if event is command-period key press then set gAbortFetch to true

#ifdef __MWERKS__
        if (gotEvent) { SIOUXHandleOneEvent(&event); }
#endif

        // Give other threads CPU time:
        YieldToAnyThread();
    }

    return noErr;
}


const char *System::httpFetch( const char *url )
{
    OSStatus junk, err = noErr;
    Ptr transferBuffer = nil;
    EndpointRef ep = kOTInvalidEndpointRef;
    TCall sndCall;
    DNSAddress hostDNSAddress;
    OTFlags junkFlags;
    OTResult bytesSent, bytesReceived, lookResult;
    Boolean bound = false;
    char * fetchedFilename = 0;
    char hostname[256];

    FSSpec destFSSpec;
    short destFileRefNum;
    Str255 destFileName;
    static counter = 0;

    url += strlen("http://");

    // Count the characters before the next slash.
    size_t hostCharCount = strcspn(url, "/");

    // Extract those characters from the URL into hostName
    //  and then make sure it's null terminated.
    strncpy(hostname, url, hostCharCount);
    hostname[hostCharCount] = 0;
    url += hostCharCount;

    theSystem->inform("Connecting to %s...", hostname);

    // Add a ":80" to the host name if necessary.
    if (strchr( hostname, ':' ) == nil) { strcat( hostname, ":80" ); }

    // Create a temporary file in the current folder

    if (err == noErr) {
        char tmpName[16];
        sprintf(tmpName, "vr_%i", counter);
        fetchedFilename = strdup(tmpName);
        if (!fetchedFilename) {
            theSystem->error("Connect failed: Memory allocation error\n");
            return 0;
        }
        c2str255(fetchedFilename, destFileName);
        FSMakeFSSpec(0, 0, destFileName, &destFSSpec);
        FSpCreate(&destFSSpec, 'ttxt', 'TEXT', 0);
        err = FSpOpenDF(&destFSSpec, fsRdWrPerm, &destFileRefNum);
        if (err != noErr) {
            theSystem->warn("Connect failed: Failed to create temporary file\n"); {
                return 0;
            }
        }
        counter++;
    }

    // construct httpCommand

    if (*url == 0) { url = "/"; }

    char httpCommand[256];
    sprintf( httpCommand, "GET %s HTTP/1.0\nAccept: */*\n\r\n", url);

    if (!hostname || !httpCommand) {
        theSystem->warn("Connect failed: Failed to construct HTTP request\n");
        return 0;
    }

    ThreadID progressThread;
    err = NewThread(kCooperativeThread,
                    (ThreadEntryProcPtr) ProgressThread, nil,
                    0, kCreateIfNeeded,
                    nil,
                    &progressThread);

    if (err != noErr) {
        theSystem->error("Connect failed: Failed to create cooperative thread\n");
        return 0;
    }

    // Initialise Open Transport and get on with it...

    err = InitOpenTransport();
    if (err != noErr) {
        theSystem->error("Connect failed: Unable to initialise Open Transport\n");
        junk = DisposeThread(progressThread, nil, true);
        return NULL;
    }

    err = noErr;
    transferBuffer = (char*)OTAllocMem(kTransferBufferSize);
    if (transferBuffer == nil) { err = kENOMEMErr; }


    if (err == noErr) {
        ep = OTOpenEndpoint(OTCreateConfiguration(kTCPName), 0, nil, &err);
    }

    if (err == noErr) { err = OTSetSynchronous(ep); }

    if (err == noErr) { err = OTSetBlocking(ep); }

    if (err == noErr) { err = OTInstallNotifier(ep, YieldingNotifier, nil); }

    if (err == noErr) { err = OTUseSyncIdleEvents(ep, true); }

    if (err == noErr) {
        err = OTBind(ep, nil, nil);
        bound = (err == noErr);
    }

    if (err == noErr) {
        sndCall.addr.buf    = (UInt8 *) &hostDNSAddress;
        sndCall.addr.len    = OTInitDNSAddress(&hostDNSAddress, (char *) hostname);
        sndCall.opt.buf     = nil;        // no connection options
        sndCall.opt.len     = 0;
        sndCall.udata.buf   = nil;        // no connection data
        sndCall.udata.len   = 0;
        sndCall.sequence    = 0;        // ignored by OTConnect

        err = OTConnect(ep, &sndCall, nil);
    }

    // Send the HTTP command to the server.

    if (err == noErr) {
        bytesSent = OTSnd(ep, (void *) httpCommand, OTStrLength(httpCommand), 0);
        if (bytesSent > 0) {
            err = noErr;
        } else {
            err = bytesSent;
        }
    }

    if (err == noErr) {
        int gothdr = 0, nread = 0;
        char *start;
        do {
            bytesReceived = OTRcv(ep, (void *) transferBuffer, kTransferBufferSize, &junkFlags);
            if (bytesReceived > 0) {

                nread += bytesReceived;

                // Skip header (should read return code, content-type...)
                if (gothdr) {
                    start = transferBuffer;
                } else {
                    start = strstr(transferBuffer, "\r\n\r\n");
                    if (start) {
                        start += 4;
                    } else {
                        start = strstr(transferBuffer, "\n\n");
                        if (start) { start += 2; }
                    }
                    if (!start) { continue; }
                    gothdr = 1;
                }

                bytesReceived -= (start - transferBuffer);

                err = FSWrite(destFileRefNum, &bytesReceived, start);

            } else {
                err = bytesReceived;
            }
            theSystem->inform("Read %dk", (nread+1023)/1024);
        } while (err == noErr && !gAbortFetch);
    }

    if (gAbortFetch == true) {
        gAbortFetch = false;
        theSystem->debug("Aborted file download\n");
    }

    if (err == kOTLookErr) {
        lookResult = OTLook(ep);
        switch (lookResult) {
        case T_DISCONNECT:
            err = OTRcvDisconnect(ep, nil);
            break;
        case T_ORDREL:
            err = OTRcvOrderlyDisconnect(ep);
            if (err == noErr) {
                err = OTSndOrderlyDisconnect(ep);
            }
            break;
        default:
            break;
        }
    }

    if ((err == noErr) && bound) { junk = OTUnbind(ep); }

    if (ep != kOTInvalidEndpointRef) { junk = OTCloseProvider(ep); }

    if (transferBuffer != nil) { OTFreeMem(transferBuffer); }

    long fileSize;

    err = GetFPos(destFileRefNum, &fileSize);

    junk = DisposeThread(progressThread, nil, true);

    CloseOpenTransport();

    FSClose(destFileRefNum);

    if ( err == noErr ) {
        if (fileSize > 0) {
            theSystem->inform("Retrieved file (size %dk).", (fileSize+1023)/1024);
        } else {
            theSystem->inform("Failed to retrieve file.");
        }
    } else {
        theSystem->inform("Failed to retrieve file from %s.", hostname);
        free(fetchedFilename);
        return NULL;
    }

    return fetchedFilename;
}

#endif // ndef macintosh

void System::removeFile(const char * fn)
{
    remove(fn);
}
