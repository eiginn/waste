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
 *******************************************************************************
 *
 *	\Program
 *		PadLockSL
 *
 *	\Author
 *		MichaelZhu
 *
 *	\File
 *		preference.h
 *
 *	\Brief
 *		Define preference set window used by StanleySan
 *
 *	\Code
 *		Date		Version	Author		Description
 *		--------	----	------------	------------------------
 *		03/12/31	1.0	MichaelZhu	Implement all functions
 *		04/02/05	1.0	MichaelZhu	Remove all and note debug info
 *	\End code
 *
 *	\Platform 1:
 *		\Environment
 *			RedHat Linux 9
 *		\Compiler
 *			gcc 3.2.2
 *			QT/X11 3.2.3
 *	\Platform 2:
 *		\Environment
 *			Windows 2000
 *		\Compiler
 *			Visual C++ 6.0
 *			QT/commercial 3.0.1
 *
 *******************************************************************************
*/

#ifndef PREFERENCE_H
#define PREFERENCE_H

#include <qtabdialog.h>

#include <qwidget.h>
#include <qvbox.h>
class Panel;

class WndPreference: public QTabDialog
{
	Q_OBJECT;

private:
	QVBox *m_tabs[4];
	Panel *m_panels[4];

public:
	WndPreference(QWidget * =0, const char * =0);
	~WndPreference();

private:
	void initializeItems();
	void destroyItems();

signals:
	//No use, just avoid runtime warning of no such signal
	void signalAppendWndTitle(const QString &, bool);

/* Added by LoraYin 2004.2.27 */
public slots:
	void slotOK();
};

#endif	//PREFERENCE_H
