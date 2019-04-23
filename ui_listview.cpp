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

#include "platform.h"
#include "ui_listview.h"
#include "util.h"

#include <qpixmap.h>

//#define _LV_DEBUG 1

VUser_TreeViewItem::VUser_TreeViewItem( QListView* parent ,char* text, int param,const char* icon)
	:QListViewItem(parent,text)
{
    m_param = param;
    if(icon)
    {
	QPixmap pix(icon);
	setPixmap(0,pix);
    }
}

VUser_TreeViewItem::~VUser_TreeViewItem()
{
}

VUser_TreeViewItem::VUser_TreeViewItem( QListView* parent ,QListViewItem* after,char* text, int param)
	:QListViewItem(parent,after,text)
{
    m_param = param;
}


int W_ListView::GetCount(void)
{
    if(m_hwnd)
	return	m_hwnd->childCount();

    return 0;
}

void W_ListView::AddCol(char *text, int w)
{
    if(m_hwnd)
	m_hwnd->addColumn(text,w);
    m_col++;
}

int W_ListView::GetColumnWidth(int col)
{
  if (col < 0 || col >= m_col) return 0;
    if(m_hwnd)
	return m_hwnd->columnWidth(col);
    return 0;
}


int W_ListView::GetParam(int p)
{
    if(m_hwnd)
    {
	int loop = 0;
	VUser_TreeViewItem* pItem = (VUser_TreeViewItem*)m_hwnd->firstChild();
	for(loop=0;loop<p;loop++,pItem = (VUser_TreeViewItem*)pItem->nextSibling())
	{
		if(!pItem)
			break;
	
	}
	if(loop<p)
	{
		debug_printf("W_ListView::GetParam error inefficient num\n");
		return 0;
	}

	return pItem->m_param;
    }

  return 0;
}

int W_ListView::InsertItem(int p, char *text, int param)
{
    debug_printf("W_ListView::InsertItem p=%d,text=%s,param=%d\n",p,text,param);

    if(p<0)
    {
	debug_printf("W_ListView::InsertItem error  p num\n");
	return -1;
    }
	
    if(m_hwnd)
    {
	int loop = 0;
	VUser_TreeViewItem* pItem = (VUser_TreeViewItem*)m_hwnd->firstChild();
	VUser_TreeViewItem* pBackItem = pItem;
	for(loop=0;loop<p;loop++,pItem = (VUser_TreeViewItem*)pItem->nextSibling())
	{
		if(!pItem)
			break;
		pBackItem = pItem;
	}
	if(loop<p)
	{
		debug_printf("W_ListView::InsertItem error inefficient num\n");
		return -1;
	}

	VUser_TreeViewItem* pNewIt = NULL;
	if(pItem)
	{
		pNewIt = new VUser_TreeViewItem( m_hwnd , pItem , text , param);
		pItem->moveItem(pNewIt);
	}
	else
	{
		pNewIt = new VUser_TreeViewItem( m_hwnd , pBackItem , text  , param);
	}
	if(!pNewIt)
	{
		debug_printf("W_ListView::InsertItem error generate Item\n");
		return -1;
	}

#if _LV_DEBUG
	DumpList();
#endif
    	return p;
    }

    return -1;
}

void W_ListView::GetText(int p, int si, char *text, int maxlen)
{
    if(p<0)
    {
	debug_printf("W_ListView::GetText error  p num\n");
	return ;
    }

    if(NULL==text)
    {
	debug_printf("W_ListView::GetText error text=NULL\n");
	return ;
    }

    if(m_hwnd)
    {
	int loop = 0;
	VUser_TreeViewItem* pItem = (VUser_TreeViewItem*)m_hwnd->firstChild();
	for(loop=0;loop<p;loop++,pItem = (VUser_TreeViewItem*)pItem->nextSibling())
	{
		if(!pItem)
			break;
	
	}
	if(!pItem)
	{
		debug_printf("W_ListView::GetText error inefficient num\n");
		return ;
	}
	
	if(!pItem->text(si).isNull())
		strncpy(text,pItem->text(si).ascii(),maxlen);
    }
	
}

int W_ListView::InsertItemSorted(char *text, int param, char *sorttext)
{
  int l=GetCount();
  int x;
  for (x = 0; x < l; x ++)
  {
    char thetext[512];
    GetText(x,m_sort_col,thetext,sizeof(thetext));
    if (_docompare(sorttext,thetext)<0)
    {
      break;
    }
  }
  return InsertItem(x,text,param);
}

void W_ListView::SetItemText(int p, int si, char *text)
{
#ifdef _LV_DEBUG
    qDebug("W_ListView::SetItemText() p=%d,si=%d,text=%s",p,si,text);
#endif

    if(p<0)
    {
	debug_printf("W_ListView::SetItem error  p num\n");
	return ;
    }

    if(m_hwnd)
    {
	int loop = 0;
	VUser_TreeViewItem* pItem = (VUser_TreeViewItem*)m_hwnd->firstChild();
	for(loop=0;loop<p;loop++,pItem = (VUser_TreeViewItem*)pItem->nextSibling())
	{
		if(!pItem)
			break;
	
	}
	if(!pItem)
	{
		debug_printf("W_ListView::SetItemText error inefficient num\n");
		return ;
	}
	pItem->setText(si,text);

#if _LV_DEBUG
	DumpList();
#endif
    }
}

