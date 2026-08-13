/*
* \file      ObjectLogScope.cpp
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

#include "ObjectLogScope.h"
#include <vector>

namespace CairnLogger
{
    namespace
    {
        // Thread-local stack - supports nested scopes (e.g. SubModel inside MilpComponent)
        thread_local std::vector<std::string> gContextStack;
        thread_local std::string gEmptyContext;
    }

    const std::string& currentObjectContext()
    {
        return gContextStack.empty() ? gEmptyContext : gContextStack.back();
    }

    void pushObjectContext(const std::string& name)
    {
        gContextStack.push_back(name);
    }

    void popObjectContext()
    {
        if (!gContextStack.empty())
            gContextStack.pop_back();
    }
}