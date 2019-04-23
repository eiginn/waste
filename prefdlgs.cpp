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
 *		prefdlgs.cpp
 *
 *	\Brief
 *		Implement common dialogs used by MichaelZhu
 *
 *	\Code
 *		Date		Version	Author		Description
 *		--------	----	------------	------------------------
 *		03/12/31	1.0	MichaelZhu	Implement all dialogs
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

//include class declarations
#include "prefdlgs.h"

//include frequently used classes and macros
#include "common.h"

//include global variants
#include "main.h"
#include "util.h"
#include "netkern.h"
#include "srchwnd.h"
#include "prefinterface.h"
#include "vq_chat.h"

//include tool classes
#include <string.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qlistview.h>
#include <qtimer.h>
#include <stdio.h>
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfont.h>
#include <qpixmap.h>

#ifdef _LINUX
#include <dirent.h>
#include <sys/types.h>
#endif	//_LINUX

bool kg_end=false;

time_t m_startTime;

//class DlgGetPassword
DlgGetPassword::DlgGetPassword(QWidget *parent, const char *name)
	:QDialog(parent, name, TRUE)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);
	QString WndTitle=QString("Password - ") + QString(APP_NAME);
	//QString WndTitle=QString(APP_NAME) + QString(":password");
	setCaption(WndTitle);

	setFixedSize(340, 120);

	m_hint = new QLabel("Password:", this);
	m_hint->setGeometry(20, 20, 60, 20);
	m_hint->setAlignment(Qt::AlignRight);

	m_password = new QLineEdit(this);
	m_password->setEchoMode(QLineEdit::Password);
	m_password->setMaxLength(255);
	m_password->setGeometry(100, 20, 220, 20);

	m_save = new  QCheckBox("Remember (not recommended)", this);
	m_save->setGeometry(20, 50, 300, 20);

	m_OK = new QPushButton("&OK", this);
	m_OK->setGeometry(180, 80, 60, 20);

	m_CANCEL = new QPushButton("&Cancel", this);
	m_CANCEL->setGeometry(260, 80, 60, 20);

	connect(
		m_password, SIGNAL(returnPressed()),
		this, SLOT(slotOK())
	       );
	connect(
		m_OK, SIGNAL(clicked()),
		this, SLOT(slotOK())
	       );
	connect(
		m_CANCEL, SIGNAL(clicked()),
		this, SLOT(slotCANCEL())
	       );
}

DlgGetPassword::~DlgGetPassword()
{
	delete m_CANCEL;
	delete m_OK;
	delete m_save;
	delete m_password;
	delete m_hint;
}

void DlgGetPassword::initialize()
{
	if(g_config->ReadInt("storepass", 0)){
		m_save->setChecked(TRUE);
	}
}

const char *DlgGetPassword::getPassword()
{
	return (const char *)(m_password->text());
}

void DlgGetPassword::slotOK()
{
	if(m_password->text().isEmpty()){
		QMessageBox::critical(this, QString(APP_NAME " - Warning"), QString(APP_NAME " password should not be empty!"), "&OK");

		return;
	}

	char passBuf[256];
	if((const char *)(m_password->text()) != 0){
		strcpy(passBuf, (const char *)(m_password->text()));
	}
	else{
		strcpy(passBuf, "");
	}
	passBuf[255] = 0;

	if(m_save->isChecked()){
		g_config->WriteInt("storepass", 1);
		g_config->WriteString("keypass", passBuf);
	}
	else{
		g_config->WriteInt("storepass", 0);
		g_config->WriteString("keypass", "");
	}

	accept();
}

void DlgGetPassword::slotCANCEL()
{
	reject();
}
//end class DlgGetPassword

//class ProfileManager
ProfileManager::ProfileManager(QWidget *parent, const char *name)
:QDialog(parent, name)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);
	setCaption(QString(APP_NAME) + QString(" Profile Manager"));

	setFixedSize(240, 200);

	m_hintProfileManager = new QLabel("Profiles:", this);
	m_hintProfileManager->setGeometry(20, 20, 200, 20);
	m_buttonCreate = new QPushButton("Create", this);
	m_buttonCreate->setGeometry(20, 40, 60, 20);
	m_buttonDelete = new QPushButton("Delete", this);
	m_buttonDelete->setGeometry(20, 60, 60, 20);
	m_buttonCopy = new QPushButton("Copy", this);
	m_buttonCopy->setGeometry(20, 80, 60, 20);
	m_buttonRename = new QPushButton("Rename", this);
	m_buttonRename->setGeometry(20, 100, 60, 20);
	m_userNames = new QListBox(this);
	m_userNames->setGeometry(80, 40, 140, 100);
	m_startup = new QCheckBox(this);
	m_startup->setGeometry(20, 140, 20, 20);
	m_hintStartup = new QLabel("Show profile manager onstartup", this);
	m_hintStartup->setGeometry(40, 140, 180, 20);
	m_buttonLaunch = new QPushButton("Launch with profile", this);
	m_buttonLaunch->setGeometry(20, 160, 120, 20);
	m_buttonQuit = new QPushButton("Quit", this);
	m_buttonQuit->setGeometry(160, 160, 60, 20);

	connect(
		m_buttonCreate, SIGNAL(clicked()),
		this, SLOT(slotCreate())
		);
	connect(
		m_buttonDelete, SIGNAL(clicked()),
		this, SLOT(slotDelete())
		);
	connect(
		m_buttonCopy, SIGNAL(clicked()),
		this, SLOT(slotCopy())
		);
	connect(
		m_buttonRename, SIGNAL(clicked()),
		this, SLOT(slotRename())
		);
	connect(
		m_startup, SIGNAL(clicked()),
		this, SLOT(slotStartup())
		);
	connect(
		m_buttonLaunch, SIGNAL(clicked()),
		this, SLOT(slotLaunch())
		);
	connect(
		m_buttonQuit, SIGNAL(clicked()),
		this, SLOT(slotQuit())
		);
}

ProfileManager::~ProfileManager()
{
	delete m_buttonQuit;
	delete m_buttonLaunch;
	delete m_hintStartup;
	delete m_startup;
	delete m_userNames;
	delete m_buttonRename;
	delete m_buttonCopy;
	delete m_buttonDelete;
	delete m_buttonCreate;
	delete m_hintProfileManager;
}

void ProfileManager::initialize()
{
	if(GetPrivateProfileInt("config", "showprofiles", 0, g_config_mainini)){
		m_startup->setChecked(TRUE);
	}
	else{
		m_startup->setChecked(FALSE);
	}

	m_userNames->clear();
#ifdef _LINUX
	DIR *dp;
	dirent *dirp;
	QString fn;
	int fileIndex=0;
	bool find=false;
	if((dp = opendir(".")) == NULL){
		return;
	}
	while((dirp = readdir(dp)) != NULL){
		//Read filename
		//printf("%s\n", dirp->d_name);
		fn = QString(dirp->d_name);
		if(fn.contains(".pr0")){
			fn.truncate(fn.length() - 4);
			m_userNames->insertItem(fn);
			if(!find){
				if(strcmp(g_profile_name, (const char *)fn) != 0){
					++fileIndex;
				}
				else{
					find = true;
				}
			}
		}
	}
	closedir(dp);
#elif defined(_WIN32)
	WIN32_FIND_DATA fd;
	HANDLE h;
	int fileIndex=0;
	bool find=false;
	char tmp[1024];

	getProfilePath(tmp);
	strcat(tmp, "*.pr0");
	h = FindFirstFile(tmp, &fd);
	if(h){
		do{
			if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
				char *p=fd.cFileName;
				while(*p) ++p;
				while(p>=fd.cFileName && *p!='.') --p;
				if(p > fd.cFileName){
					*p = 0;
					if(strlen(fd.cFileName) < sizeof(g_profile_name)){
						m_userNames->insertItem(QString(fd.cFileName));
					}

					if(!find){
						if(strcmp(g_profile_name, fd.cFileName) != 0){
							++fileIndex;
						}
						else{
							find = true;
						}
					}
				}
			}
		}while(FindNextFile(h, &fd));
		FindClose(h);
	}
