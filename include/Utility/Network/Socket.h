#ifndef UTILITY_NETWORK_SOCKET_H
#define UTILITY_NETWORK_SOCKET_H

#include "Utility/DllExport.h"
#include <cstdint>

namespace utility {
namespace network {
DllExport uint16_t getAvailableTcpPort(uint16_t port_begin = 10305);
} // namespace network
} // namespace utility

#endif // UTILITY_NETWORK_SOCKET_H
