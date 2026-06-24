#ifndef UTILITY_FILESYSTEM_FILESYSTEM_H
#define UTILITY_FILESYSTEM_FILESYSTEM_H

#include "Utility/DllExport.h"
#include <filesystem>
#include <initializer_list>

namespace utility {
namespace filesystem {
using PathListTy = std::initializer_list<std::filesystem::path>;

DllExport void createDirectory(const std::filesystem::path &path);

// remove: The file or empty directory identified by the `path` is deleted
DllExport bool remove(const std::filesystem::path &path);
DllExport bool remove(const PathListTy &path);

// removeAll: Deletes the contents of `path` (if it is a directory) and the
// contents of all its subdirectories, recursively, then deletes `path` itself
DllExport bool removeAll(const std::filesystem::path &path);
DllExport bool removeAll(const PathListTy &path);

// select specified file or folder in Explorer(Windows).
// @param path: MUST be encoded by UTF-8, and the seperator MUST be '\' instead
// of '/'.
DllExport bool select(const std::filesystem::path &path);
} // namespace filesystem
} // namespace utility

#endif // UTILITY_FILESYSTEM_FILESYSTEM_H
