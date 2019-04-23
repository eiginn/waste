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
 *		prefdlgs.h
 *
 *	\Brief
 *		Define common dialogs used by MichaelZhu
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

#ifndef PREFDLGS_H
#define PREFDLGS_H

//use QT classes for inheriting
#include <qwidget.h>
#include <qdialog.h>
#include <qvbox.h>

//include QT tool classes
#include <qlabel.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qfiledialog.h>
#include <qprogressbar.h>
#include <qevent.h>
#include <time.h>

#ifdef _LINUX
#include <pthread.h>
#elif defined(_WIN32)
#include <windows.h>
#endif	//_LINUX+_WIN32

extern bool kg_end;

extern time_t m_startTime;

class DlgGetPassword: public QDialog
{
	Q_OBJECT;

private:
	QLabel *m_hint;
	QLineEdit *m_password;
	QCheckBox *m_save;
	QPushButton *m_OK;
	QPushButton *m_CANCEL;

public:
	DlgGetPassword(QWidget * =0, const char * =0);
	~DlgGetPassword();

	void initialize();
	const char *getPassword();

public slots:
	void slotOK();
	void slotCANCEL();
};

class ProfileManager:public QDialog
{
	Q_OBJECT;

private:
	QLabel *m_hintProfileManager;
	QPushButton *m_buttonCreate;
	QPushButton *m_buttonDelete;
	QPushButton *m_buttonCopy;
	QPushButton *m_buttonRename;
	QListBox *m_userNames;
	QCheckBox *m_startup;
	QLabel *m_hintStartup;
	QPushButton *m_buttonLaunch;
	QPushButton *m_buttonQuit;

public:
	ProfileManager(QWidget * =0, const char * =0);
	~ProfileManager();

	void initialize();
	void setLaunchEnabled(bool);

public slots:
	void slotCreate();
	void slotDelete();
	void slotCopy();
	void slotRename();
	void slotStartup();
	void slotLaunch();
	void slotQuit();
};

class DlgProfile:public QDialog
{
	Q_OBJECT;

private:
	QLabel *m_hintSrc;
	QLineEdit *m_src;
	QLabel *m_hintDest;
	QLineEdit *m_dest;
	QPushButton *m_buttonOK;
	QPushButton *m_buttonCancel;

public:
	DlgProfile(QWidget * =0, const char * =0);
	~DlgProfile();

	void initialize(const char *title, const char * =NULL, const char * =NULL);
	const char *getSrc();
	const char *getDest();

public slots:
	void slotOK();
	void slotCancel();
};

//profile setup wizard
class ProfileWizard1;
class ProfileWizard2;
class ProfileWizard3;

class WizardManager: public QObject
{
	Q_OBJECT;

private:
	ProfileWizard1 *m_wizard1;
	ProfileWizard2 *m_wizard2;
	ProfileWizard3 *m_wizard3;

	bool m_exeState;

public:
	WizardManager();
	~WizardManager();

	bool exec();

public slots:
	void slotQuit();
	void slotFinished();
	void slotShowPage1();
	void slotShowPage2();
	void slotShowPage3();
};

class ProfileWizard1: public QDialog
{
	Q_OBJECT;

private:
	QLabel *m_info;

	QLabel *m_hint;

	QLabel *m_hintNickname;
	QLineEdit *m_nickname;
	QLabel *m_hintFullname;
	QLineEdit *m_fullname;

	QPushButton *m_buttonBack;
	QPushButton *m_buttonNext;
	QPushButton *m_buttonCancel;

public:
	ProfileWizard1(QWidget * =0, const char * =0);
	~ProfileWizard1();

	void initialize();

public slots:
	void slotSaveNickname(const QString &);
	void slotSaveFullname(const QString &);

	void slotNext();
	void slotCancel();

signals:
	void signalNext();
	void signalCancel();
};

class ProfileWizard2: public QDialog
{
	Q_OBJECT;

private:
	QLabel *m_info;

