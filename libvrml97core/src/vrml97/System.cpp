//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
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

#ifdef _WIN32
#include <string.h>
#endif


// A default System object
static System defaultSystem;

// The global System object
System *theSystem = &defaultSystem;


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
  if (strcmp(lastbuf, buf))
    {
      fprintf(stderr,"%s", buf);
      putc('\n', stderr);
      strcpy(lastbuf, buf);
    }
}

#if DEBUG
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

#ifdef _WIN32

// Windows

#include <sys/timeb.h>
#include <time.h>

double System::time()	
{
   struct _timeb timebuffer;
   _ftime( &timebuffer );
	return (double) timebuffer.time + 1.e-3 * (double) timebuffer.millitm;
}

#else

#ifdef macintosh
#define kTwoPower32 4294967296.0

double System::time()
{
  UnsignedWide currentTime;
  Microseconds(&currentTime);
  return (((double)currentTime.hi * kTwoPower32 + (double)currentTime.lo) * 1.e-6);
}

#else

// Unix/Linux
#include <sys/time.h>
#include <unistd.h>

double System::time()
{
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  
  return (double) tv.tv_sec + 1.e-6 * (double) tv.tv_usec;
}

#endif // macintosh
#endif // _WIN32


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

#ifndef _WIN32
# include <stdlib.h>
#endif

bool System::loadUrl(const char *url, int np, char **parameters )
{
  if (! url) return false;
#ifndef _WIN32
  char buf[1024];
  if (np)
    sprintf(buf,"/bin/csh -c \"netscape -remote 'openURL(%s, %s)'\" &",
	    url, parameters[0]);
  else
    sprintf(buf,"/bin/csh -c \"netscape -remote 'openURL(%s)'\" &", url);
  return system(buf) != -1;
#else
  return false;
#endif // _WIN32
}

// added for working under windows (and is not needed for mac os)...
#ifdef _WIN32
#include <sys/types.h>
#include <winsock2.h>
#include <string.h>
#include <ctype.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>		// memset
#include <ctype.h>
#endif // _WIN32

#endif // macintosh

int System::connectSocket( const char *host, int port )
{
//#ifdef _WIN32
//  return -1;
//#endif
#ifdef macintosh
  return -1;
#else
  struct sockaddr_in sin;
  struct hostent *he;

#ifdef _WIN32            
  WSADATA wsaData;
  WORD wVersionRequested;
#endif

  int sockfd = -1;

 #ifdef _WIN32   
   wVersionRequested = MAKEWORD( 1, 0 );
   if (WSAStartup(wVersionRequested,&wsaData) == SOCKET_ERROR)
   {
                theSystem->error("WSAStartup failed with error %d\n",WSAGetLastError());
                WSACleanup();
                return -1;
        }
#endif                            
  memset( &sin, 0, sizeof(sin) );
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);

  // Check for dotted number format
  char *s;
  for (s=(char*)host; *s; ++s)
    if ( ! (isdigit(*s) || *s == '.') ) break;

  if (*s)			// Not dotted number
    he = gethostbyname(host);
  else
    {
      u_long addr = inet_addr(host);
      he = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
    }

  if (he)
    {
      memcpy((char *)&sin.sin_addr, he->h_addr, he->h_length);
      sockfd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
      if (sockfd != -1)
	if (connect( sockfd, (struct sockaddr *)&sin, sizeof(sin)) == -1)
	  {
#ifdef _WIN32                  
                closesocket(sockfd);
                WSACleanup();
#else

	    close( sockfd );
#endif
	    sockfd = -1;
	  }
    }

  return sockfd;
#endif
}


#include <fcntl.h>		// open() modes
#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif
#include <stdlib.h>
#include <errno.h>

const char *System::httpHost( const char *url, int *port )
{
  static char hostname[256];
  const char *s = strstr(url,"//");
  char *p = hostname;

  if (s)
    {
      for (s+=2; *s && *s != '/' && *s != ':'; *p++ = *s++)
	/* empty */ ;
      if (*s == ':' && isdigit(*(s+1)))
	*port = atoi(s+1);
    }
  *p = '\0';
  return hostname;
}

// This isn't particularly robust or complete...

#ifndef macintosh

