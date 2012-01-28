#ifndef NotImplementedException_h__
#define NotImplementedException_h__

#include "Exception.h"

namespace Util
{
    class NotImplementedException : public Exception
    {
    public:
        NotImplementedException(const QString& message) : Exception(message)
        {
        }

        NotImplementedException()
        {
        }
    };
}

#endif // NotImplementedException_h__