#endif	//_LINUX+_WIN32
	if(m_userNames->count() > 0){
		if(find){
			m_userNames->setCurrentItem(fileIndex);
		}
		else{
			m_userNames->setCurrentItem(0);
		}
	}

	update();
}

void ProfileManager::setLaunchEnabled(bool launchEnabled)
{
	m_buttonLaunch->setEnabled(launchEnabled);
}

void ProfileManager::slotCreate()
{
	DlgProfile dlgProfile;
	dlgProfile.initialize("Create Profile");

	int ret=dlgProfile.exec();
	if(ret == QDialog::Accepted){
		char tmp[2048];
		if(dlgProfile.getDest() != 0){
			if(dlgProfile.getDest() != 0){
				strcpy(tmp, dlgProfile.getDest());
			}
			else{
				strcpy(tmp, "");
			}
			strcat(tmp, ".pr0");
			FILE *fp;
			fp = fopen(tmp, "a");
			if(!fp){
				QMessageBox::warning(0, QString(APP_NAME " Error"), "Error creating profile");

				return;
			}
			fclose(fp);
		}
	}
}

void ProfileManager::slotDelete()
{
	char tmp[2048];
	char profile[2048];
	if((const char *)(m_userNames->currentText()) != 0){
		strcpy(tmp, (const char *)(m_userNames->currentText()));
	}
	else{
		return;
	}
	strcpy(profile, tmp);
	strcat(tmp, ".pr0");
	if(DeleteFile(tmp) == 0){
		tmp[strlen(tmp)-1]++;
		DeleteFile(tmp);
		tmp[strlen(tmp)-1]++;
		DeleteFile(tmp);
		tmp[strlen(tmp)-1]++;
		DeleteFile(tmp);
		tmp[strlen(tmp)-1]++;
		DeleteFile(tmp);

		//strcpy(g_profile_name, "");;
		if(strcmp(profile, g_profile_name) == 0){
			g_profile_name[0]='\0';
		}

		initialize();
	}
	else{
		QMessageBox::warning(0, QString(APP_NAME " Error"), "Error deleting profile");
	}
}

void ProfileManager::slotCopy()
{
	DlgProfile dlgProfile;
	dlgProfile.initialize("Copy Profile", (const char *)(m_userNames->currentText()));

	int ret=dlgProfile.exec();
	if(ret == QDialog::Accepted){
		if(dlgProfile.getDest() != 0){
			char oldfn[2048];
			char newfn[2048];
			if(dlgProfile.getSrc() != 0){
				strcpy(oldfn, dlgProfile.getSrc());
			}
			else{
				return;
			} 
			strcat(oldfn, ".pr0");
			if(dlgProfile.getDest() != 0){
				strcpy(newfn, dlgProfile.getDest());
			}
			else{
				return;
			}
			strcat(newfn, ".pr0");
			CopyFile(oldfn, newfn);
			oldfn[strlen(oldfn)-1]++;
			newfn[strlen(newfn)-1]++;
			CopyFile(oldfn, newfn);
			oldfn[strlen(oldfn)-1]++;
			newfn[strlen(newfn)-1]++;
			CopyFile(oldfn, newfn);
			oldfn[strlen(oldfn)-1]++;
			newfn[strlen(newfn)-1]++;
			CopyFile(oldfn, newfn);
			oldfn[strlen(oldfn)-1]++;
			newfn[strlen(newfn)-1]++;
			CopyFile(oldfn, newfn);
		}
	}
}

void ProfileManager::slotRename()
{
	DlgProfile dlgProfile;
	dlgProfile.initialize("Rename Profile", (const char *)(m_userNames->currentText()), (const char *)(m_userNames->currentText()));

	int ret=dlgProfile.exec();
	if(ret == QDialog::Accepted){
		char oldfn[2048];
		char newfn[2048];
		char profile[2048];
		if(dlgProfile.getSrc() != 0){
			strcpy(oldfn, dlgProfile.getSrc());
		}
		else{
			return;
		}
		strcat(oldfn, ".pr0");
		if(dlgProfile.getDest() != 0){
			strcpy(newfn, dlgProfile.getDest());
		}
		else{
			return;
		}
		strcpy(profile, newfn);
		strcat(newfn, ".pr0");
		if(rename(oldfn, newfn) == 0){
			strcpy(g_profile_name, profile);
			DeleteFile(oldfn);
			oldfn[strlen(oldfn)-1]++;
			newfn[strlen(newfn)-1]++;
			rename(oldfn, newfn);
			DeleteFile(oldfn);
			oldfn[strlen(oldfn)-1]++;
			newfn[strlen(newfn)-1]++;
			rename(oldfn, newfn);
			DeleteFile(oldfn);
			oldfn[strlen(oldfn)-1]++;
			newfn[strlen(newfn)-1]++;
			rename(oldfn, newfn);
			DeleteFile(oldfn);
			oldfn[strlen(oldfn)-1]++;
			newfn[strlen(newfn)-1]++;
			rename(oldfn, newfn);
			DeleteFile(oldfn);

			initialize();
		}
		else{
			QMessageBox::warning(0, QString(APP_NAME " Error"), "Error rename profile");
		} 
	}
}

void ProfileManager::slotStartup()
{
	if(m_startup->isChecked()){
		WritePrivateProfileString("config", "showprofiles", "1", g_config_mainini);
	}
	else{
		WritePrivateProfileString("config", "showprofiles", "0", g_config_mainini);
	}
}

void ProfileManager::slotLaunch()
{
	if((const char *)(m_userNames->currentText()) != 0){
		strcpy(g_profile_name, (const char *)(m_userNames->currentText()));

		accept();
	}
}

void ProfileManager::slotQuit()
{
	reject();
}
//end class ProfileManager

//class DlgProfile
DlgProfile::DlgProfile(QWidget *parent, const char *name)
:QDialog(parent, name)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);

	m_hintSrc = new QLabel("Source:", this);
	m_hintSrc->setGeometry(20, 20, 100, 20);
	m_src = new QLineEdit(this);
	m_src->setGeometry(130, 20, 200, 20);
	m_hintDest = new QLabel("Destination:", this);
	m_hintDest->setGeometry(20, 50, 100, 20);
	m_dest = new QLineEdit(this);
	m_dest->setGeometry(130, 50, 200, 20);
	m_buttonOK = new QPushButton("&OK", this);
	m_buttonOK->setGeometry(20, 80, 60, 20);
	m_buttonCancel = new QPushButton("&Cancel", this);
	m_buttonCancel->setGeometry(90, 80, 60, 20);

	setFixedSize(350, 120);
	connect(
		m_buttonOK, SIGNAL(clicked()),
		this, SLOT(slotOK())
		);
	connect(
		m_buttonCancel, SIGNAL(clicked()),
		this, SLOT(slotCancel())
		);
	connect(
		m_dest, SIGNAL(returnPressed()),
		this, SLOT(slotOK())
		);
}

DlgProfile::~DlgProfile()
{
	delete m_buttonCancel;
	delete m_buttonOK;
	delete m_dest;
	delete m_hintDest;
	delete m_src;
	delete m_hintSrc;
}

void DlgProfile::initialize(const char *title, const char *source, const char *destination)
{
	setCaption(title);

	if(source != NULL){
		m_src->setText(source);
	}
	else{
		m_src->setText("NULL");
	}
	m_src->setEnabled(FALSE);

	if(destination != NULL){
		m_dest->setText(destination);
	}
	else{
		m_dest->setText("new profile");
	}
}

const char *DlgProfile::getSrc()
{
	if(m_src->text() == QString("NULL")){
		return NULL;
	}
	else{
		return (const char *)(m_src->text());
	}
}

const char *DlgProfile::getDest()
{
	return (const char *)(m_dest->text());
}

void DlgProfile::slotOK()
{
	accept();
}

void DlgProfile::slotCancel()
{
	reject();
}
//end class DlgProfile

