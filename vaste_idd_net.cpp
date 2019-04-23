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
 * This file implements the display of net status
 * Author: Raymond Cai
 * Date Created: 2003.12.1
*/
#include "main.h"
#include "vaste_idd_net.h"
#include "ui_listview.h"
#include "config.h"
#include "mqueuelist.h"
#include <qcombobox.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qheader.h>

#include "tag.h"
#if defined(QTUI)
extern W_ListView g_lvnetcons;
#endif
#ifndef SAVE_CB_HOSTS
	#define SAVE_CB_HOSTS // turn this off for more privacy
	#define MAX_CBHOSTS 32
#endif
extern C_Config *g_config;
extern W_ListView g_lvnetcons;
extern C_MessageQueueList *g_mql;
extern void AddConnection(char *, int, int); //stanleysan
extern int g_extrainf;
extern C_ItemList<C_Connection> g_new_net;
extern int g_keepup;

Vaste_Idd_Net::Vaste_Idd_Net( QWidget* parent , const char* name , WFlags fl )
    : IDD_NET( parent, name, fl )
{
#include "vaste_widget_palette.xx"
    setPalette( dialog_pal );
    ComboBox_IDC_NEWCONNECTION->setPalette( combobox_pal );
    PushButton_IDC_ADDCONNECTION->setPalette( pushbutton_pal );
    PushButton_IDC_CONNECTCATCH->setPalette( pushbutton_pal );
    PushButton_IDC_REMOVECONNECTION->setPalette( pushbutton_pal );
    PushButton_IDC_REMOVECATCH->setPalette( pushbutton_pal );
    LineEdit_IDC_NUMCONUP->setPalette( texteditor_pal );
    ListView_IDC_NETCONS->setPalette( listview_pal );
	Main_Pane->setPalette( frame_pal );

	//set button icons here.

    QPixmap image1( "icons/vaste_mainwindow_network_enable.png" );
    QPixmap image2( "icons/vaste_net_reconnect_enable.png" );
    QPixmap image3( "icons/vaste_net_disconnect_enable.png" );
    QPixmap image4( "icons/vaste_net_delete_enable.png" );

	PushButton_IDC_ADDCONNECTION->setPixmap( image1 );
	PushButton_IDC_CONNECTCATCH->setPixmap( image2 );
	PushButton_IDC_REMOVECONNECTION->setPixmap( image3 );
	PushButton_IDC_REMOVECATCH->setPixmap( image4 );
	
	// skin settings here.
	QPixmap button_bg("skins/vaste_toolbutton_bg.png");
	
	PushButton_IDC_ADDCONNECTION->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_CONNECTCATCH->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_REMOVECONNECTION->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_REMOVECATCH->setPaletteBackgroundPixmap(button_bg);

	QPixmap dlg_bg("skins/vaste_pane_bg.png");
	QPixmap pane_bg("skins/vaste_pane_bg1.png");
	
	setPaletteBackgroundPixmap(dlg_bg);
	Main_Pane->setPaletteBackgroundPixmap(pane_bg);

	QPixmap listview_head("skins/vaste_listview_header.png");
	ListView_IDC_NETCONS->header()->setPaletteBackgroundPixmap(listview_head);
	// skin settings end.


	startTimer(250);
	g_lvnetcons.setwnd(ListView_IDC_NETCONS);
	g_lvnetcons.AddCol("Status", g_config->ReadInt("Net_Col3", 133));
	g_lvnetcons.AddCol("Host", g_config->ReadInt("Net_Col1", 126));
	g_lvnetcons.AddCol("Rating", g_config->ReadInt("Net_Col2", 45));
	g_lvnetcons.AddCol("User(Key)", g_config->ReadInt("Net_Col4", 108));
	
	g_mql->SetStatusListView(&g_lvnetcons);
	char numComup[50];
	sprintf(numComup, "%d", g_keepup);
	LineEdit_IDC_NUMCONUP->setText(numComup);
#ifdef SAVE_CB_HOSTS
      {
        int x;
        int p=g_config->ReadInt("concb_pos",0);
        for (x = 0 ; x < MAX_CBHOSTS; x ++)
        {
          char buf[123];
          sprintf(buf,"concb_%d",(p+x)%MAX_CBHOSTS);
          char *o=g_config->ReadString(buf,"");
          if (*o) 
	  {
   		ComboBox_IDC_NEWCONNECTION->insertItem(o, 0);
		
	  }
        }
      }
#endif
      //for connect enterpressed
      QLineEdit *LineEditOfComboBox = ComboBox_IDC_NEWCONNECTION->lineEdit();
    	connect( LineEditOfComboBox, SIGNAL(returnPressed()), this, SLOT( onAddConnectionClicked() ) );

}

