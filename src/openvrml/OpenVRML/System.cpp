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

// This won't work under windows or if netscape isn't running...

bool System::loadUrl(const std::string & url, const mfstring & parameters)
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

int System::connectSocket(const char * host, int port)
{
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
    char * result = 0;
    char temp_name[L_tmpnam] = { "VR" };
    if (sockfd != -1 && (result = tmpnam(temp_name))) {
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

void System::removeFile(const char * fn)
{
    remove(fn);
}
