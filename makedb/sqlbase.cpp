
/*
 * CodeQuery
 * Copyright (C) 2013 ruben2020 https://github.com/ruben2020/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "sqlbase.h"

sqlbase::sqlbase()
        :m_db(NULL)
        ,m_debug(false)
{
}

sqlbase::~sqlbase()
{
    sqlite3_close(m_db);
}

void sqlbase::setDebug(bool val)
{
    m_debug = val;
}

int sqlbase::vacuum(const char* fn, const bool& debug)
{
    int rc;
    sqlite3 *sqdb;
    rc = sqlite3_open_v2(fn, &sqdb, SQLITE_OPEN_READWRITE, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(sqdb);
        return rc;
    }
    rc=sqlite3_exec(sqdb, "PRAGMA synchronous = OFF;"
                    "PRAGMA journal_mode = OFF;"
                    "PRAGMA locking_mode = EXCLUSIVE;"
                    "PRAGMA automatic_index = FALSE;"
                    "PRAGMA cache_size = 20000;"
                    "VACUUM;ANALYZE;", NULL, 0, NULL);
    if (rc != SQLITE_OK)
    {
        if (debug) printf("SQLBaseErr099: %d, %s\n", rc, sqlite3_errmsg(sqdb));
    }
    sqlite3_close(sqdb);
    return rc;
}

int sqlbase::analyze(const char* fn, const bool& debug)
{
    int rc;
    sqlite3 *sqdb;
    rc = sqlite3_open_v2(fn, &sqdb, SQLITE_OPEN_READWRITE, NULL);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(sqdb);
        return rc;
    }
    rc=sqlite3_exec(sqdb, "PRAGMA synchronous = OFF;"
                    "PRAGMA journal_mode = OFF;"
                    "PRAGMA locking_mode = EXCLUSIVE;"
                    "PRAGMA automatic_index = FALSE;"
                    "PRAGMA cache_size = 20000;"
                    "ANALYZE;", NULL, 0, NULL);
    if (rc != SQLITE_OK)
    {
        if (debug) printf("SQLBaseErr100: %d, %s\n", rc, sqlite3_errmsg(sqdb));
    }
    sqlite3_close(sqdb);
    return rc;
}

int sqlbase::prepare_stmt(sqlite3_stmt** pStmt, const char* sqlquery)
{
    int rc;
    rc = sqlite3_prepare_v2(m_db, sqlquery, strlen(sqlquery), pStmt, NULL);
    if (rc != SQLITE_OK)
    {
        if (m_debug) printf("SQLBaseErr001: %d, %s, line=%s\n", rc, sqlite3_errmsg(m_db), sqlquery);
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

int sqlbase::execstmt(sqlite3_stmt* pstmt, const char* v1)
{
    strvect_t v;
    v.clear();
    v.push_back(v1);
    return execstmt(pstmt,v);
}

int sqlbase::execstmt(sqlite3_stmt* pstmt, const char* v1, const char* v2)
{
    strvect_t v;
    v.clear();
    v.push_back(v1);
    v.push_back(v2);
    return execstmt(pstmt,v);
}

int sqlbase::execstmt(sqlite3_stmt* pstmt, const char* v1, const char* v2, const char* v3)
{
    strvect_t v;
    v.clear();
    v.push_back(v1);
    v.push_back(v2);
    v.push_back(v3);
    return execstmt(pstmt,v);
}

int sqlbase::execstmt(sqlite3_stmt* pstmt, const char* v1, const char* v2, const char* v3, const char* v4)
{
    strvect_t v;
    v.clear();
    v.push_back(v1);
    v.push_back(v2);
    v.push_back(v3);
    v.push_back(v4);
    return execstmt(pstmt,v);
}

int sqlbase::execstmt(sqlite3_stmt* pstmt, const char* v1, const char* v2, const char* v3, const char* v4, const char* v5)
{
    strvect_t v;
    v.clear();
    v.push_back(v1);
    v.push_back(v2);
    v.push_back(v3);
    v.push_back(v4);
    v.push_back(v5);
    return execstmt(pstmt,v);
}

int sqlbase::execstmt(sqlite3_stmt* pstmt, strvect_t &vecstr)
{
    int rc;
    rc = sqlite3_reset(pstmt);
    if (rc != SQLITE_OK)
    {
        if (m_debug)
            printf("%s:%d %d, %s\n", __FUNCTION__,__LINE__,rc, sqlite3_errmsg(m_db));
        return SQLITE_ERROR;
    }
    //printf("%s:%d BIND ",__FUNCTION__,__LINE__);
    for (unsigned int i=0; i<vecstr.size(); i++)
    {
        rc = sqlite3_bind_text(pstmt, i+1, (char *)vecstr[i].c_str(), vecstr[i].length(), SQLITE_STATIC);
        //printf("(%d:%s) ",(i+1),vecstr[i].c_str());
        if (rc != SQLITE_OK)
        {
            if (m_debug)
                printf("%s:%d %d, %s\n", __FUNCTION__,__LINE__,rc, sqlite3_errmsg(m_db));
            return SQLITE_ERROR;
        }
    }
    //printf("\n");
    rc = sqlite3_step(pstmt);
    if ((rc == SQLITE_ROW)||(rc == SQLITE_BUSY)) return rc;
    if (rc != SQLITE_DONE)
    {
        if (m_debug) printf("%s:%d  %d, %s\n", __FUNCTION__,__LINE__,rc, sqlite3_errmsg(m_db));
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