//profile setup wizard
//class WizardManager
WizardManager::WizardManager()
:m_exeState(false)
{
	m_wizard1 = new ProfileWizard1();
	m_wizard1->initialize();
	m_wizard1->show();
	m_wizard2 = new ProfileWizard2();
	m_wizard2->initialize();
	m_wizard2->hide();
	m_wizard3 = new ProfileWizard3();
	m_wizard3->initialize();
	m_wizard3->hide();

	connect(
		m_wizard1, SIGNAL(signalNext()),
		this, SLOT(slotShowPage2())
	       );
	connect(
		m_wizard1, SIGNAL(signalCancel()),
		this, SLOT(slotQuit())
	       );
	connect(
		m_wizard2, SIGNAL(signalBack()),
		this, SLOT(slotShowPage1())
	       );
	connect(
		m_wizard2, SIGNAL(signalNext()),
		this, SLOT(slotShowPage3())
	       );
	connect(
		m_wizard2, SIGNAL(signalCancel()),
		this, SLOT(slotQuit())
		);
	connect(
		m_wizard3, SIGNAL(signalBack()),
		this, SLOT(slotShowPage2())
	       );
	connect(
		m_wizard3, SIGNAL(signalDone()),
		this, SLOT(slotFinished())
	       );
}

WizardManager::~WizardManager()
{
	delete m_wizard3;
	delete m_wizard2;
	delete m_wizard1;
}

bool WizardManager::exec()
{
	slotShowPage1();

	if(m_exeState){
		return true;
	}

	return false;
}

void WizardManager::slotQuit()
{
	m_exeState = false;
}

void WizardManager::slotFinished()
{
	m_exeState = true;
}

void WizardManager::slotShowPage1()
{
	m_wizard2->hide();
	m_wizard3->hide();
	m_wizard1->show();

	m_wizard1->exec();
}

void WizardManager::slotShowPage2()
{
	m_wizard1->hide();
	m_wizard3->hide();
	m_wizard2->show();

	m_wizard2->exec();
}

void WizardManager::slotShowPage3()
{
	m_wizard1->hide();
	m_wizard2->hide();
	m_wizard3->show();

	m_wizard3->exec();
}
//end class WizardManager

//class ProfileWizard1
ProfileWizard1::ProfileWizard1(QWidget *parent, const char *name)
:QDialog(parent, name)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);
	setCaption(QString(APP_NAME " Profile Wizard - 1/3"));
	setFixedSize(400, 280);

	m_info = new QLabel("", this);
	m_info->setGeometry(20, 20, 100, 200);
	m_info->setBackgroundColor(Qt::darkCyan);

	m_hint = new QLabel("General Information", this);
	m_hint->setGeometry(140, 20, 200, 40);
	QFont font;
	font.setPointSize(static_cast<int>(font.pointSize() * 1.5));
	font.setBold(TRUE);
	font.setItalic(TRUE);
	m_hint->setFont(font);

	m_hintNickname = new QLabel("Display name:", this);
	m_hintNickname->setGeometry(120, 80, 110, 20);
	m_hintNickname->setAlignment(Qt::AlignRight);
	m_nickname = new QLineEdit(this);
	m_nickname->setGeometry(240, 80, 140, 20);

	m_hintFullname = new QLabel("User information:", this);
	m_hintFullname->setGeometry(120, 110, 110, 20);
	m_hintFullname->setAlignment(Qt::AlignRight);
	m_fullname = new QLineEdit(this);
	m_fullname->setGeometry(240, 110, 140, 20);
	m_buttonBack = new QPushButton("<&Back", this);
	m_buttonBack->setGeometry(160, 240, 60, 20);
	m_buttonBack->setEnabled(FALSE);
	m_buttonNext = new QPushButton("&Next>", this);
	m_buttonNext->setGeometry(230, 240, 60, 20);
	m_buttonNext->setFocus();
	m_buttonCancel = new QPushButton("&Cancel", this);
	m_buttonCancel->setGeometry(320, 240, 60, 20);

	connect(
		m_nickname, SIGNAL(textChanged(const QString &)),
		this, SLOT(slotSaveNickname(const QString &))
		);
	connect(
		m_fullname, SIGNAL(textChanged(const QString &)),
		this, SLOT(slotSaveFullname(const QString &))
		);
	connect(
		m_buttonNext, SIGNAL(clicked()),
		this, SLOT(slotNext())
		);
	connect(
		m_buttonCancel, SIGNAL(clicked()),
		this, SLOT(slotCancel())
		);
}

ProfileWizard1::~ProfileWizard1()
{
	delete m_buttonCancel;
	delete m_buttonNext;
	delete m_buttonBack;
	delete m_fullname;
	delete m_hintFullname;
	delete m_nickname;
	delete m_hintNickname;
	delete m_hint;

	delete m_info;
}

void ProfileWizard1::initialize()
{
	if(g_config->ReadString("nick", 0) != 0){
		m_nickname->setText(g_config->ReadString("nick", ""));
	}

	if(g_config->ReadString("userinfo", 0) != 0){
		m_fullname->setText(g_config->ReadString("userinfo", ""));
	}
	else{
		m_fullname->setText(QString(APP_NAME " User"));
	}


	unsigned idx = 4;		
	g_config->WriteInt("conspeed", conspeed_speeds[idx]);

	int tab[sizeof(conspeed_strs)/sizeof(conspeed_strs[0])]={1, 2, 2, 3, 4};
	g_config->WriteInt("keepupnet", tab[idx]);
}

void ProfileWizard1::slotSaveNickname(const QString &nickname)
{
	if((const char *)nickname != 0){
		strcpy(g_regnick, (const char *)nickname);
	}
	else{
		strcpy(g_regnick, "");
	}

	g_regnick[32] = 0;

	g_config->WriteString("nick", g_regnick);
}

void ProfileWizard1::slotSaveFullname(const QString &fullname)
{
	char buf[256];
	if((const char *)fullname != 0){
		strcpy(buf, (const char *)fullname);
	}
	else{
		strcpy(buf, "");
	}

	g_config->WriteString("userinfo", buf);
}

void ProfileWizard1::slotNext()
{
	emit signalNext();

	accept();
}

void ProfileWizard1::slotCancel()
{
	switch(QMessageBox::warning(this, APP_NAME, "Abort " APP_NAME " Profile Setup Wizard?", "&Yes", "&No")){
	case 0:
		emit signalCancel();
		reject();
		break;
	case 1:
	default:
		break;
	}
}
//end class ProfileWizard1

//class ProfileWizard2
ProfileWizard2::ProfileWizard2(QWidget *parent, const char *name)
:QDialog(parent, name)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);
	setCaption(QString(APP_NAME " Profile Wizard - 2/3"));
	setFixedSize(440, 300);

	m_info = new QLabel("", this);
	m_info->setGeometry(20, 20, 100, 220);
	m_info->setBackgroundColor(Qt::darkCyan);

	m_hint = new QLabel("Network Security", this);
	m_hint->setGeometry(140, 20, 200, 40);
	QFont font;
	font.setPointSize(static_cast<int>(font.pointSize() * 1.5));
	font.setBold(TRUE);
	font.setItalic(TRUE);
	m_hint->setFont(font);

	m_groupPrivateKey = new QGroupBox("Authentication", this);
	m_groupPrivateKey->setGeometry(140, 60, 280, 100);
	m_hintPrivateKey = new QLabel("Press Create Key Pair button to make new\npublic/private key pair.", m_groupPrivateKey);
	m_hintPrivateKey->setGeometry(10, 15, 260, 30);
	m_buttonGenerator = new QPushButton("Create Key Pair ...", m_groupPrivateKey);
	m_buttonGenerator->setGeometry(10, 50, 120, 20);
	
	m_groupPublicKey = new QGroupBox("Public Key", this);
	m_groupPublicKey->setGeometry(140, 165, 280, 75);
	m_hintPublicKey = new QLabel("You can import the public keys for hosts \nthat you would like to connect to.", m_groupPublicKey);
	m_hintPublicKey->setGeometry(10, 15, 260, 30);
	m_buttonImportPublicKey = new QPushButton("Import ...", m_groupPublicKey);
	m_buttonImportPublicKey->setGeometry(10, 50, 120, 20);

	m_buttonBack = new QPushButton("<&Back", this);
	m_buttonBack->setGeometry(200, 260, 60, 20);
	m_buttonNext = new QPushButton("&Next>", this);
	m_buttonNext->setGeometry(270, 260, 60, 20);
	m_buttonNext->setFocus();
	m_buttonCancel = new QPushButton("&Cancel", this);
	m_buttonCancel->setGeometry(360, 260, 60, 20);

	connect(
		m_buttonGenerator, SIGNAL(clicked()),
		this, SLOT(slotGenerator())
		);
	connect(
		m_buttonImportPublicKey, SIGNAL(clicked()),
		this, SLOT(slotImportPublicKey())
		);
	connect(
		m_buttonBack, SIGNAL(clicked()),
		this, SLOT(slotBack())
		);
	connect(
		m_buttonNext, SIGNAL(clicked()),
		this, SLOT(slotNext())
		);
	connect(
		m_buttonCancel, SIGNAL(clicked()),
		this, SLOT(slotCancel())
		);
}

