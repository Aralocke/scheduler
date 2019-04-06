#pragma once

#include <iosfwd>

namespace Scheduler {

    enum Error
    {
      E_FAILURE = -1,
      E_SUCCESS = 0
    };

    const char* ErrorToStr(Error e);

    std::ostream& operator<<(std::ostream& o, Error e);

}  // namespace Scheduler