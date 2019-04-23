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
 * This file implement the interfaces which display the status of netstatus
 * Author: Raymond Cai
 * Date Created: 2003.12.1
*/
#ifndef VASTE_IDD_NET_H
#define VASTE_IDD_NET_H

#include "idd_net.h"
#include <qlistbox.h>

class Vaste_Idd_Net : public IDD_NET
{
    Q_OBJECT

public:
    Vaste_Idd_Net( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Vaste_Idd_Net();

    void UpdateWinTitle();
    void closeEvent(QCloseEvent *e);
protected slots:
    virtual void onAddConnectionClicked();
    virtual void onConnectionCatchClicked();
    virtual void onRemoveConnectionClicked();
    virtual void onRemoveCatchClicked();
    virtual void timerEvent(QTimerEvent *tEvent);
    virtual void onComboBoxTextChanged(const QString &str);
    virtual void onComboBoxEnterPressed();
    virtual void onListViewNetConnsSelectionChanged();

private:
};

#endif
