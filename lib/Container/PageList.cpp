#include "Utility/Container/PageList.h"

namespace utility {
namespace container {
bbdown::PageList::iterator Find(bbdown::PageList &list, const bbdown::CID cid) {
  return std::find_if(list.begin(), list.end(), [cid](const bbdown::Page &rhs) {
    return rhs.cid == cid;
  });
}
} // namespace container
} // namespace utility
