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
 * This file implements the display of file transfer status
 * Author: Raymond Cai
 * Date Created: 2003.12.1
*/

#ifndef VASTE_IDD_XFERS_H
#define VASTE_IDD_XFERS_H

#include "idd_xfers.h"

class Vaste_Idd_Xfers : public IDD_XFERS 
{
    Q_OBJECT

public:
    Vaste_Idd_Xfers( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Vaste_Idd_Xfers();
	
    void recvQ_UpdateStatusText();
    void UpdateTitle();
    void show();
    void doExecuteOfDownload(int x, int copy);
    void closeEvent(QCloseEvent *e);
    
public slots:
    virtual void pushBtnIDCResume();
    virtual void pushBtnIDCDndAbortSel();
    virtual void pushBtnIDCDndClearComp();
    virtual void pushBtnIDCDndClearComp3();
    virtual void pushBtnIDCDndClearAll();
    virtual void chkBoxIDCDndAutoClear(int );
    virtual void chkBoxIDCDndAutoResume(int);
    virtual void pushBtnMoveToBottom( );
    virtual void pushBtnDlNow();
    virtual void pushBtnMoveToTop();
    virtual void pushBtnRemoveQueue();
    virtual void pushBtnClearQueue();
    virtual void pushBtnSendAbortSel();
    virtual void pushBtnSendClearComp();
    virtual void pushBtnSendClearComp2();
    virtual void chkBoxSendAutoClear(int);
    virtual void lstViewRecvDClicked();
    virtual void lstViewRecvSelectChanged();
    virtual void lstViewRecvQDclicked();
    virtual void lstViewRecvQSelectedChanged();
    virtual void lstViewSendDClicked();
    virtual void lstViewSendSelectChanged();
    virtual void lnEditMaxSimulDLTextChanged(const QString &);
protected slots:

};

#endif // VASTE_IDD_XFERS_H
