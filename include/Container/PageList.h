#ifndef UTILITY_CONTAINER_PAGELIST_H
#define UTILITY_CONTAINER_PAGELIST_H

#include "DllExport.h"
#include "Type/ID.h"
#include "Type/Page.h"
#include "Type/View.h"

namespace utility {
namespace container {
DllExport bbdown::PageList::iterator Find(bbdown::PageList &list,
                                          const bbdown::CID cid);
} // namespace container
} // namespace utility

#endif // UTILITY_CONTAINER_PAGELIST_H
