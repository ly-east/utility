#ifndef UTILITY_PROCESS_LAUNCH_H
#define UTILITY_PROCESS_LAUNCH_H

#include "Utility/DllExport.h"
#include "Utility/String/Type.h"
#include <functional>
#include <future>
#include <string>

namespace utility {
namespace process {
// Type of Redirect Callback Function
using RdtCbFunc = std::function<void(std::string &&output)>;

// accepts wchar, wstring only(which indicates UTF-16)

// Get full path of specific program from current path or environment variable
// PATH.
// @return returns empty string if required program cannot be found.
DllExport utility::string::Str
getExecutableFilePath(const utility::string::Str &name);

// This function will set true/false to promise `p`(if provided) when required
// process is launched successfully or failed
// @returns Exit code of the program
// @warning `arg` must refer to an editable buffer
DllExport int launchHiddenProgram(const utility::string::Str &path,
                                  utility::string::Char *arg, RdtCbFunc func,
                                  std::promise<bool> *p = nullptr);

// use WHERE command to find program in environment variable
DllExport utility::string::Str locateProgram(const utility::string::Str &name);
} // namespace process
} // namespace utility

#endif // UTILITY_PROCESS_LAUNCH_H
