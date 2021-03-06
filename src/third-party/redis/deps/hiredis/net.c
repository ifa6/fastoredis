/* Extracted from anet.c to work properly with Hiredis error reporting.
 *
 * Copyright (c) 2006-2011, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2010-2011, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef FASTOREDIS
    #include "fmacros.h"
    #include <sys/types.h>
    #include <limits.h>
    #include <errno.h>
    #include <stdarg.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <string.h>
    #ifdef OS_WIN
        #include <winsock2.h>
        #include <wspiapi.h>
    #else
        #include <sys/socket.h>
        #include <sys/select.h>
        #include <sys/un.h>
        #include <netinet/in.h>
        #include <netinet/tcp.h>
        #include <arpa/inet.h>
        #include <netdb.h>
        #include <poll.h>
    #endif
    #include "net.h"
    #include "sds.h"
#else
#include "fmacros.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <poll.h>
#include <limits.h>

#include "net.h"
#include "sds.h"
#endif
/* Defined in hiredis.c */
void __redisSetError(redisContext *c, int type, const char *str);

static void redisContextCloseFd(redisContext *c) {
    if (c && c->fd >= 0) {
        close(c->fd);
        c->fd = -1;
    }
}
#ifdef FASTOREDIS
#ifdef OS_WIN
int strerror_r(int err, char *text, int size)
{
    #define UPREFIX "Unknown error: %u"
    unsigned int errnum = err;
    int retval = 0;
    size_t slen = 0;
    if (errnum < (unsigned int) sys_nerr) {
        memcpy(text, sys_errlist[errnum], size);
        slen = strlen(text);
    } else {
        slen = snprintf(text, size, UPREFIX, errnum);
        retval = EINVAL;
    }

    if (slen >= size)
        retval = ERANGE;

    return retval;
}
#endif
#endif

static void __redisSetErrorFromErrno(redisContext *c, int type, const char *prefix) {
    char buf[128] = { 0 };
    size_t len = 0;

    if (prefix != NULL)
        len = snprintf(buf,sizeof(buf),"%s: ",prefix);
    __redis_strerror_r(errno, (char *)(buf + len), sizeof(buf) - len);
    __redisSetError(c,type,buf);
}

