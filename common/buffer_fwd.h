#ifndef BUFFER_FWD_H
#define BUFFER_FWD_H

namespace buffer {
class ptr;
class list;
class hash;
}

using bufferptr = buffer::ptr;
using bufferlist = buffer::list;
using bufferhash = buffer::hash;

#endif // BUFFER_FWD_H
