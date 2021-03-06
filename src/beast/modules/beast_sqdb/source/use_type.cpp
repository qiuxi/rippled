//------------------------------------------------------------------------------
/*
    This file is part of Beast: https://github.com/vinniefalco/Beast
    Copyright 2013, Vinnie Falco <vinnie.falco@gmail.com>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

    Portions based on SOCI - The C++ Database Access Library: 

    SOCI: http://soci.sourceforge.net/

    This file incorporates work covered by the following copyright
    and permission notice:

    Copyright (C) 2004 Maciej Sobczak, Stephen Hutton, Mateusz Loskot,
    Pawel Aleksander Fedorynski, David Courtney, Rafal Bobrowski,
    Julian Taylor, Henning Basold, Ilia Barahovski, Denis Arnaud,
    Daniel Lidstr�m, Matthieu Kermagoret, Artyom Beilis, Cory Bennett,
    Chris Weed, Michael Davidsaver, Jakub Stachowski, Alex Ott, Rainer Bauer,
    Martin Muenstermann, Philip Pemberton, Eli Green, Frederic Chateau,
    Artyom Tonkikh, Roger Orr, Robert Massaioli, Sergey Nikulov,
    Shridhar Daithankar, S�ren Meyer-Eppler, Mario Valesco.

    Boost Software License - Version 1.0, August 17th, 2003

    Permission is hereby granted, free of charge, to any person or organization
    obtaining a copy of the software and accompanying documentation covered by
    this license (the "Software") to use, reproduce, display, distribute,
    execute, and transmit the Software, and to prepare derivative works of the
    Software, and to permit third-parties to whom the Software is furnished to
    do so, all subject to the following:

    The copyright notices in the Software and this entire statement, including
    the above license grant, this restriction and the following disclaimer,
    must be included in all copies of the Software, in whole or in part, and
    all derivative works of the Software, unless such copies or derivative
    works are solely in the form of machine-executable object code generated by
    a source language processor.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/
//==============================================================================

namespace sqdb
{

namespace detail
{

standard_use_type::~standard_use_type()
{
}

void standard_use_type::bind(statement_imp& st, int& iParam)
{
    m_st = &st;
    m_iParam = iParam++;
}

namespace
{

template<typename T>
inline T const& as(void const* v)
{
    return *static_cast <T const*>(v);
}

template<typename T, typename L>
inline T const& as(void const* v)
{
    T const& val = *static_cast <T const*>(v);

    check_precondition (val <= T(std::numeric_limits<L>::max()));

    return val;
}

}

void standard_use_type::do_use()
{
    int result = SQLITE_OK;

    convert_to_base();

    switch (m_type)
    {
    case x_null:
        result = sqlite3_bind_null(m_st->m_stmt, m_iParam);
        break;

    case x_bool:
        result = sqlite3_bind_int(m_st->m_stmt, m_iParam, as <bool> (m_data) ? 1 : 0);
        break;

    case x_char:
        result = sqlite3_bind_int(m_st->m_stmt, m_iParam, as <char> (m_data));
        break;

    case x_short:
        result = sqlite3_bind_int(m_st->m_stmt, m_iParam, as <short> (m_data));
        break;

    case x_int:
        result = sqlite3_bind_int(m_st->m_stmt, m_iParam, as <int> (m_data));
        break;

    case x_long:
        result = sqlite3_bind_int(m_st->m_stmt, m_iParam, as <long> (m_data));
        break;

    case x_int64:
        result = sqlite3_bind_int64(m_st->m_stmt, m_iParam, as <int64> (m_data));
        break;

    case x_uchar:
        result = sqlite3_bind_int(m_st->m_stmt, m_iParam, as <unsigned char> (m_data));
        break;

    case x_ushort:
        result = sqlite3_bind_int(m_st->m_stmt, m_iParam, as <unsigned short> (m_data));
        break;

    case x_uint:
        result = sqlite3_bind_int64(m_st->m_stmt, m_iParam, as <unsigned int> (m_data));
        break;

    case x_ulong:
        result = sqlite3_bind_int64(m_st->m_stmt, m_iParam, as <unsigned long> (m_data));
        break;

    case x_uint64:
        result = sqlite3_bind_int64(m_st->m_stmt, m_iParam, as <sqlite3_uint64, sqlite3_int64> (m_data));
        break;

    case x_float:
        result = sqlite3_bind_double(m_st->m_stmt, m_iParam, as <float> (m_data));
        break;

    case x_double:
        result = sqlite3_bind_double(m_st->m_stmt, m_iParam, as <double> (m_data));
        break;

    case x_cstring:
        result = sqlite3_bind_text(m_st->m_stmt, m_iParam, as <char*> (m_data), -1, SQLITE_STATIC);
        break;

    case x_cwstring:
        result = sqlite3_bind_text16(m_st->m_stmt, m_iParam, as <wchar_t*> (m_data), -1, SQLITE_STATIC);
        break;

    case x_stdstring:
    {
        std::string const& s = as <std::string> (m_data);
        result = sqlite3_bind_text(m_st->m_stmt,
                                   m_iParam,
                                   s.c_str(),
                                   s.size() * sizeof(s[0]),
                                   SQLITE_STATIC);
    }
    break;

    case x_stdwstring:
    {
        std::wstring const& s = as <std::wstring> (m_data);
        result = sqlite3_bind_text16(m_st->m_stmt,
                                     m_iParam,
                                     s.c_str(),
                                     s.size() * sizeof(s[0]),
                                     SQLITE_STATIC);
    }
    break;

    case x_beastString:
    {
        String const& s = as <String> (m_data);
        result = sqlite3_bind_text(m_st->m_stmt,
                                   m_iParam,
                                   s.toUTF8(),
                                   -1,
                                   SQLITE_STATIC);
    }
    break;

    case x_stdtm:
    case x_blob:
    default:
        fatal_error ("bad parameter");
    }

    if (result != SQLITE_OK)
        Throw(detail::sqliteError(__FILE__, __LINE__, result));
}

void standard_use_type::post_use()
{
    convert_from_base();
}

void standard_use_type::clean_up()
{
}

}

}