static int redisSetReuseAddr(redisContext *c) {
    int on = 1;
    if (setsockopt(c->fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        __redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
        redisContextCloseFd(c);
        return REDIS_ERR;
    }
    return REDIS_OK;
}

static int redisCreateSocket(redisContext *c, int type) {
    int s;
    if ((s = socket(type, SOCK_STREAM, 0)) == -1) {
        __redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
        return REDIS_ERR;
    }
    c->fd = s;
    if (type == AF_INET) {
        if (redisSetReuseAddr(c) == REDIS_ERR) {
            return REDIS_ERR;
        }
    }
    return REDIS_OK;
}

static int redisSetBlocking(redisContext *c, int blocking) {
#ifdef FASTOREDIS
    #ifdef OS_WIN
        unsigned long flags = blocking;
        int res = ioctlsocket(c->fd, FIONBIO, &flags);
        if (res == SOCKET_ERROR) {
            __redisSetErrorFromErrno(c,REDIS_ERR_IO,"ioctlsocket(FIONBIO)");
            redisContextCloseFd(c);
            return REDIS_ERR;
        }
    #else
        int flags;

        /* Set the socket nonblocking.
         * Note that fcntl(2) for F_GETFL and F_SETFL can't be
         * interrupted by a signal. */
        if ((flags = fcntl(c->fd, F_GETFL)) == -1) {
            __redisSetErrorFromErrno(c,REDIS_ERR_IO,"fcntl(F_GETFL)");
            redisContextCloseFd(c);
            return REDIS_ERR;
        }

        if (blocking)
            flags &= ~O_NONBLOCK;
        else
            flags |= O_NONBLOCK;

        if (fcntl(c->fd, F_SETFL, flags) == -1) {
            __redisSetErrorFromErrno(c,REDIS_ERR_IO,"fcntl(F_SETFL)");
            redisContextCloseFd(c);
            return REDIS_ERR;
        }
    #endif
#else
    int flags;

    /* Set the socket nonblocking.
     * Note that fcntl(2) for F_GETFL and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags = fcntl(c->fd, F_GETFL)) == -1) {
        __redisSetErrorFromErrno(c,REDIS_ERR_IO,"fcntl(F_GETFL)");
        redisContextCloseFd(c);
        return REDIS_ERR;
    }

    if (blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;

    if (fcntl(c->fd, F_SETFL, flags) == -1) {
        __redisSetErrorFromErrno(c,REDIS_ERR_IO,"fcntl(F_SETFL)");
        redisContextCloseFd(c);
        return REDIS_ERR;
    }
#endif
    return REDIS_OK;
}

int redisKeepAlive(redisContext *c, int interval) {
    int val = 1;
    int fd = c->fd;

    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == -1){
        __redisSetError(c,REDIS_ERR_OTHER,strerror(errno));
        return REDIS_ERR;
    }

    val = interval;

#ifdef _OSX
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE, &val, sizeof(val)) < 0) {
        __redisSetError(c,REDIS_ERR_OTHER,strerror(errno));
        return REDIS_ERR;
    }
#else
#if defined(__GLIBC__) && !defined(__FreeBSD_kernel__)
    val = interval;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val)) < 0) {
        __redisSetError(c,REDIS_ERR_OTHER,strerror(errno));
        return REDIS_ERR;
    }

    val = interval/3;
    if (val == 0) val = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) {
        __redisSetError(c,REDIS_ERR_OTHER,strerror(errno));
        return REDIS_ERR;
    }

    val = 3;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val)) < 0) {
        __redisSetError(c,REDIS_ERR_OTHER,strerror(errno));
        return REDIS_ERR;
    }
#endif
#endif

    return REDIS_OK;
}

static int redisSetTcpNoDelay(redisContext *c) {
    int yes = 1;
    if (setsockopt(c->fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1) {
        __redisSetErrorFromErrno(c,REDIS_ERR_IO,"setsockopt(TCP_NODELAY)");
        redisContextCloseFd(c);
        return REDIS_ERR;
    }
    return REDIS_OK;
}

#define __MAX_MSEC (((LONG_MAX) - 999) / 1000)

static int redisContextWaitReady(redisContext *c, const struct timeval *timeout) {
#ifdef FASTOREDIS
    #ifdef OS_WIN
        fd_set master_set;
        FD_ZERO(&master_set);
        int max_sd = c->fd;
        FD_SET(c->fd, &master_set);

        struct timeval tm;
        tm.tv_sec  = 60;
        tm.tv_usec = 0;

        /* Only use timeout when not NULL. */
        if (timeout != NULL) {
            if (timeout->tv_usec > 1000000 || timeout->tv_sec > __MAX_MSEC) {
                redisContextCloseFd(c);
                return REDIS_ERR;
            }
            tm = *timeout;
        }

        if (errno == EINPROGRESS) {
            int res;

            if ((res = select(max_sd + 1, &master_set, NULL, NULL, &tm)) == -1) {
                __redisSetErrorFromErrno(c, REDIS_ERR_IO, "select(2)");
                redisContextCloseFd(c);
                return REDIS_ERR;
            } else if (res == 0) {
                errno = ETIMEDOUT;
                __redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
                redisContextCloseFd(c);
                return REDIS_ERR;
            }

            if (redisCheckSocketError(c) != REDIS_OK)
                return REDIS_ERR;

            return REDIS_OK;
        }

        __redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
        redisContextCloseFd(c);
        return REDIS_ERR;
    #else
        struct pollfd   wfd[1];
        long msec;

        msec          = -1;
        wfd[0].fd     = c->fd;
        wfd[0].events = POLLOUT;

        /* Only use timeout when not NULL. */
        if (timeout != NULL) {
            if (timeout->tv_usec > 1000000 || timeout->tv_sec > __MAX_MSEC) {
                __redisSetErrorFromErrno(c, REDIS_ERR_IO, NULL);
                redisContextCloseFd(c);
                return REDIS_ERR;
            }

            msec = (timeout->tv_sec * 1000) + ((timeout->tv_usec + 999) / 1000);

            if (msec < 0 || msec > INT_MAX) {
                msec = INT_MAX;
            }
        }

        if (errno == EINPROGRESS) {
            int res;

            if ((res = poll(wfd, 1, msec)) == -1) {
                __redisSetErrorFromErrno(c, REDIS_ERR_IO, "poll(2)");
                redisContextCloseFd(c);
                return REDIS_ERR;
            } else if (res == 0) {
                errno = ETIMEDOUT;
                __redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
                redisContextCloseFd(c);
                return REDIS_ERR;
            }

            if (redisCheckSocketError(c) != REDIS_OK)
                return REDIS_ERR;

            return REDIS_OK;
        }

        __redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
        redisContextCloseFd(c);
        return REDIS_ERR;
    #endif
#else
    struct pollfd   wfd[1];
    long msec;

    msec          = -1;
    wfd[0].fd     = c->fd;
    wfd[0].events = POLLOUT;

    /* Only use timeout when not NULL. */
    if (timeout != NULL) {
        if (timeout->tv_usec > 1000000 || timeout->tv_sec > __MAX_MSEC) {
            __redisSetErrorFromErrno(c, REDIS_ERR_IO, NULL);
            redisContextCloseFd(c);
            return REDIS_ERR;
        }

        msec = (timeout->tv_sec * 1000) + ((timeout->tv_usec + 999) / 1000);

        if (msec < 0 || msec > INT_MAX) {
            msec = INT_MAX;
        }
    }

    if (errno == EINPROGRESS) {
        int res;

        if ((res = poll(wfd, 1, msec)) == -1) {
            __redisSetErrorFromErrno(c, REDIS_ERR_IO, "poll(2)");
            redisContextCloseFd(c);
            return REDIS_ERR;
        } else if (res == 0) {
            errno = ETIMEDOUT;
            __redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
            redisContextCloseFd(c);
            return REDIS_ERR;
        }

        if (redisCheckSocketError(c) != REDIS_OK)
            return REDIS_ERR;

        return REDIS_OK;
    }

    __redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
    redisContextCloseFd(c);
    return REDIS_ERR;
#endif
}

int redisCheckSocketError(redisContext *c) {
    int err = 0;
    socklen_t errlen = sizeof(err);

    if (getsockopt(c->fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
        __redisSetErrorFromErrno(c,REDIS_ERR_IO,"getsockopt(SO_ERROR)");
        return REDIS_ERR;
    }

    if (err) {
        errno = err;
        __redisSetErrorFromErrno(c,REDIS_ERR_IO,NULL);
        return REDIS_ERR;
    }

    return REDIS_OK;
}

int redisContextSetTimeout(redisContext *c, const struct timeval tv) {
    if (setsockopt(c->fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv)) == -1) {
        __redisSetErrorFromErrno(c,REDIS_ERR_IO,"setsockopt(SO_RCVTIMEO)");
        return REDIS_ERR;
    }
    if (setsockopt(c->fd,SOL_SOCKET,SO_SNDTIMEO,&tv,sizeof(tv)) == -1) {
        __redisSetErrorFromErrno(c,REDIS_ERR_IO,"setsockopt(SO_SNDTIMEO)");
        return REDIS_ERR;
    }
    return REDIS_OK;
}

static int _redisContextConnectTcp(redisContext *c, const char *addr, int port,
                                   const struct timeval *timeout,
                                   const char *source_addr) {
#ifdef FASTOREDIS
    if(c->session){
        if (!(c->channel = libssh2_channel_direct_tcpip(c->session, addr, port))) {
            __redisSetError(c, REDIS_ERR_OTHER, "Unable to open a ssh session");
            return REDIS_ERR;
        }

        c->flags |= REDIS_CONNECTED;
        return REDIS_OK;
    }
#endif
    int s, rv, n;
        char _port[6];  /* strlen("65535"); */
    struct addrinfo hints, *servinfo, *bservinfo, *p, *b;
        int blocking = (c->flags & REDIS_BLOCK);
    int reuseaddr = (c->flags & REDIS_REUSEADDR);
    int reuses = 0;

        snprintf(_port, 6, "%d", port);
        memset(&hints,0,sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        /* Try with IPv6 if no IPv4 address was found. We do it in this order since
         * in a Redis client you can't afford to test if you have IPv6 connectivity
         * as this would add latency to every connect. Otherwise a more sensible
         * route could be: Use IPv6 if both addresses are available and there is IPv6
         * connectivity. */
        if ((rv = getaddrinfo(addr,_port,&hints,&servinfo)) != 0) {
             hints.ai_family = AF_INET6;
             if ((rv = getaddrinfo(addr,_port,&hints,&servinfo)) != 0) {
                __redisSetError(c,REDIS_ERR_OTHER,gai_strerror(rv));
                return REDIS_ERR;
            }
        }
        for (p = servinfo; p != NULL; p = p->ai_next) {
addrretry:
            if ((s = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
                continue;

            c->fd = s;
            if (redisSetBlocking(c,0) != REDIS_OK)
                goto error;
        if (source_addr) {
            int bound = 0;
            /* Using getaddrinfo saves us from self-determining IPv4 vs IPv6 */
            if ((rv = getaddrinfo(source_addr, NULL, &hints, &bservinfo)) != 0) {
                char buf[128];
                snprintf(buf,sizeof(buf),"Can't get addr: %s",gai_strerror(rv));
                __redisSetError(c,REDIS_ERR_OTHER,buf);
                goto error;
            }

            if (reuseaddr) {
                n = 1;
                if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*) &n,
                               sizeof(n)) < 0) {
            goto error;
        }
            }

            for (b = bservinfo; b != NULL; b = b->ai_next) {
                if (bind(s,b->ai_addr,b->ai_addrlen) != -1) {
                    bound = 1;
                    break;
                }
            }
            freeaddrinfo(bservinfo);
            if (!bound) {
                char buf[128];
                snprintf(buf,sizeof(buf),"Can't bind socket: %s",strerror(errno));
                __redisSetError(c,REDIS_ERR_OTHER,buf);
                goto error;
            }
        }
            if (connect(s,p->ai_addr,p->ai_addrlen) == -1) {
                if (errno == EHOSTUNREACH) {
                    redisContextCloseFd(c);
                    continue;
                } else if (errno == EINPROGRESS && !blocking) {
                    /* This is ok. */
            } else if (errno == EADDRNOTAVAIL && reuseaddr) {
                if (++reuses >= REDIS_CONNECT_RETRIES) {
                    goto error;
                } else {
                    goto addrretry;
                }
                } else {
                    if (redisContextWaitReady(c,timeout) != REDIS_OK)
                        goto error;
                }
            }
            if (blocking && redisSetBlocking(c,1) != REDIS_OK)
                goto error;
            if (redisSetTcpNoDelay(c) != REDIS_OK)
                goto error;

            c->flags |= REDIS_CONNECTED;
            rv = REDIS_OK;
            goto end;
        }
        if (p == NULL) {
            char buf[128];
            snprintf(buf,sizeof(buf),"Can't create socket: %s",strerror(errno));
            __redisSetError(c,REDIS_ERR_OTHER,buf);
            goto error;
        }

error:
        rv = REDIS_ERR;
end:
        freeaddrinfo(servinfo);
        return rv;  // Need to return REDIS_OK if alright
}

int redisContextConnectTcp(redisContext *c, const char *addr, int port,
                           const struct timeval *timeout) {
    return _redisContextConnectTcp(c, addr, port, timeout, NULL);
}

int redisContextConnectBindTcp(redisContext *c, const char *addr, int port,
                               const struct timeval *timeout,
                               const char *source_addr) {
    return _redisContextConnectTcp(c, addr, port, timeout, source_addr);
}

int redisContextConnectUnix(redisContext *c, const char *path, const struct timeval *timeout) {
#ifdef FASTOREDIS
    #ifdef OS_WIN
        return REDIS_ERR;
    #else
        int blocking = (c->flags & REDIS_BLOCK);
        struct sockaddr_un sa;

        if (redisCreateSocket(c,AF_LOCAL) < 0)
            return REDIS_ERR;
        if (redisSetBlocking(c,0) != REDIS_OK)
            return REDIS_ERR;

        sa.sun_family = AF_LOCAL;
        strncpy(sa.sun_path,path,sizeof(sa.sun_path)-1);
        if (connect(c->fd, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
            if (errno == EINPROGRESS && !blocking) {
                /* This is ok. */
            } else {
                if (redisContextWaitReady(c,timeout) != REDIS_OK)
                    return REDIS_ERR;
            }
        }

        /* Reset socket to be blocking after connect(2). */
        if (blocking && redisSetBlocking(c,1) != REDIS_OK)
            return REDIS_ERR;

        c->flags |= REDIS_CONNECTED;
        return REDIS_OK;
    #endif
#else
    int blocking = (c->flags & REDIS_BLOCK);
    struct sockaddr_un sa;

    if (redisCreateSocket(c,AF_LOCAL) < 0)
        return REDIS_ERR;
    if (redisSetBlocking(c,0) != REDIS_OK)
        return REDIS_ERR;

    sa.sun_family = AF_LOCAL;
    strncpy(sa.sun_path,path,sizeof(sa.sun_path)-1);
    if (connect(c->fd, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
        if (errno == EINPROGRESS && !blocking) {
            /* This is ok. */
        } else {
            if (redisContextWaitReady(c,timeout) != REDIS_OK)
                return REDIS_ERR;
        }
    }

    /* Reset socket to be blocking after connect(2). */
    if (blocking && redisSetBlocking(c,1) != REDIS_OK)
        return REDIS_ERR;

    c->flags |= REDIS_CONNECTED;
    return REDIS_OK;
#endif
}
