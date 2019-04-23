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
#ifndef VASTE_IDD_SEARCH_H
#define VASTE_IDD_SEARCH_H

#include "idd_search.h"
class Vaste_Idd_Search : public IDD_SEARCH
{
    Q_OBJECT

public:
    Vaste_Idd_Search( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Vaste_Idd_Search();
     void SetWndTitle();
     void Notify(int item);
     void closeEvent(QCloseEvent *e);

public slots:
    virtual void onGoRoot();
    virtual void onGoParent();
    virtual void onGoLeft();
    virtual void onGoForward();
    virtual void onGoRight();
    virtual void onGoBack();
    virtual void onGoSearchText();
    virtual void onGoSearch();
    virtual void onColumnClick();
    virtual void onGoDoubleClicked();
    virtual void onGoRightClicked(QListViewItem*,const QPoint&,int);
    virtual void onGoItem(int);
    virtual void timerDone();
    virtual void onGoEdit();
    virtual void slotEnableSearch(const QString & string);
protected:

protected slots:
    virtual void languageChange();
};
#endif