	QLabel *m_hint;

	QGroupBox *m_groupPrivateKey;
	QLabel *m_hintPrivateKey;
	QPushButton *m_buttonGenerator;

	QGroupBox *m_groupPublicKey;
	QLabel *m_hintPublicKey;
	QPushButton *m_buttonImportPublicKey;

	QPushButton *m_buttonBack;
	QPushButton *m_buttonNext;
	QPushButton *m_buttonCancel;

public:
	ProfileWizard2(QWidget * =0, const char * =0);
	~ProfileWizard2();

	void initialize();

public slots:
	void slotGenerator();
	void slotImportPublicKey();

	void slotBack();
	void slotNext();
	void slotCancel();

signals:
	void signalBack();
	void signalNext();
	void signalCancel();
};

class ProfileWizard3: public QDialog
{
	Q_OBJECT;

private:
	QLabel *m_info;

	QLabel *m_hint;

	QGroupBox *m_groupReceive;
	QLabel *m_hintReceive;
	QLineEdit *m_receive;
	QPushButton *m_buttonReceive;

	QGroupBox *m_groupShare;
	QLabel *m_hintShare;
	QLineEdit *m_share;
	QPushButton *m_buttonShare;

	QPushButton *m_buttonBack;
	QPushButton *m_buttonNext;
	QPushButton *m_buttonDone;

public:
	ProfileWizard3(QWidget * =0, const char * =0);
	~ProfileWizard3();

	void initialize();

public slots:
	void slotReceive(const QString &);
	void slotSetReceive();
	void slotShare(const QString &);
	void slotSetShare();

	void slotBack();
	void slotDone();

signals:
	void signalBack();
	void signalDone();
};
//profile setup wizard

class Passphrase: public QDialog
{
	Q_OBJECT;

private:
	QLabel *m_hint;
	QLabel *m_hintPassphrase;
	QLineEdit *m_passphrase;
	QLabel *m_hintConfirm;
	QLineEdit *m_confirm;
	QLabel *m_hintKeySize;
	QComboBox *m_keySize;
	QPushButton *m_buttonCancel;
	QPushButton *m_buttonGenerate;

public:
	Passphrase(QWidget * =0, const char * =0);
	~Passphrase();

	const char *getPassphrase();
	int getKeySize();

public slots:
	void slotCancel();
	void slotGenerate();

};

class KeyGenerator: public QDialog
{
	Q_OBJECT

private:
	QLabel *m_hint;
	QProgressBar *m_progressBar;
	QTimer *m_timer;
	int m_steps;
	QPushButton *m_buttonCancel;

public:
	KeyGenerator(QWidget * =0, const char * =0);
	~KeyGenerator();

#ifdef _LINUX
	bool kgThreadCreate(pthread_t *threadID, int);
	void kgThreadCancel(pthread_t threadID);
#elif defined(_WIN32)
	bool kgThreadCreate(HANDLE &threadID, int);
	void kgThreadCancel(HANDLE &threadID);
#endif	//_LINUX+_WIN32

public slots:
	void slotPerform();
	void slotCancel();
	void slotEnd();

signals:
	void signalEnd();
};

//preference sub windows
class Panel: public QWidget
{
public:
	Panel(QWidget * =0, const char * =0);
	virtual ~Panel();

	virtual void initialize()	{};

};

class PanelIdentity: public Panel
{
	Q_OBJECT;

private:

	QGroupBox *m_groupNickname;
	QLineEdit *m_nickname;
	QLabel *m_hintNickname;

	QGroupBox *m_groupUsername;
	QLineEdit *m_username;

public:
	PanelIdentity(QWidget * =0, const char * =0);
	~PanelIdentity();

	void initialize();
	/* Added by LoraYin 2004-2-27 */
	void checkUpdate();
	void updateNickname();

public slots:
	void slotUpdateUsername(const QString &);
};