ProfileWizard2::~ProfileWizard2()
{
	delete m_buttonCancel;
	delete m_buttonNext;
	delete m_buttonBack;

	delete m_buttonImportPublicKey;
	delete m_hintPublicKey;
	delete m_groupPublicKey;

	delete m_buttonGenerator;
	delete m_hintPrivateKey;
	delete m_groupPrivateKey;

	delete m_hint;

	delete m_info;
}

void ProfileWizard2::initialize()
{
}

void ProfileWizard2::slotGenerator()
{
	char parms[2048];
	sprintf(parms, "%s.pr4", g_config_prefix);
	generateKey(parms);
}

void ProfileWizard2::slotImportPublicKey()
{
	importPublicKeys();
}

void ProfileWizard2::slotBack()
{
	emit signalBack();

	accept();
}

void ProfileWizard2::slotNext()
{
	emit signalNext();

	accept();
}

void ProfileWizard2::slotCancel()
{
	switch(QMessageBox::warning(this, APP_NAME, "Abort " APP_NAME " Profile Setup Wizard?", "&Yes", "&No")){
	case 0:
		emit signalCancel();
		reject();
		break;
	case 1:
	default:
		break;
	}
}
//end class ProfileWizard2

//class ProfileWizard3
ProfileWizard3::ProfileWizard3(QWidget *parent, const char *name)
:QDialog(parent, name)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);
	setCaption(QString(APP_NAME " Profile Wizard - 3/3"));
	setFixedSize(400, 300);

	m_info = new QLabel("", this);
	m_info->setGeometry(20, 20, 100, 220);
	m_info->setBackgroundColor(Qt::darkCyan);

	m_hint = new QLabel("File Transfer", this);
	m_hint->setGeometry(140, 20, 200, 40);
	QFont font;
	font.setPointSize(static_cast<int>(font.pointSize() * 1.5));
	font.setBold(TRUE);
	font.setItalic(TRUE);
	m_hint->setFont(font);

	m_groupReceive = new QGroupBox("Receive", this);
	m_groupReceive->setGeometry(140, 60, 240, 85);
	m_hintReceive = new QLabel("Local directory for file received:", m_groupReceive);
	m_hintReceive->setGeometry(10, 15, 220, 20);
	m_receive = new QLineEdit(m_groupReceive);
	m_receive->setGeometry(10, 35, 220, 20);
	m_buttonReceive = new QPushButton("Select", m_groupReceive);
	m_buttonReceive->setGeometry(10, 60, 80, 20);

	m_groupShare = new QGroupBox("Share", this);
	m_groupShare->setGeometry(140, 150, 240, 85);
	m_hintShare = new QLabel("Directories to share:", m_groupShare);
	m_hintShare->setGeometry(10, 15, 220, 20);
	m_share = new QLineEdit(m_groupShare);
	m_share->setGeometry(10, 35, 220, 20);
	m_buttonShare = new QPushButton("Add", m_groupShare);
	m_buttonShare->setGeometry(10, 60, 80, 20);

	m_buttonBack = new QPushButton("<&Back", this);
	m_buttonBack->setGeometry(160, 260, 60, 20);
	m_buttonNext = new QPushButton("&Next>", this);
	m_buttonNext->setGeometry(230, 260, 60, 20);
	m_buttonNext->setEnabled(FALSE);
	m_buttonDone = new QPushButton("&Done", this);
	m_buttonDone->setGeometry(320, 260, 60, 20);
	m_buttonDone->setFocus();

	connect(
		m_receive, SIGNAL(textChanged(const QString &)),
		this, SLOT(slotReceive(const QString &))
		);
	connect(
		m_buttonReceive, SIGNAL(clicked()),
		this, SLOT(slotSetReceive())
		);
	connect(
		m_share, SIGNAL(textChanged(const QString &)),
		this, SLOT(slotShare(const QString &))
		);
	connect(
		m_buttonShare, SIGNAL(clicked()),
		this, SLOT(slotSetShare())
		);
	connect(
		m_buttonBack, SIGNAL(clicked()),
		this, SLOT(slotBack())
		);
	connect(
		m_buttonDone, SIGNAL(clicked()),
		this, SLOT(slotDone())
		);
}

ProfileWizard3::~ProfileWizard3()
{
	delete m_buttonDone;
	delete m_buttonNext;
	delete m_buttonBack;

	delete m_buttonShare;
	delete m_share;
	delete m_hintShare;
	delete m_groupShare;

	delete m_buttonReceive;
	delete m_receive;
	delete m_hintReceive;
	delete m_groupReceive;

	delete m_hint;

	delete m_info;
}

void ProfileWizard3::initialize()
{
	if(g_config->ReadString("downloadpath", 0) != 0){
		m_receive->setText(g_config->ReadString("downloadpath", ""));
	}

	if(g_config->ReadString("databasepath", 0) != 0){
		m_share->setText(g_config->ReadString("databasepath", ""));
	}
}

void ProfileWizard3::slotReceive(const QString &path)
{
	char buf[1024];
	if((const char *)path != 0){
		strcpy(buf, (const char *)path);
	}
	else{
		strcpy(buf, "");
	}

	g_config->WriteString("downloadpath", buf);
}

void ProfileWizard3::slotSetReceive()
{
	QString fn=QFileDialog::getExistingDirectory();
	if(fn != QString::null){
		char buf[1024];
		if((const char *)fn != 0){
			strcpy(buf, (const char *)fn);
		}
		else{
			strcpy(buf, "");
		}
		g_config->WriteString("downloadpath", buf);

		initialize();
	}
}

void ProfileWizard3::slotShare(const QString &paths)
{
	char buf[4096];
	if((const char *)paths != 0){
		strcpy(buf, (const char *)paths);
	}
	else{
		strcpy(buf, "");
	}

	g_config->WriteString("databasepath", buf);
}

void ProfileWizard3::slotSetShare()
{
	QString fn=QFileDialog::getExistingDirectory();
	if(fn != QString::null){
		char buf[4096];
		if((const char *)(m_share->text()) != 0){
			strcpy(buf, (const char *)(m_share->text()));
		}
		else{
			strcpy(buf, "");
		}
		if(!(m_share->text()).isEmpty()){
			strcat(buf, ";");
		}
		strcat(buf, (const char *)fn);
		g_config->WriteString("databasepath", buf);

		initialize();
	}
}

void ProfileWizard3::slotBack()
{
	emit signalBack();

	accept();
}

void ProfileWizard3::slotDone()
{
	emit signalDone();

	accept();
}
//end class ProfileWizard3
//end profile setup wizard

