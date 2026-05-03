#include "Utility/Network/Socket.h"
#include "ulog/ulog.h"

#if defined(_WIN32)
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#elif defined(__linux__)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
namespace {
#if defined(_WIN32)
uint16_t getAvailablePortWindows(uint16_t port_begin, int type) {
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    ulg.error("WSAStartup failed with error: {}", iResult);
    return UINT16_MAX;
  }

  SOCKET sock = INVALID_SOCKET;
  sockaddr_in service;
  uint16_t port_available = UINT16_MAX;

  for (uint16_t portnum = port_begin; portnum < UINT16_MAX; ++portnum) {
    sock = socket(AF_INET, type, 0);
    if (sock == INVALID_SOCKET) {
      ulg.error("socket() failed with error: {}", WSAGetLastError());
      break;
    }

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(portnum);

    // try to bind current port
    if (bind(sock, (SOCKADDR *)&service, sizeof(service)) == SOCKET_ERROR) {
      int error = WSAGetLastError();
      if (error == WSAEADDRINUSE) {
        // current port has been occupied
        sock = INVALID_SOCKET;
      } else
        ulg.error("bind() failed with error: {}", error);
      closesocket(sock);
    } else {
      // an available TCP port is found
      port_available = portnum;
      break;
    }
  }

  if (sock != INVALID_SOCKET)
    closesocket(sock);

  WSACleanup();
  return port_available;
}
#elif defined(__linux__)
uint16_t getAvailablePortLinux(uint16_t port_begin, int type) {
  unsigned short local_port;
  struct sockaddr_in sin;
  int addrlen = sizeof(sin);

  // Create a socket
  int sd = socket(AF_INET, type, IPPROTO_TCP);
  if (sd == -1) {
    // Handle error
    return 0;
  }

  // Bind the socket
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(port_begin);
  if (bind(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
    // Handle error
    close(sd);
    return 0;
  }

  // Read the binding to get the port number
  getsockname(sd, (struct sockaddr *)&sin, (socklen_t *)&addrlen);
  local_port = ntohs(sin.sin_port);

  close(sd);
  return local_port;
}
#endif
} // namespace

namespace utility {
namespace network {
uint16_t getAvailableTcpPort(uint16_t port_begin) {
#if defined(_WIN32)
  return getAvailablePortWindows(port_begin, SOCK_STREAM);
#elif defined(__linux__)
  return getAvailablePortLinux(port_begin, SOCK_STREAM);
#endif
}
} // namespace network
} // namespace utility
