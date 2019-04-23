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
 *		Implement preference set window used by StanleySan
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

#include "preference.h"

#include "common.h"

#include "prefdlgs.h"

#include <qpixmap.h>

WndPreference::WndPreference(QWidget *parent, const char *name)
:QTabDialog(parent, name)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);
	setCaption(QString("Settings - ") + QString(APP_NAME));
	//setCaption(QString(APP_NAME) + QString(" - Settings"));

	initializeItems();

	//setFixedSize(sizeHint());
	//setFixedSize(width(), height());
	setFixedSize(410, 200);

	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(slotOK()));
}

WndPreference::~WndPreference()
{
	destroyItems();
}

void WndPreference::initializeItems()
{
	m_tabs[0] = new QVBox(this);
	m_panels[0] = new PanelIdentity(m_tabs[0]);
	m_panels[0]->initialize();

	m_tabs[1] = new QVBox(this);
	m_panels[1] = new PanelNetworkPrivateKey(m_tabs[1]);
	m_panels[1]->initialize();

	m_tabs[2] = new QVBox(this);
	m_panels[2] = new PanelNetworkPublicKeys(m_tabs[2]);
	m_panels[2]->initialize();

	m_tabs[3] = new QVBox(this);
	m_panels[3] = new PanelFileTransfers(m_tabs[3]);
	m_panels[3]->initialize();
	addTab(m_tabs[0], "General");
	addTab(m_tabs[1], "Authentication");
	addTab(m_tabs[2], "Trusted friends");
	addTab(m_tabs[3], "File transfer");
}

void WndPreference::destroyItems()
{
	for(int i=5; i>=0; --i){
		delete m_panels[i];
		delete m_tabs[i];
	}
}

/* Added by LoraYin 2004-2-27 */
/* Check the panel if need to update something. */
void WndPreference::slotOK()
{
		
	((PanelIdentity*)m_panels[0])->checkUpdate();
	hide();
}