//class Passphrase
Passphrase::Passphrase(QWidget *parent, const char *name)
:QDialog(parent, name, true)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);
	setCaption(APP_NAME " Key Generator");

	setFixedSize(400, 230);

	m_hint = new QLabel("Enter your key password and key size below:", this);
	m_hint->setGeometry(20, 20, 360, 20);
	m_hintPassphrase = new QLabel("Password:", this);
	m_hintPassphrase->setGeometry(20, 80, 100, 20);
	m_passphrase = new QLineEdit(this);
	m_passphrase->setGeometry(130, 80, 250, 20);
	m_passphrase->setEchoMode(QLineEdit::Password);
	m_hintConfirm = new QLabel("Password again:", this);
	m_hintConfirm->setGeometry(20, 110, 100, 20);
	m_confirm = new QLineEdit(this);
	m_confirm->setGeometry(130, 110, 250, 20);
	m_confirm->setEchoMode(QLineEdit::Password);
	m_hintKeySize = new QLabel("Key size:", this);
	m_hintKeySize->setGeometry(20, 150, 100, 20);
	m_keySize = new QComboBox(this);
	m_keySize->setGeometry(130, 150, 250, 20);
	m_keySize->insertItem("1024 bits (weak, not recommended)", 0);
	m_keySize->insertItem("1536 bits (recommended)", 1);
	m_keySize->insertItem("2048 bits (slower, recommended)", 2);
	m_keySize->insertItem("3072 bits (slow, not recommended)", 3);
	m_keySize->insertItem("4096 bits (very slow, not recommended)", 4);
	m_keySize->setCurrentItem(1);
	m_buttonCancel = new QPushButton("&Cancel", this);
	m_buttonCancel->setGeometry(20, 190, 60, 20);
	m_buttonGenerate = new QPushButton("&Create", this);
	m_buttonGenerate->setGeometry(320, 190, 60, 20);

	connect(
		m_buttonCancel, SIGNAL(clicked()),
		this, SLOT(slotCancel())
		);
	connect(
		m_buttonGenerate, SIGNAL(clicked()),
		this, SLOT(slotGenerate())
		);
	connect(
		m_passphrase, SIGNAL(returnPressed()),
		this, SLOT(slotGenerate())
		);
	connect(
		m_confirm, SIGNAL(returnPressed()),
		this, SLOT(slotGenerate())
		);
}

Passphrase::~Passphrase()
{
	delete m_buttonGenerate;
	delete m_buttonCancel;
	delete m_keySize;
	delete m_hintKeySize;
	delete m_confirm;
	delete m_hintConfirm;
	delete m_passphrase;
	delete m_hintPassphrase;
	delete m_hint;
}

const char *Passphrase::getPassphrase()
{
	if(m_passphrase->text() != QString::null){
		return (const char *)(m_passphrase->text());
	}
	else{
		return NULL;
	}
}

int Passphrase::getKeySize()
{
	switch(m_keySize->currentItem()){
	case 0:
		return 1024;
		break;
	case 1:
		return 1536;
		break;
	case 2:
		return 2048;
		break;
	case 3:
		return 3072;
		break;
	case 4:
		return 4096;
		break;
	default:
		return 1536;
		break;
	}
}

void Passphrase::slotCancel()
{
	reject();
}

void Passphrase::slotGenerate()
{
	if(m_passphrase->text() != m_confirm->text()){
		QMessageBox::critical(
			0, QString(APP_NAME " Key Generator Error"),
			"Password mistyped"
			);

		m_passphrase->clear();
		m_confirm->clear();
		m_passphrase->setFocus();
	}
	else{
		accept();
	}
}
//end class Passphrase

//class KeyGenerator
KeyGenerator::KeyGenerator(QWidget *parent, const char *name)
:QDialog(parent, name),
 m_steps(0)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);
	setCaption(QString(APP_NAME " Key Generator"));

	setFixedSize(400, 150);

	m_hint = new QLabel("Generating key pair... please wait\n(this may take as long as 3 minutes)", this);
	m_hint->setGeometry(20, 20, 360, 40);
	m_progressBar = new QProgressBar(9000, this);
	m_progressBar->setGeometry(20, 70, 360, 20);
	m_timer = new QTimer(this);
	m_timer->start(100);
	m_buttonCancel = new QPushButton("&Cancel", this);
	m_buttonCancel->setGeometry(20, 110, 60, 20);

	kg_end = false;

	connect(
		m_buttonCancel, SIGNAL(clicked()),
		this, SLOT(slotCancel())
		);
	connect(
		m_timer, SIGNAL(timeout()),
		this, SLOT(slotPerform())
		);
	connect(
		this, SIGNAL(signalEnd()),
		this, SLOT(slotEnd())
		);
}

KeyGenerator::~KeyGenerator()
{
	delete m_buttonCancel;
	delete m_timer;
	delete m_progressBar;
	delete m_hint;
}

#ifdef _LINUX
bool KeyGenerator::kgThreadCreate(pthread_t *threadID, int kgKeySize)
#elif defined(_WIN32)
bool KeyGenerator::kgThreadCreate(HANDLE &threadID, int kgKeySize)
#endif	//_LINUX+_WIN32
{
	if(kgKeySize <= 1024){
		m_hint->setText("Generating key pair... please wait\n(this may take as long as 1 minutes)");
		m_progressBar->setTotalSteps(3000);
		m_progressBar->reset();
	}
	else if(kgKeySize <= 2048){
		m_hint->setText("Generating key pair... please wait\n(this may take as long as 3 minutes)");
		m_progressBar->setTotalSteps(9000);
		m_progressBar->reset();
	}
	else if(kgKeySize <= 3072){
		m_hint->setText("Generating key pair... please wait\n(this may take as long as 10 minutes)");
		m_progressBar->setTotalSteps(30000);
		m_progressBar->reset();
	}
	else{
		m_hint->setText("Generating key pair... please wait\n(this may take as long as 20 minutes)");
		m_progressBar->setTotalSteps(60000);
		m_progressBar->reset();
	}

	update();

#ifdef _LINUX
	if(pthread_create(threadID, 0, kgThread, 0) != 0){
		return false;
	}
#elif defined(_WIN32)
	DWORD id;
	threadID = CreateThread(NULL, 0, kgThread, NULL, 0, &id);
	if(threadID == NULL){
		return false;
	}
#endif	//_LINUX+_WIN32

	return true;
}

#ifdef _LINUX
void KeyGenerator::kgThreadCancel(pthread_t threadID)
#elif defined(_WIN32)
void KeyGenerator::kgThreadCancel(HANDLE &threadID)
#endif	//_LINUX+_WIN32
{
#ifdef _LINUX
	if(threadID){
		pthread_cancel(threadID);
	}
#elif defined(_WIN32)
	if(threadID){
		if(WaitForSingleObject(threadID, 0) == WAIT_TIMEOUT){
			CloseHandle(threadID);
			threadID = 0;
		}
	}
#endif	//_LINUX+_WIN32
}

void KeyGenerator::slotPerform()
{
	qApp->lock();
	if(!kg_end){
		m_progressBar->setProgress(m_steps);
		m_steps += 4;
		if(m_steps > m_progressBar->totalSteps()){
			m_steps = 0;
		}
	}
	else{
		emit signalEnd();
	}
	qApp->unlock();
}

void KeyGenerator::slotCancel()
{
	m_timer->stop();
	reject();
}

void KeyGenerator::slotEnd()
{
	time_t m_endTime = time((time_t *)0);
	m_timer->stop();
	m_progressBar->setProgress(m_progressBar->totalSteps());
	int elapsedTime=(int)difftime(m_endTime, m_startTime);
	char buf[256];
	sprintf(buf, "Completed in %2d minutes %2d seconds", elapsedTime/60, elapsedTime%60);
	m_hint->setText(buf);
	QMessageBox::information(0, QString(APP_NAME " Key Generator - Completed"), buf);

	accept();
}
//end class KeyGenerator

//preference sub windows
//class Panel
Panel::Panel(QWidget *parent, const char *name)
:QWidget(parent, name)
{
}

Panel::~Panel()
{
}
//end class Panel

//class PanelIdentity
PanelIdentity::PanelIdentity(QWidget *parent, const char *name)
:Panel(parent, name)
{
	setFixedSize(400, 200);

	m_groupNickname = new QGroupBox("Display name", this);
	m_groupNickname->setGeometry(20, 20, 360, 50);
	m_nickname = new QLineEdit(m_groupNickname);
	m_nickname->setGeometry(20, 20, 100, 20);

	m_groupUsername = new QGroupBox("User Information", this);
	m_groupUsername->setGeometry(20, 80, 360, 50);
	m_username = new QLineEdit(m_groupUsername);
	m_username->setGeometry(20, 20, 320, 20);
	connect(
		m_username, SIGNAL(textChanged(const QString &)),
		this, SLOT(slotUpdateUsername(const QString &))
		);
}