void W_ListView::SetItemParam(int p, int param)
{
    if(p<0)
    {
	debug_printf("W_ListView::SetItemParam error  p num\n");
	return ;
    }

    if(m_hwnd)
    {
	int loop = 0;
	VUser_TreeViewItem* pItem = (VUser_TreeViewItem*)m_hwnd->firstChild();
	for(loop=0;loop<p;loop++,pItem = (VUser_TreeViewItem*)pItem->nextSibling())
	{
		if(!pItem)
			break;
	
	}
	if(!pItem)
	{
		debug_printf("W_ListView::SetItemParam error inefficient num\n");
		return ;
	}

    	pItem->m_param = param;
#if _LV_DEBUG
	DumpList();
#endif
    }
}

int W_ListView::_docompare(char *s1, char *s2)
{
  if (m_sort_type)
  {
    int r=(atoi(s1)-atoi(s2));
    if (m_sort_dir==-1) return -r;
    return r;
  }
  int r=stricmp(s1,s2);
  if (m_sort_dir==-1) return -r;
  return r;
}

int /*CALLBACK*/ W_ListView::sortfunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  W_ListView *_this=(W_ListView *)lParamSort;
  int idx1,idx2;
  char text1[128],text2[128];
  idx1=_this->FindItemByParam(lParam1);
  idx2=_this->FindItemByParam(lParam2);
  if (idx1==-1) return _this->m_sort_dir;
  if (idx2==-1) return -_this->m_sort_dir;
  _this->GetText(idx1,_this->m_sort_col,text1,sizeof(text1));
  _this->GetText(idx2,_this->m_sort_col,text2,sizeof(text2));
  text1[127]=text2[127]=0;
  return _this->_docompare(text1,text2); 
}

void W_ListView::Resort(void)
{
//  ListView_SortItems(m_hwnd,sortfunc,(LPARAM)this);
}

int W_ListView::FindItemByParam(int param)
{
  int loop=0;
  QListViewItemIterator it(m_hwnd);
  while(it.current())
  {
	if(param==((VUser_TreeViewItem*)it.current())->m_param)
		return loop;
	loop++;
	it++;
  }
  return -1;
}

void W_ListView::DeleteItem(int n)
{ 
    if(n<0)
    {
	debug_printf("W_ListView::DeleteItem error  p num\n");
	return ;
    }

    if(m_hwnd)
    {
	int loop = 0;
	VUser_TreeViewItem* pItem = (VUser_TreeViewItem*)m_hwnd->firstChild();
	for(loop=0;loop<n;loop++,pItem = (VUser_TreeViewItem*)pItem->nextSibling())
	{
		if(!pItem)
			break;
	}
	if(!pItem)
	{
		debug_printf("W_ListView::DeleteItem error inefficient num\n");
		return ;
	}

	m_hwnd->takeItem(pItem);
	if(pItem)
		delete pItem;
#if _LV_DEBUG
	DumpList();
#endif
    }
}

void W_ListView::Clear(void)
{
    if(m_hwnd)
    {
	m_hwnd->clear();
#if _LV_DEBUG
	DumpList();
#endif
    }
}

int W_ListView::GetSelected(int x)
{
    if(x<0)
    {
	debug_printf("W_ListView::GetSelected error  x num\n");
	return 0;
    }

    if(m_hwnd)
    {
	int loop = 0;
	VUser_TreeViewItem* pItem = (VUser_TreeViewItem*)m_hwnd->firstChild();
	for(loop=0;loop<x;loop++,pItem = (VUser_TreeViewItem*)pItem->nextSibling())
	{
		if(!pItem)
			break;
	
	}
	if(!pItem)
	{
		debug_printf("W_ListView::GetSelectItem error inefficient num\n");
		return 0;
	}
	
	return m_hwnd->isSelected(pItem);
    }
}

void W_ListView::SetSelected(int x)
{
    if(x<0)
    {
	debug_printf("W_ListView::SetSelected error  x num\n");
	return ;
    }

    if(m_hwnd)
    {
	int loop = 0;
	VUser_TreeViewItem* pItem = (VUser_TreeViewItem*)m_hwnd->firstChild();
	for(loop=0;loop<x;loop++,pItem = (VUser_TreeViewItem*)pItem->nextSibling())
	{
		if(!pItem)
			break;
	
	}
	if(loop<x)
	{
		debug_printf("W_ListView::SetSelected error inefficient num\n");
		return ;
	}
	
	m_hwnd->setSelected(pItem,true);
    }
}

void W_ListView::DumpList(void)
{
	qDebug("\n\nW_ListView::Dump GetCount()=%d\n\n",GetCount());
	VUser_TreeViewItem* pItem = (VUser_TreeViewItem*)m_hwnd->firstChild();
	for(int i=0;i<GetCount();i++)
	{
		char text0[256];
		char text1[256];
		char text2[256];
		char text3[256];
		text0[0]=text1[0]=text2[0]=text3[0]='\0';
		GetText(i, 0, text0, 256);
		GetText(i, 1, text1, 256);
		GetText(i, 2, text2, 256);
		GetText(i, 3, text3, 256);
		int parm = GetParam(i);
		qDebug("item[%d]=%s,%s,%s,%s,|%d\n",i,text0,text1,text2,text3,parm);
	}
	qDebug("\n\nW_ListView::Dump END\n\n");
}

VUser_TreeViewItem* W_ListView::GetItem(int p)
{
	debug_printf("W_ListView::GetItem() p=%d",p);
    if(m_hwnd)
    {
	int loop = 0;
	VUser_TreeViewItem* pItem = (VUser_TreeViewItem*)m_hwnd->firstChild();
	for(loop=0;loop<p;loop++,pItem = (VUser_TreeViewItem*)pItem->nextSibling())
	{
		if(!pItem)
			break;
	
	}
	if(loop<p)
	{
		debug_printf("W_ListView::GetItem error inefficient num\n");
		return 0;
	}

	return pItem;
    }

  return NULL;
}
