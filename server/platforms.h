#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

/* Conveniance macro for error handling */
#define DIEP(fmt, ...) diep(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

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
	#define snprintf_s _snprintf_s
	typedef int socklen_t;
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <fcntl.h>
	#define INVALID_SOCKET (-1)
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

void diep(const char* file, int line, const char *fmt, ...) {
	const size_t BUFSIZE = 65536;
	char buf1[BUFSIZE] = { 0 };
	char buf2[BUFSIZE] = { 0 };

	va_list args;
	va_start(args, fmt);
	// Apply format to input args
	vsnprintf_s(buf1, sizeof(buf1), fmt, args);
	va_end(args);

	// Prepend file and line number to message
	snprintf_s(buf2, sizeof(buf2), "Source file '%s', line %d:\n%s", file, line, buf1);

	#if PLATFORM == PLATFORM_WINDOWS
		LPVOID errorString;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, (LPTSTR)&errorString, 0, NULL);
		MessageBox(NULL, (LPCWSTR)errorString, L"Critical Error", MB_OK | MB_ICONERROR);
	#else
		perror(buf2);
	#endif
	
	exit(1);
}

// vim:ts=4
