#ifndef UTILITY_PROCESS_TERMINATE_H
#define UTILITY_PROCESS_TERMINATE_H

#include "Utility/DllExport.h"
#include "Utility/String/Type.h"

namespace utility {
namespace process {
DllExport bool terminateProcess(const utility::string::Str &name);
} // namespace process
} // namespace utility

#endif // UTILITY_PROCESS_TERMINATE_H
