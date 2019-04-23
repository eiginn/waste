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
 * This file implements the display of MainGui
 * Author: Stanley San
 * Date Created: 2003.12.1
*/
#ifndef _VASTE_MAINWINDOW_
#define _VASTE_MAINWINDOW_

#include "main_window.h"

class WndPreference;
class QVaste_MainWin : public QVaste
{
    Q_OBJECT

public:
    QVaste_MainWin( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel/*|WType_Dialog*/ );
    ~QVaste_MainWin();

    void main_onGotNick(char* nick,int del);
    void main_onGotChannel(char* cnl);

private:
    void loopHandler(void);
    void init(void);
    void deinit(void);

public slots:
    virtual void slotAppendWndTitle(const QString &, bool =false);

    virtual void timerEvent(QTimerEvent* );

    virtual void onUserListClicked(QListViewItem*);
    virtual void onChatRoomListClicked(QListViewItem*);
    virtual void onUserListRMBClicked(QListViewItem*,const QPoint&,int);

    virtual void filePreference();
    virtual void fileExit();
    virtual void editStatus();
    virtual void editBrowse();
    virtual void editTransfer();
    virtual void editChatWin();
    virtual void editAlwaysOnTop();
    virtual void editCreateChatWin();
    virtual void onHelpHelp();
    virtual void onHelpAbout();

    virtual void makeChatSubMenu();
    virtual void processChatSubMenu(int);


    virtual void resizeEvent(QResizeEvent* e);
private:
    WndPreference* pref;
    int m_LoopTimer;
    int m_UpdateUserListTimer;

    QPopupMenu* m_ChatSubMenu;
};


#endif
