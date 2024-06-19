#include "Utility/Crypto/Hash.h"
#include "openssl/evp.h"
#include "openssl/md5.h"
#include "spdlog/spdlog.h"
#include <iomanip>
#include <sstream>

namespace utility {
namespace crypto {
std::string getMD5(const std::string &str) {
  unsigned char hash[MD5_DIGEST_LENGTH];
  MD5((unsigned char *)str.c_str(), str.size(), hash);

  std::stringstream ss;
  for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

  return ss.str();
}
} // namespace crypto
} // namespace utility
