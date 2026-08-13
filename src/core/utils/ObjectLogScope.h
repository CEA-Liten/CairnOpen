/*
* \file      ObjectLogScope.h
*
* \brief     RAII scope guard that tags all log entries produced within its 
*            lifetime with the given CairnObject name. 
*            Usage in any SubModel / CairnObject method:

*            void MySubModel::buildModel() {
*                 CairnLogger::ObjectLogScope scope(Name());
*                 cWarning() << "something is wrong"; // ErrorEntry.objectName = Name()
*            }
*
*            Scopes are stackable - nested scopes push/pop a thread-local stack,
*            so the innermost object name is always active.
*            Thread-safe: each thread has its own independent context stack.
*
* \version  1.0
* \author   mAli KASSEM
* \date     21/07/2026
*/

#pragma once
#include "ErrorSink.h"
#include <string>

namespace CairnLogger
{
    class ObjectLogScope
    {
    public:
        explicit ObjectLogScope(const std::string& name)
        {
            pushObjectContext(name);
        }

        ~ObjectLogScope()
        {
            popObjectContext();
        }

        // Non-copyable, non-movable - lifetime must match the scope
        ObjectLogScope(const ObjectLogScope&)            = delete;
        ObjectLogScope& operator=(const ObjectLogScope&) = delete;
        ObjectLogScope(ObjectLogScope&&)                 = delete;
        ObjectLogScope& operator=(ObjectLogScope&&)      = delete;
    };
}

// Convenience macro - placed at the top of any CairnObject method
#define CAIRN_LOG_SCOPE(name) \
    CairnLogger::ObjectLogScope _cairn_log_scope_(name)
