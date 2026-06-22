#ifndef UTILITY_CRYPTO_HASH_H
#define UTILITY_CRYPTO_HASH_H

#include "DllExport.h"
#include <string>

namespace utility {
namespace crypto {
DllExport std::string getMD5(const std::string &str);
}
} // namespace utility

#endif // UTILITY_CRYPTO_HASH_H
