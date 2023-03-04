#pragma once
extern int m_errno;

enum {
    NAN_ERR = 1,
    OVERFLOW_ERR,
    ENTRY_NOT_FOUND_ERR,
    SMALL_BUF_ERR,
};