#include "Utility/Network/Socket.h"
#include "spdlog/spdlog.h"
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace utility {
namespace network {
uint16_t getAvailablePort(uint16_t port_begin, int type) {
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    spdlog::error("WSAStartup failed with error: {}", iResult);
    return UINT16_MAX;
  }

  SOCKET sock = INVALID_SOCKET;
  sockaddr_in service;
  uint16_t port_available = UINT16_MAX;

  for (uint16_t portnum = port_begin; portnum < UINT16_MAX; ++portnum) {
    sock = socket(AF_INET, type, 0);
    if (sock == INVALID_SOCKET) {
      spdlog::error("socket() failed with error: {}", WSAGetLastError());
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
        closesocket(sock);
        sock = INVALID_SOCKET;
        continue;
      } else {
        spdlog::error("bind() failed with error: {}", error);
        break;
      }
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
} // namespace network
} // namespace utility