PanelIdentity::~PanelIdentity()
{
	delete m_username;
	delete m_groupUsername;

	delete m_nickname;
	delete m_groupNickname;

}

void PanelIdentity::initialize()
{
	m_nickname->setText(g_regnick);
	m_username->setText(g_config->ReadString("userinfo", APP_NAME " User"));

	update();
}

void PanelIdentity::updateNickname()
{
	printf("slotUpdateNickName\n");
	if(!(m_nickname->text().isEmpty())){
		char buf[32];
		if((const char *)(m_nickname->text()) != 0){
			strcpy(buf, (const char *)(m_nickname->text()));
		}
		else{
			strcpy(buf, "");
		}
		buf[31] = 0;
		if(strcmp(buf, g_regnick) != 0){
			char oldName[32];
			strcpy(oldName, g_regnick);
			strcpy(g_regnick, buf);

			if(g_regnick[0] == '#' || g_regnick[0] == '&'){
				g_regnick[0] = 0;
			}
			g_config->WriteString("nick", g_regnick);
			if(g_regnick[0]){
				chat_sendNickChangeFrom(oldName);
			}
		}
	}

	update();
}


void PanelIdentity::slotUpdateUsername(const QString &username)
{
	char buf[256];
	if((const char *)username != 0){
		strcpy(buf, (const char *)username);
	}
	else{
		strcpy(buf, "");
	}
	g_config->WriteString("userinfo", buf);

	update();
}

void PanelIdentity::checkUpdate()
{
	updateNickname();
}
//end class PanelIdentity


//class PanelNetworkPrivateKey
PanelNetworkPrivateKey::PanelNetworkPrivateKey(QWidget *parent, const char *name)
:Panel(parent, name)
{
	setFixedSize(400, 200);

	m_buttonGenerator = new QPushButton("Create key pair", this);
	m_buttonGenerator->setGeometry(20, 20, 180, 20);

	m_hintSignature = new QLabel("No key present.", this);
	m_hintSignature->setGeometry(20, 40, 250, 20);
	m_buttonReload = new QPushButton("Bring up key pair", this);
	m_buttonReload->setGeometry(210, 20, 180, 20);
	m_buttonChange = new QPushButton("Change password", this);
	m_buttonChange->setGeometry(20, 60, 370, 20);
	m_buttonChange->setEnabled(FALSE);
	m_buttonExportPublicKey = new QPushButton("Copy my public key to the clipboard", this);
	m_buttonExportPublicKey->setGeometry(20, 80, 370, 20);
	m_buttonExportPublicKey->setEnabled(FALSE);

	m_hintAESKeySize = new QLabel("AES key size:", this);
	m_hintAESKeySize->setGeometry(20, 120, 100, 20);
	m_hintAESKeySize->setAlignment(Qt::AlignRight);
	m_AESKeySize = new QComboBox(FALSE, this);
	m_AESKeySize->insertItem("128 bits (weak, not recommended)", 0);
	m_AESKeySize->insertItem("192 bits (recommended)", 1);
	m_AESKeySize->insertItem("256 bits (slower, recommended)", 2);
	m_AESKeySize->setGeometry(130, 120, 260, 20);

	connect(
		m_buttonGenerator, SIGNAL(clicked()),
		this, SLOT(slotGenerator())
		);
	connect(
		m_buttonReload, SIGNAL(clicked()),
		this, SLOT(slotReload())
		);
	connect(
		m_buttonChange, SIGNAL(clicked()),
		this, SLOT(slotChange())
		);
	connect(
		m_buttonExportPublicKey, SIGNAL(clicked()),
		this, SLOT(slotExportPublicKey())
		);
	connect(
		m_AESKeySize, SIGNAL(activated(int)),
		this, SLOT(slotAESKeySize(int))
		);
}

PanelNetworkPrivateKey::~PanelNetworkPrivateKey()
{
	delete m_AESKeySize;
	delete m_hintAESKeySize;
	delete m_buttonExportPublicKey;
	delete m_buttonChange;
	delete m_buttonReload;
	delete m_hintSignature;
	delete m_buttonGenerator;
}

void PanelNetworkPrivateKey::initialize()
{
	char sign[SHA_OUTSIZE*2+64];
	if(g_key.bits){
		strcpy(sign, "Key signature:");
		int t=strlen(sign);
		for(int x=0; x<SHA_OUTSIZE; x++){
			sprintf(sign+t+x*2, "%02X", g_pubkeyhash[x]);
		}
		m_buttonChange->setEnabled(TRUE);
		m_buttonExportPublicKey->setEnabled(TRUE);
		m_hintSignature->setText("");
	}
	else{
		strcpy(sign, "No key present.");
		m_buttonChange->setEnabled(FALSE);
		m_buttonExportPublicKey->setEnabled(FALSE);
		m_hintSignature->setText(sign);
	}

	switch(g_config->ReadInt("aeskeysize", 128)){
	case 128:
		m_AESKeySize->setCurrentItem(0);
		break;
	case 192:
		m_AESKeySize->setCurrentItem(1);
		break;
	case 256:
		m_AESKeySize->setCurrentItem(2);
		break;
	default:
		break;
	}

	update();
}

void PanelNetworkPrivateKey::slotGenerator()
{
	bool warn=false;
	char tmp[1024];
	sprintf(tmp, "%s.pr4", g_config_prefix);
	FILE *fp=fopen(tmp, "rb");
	if(fp){
		fclose(fp);
		warn = true;
	}

	int ahead;
	if(warn){
		ahead = QMessageBox::warning(this, QString(APP_NAME " Create Key pair Warning"), "Warning - Creating a new key pair will remove your old key pair.\n To go ahead and create a new key pair hit OK.", "&OK", "&Cancel");

		if(ahead != 0){
			return;
		}
	}

	if(!warn || (ahead == 0)){
		char parms[2048];
		sprintf(parms, "%s.pr4", g_config_prefix);
		generateKey(parms);

		initialize();
	}
	main_BroadcastPublicKey();
}

void PanelNetworkPrivateKey::slotReload()
{
	reloadKey(NULL);

	initialize();
}

void PanelNetworkPrivateKey::slotChange()
{
	DlgChPass dlgChPass;
	int ret;
INVALID:
	ret = dlgChPass.exec();
	if(ret == QDialog::Accepted){
		char oldpass[1024];
		char newpass[1024];
		if(dlgChPass.getOld() != 0){
			strcpy(oldpass, dlgChPass.getOld());
		}
		else{
			strcpy(oldpass, "");
		}
		if(dlgChPass.getNew()){
			strcpy(newpass, dlgChPass.getNew());
		}
		else{
			strcpy(newpass, "");
		}

		R_RSA_PRIVATE_KEY key;
		char keyfn[1024];
		sprintf(keyfn, "%s.pr4", g_config_prefix);
		memset(&key, 0, sizeof(key));
		if(doLoadKey(oldpass, keyfn, &key) || !key.bits){
			QMessageBox::critical(0, QString(APP_NAME " Error"), "Invalid password for private key");
			goto INVALID;
		}

		char newpasshash[SHA_OUTSIZE];
		SHAify c;
		c.add((unsigned char *)newpass, strlen(newpass));
		c.final((unsigned char *)newpasshash);

		kg_writePrivateKey(keyfn, &key, &g_random, newpasshash);

		memset(newpasshash, 0, sizeof(newpasshash));
		memset(newpass, 0, sizeof(newpass));
		memset(oldpass, 0, sizeof(oldpass));
		memset(&key, 0, sizeof(key));
	}
}

void PanelNetworkPrivateKey::slotExportPublicKey()
{
	copyMyPubKey();
}


void PanelNetworkPrivateKey::slotAESKeySize(int idx)
{
	switch(idx){
	case 0:
		g_aes_keysize = 128;
		break;
	case 1:
		g_aes_keysize = 192;
		break;
	case 2:
		g_aes_keysize = 256;
		break;
	default:
		break;
	}
	g_config->WriteInt("aeskeysize", g_aes_keysize);
}


