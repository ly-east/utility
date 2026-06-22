#ifndef UTILITY_JSON_ARIARPCENUM_H
#define UTILITY_JSON_ARIARPCENUM_H

namespace utility {
namespace aria {
enum class Method : unsigned { ADD_URI, PAUSE, UNPAUSE, REMOVE, TELL_STATUS };
enum class Key : unsigned { JSONRPC, ID, METHOD, PARAMS };
enum class Value : unsigned { JSONRPC, ID };
} // namespace aria
} // namespace utility

#endif // UTILITY_JSON_ARIARPCENUM_H