const char *System::httpFetch( const char *url )
{
// #ifdef _WIN32
//  return 0;
// #else

  int port = 80;
  const char *hostname = httpHost(url, &port);

  if (port == 80)
    theSystem->inform("Connecting to %s ...", hostname);
  else
    theSystem->inform("Connecting to %s:%d ...", hostname, port);

  int sockfd;
  if ((sockfd = System::connectSocket( hostname, port )) != -1)
    theSystem->inform("connected.");
  else
#ifdef _WIN32                          
     {
        theSystem->warn("Connect failed:  (errno %d)\n",
        WSAGetLastError());
        WSACleanup();
     }
#else
  theSystem->warn("Connect failed: %s (errno %d).\n",
		 strerror(errno), errno);
#endif
  // Copy to a local temp file
  char *result = 0;
  if (sockfd != -1 && (result = tempnam(0, "VR")))
    {

      int fd = open(result, O_RDWR|O_CREAT, 0777);

      if (fd != -1)
	{
	  char *abspath = strstr((char *)url, "//");
	  if (abspath) abspath = strchr(abspath+2, '/');
	  if (! abspath) abspath = (char*) url;

	  char request[1024];
	  sprintf(request,"GET %s HTTP/1.0\nAccept: */*\n\r\n", abspath);

	  int nbytes = strlen(request);
#ifdef _WIN32                           
          if(send(sockfd,request,nbytes,0) != nbytes)
#else

	  if (write(sockfd, request, nbytes) != nbytes)
#endif
#ifdef _WIN32                          
          {
                theSystem->warn("http GET failed:  (errno %d)\n",
                WSAGetLastError());
                WSACleanup();
          }
#else                                       

	    theSystem->warn("http GET failed: %s (errno %d)\n",
			 strerror(errno), errno);
#endif
	  else
	    {
	      int gothdr = 0, nread = 0, nwrote = 0, nmore;
	      char *start;
#ifdef _WIN32                                   
          while((nmore = recv(sockfd,request,sizeof(request)-1,0)) > 0)
#else            
	      while ((nmore = read(sockfd, request, sizeof(request)-1)) > 0)
#endif
		{
		  nread += nmore;

		  // Skip header (should read return code, content-type...)
		  if (gothdr)
		    start = request;
		  else
		    {
		      start = strstr(request, "\r\n\r\n");
		      if (start)
			start += 4;
		      else
			{
			  start = strstr(request, "\n\n");
			  if (start) start += 2;
			}
		      if (! start) continue;
		      gothdr = 1;
		    }

		  nmore -= (start - request);
		  if (write(fd, start, nmore) != nmore)
		    {
		      theSystem->warn("http: temp file write error\n");
		      break;
		    }
		  nwrote += nmore;
		}

	      theSystem->inform("Read %dk from %s", (nread+1023)/1024, url);
	      //System::debug("Wrote %d bytes to %s\n", nread, result);
	    }

	  close(fd);
	}
    }

  if (sockfd != -1)
#ifdef _WIN32   
	{
                closesocket(sockfd);
                WSACleanup();
	}
#else
  close(sockfd);
#endif
  return result;
// #endif
}

#else // Mac code for using OpenTransport to retrieve a URL

// This code contains (unrestricted) sample code from Apple

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <Files.h>
#include <TextUtils.h>
#include <DriverServices.h>
#include <PLStringFuncs.h>

//#ifndef qDebug
//#define qDebug	1
//#endif

#include <OpenTransport.h>
#include <OpenTptInternet.h>
//#include <OTDebug.h>
#include <Threads.h>


char* strdup(const char *string)
{
	char *newstr = (char*)malloc(strlen(string) + 1);
	if (newstr == NULL)
	  return NULL;
	strcpy(newstr, string);
	return newstr;
}


extern pascal void OTDebugStr(const char* str);

enum {
	kTransferBufferSize = 4096
};

enum {
	kDownloadSimple,
	kDownloadFaster
};

#define kWatchCursor 1007

static pascal void YieldingNotifier(void* contextPtr, OTEventCode code, 
									   OTResult result, void* cookie)
{
	#pragma unused(contextPtr)
	#pragma unused(result)
	#pragma unused(cookie)
	//OSStatus junk;
	switch (code) {
		case kOTSyncIdleEvent:
			/*junk = */ (void)YieldToAnyThread();
			//OTAssert("YieldingNotifier: YieldToAnyThread failed", junk == noErr);
			break;
		default: // do nothing
			break;
	}
}