class PanelNetworkPrivateKey: public Panel
{
	Q_OBJECT;

private:
	QPushButton *m_buttonGenerator;
	QLabel *m_hintSignature;
	QPushButton *m_buttonReload;
	QPushButton *m_buttonChange;
	QPushButton *m_buttonExportPublicKey;
	QLabel *m_hintAESKeySize;
	QComboBox *m_AESKeySize;

public:
	PanelNetworkPrivateKey(QWidget * =0, const char * =0);
	~PanelNetworkPrivateKey();

	void initialize();

public slots:
	void slotGenerator();
	void slotReload();
	void slotChange();
	void slotExportPublicKey();
	void slotAESKeySize(int);
};

class PanelNetworkPublicKeys: public Panel
{
	Q_OBJECT;

private:
	QGroupBox *m_groupPublicKeys;
	QListView *m_publicKeys;
	QPushButton *m_buttonAdd;
	QPushButton *m_buttonRemove;
	QPushButton *m_buttonView;
	QPushButton *m_buttonReload;

public:
	PanelNetworkPublicKeys(QWidget * =0, const char * =0);
	~PanelNetworkPublicKeys();

	void initialize();
	int itemIndex(QListViewItem *);

public slots:
	void slotAdd();
	void slotRemove();
	void slotView();
	void slotReload();
};

class PanelFileTransfers: public Panel
{
	Q_OBJECT;

private:
	QGroupBox *m_groupReceiving;
	QLabel *m_hintPath;
	QLineEdit *m_path;
	QPushButton *m_buttonPath;

	// Added by LoraYin 2004-2-27
	QGroupBox *m_groupSending;
	QLabel *m_hintPathSharing;
	QLineEdit *m_pathSharing;
	QPushButton *m_buttonAddPathSharing;
	QPushButton *m_buttonRescan;
	QLabel *m_hintScan;
	QTimer *m_timer;

public:
	PanelFileTransfers(QWidget * =0, const char * =0);
	~PanelFileTransfers();

	void initialize();

public slots:
	void slotPathChanged(const QString &);
	void slotPath();
	
	void slotPathSharingChanged(const QString &);
	void slotPathSharingAdd();
	void slotPathSharingRescan();
	void slotRefresh();
};


class DlgChPass: public QDialog
{
	Q_OBJECT;

private:
	QLabel *m_hintOld;
	QLineEdit *m_old;
	QLabel *m_hintNew;
	QLineEdit *m_new;
	QLabel *m_hintConfirm;
	QLineEdit *m_confirm;
	QPushButton *m_buttonOK;
	QPushButton *m_buttonCancel;

public:
	DlgChPass(QWidget * =0, const char * =0);
	~DlgChPass();

	const char *getOld();
	const char *getNew();

public slots:
	void slotOK();
	void slotCancel();
};

class KeyLoader;
class DlgImportPublicKeys: public QFileDialog
{
	Q_OBJECT;

private:
	KeyLoader *m_keyLoader;

public:
	const QString getFileName();

public:
	DlgImportPublicKeys(QWidget * =0, const char * =0);
	~DlgImportPublicKeys();

public slots:
	void slotLoaded();
};

class KeyLoader: public QVBox
{
	Q_OBJECT;

private:
	QLabel *m_hintKeyText;
	QMultiLineEdit *m_keyText;
	QPushButton *m_buttonLoad;

public:
	KeyLoader(QWidget * =0, const char * =0);
	~KeyLoader();

public slots:
	void slotLoad();

signals:
	void signalLoaded();
};

class KeyViewer: public QDialog
{
	Q_OBJECT;

private:
	QMultiLineEdit *m_text;
	QPushButton *m_buttonOK;

public:
	KeyViewer(QWidget * =0, const char * =0);
	~KeyViewer();

	void ReadText(const QString &);

public slots:
	void slotExit();
};

#endif	//PREFDLGS_H
