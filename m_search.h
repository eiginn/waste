/*
    WASTE - m_search.h (Search/browse message builder/parser class)
    Copyright (C) 2003 Nullsoft, Inc.

    WASTE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    WASTE  is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WASTE; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _C_MSEARCH_H_
#define _C_MSEARCH_H_

#include "shbuf.h"
#include "util.h"

#define SEARCHREPLY_MAX_METASIZE 128
#define SEARCHREPLY_MAX_FILESIZE 512

class C_MessageSearchReply
{
  public:
    C_MessageSearchReply();
    C_MessageSearchReply(C_SHBuf *in);
    ~C_MessageSearchReply();

    C_SHBuf *Make(void);

    int get_conspeed(void) { return m_conspeed; }
    int get_numitems(void) { return m_numitems; }
    T_GUID *get_guid(void) { return &m_guid; }

    void set_conspeed(int speed) { m_conspeed=speed; }
    void set_guid(T_GUID *guid) { m_guid=*guid; }

    void clear_items(void);
    int would_fit(char *name, char *metadata);
    void add_item(int id, char *name, char *metadata, int length_bytes_low, int length_bytes_high, int file_time);
    int get_item(int index, int *id, char *name, char *metadata, int *length_bytes_low, int *length_bytes_high, int *file_time);
    void delete_item(int index);
    int find_item(char *name, char *meta, int maxitems);

    void addsize(int item, int add_low, int add_hi, int newt)
    {
      if (m_replies && item >= 0 && item < m_numitems)
      {
        m_replies[item].length_bytes_low+=add_low;
        m_replies[item].length_bytes_high+=add_hi;
        if (newt > m_replies[item].file_time) m_replies[item].file_time=newt;
      }
    }

    void addlastsize(int add_low, int add_hi, int newt)
    {
      if (m_replies && m_numitems)
      {
        m_replies[m_numitems-1].length_bytes_low+=add_low;
        m_replies[m_numitems-1].length_bytes_high+=add_hi;
        if (newt > m_replies[m_numitems-1].file_time) m_replies[m_numitems-1].file_time=newt;
      }
    }

    int _latency; // user var
    char _guidstr[33];

  protected:
    T_GUID m_guid;
    typedef struct
    {
      int id;
      int length_bytes_low,length_bytes_high;
      int file_time;
      char name[SEARCHREPLY_MAX_FILESIZE];
      char metadata[SEARCHREPLY_MAX_METASIZE];
    } ReplyEntry;

    int m_conspeed;
    int m_numitems;
    int m_size;

    ReplyEntry *m_replies;

};

class C_MessageSearchRequest
{
  public:
    C_MessageSearchRequest();
    C_MessageSearchRequest(C_SHBuf *in);
    ~C_MessageSearchRequest();

    C_SHBuf *Make(void);

    char *get_searchstring(void) { return m_searchstring; }
    int get_min_conspeed(void) { return m_min_conspeed; }

    void set_min_conspeed(int speed) { m_min_conspeed=speed; }
    void set_searchstring(char *str) { ::strncpy(m_searchstring,str,255); m_searchstring[255]=0; }

  protected:
    int m_min_conspeed;
    char m_searchstring[256];

};

#endif//_C_MSEARCH_H_

