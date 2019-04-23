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
 * This file implements the treeview control
 * Author: Stanley San
 * Date Created: 2003.12.1
*/
#include "ui_treeview.h"
#include "ui_listview.h"

VUser_TreeViewItem* TreeView_GetRootChild(QListView* htree)
{
	return (VUser_TreeViewItem*)htree->firstChild();
}
void TreeView_GetItem(QListView* htree,VUser_TreeViewItem *i)
{
}
void TreeView_DeleteItem(QListView* htree,VUser_TreeViewItem *i)
{
	qDebug("TreeView_DeleteItem htree=%p,i=%p",htree,i);
	if(i)
	{
		htree->takeItem(i);
		delete i;
		i = NULL;
	}
}
void TreeView_SetItem(QListView* htree,VUser_TreeViewItem *i,char* text,int param)
{
	qDebug("TreeView_SetItem htree=%p,i=%p,text=%s,param=%d",htree,i,text,param);
	if(i)
	{
		i->setText(0,text);
		i->m_param = param;
	}
}
VUser_TreeViewItem* TreeView_GetNextSibling(QListView* htree,VUser_TreeViewItem *i)
{
	qDebug("TreeView_GetNextSibling htree=%p,i=%p",htree,i);
	if(i)
		return (VUser_TreeViewItem*)i->nextSibling();
	else
		return NULL;
}

void TreeView_InsertItem(QListView* htree,char* text,int param,const char* icon)
{
	qDebug("TreeView_InsertItem htree=%p,text=%s,p=%d,icon=%s",htree,text,param,icon);
	if(htree)
	{
		htree->insertItem(new VUser_TreeViewItem(htree,text,param,icon));	
	}
}

VUser_TreeViewItem* TreeView_GetSelection(QListView* htree)
{
	return (VUser_TreeViewItem*)htree->selectedItem();
}
