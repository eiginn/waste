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
 * This file implements the display of UploadFileDialog
 * Author: Stanley San
 * Date Created: 2003.12.1
*/
#ifndef QULF_DIALOG_H
#define QULF_DIALOG_H
	
#if defined(QTUI)


#include <qfiledialog.h>
#include <qlineedit.h>

class Vaste_UploadFileDlg : public QFileDialog
{
    Q_OBJECT

public:
    Vaste_UploadFileDlg( QWidget* parent = 0, const char* name = 0);
    ~Vaste_UploadFileDlg();

    QString getUpLoadPath(void);

private:
    QLineEdit m_ulpath;

};

#endif

#endif
