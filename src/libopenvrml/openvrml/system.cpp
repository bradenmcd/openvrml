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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <cctype>
# include <cerrno>
# include <cstdarg>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <algorithm>
# include <sstream>

// added for working under windows (and is not needed for mac os)...
# if defined(_WIN32) && !defined(__CYGWIN__)
#   include <sys/types.h>
#   include <winsock2.h>
# else
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <unistd.h>
#   include <netdb.h>
# endif

#include <fcntl.h>        // open() modes
#if defined(_WIN32) && !defined(__CYGWIN__)
# include <io.h>
#else
# include <unistd.h>
#endif

# include "private.h"
# include "system.h"

# ifndef NDEBUG
#   define SYSTEM_DEBUG
# endif

namespace openvrml {

// A default system object
static system defaultSystem;

/**
 * @brief The global system object.
 */
system * the_system = &defaultSystem;

// This won't work under windows or if netscape isn't running...

bool system::load_url(const std::string & url, const mfstring & parameters)
{
    return false;
}

int system::connect_socket(const char * host, int port)
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
        std::cerr << "WSAStartup failed with error " << WSAGetLastError()
                  << std::endl;
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
        sockfd = int(socket(PF_INET, SOCK_STREAM, IPPROTO_TCP));
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

const char * system::http_host(const char * url, int * port)
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

const char *system::http_fetch(const char * url)
{
    int port = 80;
    const char *hostname = this->http_host(url, &port);

    if (port == 80) {
        OPENVRML_PRINT_MESSAGE_("Connecting to " + std::string(hostname)
                                + " ...");
    } else {
        std::ostringstream port_sstream;
        port_sstream << port;
        OPENVRML_PRINT_MESSAGE_("Connecting to " + std::string(hostname)
                                + ":" + port_sstream.str() << " ...");
    }

    int sockfd;
    if ((sockfd = system::connect_socket( hostname, port )) != -1) {
        OPENVRML_PRINT_MESSAGE_("connected.");
    } else {
        std::ostringstream error_sstream;
#if defined(_WIN32) && !defined(__CYGWIN__)
        error_sstream << WSAGetLastError();
        OPENVRML_PRINT_MESSAGE_("Connect failed:  (errno "
                                + error_sstream.str() + ").");
        WSACleanup();
# else
        error_sstream << errno;
        OPENVRML_PRINT_MESSAGE_("Connect failed: "
                                + std::string(strerror(errno)) + " (errno "
                                + error_sstream.str() + ").");
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

            size_t nbytes = strlen(request);
#if defined(_WIN32) && !defined(__CYGWIN__)
            if (send(sockfd, request, int(nbytes), 0) != nbytes) {
#else
            if (write(sockfd, request, nbytes) != nbytes) {
#endif
                std::ostringstream error_sstream;
#if defined(_WIN32) && !defined(__CYGWIN__)
                error_sstream << WSAGetLastError();
                OPENVRML_PRINT_MESSAGE_("http GET failed:  (errno "
                                        + error_sstream.str() + ").");
                WSACleanup();
#else
                error_sstream << errno;
                OPENVRML_PRINT_MESSAGE_("http GET failed: "
                                        + std::string(strerror(errno))
                                        + " (errno " + error_sstream.str()
                                        + ").");
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

                    nmore -= int(start - request);
# if defined(_WIN32) && !defined(__CYGWIN__)
                    if (_write(fd, start, nmore) != nmore) {
# else
                    if (write(fd, start, nmore) != nmore) {
# endif
                        OPENVRML_PRINT_MESSAGE_("http: temp file write error");
                        break;
                    }
                    nwrote += nmore;
                }
                std::ostringstream nread_sstream;
                nread_sstream << (nread + 1023) / 1024;
                OPENVRML_PRINT_MESSAGE_("Read " + nread_sstream.str()
                                        + "k from " + std::string(url));
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

void system::remove_file(const char * fn)
{
    remove(fn);
}

} // namespace openvrml