Vaste_Idd_Net::~Vaste_Idd_Net()
{
      	g_config->WriteInt("net_col3",g_lvnetcons.GetColumnWidth(0));
	g_config->WriteInt("net_col1",g_lvnetcons.GetColumnWidth(1));
     	g_config->WriteInt("net_col2",g_lvnetcons.GetColumnWidth(2));
      	g_config->WriteInt("net_col4",g_lvnetcons.GetColumnWidth(3));
}

void Vaste_Idd_Net::closeEvent(QCloseEvent *e)
{
	g_config->WriteInt("net_vis", 0);

	IDD_NET::closeEvent(e);
}

void Vaste_Idd_Net::onAddConnectionClicked()
{
	QString qStr;
	int port=1337;
        char *p=NULL;
        
	qStr = ComboBox_IDC_NEWCONNECTION->currentText();
	p = (char *)qStr.ascii();
	RTRACE("AddCon" << p << "\n");
	
	if (!qStr.isEmpty())
       	{
	    QListBox *list = ComboBox_IDC_NEWCONNECTION->listBox();
	    if (!(list->findItem(qStr)))
	    {
		    if (list->count() > MAX_CBHOSTS)
		    {
			    ComboBox_IDC_NEWCONNECTION->removeItem(MAX_CBHOSTS-1);
		    }
		    ComboBox_IDC_NEWCONNECTION->insertItem(qStr, 0);
#ifdef SAVE_CB_HOSTS
                    int pos=g_config->ReadInt("concb_pos",0);
                    char buf[123];
                    sprintf(buf,"concb_%d",pos);
                    g_config->WriteString(buf,p);
                    pos++;
                    g_config->WriteInt("concb_pos",pos%MAX_CBHOSTS);
#endif
	     	}
              	while (*p && *p != ':') p++;
              	if (*p==':') 
              	{
              		*p++=0;
                	port=atoi(p);
              	}
              	if (port)
              	{
                	AddConnection((char*)qStr.ascii(),port,1);
              	}
	}
}

void Vaste_Idd_Net::onConnectionCatchClicked()
{
	TAG tag("Vaste_Idd_Net::onConnectionCatchClicked()");
	int n=g_lvnetcons.GetCount();
	int x;

	for (x = 0; x < n; x++)
	{
		if (g_lvnetcons.GetSelected(x))
		{
			if (!g_lvnetcons.GetParam(x))
			{
				char text[512];
				int port;
				char rat[32];
			
				g_lvnetcons.GetText(x,1,text,sizeof(text));
				g_lvnetcons.GetText(x,2,rat,sizeof(rat));
	
				char *p=text;
				while (*p != ':' && *p) p++;
				if (*p)
				{
					*p++=0;
					port=atoi(p);
				}
				else port=1337;

				g_lvnetcons.DeleteItem(x--);
				AddConnection(text,port,atoi(rat));
			}//if
		}//if
	}//for
}

void Vaste_Idd_Net::onRemoveConnectionClicked()
{
	TAG tag("Vaste_Idd_Net::onRemoveConnectionClicked()");
	int n=g_lvnetcons.GetCount();
	int x;

	for (x = 0; x < n; x++)
	{
		if (g_lvnetcons.GetSelected(x))
		{
			C_Connection *t= (C_Connection*)g_lvnetcons.GetParam(x);
			if (t)
				t->close(1);
		}
	}
}

