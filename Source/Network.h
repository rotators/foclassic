#ifndef __NETWORK__
#define __NETWORK__

#ifdef FO_WINDOWS
# include <winsock2.h>
# define socklen_t         int
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# define SOCKET            int
# define INVALID_SOCKET    (-1)
# define SOCKET_ERROR      (-1)
# define closesocket       close
# define SD_RECEIVE        SHUT_RD
# define SD_SEND           SHUT_WR
# define SD_BOTH           SHUT_RDWR
#endif

extern const char* GetLastSocketError();

#endif // __NETWORK__ //