//end class PanelNetworkPrivateKey

//class PanelNetworkPublicKeys
PanelNetworkPublicKeys::PanelNetworkPublicKeys(QWidget *parent, const char *name)
:Panel(parent, name)
{
	setFixedSize(400, 200);

	m_groupPublicKeys = new QGroupBox("Trusted public keys", this);
	m_groupPublicKeys->setGeometry(0, 0, 400, 200);

	m_publicKeys = new QListView(m_groupPublicKeys);
	m_publicKeys->setSorting(-1);
	m_publicKeys->setGeometry(20, 20, 360, 140);
	m_publicKeys->addColumn("Name", 150);
	m_publicKeys->addColumn("Authentication code", 210);
	m_buttonAdd = new QPushButton("Add", m_groupPublicKeys);
	m_buttonAdd->setGeometry(20, 170, 70, 20);
	m_buttonRemove= new QPushButton("Delete", m_groupPublicKeys);
	m_buttonRemove->setGeometry(100, 170, 70, 20);
	m_buttonView = new QPushButton("View", m_groupPublicKeys);
	m_buttonView->setGeometry(230, 170, 70, 20);
	m_buttonReload = new QPushButton("Refresh", m_groupPublicKeys);
	m_buttonReload->setGeometry(310, 170, 70, 20);

	connect(
		m_buttonAdd, SIGNAL(clicked()),
		this, SLOT(slotAdd())
		);
	connect(
		m_buttonRemove, SIGNAL(clicked()),
		this, SLOT(slotRemove())
		);
	connect(
		m_buttonView, SIGNAL(clicked()),
		this, SLOT(slotView())
		);
	connect(
		m_buttonReload, SIGNAL(clicked()),
		this, SLOT(slotReload())
		);
}

PanelNetworkPublicKeys::~PanelNetworkPublicKeys()
{
	delete m_buttonReload;
	delete m_buttonView;
	delete m_buttonRemove;
	delete m_buttonAdd;
	delete m_publicKeys;
	delete m_groupPublicKeys;
}

void PanelNetworkPublicKeys::initialize()
{
	m_publicKeys->clear();

	int x;
	char buf[128];
	int a;
	for(x=0; x<g_pklist.GetSize(); ++x){
		PKitem *p=g_pklist.Get(x);
		for(a=0; a<SHA_OUTSIZE; ++a){
			sprintf(buf+a*2, "%02X", p->hash[a]);
		}
		m_publicKeys->insertItem(new QListViewItem(m_publicKeys, p->name, buf));
	}

	update();
}

int PanelNetworkPublicKeys::itemIndex(QListViewItem *item)
{
	int retVal=-1;
	int index=-1;
	QListViewItem *i=m_publicKeys->firstChild();
	while(i != 0){
		++index;
		if(item == i){
			retVal = index;
			break;
		}

		i = i->nextSibling();
	}

	return retVal;
}

void PanelNetworkPublicKeys::slotAdd()
{
	importPublicKeys();

	initialize();
}

void PanelNetworkPublicKeys::slotRemove()
{
	int x=itemIndex(m_publicKeys->currentItem());

	if(x != -1){
		x = m_publicKeys->childCount()-x-1;
		free(g_pklist.Get(x));
		g_pklist.Del(x);
		--x;

		savePKList();

		initialize();
	}
}

void PanelNetworkPublicKeys::slotView()
{
	char str[2048];
	sprintf(str, "%s.pr3", g_config_prefix);
	QString fn=QString(str);

	KeyViewer keyViewer;
	keyViewer.ReadText(fn);
	keyViewer.exec();
}

void PanelNetworkPublicKeys::slotReload()
{
	int x=g_pklist.GetSize() - 1;
	while(x >= 0){
		free(g_pklist.Get(x));
		g_pklist.Del(x);
		--x;
	}
	x=g_pklist_pending.GetSize()-1;
	while(x >= 0){
		free(g_pklist_pending.Get(x));
		g_pklist_pending.Del(x);
		--x;
	}
	loadPKList();

	initialize();
}
//end class PanelNetworkPublicKeys

//class PanelFileTransfers
PanelFileTransfers::PanelFileTransfers(QWidget *parent, const char *name)
:Panel(parent, name)
{
	setFixedSize(400, 200);

	m_groupReceiving = new QGroupBox("Receiving files", this);
	m_groupReceiving->setGeometry(0, 0, 400, 100);

	m_hintPath = new QLabel("Local directory for file received:", m_groupReceiving);
	m_hintPath->setGeometry(20, 20, 360, 20);
	m_path = new QLineEdit("~/", m_groupReceiving);
	m_path->setGeometry(20, 40, 360, 20);
	m_buttonPath = new QPushButton("Select", m_groupReceiving);
	m_buttonPath->setGeometry(20, 60, 80, 20);

	// Added by LoraYin 2004-2-27
	m_groupSending = new QGroupBox("Sending files", this);
	m_groupSending->setGeometry(0, 100, 400, 100);

	m_hintPathSharing = new QLabel("Directories to share:", m_groupSending);
	m_hintPathSharing->setGeometry(20, 20, 340, 20);
	m_pathSharing = new QLineEdit(m_groupSending);
	m_pathSharing->setGeometry(20, 40, 360, 20);
	m_buttonAddPathSharing = new QPushButton("Add", m_groupSending);
	m_buttonAddPathSharing->setGeometry(20, 60, 60, 20);
	m_buttonRescan = new QPushButton("Refresh", m_groupSending);
	m_buttonRescan->setGeometry(90, 60, 60, 20);
	m_hintScan = new QLabel("Total files shared:", m_groupSending);
	m_hintScan->setGeometry(230, 60, 150, 20);

	m_timer = new QTimer(this);
	m_timer->start(300);

	connect(
		m_path, SIGNAL(textChanged(const QString &)),
		this, SLOT(slotPathChanged(const QString &))
		);
	connect(
		m_buttonPath, SIGNAL(clicked()),
		this, SLOT(slotPath())
		);
	connect(
		m_pathSharing, SIGNAL(textChanged(const QString &)),
		this, SLOT(slotPathSharingChanged(const QString &))
		);
	connect(
		m_buttonAddPathSharing, SIGNAL(clicked()),
		this, SLOT(slotPathSharingAdd())
		);
	connect(
		m_buttonRescan, SIGNAL(clicked()),
		this, SLOT(slotPathSharingRescan())
		);

	connect(
		m_timer, SIGNAL(timeout()),
		this, SLOT(slotRefresh())
		);
}

PanelFileTransfers::~PanelFileTransfers()
{
	delete m_buttonPath;
	delete m_path;
	delete m_hintPath;
	delete m_groupReceiving;

	delete m_groupSending;
	delete m_hintScan;
	delete m_buttonRescan;
	delete m_buttonAddPathSharing;
	delete m_pathSharing;
	delete m_hintPathSharing;
	delete m_timer;
}

void PanelFileTransfers::initialize()
{
	m_path->setText(g_config->ReadString("downloadpath", ""));
	if(g_scan_status_buf){
		m_hintScan->setText(g_scan_status_buf);
	}
	m_pathSharing->setText(g_config->ReadString("databasepath", ""));

	update();
}

void PanelFileTransfers::slotPathChanged(const QString &path)
{
	char buf[1024];
	if((const char *)path != 0){
		strcpy(buf, (const char *)path);
	}
	else{
		strcpy(buf, "");
	}
	g_config->WriteString("downloadpath", buf);
}

void PanelFileTransfers::slotPath()
{
	char buf[1024];
	QString fn=QFileDialog::getExistingDirectory(
		"~",
		this,
		"set Receive path",
		"Choose a directory"
		);
	if(fn != QString::null){
		if((const char *)fn != 0){
			strcpy(buf, (const char *)fn);
		}
		else{
			strcpy(buf, "");
		}
		g_config->WriteString("downloadpath", buf);
		m_path->setText(fn);
	}

	initialize();
}

void PanelFileTransfers::slotPathSharingChanged(const QString &path)
{
	char buf[4096];
	if((const char *)path != 0){
		strcpy(buf, (const char *)path);
	}
	else{
		strcpy(buf, "");
	}
	g_config->WriteString("databasepath", buf);

	doDatabaseRescan();
}

