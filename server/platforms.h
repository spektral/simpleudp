#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
	#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
	#define PLATFORM PLATFORM_MAC
#else
	#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
	#pragma comment(lib, "wsock32.lib")
	#include <winsock2.h>
	typedef int socklen_t;
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <fcntl.h>
	#define INVALID_SOCKET -1
#endif

inline bool initializeSocket() {
	#if PLATFORM == PLATFORM_WINDOWS
		WSAData wsaData;
		return (WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR);
	#else
		return true;
	#endif
}

inline void shutdownSocket() {
	#if PLATFORM == PLATFORM_WINDOWS
		WSACleanup();
	#endif
}

inline bool setNonBlocking(int sockfd) {
	#if PLATFORM == PLATFORM_UNIX
		if (fcntl(sockfd, F_SETFD, O_NONBLOCK) == -1)
			return false;
	#elif PLATFORM == PLATFORM_WINDOWS
		DWORD nonBlocking = 1;
		if (ioctlsocket(sockfd, FIONBIO, &nonBlocking) != 0)
			return false;
	#endif
	return true;
}

inline bool eWouldBlock() {
    #if PLATFORM == PLATFORM_WINDOWS
        return (WSAGetLastError() == WSAEWOULDBLOCK);
    #else
        return (errno == EWOULDBLOCK);
    #endif
}

// vim:ts=4
