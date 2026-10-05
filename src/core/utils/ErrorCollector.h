/**
* \file		ErrorCollector.h
* \brief    A interface for ErrorSink
* \version	1.0
* \author	Ali Kassem
* \date		23/06/2026
*/

#pragma once
#include "ErrorSink.h"
#include <vector>
#include <string>

namespace CairnLogger
{
    class ErrorCollector
    {
    public:
        // Flush all collected warnings and errors and clear the sink
        static std::vector<ErrorEntry> flush()
        {
            return ErrorSink::instance()->getAndClear();
        }

        // Check if any warnings occurred since last flush
        static bool hasWarnings()
        {
            return ErrorSink::instance()->hasWarnings();
        }

        // Check if any errors occurred since last flush
        static bool hasErrors()
        {
            return ErrorSink::instance()->hasErrors();
        }

        // Get warning count without clearing
        static int warningCount()
        {
            return ErrorSink::instance()->warningCount();
        }

        // Get error count without clearing
        static int errorCount()
        {
            return ErrorSink::instance()->errorCount();
        }

        // Get warning and error count without clearing
        static int count() 
        {
            return ErrorSink::instance()->count();
        }

        // Clear without returning
        static void clear()
        {
            ErrorSink::instance()->clear();
        }
    };
}