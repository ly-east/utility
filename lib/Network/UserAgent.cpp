#include "Utility/Network/UserAgent.h"
#include "Utility/Math/Random.h"
#include <array>

namespace utility {
namespace network {
const std::string getUserAgent() {
  // refer: http://useragent.kuzhazha.com/PC/ https://www.useragents.me/
  static const std::array<std::string, 4> ua{
      // Edge on Win64
      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
      "like Gecko) Chrome/124.0.0.0 Safari/537.36 Edg/124.0.0.0",
      // Chrome on Mac
      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_3) AppleWebKit/537.36 "
      "(KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36",
      // Safari on Mac
      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 "
      "(KHTML, like Gecko) Version/17.5 Safari/605.1.1",
      // 360se
      "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like "
      "Gecko) Chrome/89.0.2790.52 Safari/537.36 QIHU 360SE"};

  return ua.at(utility::math::getRandom<size_t>(ua.size()));
}
} // namespace network
} // namespace utility