void PanelFileTransfers::slotPathSharingAdd()
{
	char buf[4096];
	if((const char *)(m_pathSharing->text()) != 0){
		strcpy(buf, (const char *)(m_pathSharing->text()));
	}
	else{
		strcpy(buf, "");
	}
	QString fn=QFileDialog::getExistingDirectory(
		"~",
		this,
		"Add share path",
		"Choose a directory"
		);
	if(fn != QString::null){
		if(buf[0] && buf[strlen(buf)-1]!=';'){
			strcat(buf, ";");
		}
		strcat(buf, (const char *)fn);
		m_pathSharing->setText(fn);
		g_config->WriteString("databasepath", buf);
	}

	initialize();
}

void PanelFileTransfers::slotPathSharingRescan()
{
	doDatabaseRescan();
	initialize();
}


void PanelFileTransfers::slotRefresh()
{
	m_hintScan->setText(g_scan_status_buf);

	update();
}
//end class PanelFileTransfers

//class DlgChPass
DlgChPass::DlgChPass(QWidget *parent, const char *name)
:QDialog(parent, name, true)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);
	setCaption("Change Password");

	setFixedSize(400, 160);

	m_hintOld = new QLabel("Old password:", this);
	m_hintOld->setGeometry(20, 20, 150, 20);
	m_old = new QLineEdit(this);
	m_old->setGeometry(180, 20, 200, 20);
	m_old->setEchoMode(QLineEdit::Password);
	m_hintNew = new QLabel("New password:", this);
	m_hintNew->setGeometry(20, 50, 150, 20);
	m_new = new QLineEdit(this);
	m_new->setGeometry(180, 50, 200, 20);
	m_new->setEchoMode(QLineEdit::Password);
	m_hintConfirm = new QLabel("New password again:", this);
	m_hintConfirm->setGeometry(20, 80, 150, 20);
	m_confirm = new QLineEdit(this);
	m_confirm->setGeometry(180, 80, 200, 20);
	m_confirm->setEchoMode(QLineEdit::Password);
	m_buttonOK = new QPushButton("&OK", this);
	m_buttonOK->setGeometry(20, 120, 60, 20);
	m_buttonCancel = new QPushButton("&Cancel", this);
	m_buttonCancel->setGeometry(100, 120, 60, 20);

	connect(
		m_buttonOK, SIGNAL(clicked()),
		this, SLOT(slotOK())
		);
	connect(
		m_buttonCancel, SIGNAL(clicked()),
		this, SLOT(slotCancel())
		);
	connect(
		m_old, SIGNAL(returnPressed()),
		this, SLOT(slotOK())
		);
	connect(
		m_new, SIGNAL(returnPressed()),
		this, SLOT(slotOK())
		);
	connect(
		m_confirm, SIGNAL(returnPressed()),
		this, SLOT(slotOK())
		);
}

DlgChPass::~DlgChPass()
{
	delete m_buttonCancel;
	delete m_buttonOK;
	delete m_confirm;
	delete m_hintConfirm;
	delete m_new;
	delete m_hintNew;
	delete m_old;
	delete m_hintOld;
}

const char *DlgChPass::getOld()
{
	if(m_old->text() != QString::null){
		return (const char *)(m_old->text());
	}

	return NULL;
}

const char *DlgChPass::getNew()
{
	if(m_new->text() != QString::null){
		return (const char *)(m_new->text());
	}

	return NULL;
}

void DlgChPass::slotOK()
{
	if(m_new->text() == m_confirm->text()){
		accept();
	}
	else{
		QMessageBox::critical(0, QString(APP_NAME " Error"), "New password mistyped");
		m_new->clear();
		m_confirm->clear();
		m_new->setFocus();
	}
}

void DlgChPass::slotCancel()
{
	reject();
}
//end class DlgChPass

//class DlgImportPublicKeys
DlgImportPublicKeys::DlgImportPublicKeys(QWidget *parent, const char *name)
:QFileDialog(parent, name)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);
	setCaption("Import public key file(s)");

	QString types("Public key files (*.pub;*.txt)\nAll files (*.*)");
	setFilters(types);

	m_keyLoader = new KeyLoader(this);
	addWidgets(0, m_keyLoader, 0);

	setFixedSize(sizeHint());

	connect(
		m_keyLoader, SIGNAL(signalLoaded()),
		this, SLOT(slotLoaded())
		);
}

DlgImportPublicKeys::~DlgImportPublicKeys()
{
	delete m_keyLoader;
}

const QString DlgImportPublicKeys::getFileName()
{
	return selectedFile();
}

void DlgImportPublicKeys::slotLoaded()
{
	reject();
}
//end class DlgImportPublicKeys

//class KeyLoader
KeyLoader::KeyLoader(QWidget *parent, const char *name)
:QVBox(parent, name)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);

	m_hintKeyText = new QLabel("Public key text (optionally paste in public keys here):", this);
	m_keyText = new QMultiLineEdit(this);
	m_keyText->setFixedSize(300, 100);
	m_buttonLoad = new QPushButton("&Load key text", this);
	m_buttonLoad->setFixedSize(150, 40);

	connect(
		m_buttonLoad, SIGNAL(clicked()),
		this, SLOT(slotLoad())
		);
}

KeyLoader::~KeyLoader()
{
	delete m_buttonLoad;
	delete m_keyText;
	delete m_hintKeyText;
}

void KeyLoader::slotLoad()
{
#ifdef _LINUX
	char *fn="/tmp/wstXXXXXX";
#elif defined (_WIN32)
	char fn[1024];
	if(!GetTempPath(1024, fn)){
		strcpy(fn, ".\\");
	}

	strcat(fn, "wstTempFile");
#endif	//_LINUX+_WIN32
	FILE *fp;
	int num=0;

	fp = fopen(fn, "wt");
	if(fp){
		int tlen=strlen((const char *)(m_keyText->text()));
		char *buf=new char[tlen+1];

		if(buf){
			if((const char *)(m_keyText->text()) != 0){
				strcpy(buf, (const char *)(m_keyText->text()));
			}
			else{
				strcpy(buf, "");
			}
			buf[tlen] = 0;

			fwrite(buf, 1, strlen(buf), fp);

			delete [] buf;
		}

		fclose(fp);
		num += loadPKList(fn);
	}

#ifdef _WIN32
	if(fn[0] == '.'){
		_unlink(fn);
	}
#endif	//_LINUX+_WIN32

	if(!num){
		QMessageBox::warning(0, QString(APP_NAME " Public Key Import Error"), "Error: No key(s) found in text");
		return;
	}

	savePKList();
	char buf[1024];
	sprintf(buf, "Imported %d public keys successfully.", num);
	QMessageBox::information(0, QString(APP_NAME " Public Key Import"), buf);

	emit signalLoaded();
}
//end class KeyLoader

//class KeyViewer
KeyViewer::KeyViewer(QWidget *parent, const char *name)
:QDialog(parent, name)
{
	QPixmap appIcon("icons/new_icon.png");
	setIcon(appIcon);
	setCaption("Public key viewer");

	m_text = new QMultiLineEdit(this);
	m_text->setGeometry(20, 20, 460, 240);
	m_text->setReadOnly(TRUE);
	m_buttonOK = new QPushButton("&OK", this);
	m_buttonOK->setGeometry(210, 280, 80, 40);
	m_buttonOK->setFocus();

	setFixedSize(500, 340);

	connect(
		m_buttonOK, SIGNAL(clicked()),
		this, SLOT(slotExit())
		);
}

KeyViewer::~KeyViewer()
{
	delete m_buttonOK;
	delete m_text;
}

void KeyViewer::ReadText(const QString &fn)
{
	m_text->setAutoUpdate(FALSE);

	QFile f(fn);
	if(!f.open(IO_ReadOnly)){
		return;
	}

	QTextStream t(&f);

	m_text->setText(t.read());

	f.close();

	m_text->repaint();
}

void KeyViewer::slotExit()
{
	accept();
}
//end class KeyViewer

