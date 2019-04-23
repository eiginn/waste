/*
  Copyright (c) 1998-2003 VIA Technologies, Inc.

  This program is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTIES OR REPRESENTATIONS; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
*/
/*
 * This file implements listview control
 * Author: Stanley San
 * Date Created: 2003.12.1
*/
#ifndef _QT_LISTVIEW_H_
#define _QT_LISTVIEW_H_

#ifdef QTUI
#include <qlistview.h>

class VUser_TreeViewItem : public QListViewItem
{
public:
    VUser_TreeViewItem( QListView* parent , char* text, int param,const char* icon=0);
    VUser_TreeViewItem( QListView* parent , QListViewItem* after, char* text ,int param);
    ~VUser_TreeViewItem();

    int m_param;
};


class W_ListView 
{
#define HWND QListView*

  public:
    W_ListView() { m_hwnd=NULL; m_sort_type=0; m_sort_col=0; m_sort_dir=1; m_col=0; }
    W_ListView(HWND hwnd) { m_hwnd=hwnd; m_col=0; }
    ~W_ListView() { };

    void setwnd(HWND hwnd)
    { 
      m_hwnd=hwnd; 
      m_hwnd->setSorting(-1);
      //ListView_SetExtendedListViewStyle(m_hwnd,LVS_EX_FULLROWSELECT);
    }

    //add for icon(frank) , export my internal to frank:(
    VUser_TreeViewItem* GetItem(int p);

    void AddCol(char *text, int w);
    int GetCount(void);
    int GetParam(int p);
    void DeleteItem(int n);
    void Clear(void);
    int GetSelected(int x); 

    void SetSelected(int x);
    int InsertItem(int p, char *text, int param);
    int InsertItemSorted(char *text, int param, char *sorttext);
    void SetItemText(int p, int si, char *text);
    void SetItemParam(int p, int param);

    void GetText(int p, int si, char *text, int maxlen); 

    int FindItemByParam(int param);

    void SetSortColumn(int col, int bNumeric=0) 
    { 
      if (col == m_sort_col) m_sort_dir=-m_sort_dir;
      else m_sort_dir=1;
      m_sort_col=col; 
      m_sort_type=bNumeric; 
    }
    void SetSortDir(int dir) { m_sort_dir=dir; }
    int GetSortColumn(void) { return m_sort_col; }
    int GetSortDir(void) { return m_sort_dir; }
    int GetColumnWidth(int col);
    void Resort(void);
    HWND getwnd(void) { return m_hwnd; }
//Dump Whole ListView
    void DumpList(void);

  protected:
    HWND m_hwnd;
    int m_col;
    int m_sort_col;
    int m_sort_dir;
    int m_sort_type;

typedef int LPARAM ; 
    static int /*CALLBACK*/ sortfunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 
    int _docompare(char *s1, char *s2);
};
#endif
#endif//_LISTVIEW_H_

