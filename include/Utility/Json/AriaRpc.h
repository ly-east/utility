#ifndef UTILITY_JSON_ARIARPC_H
#define UTILITY_JSON_ARIARPC_H

#include "Utility/DllExport.h"
#include "Utility/Json/AriaRpcEnum.h"
#include <initializer_list>
#include <string>
#include <utility>

namespace utility {
namespace aria {
struct StatusResult;
} // namespace aria
} // namespace utility

namespace utility {
namespace aria {
using RpcArgTy = std::initializer_list<std::pair<std::string, std::string>>;

DllExport std::string getRpcRequest(Method method, const std::string &param,
                                    const RpcArgTy &option = {});

// Extract `GID` in `response`
// @return true if succeeded, or false if failed
DllExport bool getRpcResponseGid(const std::string &response, std::string &gid);

// Construct `StatusResult` by `response`
// @return true if succeeded, or false if failed
DllExport bool getRpcResponseStatus(const std::string &response,
                                    StatusResult &status);
} // namespace aria
} // namespace utility

#endif // UTILITY_JSON_ARIARPC_H