#ifdef __MWERKS__
#include <sioux.h>
#endif

//static UInt32 gLastPrinted = 0;

static pascal OSStatus ProgressThread(void *junkParam)
{
	#pragma unused(junkParam)
	//OSStatus junk;


// This function should really set some sort of progress variable that can update
// a progress bar somewhere. Instead, it just sets the cursor to 'busy' and gives
// CPU time to other processses.

	  while (true) {
	//	if ( TickCount() > (gLastPrinted + 60) ) {
	//	    printf("#");    // should really update a queriable % completed value
	//		                // that can be used to update a progress bar...
	//		fflush(stdout);
	//		gLastPrinted = TickCount();
	//	}
		//This ensures that as long as this application is active, the cursor is
		//a watch
        SetCursor(*(GetCursor(kWatchCursor)));  // could have had a cool rotating
			                                    // cursor here... This assumes that
			                                    // cursor is i kWatchCursor, but
			                                    // should do nothing if that cursor
			                                    // is not defined. Mac GLUT uses
			                                    // 1007 for the cursor
		// Give other applications CPU time:
		static EventRecord event;
		WaitNextEvent (everyEvent & ~highLevelEventMask, &event, 0, nil);
		#ifdef __MWERKS__
		SIOUXHandleOneEvent(&event); // Let SIOUX update the console if it needs to
		#endif
		// Give other threads CPU time:
		/*junk = */ (void)YieldToAnyThread();
		//OTAssert("ProgressThread: YieldToAnyThread failed", junk == noErr);
	}

	return (noErr);
}


