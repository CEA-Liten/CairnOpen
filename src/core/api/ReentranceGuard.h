#pragma once

struct ReentranceGuard
{
    explicit ReentranceGuard(bool& flag) : m_flag(flag)
    {
        m_flag = true;
    }

    ~ReentranceGuard()
    {
        m_flag = false; // always resets - even on exception
    }

    // Non-copyable, non-movable
    ReentranceGuard(const ReentranceGuard&)            = delete;
    ReentranceGuard& operator=(const ReentranceGuard&) = delete;
    ReentranceGuard(ReentranceGuard&&)                 = delete;
    ReentranceGuard& operator=(ReentranceGuard&&)      = delete;

private:
    bool& m_flag; // reference to a flag
};