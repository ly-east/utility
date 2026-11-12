#ifndef UTILITY_TIME_UNIXTIMESTAMP_H
#define UTILITY_TIME_UNIXTIMESTAMP_H

#include "Utility/DllExport.h"
#include <cstdint>

namespace utility {
namespace time {
DllExport int64_t getUnixTimestamp();
}
} // namespace utility

#endif // UTILITY_TIME_UNIXTIMESTAMP_H
