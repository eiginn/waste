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
 * This file implements the display of chat
 * Author: Stanley San
 * Date Created: 2003.12.1
*/

#ifndef VASTE_CHAT_H
#define VASTE_CHAT_H

#include "idd_chatchan.h"
#include "idd_chatpriv.h"

#include "vq_chat.h"


/*
class VChatSubAction : public QAction
{
    Q_OBJECT

public:
    VChatSubAction( QObject* parent = 0, const char* name = 0);
    ~VChatSubAction();
};
*/

class QShowEvent;
class Vaste_ChatChan : public IDD_CHATCHAN
{
    Q_OBJECT

public:
    Vaste_ChatChan( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Vaste_ChatChan();

    void titleUpdate();

//function
    void formatChatString(char *txt, char *src, char *str);
    void gotNick(char *nick, int state, int statemask, int delifold);

//data
    int m_t1kId;

    void closeEvent(QCloseEvent* e);

protected slots:
    virtual void processChatLine();
    virtual void onUserListDBClicked(QListViewItem*);
    void showEvent(QShowEvent *e);

    virtual void onUserListRMBClicked(QListViewItem*,const QPoint&,int);
    virtual void onUserInfoClicked();
    virtual void onBrowseClicked();
    virtual void onSendFileClicked();

protected slots:

public:
//data 
    chatroom_item* data;
};


class Vaste_ChatPriv : public IDD_CHATPRIV
{
    Q_OBJECT

public:
    Vaste_ChatPriv( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Vaste_ChatPriv();

    void titleUpdate();

//function
    void formatChatString(char *txt, char *src, char *str);
    void gotNick(char *nick, int state, int statemask, int delifold);

//data
    int m_t1kId;

    void closeEvent(QCloseEvent* e);

protected slots:
    virtual void processChatLine();
    virtual void onBrowseClicked();
    virtual void onUserInfoClicked();
    virtual void onSendFile();
    virtual void onChatSendClicked();

    void showEvent(QShowEvent *e);

public:
//data 
    chatroom_item* data;
};



#endif
