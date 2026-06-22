#ifndef UTILITY_FILESYSTEM_DUMPFILE_H
#define UTILITY_FILESYSTEM_DUMPFILE_H

#include "DllExport.h"
#include <fstream>
#include <string>

namespace utility {
namespace filesystem {
// Write content to specified file
DllExport bool
DumpFile(const std::string &file_name, const std::string &content,
         const std::ios_base::openmode mode = std::ios_base::out);

DllExport bool CentralizedDump(const std::string &folder,
                               const std::string &filename,
                               const std::string &content);
} // namespace filesystem
} // namespace utility

#endif // UTILITY_FILESYSTEM_DUMPFILE_H
