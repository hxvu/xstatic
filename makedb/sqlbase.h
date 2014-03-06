#ifndef _SQLBASE_H
#define _SQLBASE_H

#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <vector>
#include <string>

typedef std::vector<std::string> strvect_t;
class sqlbase
{

public:
    sqlbase();
    ~sqlbase();
    void setDebug(bool val);
    static int vacuum(const char* fn, const bool& debug);
    static int analyze(const char* fn, const bool& debug);
    int prepare_stmt(sqlite3_stmt** pStmt, const char* sqlquery);
    int execstmt(sqlite3_stmt* pstmt, const char* v1);
    int execstmt(sqlite3_stmt* pstmt, const char* v1, const char* v2);
    int execstmt(sqlite3_stmt* pstmt, const char* v1, const char* v2,
                 const char* v3);
    int execstmt(sqlite3_stmt* pstmt, const char* v1, const char* v2,
                 const char* v3, const char* v4);
    int execstmt(sqlite3_stmt* pstmt, const char* v1, const char* v2,
                 const char* v3, const char* v4, const char* v5);

    // Single
    int execstmt(sqlite3_stmt* pstmt, strvect_t &v);

protected:
    bool m_debug;
    sqlite3 *m_db;

};

#endif
