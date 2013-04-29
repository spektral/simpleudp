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
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

bool initializeSocket() {
#if PLATFORM == PLATFORM_WINDOWS
	WSAData wsaData;
	return (WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR);
#else
	return true;
#endif
}

void shutdownSocket() {
#if PLATFORM == PLATFORM_WINDOWS
	WSACleanup();
#endif
}