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

#ifndef _USER_TREE_VIEW_H
#define _USER_TREE_VIEW_H

//rapper funciton 
#ifdef QTUI

#if defined(TreeView_GetRootChild)
#undef TreeView_GetRootChild
#endif

#if defined(TreeView_GetItem)
#undef TreeView_GetItem
#endif

#if defined(TreeView_GetNextSibling)
#undef TreeView_GetNextSibling
#endif

#if defined(TreeView_DeleteItem)
#undef TreeView_DeleteItem
#endif

#if defined(TreeView_SetItem)
#undef TreeView_SetItem
#endif

#if defined(TreeView_InsertItem)
#undef TreeView_InsertItem
#endif

#if defined(TreeView_GetSelection)
#undef TreeView_GetSelection
#endif

class VUser_TreeViewItem;
class QListView;
class QPixmap;
VUser_TreeViewItem* TreeView_GetRootChild(QListView* htree);
void TreeView_GetItem(QListView* htree,VUser_TreeViewItem *i);
void TreeView_DeleteItem(QListView* htree,VUser_TreeViewItem *i);
void TreeView_SetItem(QListView* htree,VUser_TreeViewItem *i,char* text=0,int param=0);
VUser_TreeViewItem* TreeView_GetNextSibling(QListView* htree,VUser_TreeViewItem *i);
//void TreeView_InsertItem(QListView* htree,char* text=0,int param=0,const QPixmap * icon=0);
void TreeView_InsertItem(QListView* htree,char* text=0,int param=0,const char* icon=0);
VUser_TreeViewItem* TreeView_GetSelection(QListView* htree);
#endif





#endif
