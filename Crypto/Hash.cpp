#include "Utility/Crypto/Hash.h"
#include "Utility/String/Format.h"
#include "ulog/ulog.h"

#define USE_MD5_LEGACY 0

#if USE_MD5_LEGACY
#include "openssl/md5.h"
#else
#include "openssl/evp.h"
#endif // USE_MD5_LEGACY

namespace {
#if USE_MD5_LEGACY
static std::string getMD5Legacy(const std::string &str) {
  unsigned char hash[MD5_DIGEST_LENGTH];
  MD5((unsigned char *)str.c_str(), str.size(), hash);

  return utility::string::binToHex(hash, MD5_DIGEST_LENGTH);
}
#endif // USE_MD5_LEGACY

static std::string getMD5Modern(const std::string &str) {
  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int hash_len = 0;

  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  if (!ctx) {
    ulg.error("getMD5Modern: EVP_MD_CTX_new failed");
    return "";
  }

  bool is_succeeded = false;
  do {
    if (!EVP_DigestInit_ex(ctx, EVP_md5(), nullptr)) {
      ulg.error("EVP_DigestInit_ex failed");
      break;
    }

    if (!EVP_DigestUpdate(ctx, str.data(), str.size())) {
      ulg.error("EVP_DigestUpdate failed");
      break;
    }

    if (!EVP_DigestFinal_ex(ctx, hash, &hash_len)) {
      ulg.error("EVP_DigestFinal_ex failed");
      break;
    }

    is_succeeded = true;
  } while (false);

  EVP_MD_CTX_free(ctx);

  return is_succeeded ? utility::string::binToHex(hash, hash_len)
                      : std::string();
}
} // namespace

namespace utility {
namespace crypto {
std::string getMD5(const std::string &str) {
  if (str.empty()) {
    ulg.error("getMD5: empty string?");
    return std::string();
  }

#if USE_MD5_LEGACY
  return getMD5Legacy(str);
#else
  return getMD5Modern(str);
#endif // USE_MD5_LEGACY
}
} // namespace crypto
} // namespace utility