void Vaste_Idd_Net::onRemoveCatchClicked()
{
	TAG tag("Vaste_Idd_Net::onRemoveCatchClicked()");
	int n=g_lvnetcons.GetCount();
	int x;
	RTRACE("Remove Catch Count = " << n);
	for (x = 0; x < n; x++)
	{
		if (g_lvnetcons.GetSelected(x))
		{
			RTRACE("Remove Catch = " << x);
			C_Connection *t= (C_Connection*)g_lvnetcons.GetParam(x);
			if (t)
				t->close(1);
			g_lvnetcons.DeleteItem(x--);
			n--;
		}	
	}
}

void Vaste_Idd_Net::timerEvent(QTimerEvent *tEvent)
{
	TAG tag("Vaste_Idd_net::timerEvent()");
	
	if (isVisible())
	{
		int l=g_lvnetcons.GetCount();
		for (int x = 0; x < l; x ++)
		{
			C_Connection *thiscon = (C_Connection *)g_lvnetcons.GetParam(x);
			if (thiscon && thiscon->was_ever_connected())
			{
				char str[128];
				int sbps, rbps;
				thiscon->get_last_bps(&sbps,&rbps);

				sprintf(str,"%d.%dk/s out, %d.%dk/s in",
					sbps/1024,((sbps*10)/1024)%10,
					rbps/1024,((rbps*10)/1024)%10);
				if (g_extrainf)
				{
					for (int n=g_mql->GetNumQueues()-1; n >= 0; n --)
					{
						C_MessageQueue *q=g_mql->GetQueue(n);
						if (thiscon == q->get_con())
						{
							sprintf(str+strlen(str), " (%d,%d,%d)",
							q->get_stat_send(),
							q->get_stat_recv(),
							q->get_stat_drop());
							break;
						}
					}
				}

				g_lvnetcons.SetItemText(x,0,str);
			}
		}//for
	}

}

void Vaste_Idd_Net::UpdateWinTitle()
{
	char buf[1024];
	strcpy(buf,"Network Status");
	int numcons=g_mql->GetNumQueues();
	int numneg=g_new_net.GetSize();
	int a=0;
	
	if (numcons)
		sprintf(buf+strlen(buf),"%s%d connection%s up",a++?", ":" (",numcons,numcons==1?"":"s");

	if (numneg)
		sprintf(buf+strlen(buf),"%s%d connecting",a++?", ":" (",numneg);

	if (a) strcat(buf,")");
	else strcat(buf," (network down)");
       
	strcat(buf," - " APP_NAME);
        SetWndTitle(this, buf);
}

void Vaste_Idd_Net::onComboBoxTextChanged(const QString &str)
{
	if (!str.isEmpty())
		PushButton_IDC_ADDCONNECTION->setEnabled(true);
	else
		PushButton_IDC_ADDCONNECTION->setEnabled(false);

		
}

void Vaste_Idd_Net::onComboBoxEnterPressed()
{
	onAddConnectionClicked();
}


void Vaste_Idd_Net::onListViewNetConnsSelectionChanged()
{
          int isSel=1;
          int isCon=0;
          int isDiscon=0;
          if (isSel)
          {
            int a,n=g_lvnetcons.GetCount();
            for (a = 0; a < n; a ++)
            {
              if (g_lvnetcons.GetSelected(a))
              {
                if (g_lvnetcons.GetParam(a)) isCon++;
                else isDiscon++;
              }
              if (isCon && isDiscon) break;
            }
	    isSel = isCon + isDiscon;
          }
    PushButton_IDC_CONNECTCATCH->setEnabled(isDiscon);
    PushButton_IDC_REMOVECONNECTION->setEnabled(isCon);
    PushButton_IDC_REMOVECATCH->setEnabled(isSel);

}