const char *System::httpFetch( const char *url )
{
	OSStatus 	junk, err             = noErr;
	Ptr			transferBuffer 	= nil;
	EndpointRef ep 				= kOTInvalidEndpointRef;
	TCall 		sndCall;
	DNSAddress 	hostDNSAddress;
	OTFlags 	junkFlags;
	OTResult 	bytesSent, bytesReceived, lookResult;
	Boolean		bound			= false;
    char*       fetchedFilename = NULL;
	char        hostname[256];

	FSSpec      destFSSpec;
	short       destFileRefNum;
    Str255      destFileName;
    static      counter = 0;
    
	url += strlen("http://");
		
	// Count the characters before the next slash.
	size_t hostCharCount = strcspn(url, "/");
		
	// Extract those characters from the URL into hostName
	//  and then make sure it's null terminated.
	(void) strncpy(hostname, url, hostCharCount);
	hostname[hostCharCount] = 0;
	url += hostCharCount;
	
    theSystem->inform("Fetching data from %s... ", hostname);
    
	// Add a ":80" to the host name if necessary.
	if ( strchr( hostname, ':' ) == nil ) {
		strcat( hostname, ":80" );
	}

    // Create a temporary file in the current folder

	if (err == noErr) {
	    char tmpName[16];
	    sprintf(tmpName, "vr_%i", counter);
        fetchedFilename = strdup(tmpName);
        if (!fetchedFilename) {
           theSystem->error("Connect failed: Memory allocation error\n");
           return NULL;
        }
        Str255 counterPString;
        NumToString(counter, counterPString);
	    PStrCopy(destFileName, "\pvr_");
		PStrCat(destFileName, counterPString);        
		(void) FSMakeFSSpec(0, 0, destFileName, &destFSSpec);
		(void) FSpCreate(&destFSSpec, 'ttxt', 'TEXT', 0);
		err = FSpOpenDF(&destFSSpec, fsRdWrPerm, &destFileRefNum);
		if (err != noErr) {
	        theSystem->warn("Connect failed: Failed to create temporary file\n");
			return NULL;
		}
		counter++;
	}

    // construct httpCommand

    if ( *url == 0 )
		url = "/";

	char httpCommand[256];
	sprintf( httpCommand, "GET %s HTTP/1.0\nAccept: */*\n\r\n", url);

    if ( !hostname || !httpCommand ) {
	    theSystem->warn("Connect failed: Failed to construct HTTP request\n");
        return NULL;
    }

	ThreadID progressThread;
	err = NewThread(kCooperativeThread,
					(ThreadEntryProcPtr) ProgressThread, nil,
					0, kCreateIfNeeded,
					nil,
					&progressThread);

	if (err != noErr) {
	    theSystem->error("Connect failed: Failed to create cooperative thread\n");
	    return NULL;
	 }

    // Initialise Open Transport and get on with it...
    
	err = InitOpenTransport();
	if (err != noErr) {
	    theSystem->error("Connect failed: Unable to initialise Open Transport\n");
	    return NULL;
	 }
	
	err = noErr;
	transferBuffer = (char*)OTAllocMem(kTransferBufferSize);
	if ( transferBuffer == nil ) {
		err = kENOMEMErr;
	}
		
	if (err == noErr) {
		ep = OTOpenEndpoint(OTCreateConfiguration(kTCPName), 0, nil, &err);
	}
		
	if (err == noErr) {
		junk = OTSetSynchronous(ep);
		//OTAssert("DownloadHTTPSimple: OTSetSynchronous failed", junk == noErr);
		
		junk = OTSetBlocking(ep);
		//OTAssert("DownloadHTTPSimple: OTSetBlocking failed", junk == noErr);
		
		junk = OTInstallNotifier(ep, YieldingNotifier, nil);
		//OTAssert("DownloadHTTPSimple: OTInstallNotifier failed", junk == noErr);
		
		junk = OTUseSyncIdleEvents(ep, true);
		//OTAssert("DownloadHTTPSimple: OTUseSyncIdleEvents failed", junk == noErr);
				
		err = OTBind(ep, nil, nil);
		bound = (err == noErr);
	}
		
	if (err == noErr) {
		sndCall.addr.buf 	= (UInt8 *) &hostDNSAddress;
		sndCall.addr.len 	= OTInitDNSAddress(&hostDNSAddress, (char *) hostname);
		sndCall.opt.buf 	= nil;		// no connection options
		sndCall.opt.len 	= 0;
		sndCall.udata.buf 	= nil;		// no connection data
		sndCall.udata.len 	= 0;
		sndCall.sequence 	= 0;		// ignored by OTConnect
		
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
		    if (gothdr)
		      start = transferBuffer;
		    else {
		      start = strstr(transferBuffer, "\r\n\r\n");
		      if (start)
			    start += 4;
		      else {
			    start = strstr(transferBuffer, "\n\n");
			    if (start)
			      start += 2;
			  }
		      if (! start)
		        continue;
		      gothdr = 1;
		    }

		      bytesReceived -= (start - transferBuffer);
		  			
			  err = FSWrite( destFileRefNum, &bytesReceived, start );
				
		    } else {
				err = bytesReceived;
			}
 		} while (err == noErr);
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

	if ( (err == noErr) && bound ) {
		junk = OTUnbind(ep);
		//OTAssert("DownloadHTTPSimple: OTUnbind failed.", junk == noErr);
	}
	
	if (ep != kOTInvalidEndpointRef) {
		junk = OTCloseProvider(ep);
		//OTAssert("DownloadHTTPSimple: OTCloseProvider failed.", junk == noErr);
	}
	if (transferBuffer != nil) {
		OTFreeMem(transferBuffer);
	}
    
    long fileSize;

    err = GetFPos(destFileRefNum, &fileSize);
    
    //OTAssert( "DEBUG: GetFPos failed", err == noErr);

	junk = DisposeThread(progressThread, nil, true);
	//OTAssert("main: DisposeThread failed", junk == noErr);

	CloseOpenTransport();

	FSClose(destFileRefNum);
	
    if ( err == noErr ) {
	    theSystem->inform("[done]");
	    theSystem->debug("Completed reading file from %s", hostname);
		theSystem->debug("Bytes downloaded: %d.\n", fileSize);
	    theSystem->debug("Wrote %s from %\n", fetchedFilename, hostname);
	}
    else {
	    theSystem->warn("Connect failed: Failed to retrieve file from %s\n", hostname);
	    free (fetchedFilename);
        return NULL;
    }
    
	return fetchedFilename;
}

#endif // ndef macintosh



void System::removeFile( const char *fn )
{
  remove(fn);
}
