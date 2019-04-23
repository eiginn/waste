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

#include <qwidget.h>
#include <qtabwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qheader.h>

#include "common.h"
#include "vaste_idd_xfers.h"
#include "config.h"
#include "main.h"
#include "ui_listview.h"
#include "itemlist.h"
#include "mqueuelist.h"
#include "xferwnd.h"
#include "xfers.h"
#include "tag.h"

extern int g_files_in_download_queue;
extern int g_max_simul_dl;
extern W_ListView g_lvrecv;
extern W_ListView g_lvsend;
extern W_ListView g_lvrecvq;
extern C_Config *g_config;

extern C_ItemList<XferSend> g_sends;
extern C_ItemList<XferRecv> g_recvs;
extern C_ItemList<char> g_uploads;
extern C_ItemList<C_SHBuf> g_uploadqueue;
extern unsigned int g_uploadqueue_lastsend;
extern C_MessageQueueList *g_mql;
/*
 */
Vaste_Idd_Xfers::Vaste_Idd_Xfers( QWidget* parent, const char* name, WFlags fl )
    : IDD_XFERS( parent, name, fl )
{
#include "vaste_widget_palette.xx"
	setPalette( dialog_pal );
	TabWidget_IDC_TAB->setPalette( frame_pal );
    PushButton_IDC_RESUME->setPalette( pushbutton_pal );
    PushButton_IDC_DndABORTSEL->setPalette( pushbutton_pal );
    PushButton_IDC_DndCLEARCOMP->setPalette( pushbutton_pal );
    PushButton_IDC_DndCLEARCOMP3->setPalette( pushbutton_pal );
    PushButton_IDC_DndCLEARALL->setPalette( pushbutton_pal );
    CheckBox_IDC_DndAUTOCLEAR->setPalette( checkbox_pal );
    CheckBox_IDC_DndAUTORESUME->setPalette( checkbox_pal );
    PushButton_IDC_DLNOW->setPalette( pushbutton_pal );
    PushButton_IDC_MOVETOTOP->setPalette( pushbutton_pal );
    PushButton_IDC_MOVETOBOTTOM->setPalette( pushbutton_pal );
    PushButton_IDC_REMOVEQUEUE->setPalette( pushbutton_pal );
    PushButton_IDC_CLEARQUEUE->setPalette( pushbutton_pal );
    PushButton_IDC_SendABORTSEL->setPalette( pushbutton_pal );
    PushButton_IDC_SendCLEARCOMP->setPalette( pushbutton_pal );
    PushButton_IDC_SendCLEARCOMP2->setPalette( pushbutton_pal );
    CheckBox_IDC_SENDAUTOCLEAR->setPalette( checkbox_pal );
	ListView_IDC_RECVLIST->setPalette( listview_pal );
	ListView_IDC_RECVLISTQ->setPalette( listview_pal );
	ListView_IDC_SENDLIST->setPalette( listview_pal );

	PushButton_IDC_RESUME->setPixmap(QPixmap("icons/vaste_resume_selected_download.png"));
	PushButton_IDC_DndABORTSEL->setPixmap(QPixmap("icons/vaste_download_now.png"));
	PushButton_IDC_DndCLEARCOMP->setPixmap(QPixmap("icons/vaste_xfers_clear_enable.png"));
	PushButton_IDC_DndCLEARCOMP3->setPixmap(QPixmap("icons/vaste_xfers_complet_clear_enable.png"));
	PushButton_IDC_DndCLEARALL->setPixmap(QPixmap("icons/vaste_xfers_clearall_enable.png"));

	PushButton_IDC_DLNOW->setPixmap(QPixmap("icons/vaste_download_now.png"));
	PushButton_IDC_MOVETOTOP->setPixmap(QPixmap("icons/vaste_move_to_top.png"));
	PushButton_IDC_MOVETOBOTTOM->setPixmap(QPixmap("icons/vaste_move_to_bottom.png"));
	PushButton_IDC_REMOVEQUEUE->setPixmap(QPixmap("icons/vaste_xfers_clear_enable.png"));
	PushButton_IDC_CLEARQUEUE->setPixmap(QPixmap("icons/vaste_xfers_clearall_enable.png"));

	PushButton_IDC_SendABORTSEL->setPixmap(QPixmap("icons/vaste_download_now.png"));
	PushButton_IDC_SendCLEARCOMP->setPixmap(QPixmap("icons/vaste_xfers_complet_clear_enable.png"));
	PushButton_IDC_SendCLEARCOMP2->setPixmap(QPixmap("icons/vaste_xfers_clearall_enable.png"));

	// skin settings here.
	QPixmap button_bg("skins/vaste_toolbutton_bg.png");

	PushButton_IDC_RESUME->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_DndABORTSEL->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_DndCLEARCOMP->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_DndCLEARCOMP3->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_DndCLEARALL->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_DLNOW->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_MOVETOTOP->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_MOVETOBOTTOM->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_REMOVEQUEUE->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_CLEARQUEUE->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_SendABORTSEL->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_SendCLEARCOMP->setPaletteBackgroundPixmap(button_bg);
	PushButton_IDC_SendCLEARCOMP2->setPaletteBackgroundPixmap(button_bg);

	QPixmap dlg_bg("skins/vaste_pane_bg.png");
	QPixmap pane_bg("skins/vaste_pane_bg1.png");
	
	setPaletteBackgroundPixmap(dlg_bg);
	TabWidget_IDC_TAB->setPaletteBackgroundPixmap(pane_bg);
	CheckBox_IDC_DndAUTOCLEAR->setPaletteBackgroundPixmap(pane_bg);
	CheckBox_IDC_DndAUTORESUME->setPaletteBackgroundPixmap(pane_bg);
	CheckBox_IDC_SENDAUTOCLEAR->setPaletteBackgroundPixmap(pane_bg);

	QPixmap listview_head("skins/vaste_listview_header.png");
	ListView_IDC_RECVLIST->header()->setPaletteBackgroundPixmap(listview_head);
	ListView_IDC_RECVLISTQ->header()->setPaletteBackgroundPixmap(listview_head);
	ListView_IDC_SENDLIST->header()->setPaletteBackgroundPixmap(listview_head);
	// skin setting end.

      g_lvrecv.setwnd(ListView_IDC_RECVLIST);
      g_lvrecv.AddCol("File",g_config->ReadInt("recv_col1",150));
      g_lvrecv.AddCol("Size",g_config->ReadInt("recv_col2",100));
      g_lvrecv.AddCol("Status",g_config->ReadInt("recv_col3",101));
      g_lvrecv.AddCol("Location",g_config->ReadInt("recv_col4",100));

      g_lvrecvq.setwnd(ListView_IDC_RECVLISTQ);
      g_lvrecvq.AddCol("File",g_config->ReadInt("recv_qcol1",150));
      g_lvrecvq.AddCol("Size",g_config->ReadInt("recv_qcol2",100));
      g_lvrecvq.AddCol("Location",g_config->ReadInt("recv_qcol3",0));
      
      g_lvsend.setwnd(ListView_IDC_SENDLIST);
      g_lvsend.AddCol("File",g_config->ReadInt("send_col1",150));
      g_lvsend.AddCol("User",g_config->ReadInt("send_col2",50));
      g_lvsend.AddCol("Size",g_config->ReadInt("send_col3",112));
      g_lvsend.AddCol("Status",g_config->ReadInt("send_col4",88));

      CheckBox_IDC_SENDAUTOCLEAR->setChecked(g_config->ReadInt("send_autoclear", 0));
      CheckBox_IDC_DndAUTOCLEAR->setChecked(g_config->ReadInt("recv_autoclear",0));
      CheckBox_IDC_DndAUTORESUME->setChecked(g_config->ReadInt("recv_autores",1));
      char buf[256];
      sprintf(buf, "%d", g_max_simul_dl);
      LineEdit_IDC_MAXSIMULDL->setText(buf);
}

/*
 *  Destroys the object and frees any allocated resources
 */
Vaste_Idd_Xfers::~Vaste_Idd_Xfers()
{
	g_config->WriteInt("recv_col1",g_lvrecv.GetColumnWidth(0));
	g_config->WriteInt("recv_col2",g_lvrecv.GetColumnWidth(1));
	g_config->WriteInt("recv_col3",g_lvrecv.GetColumnWidth(2));
	g_config->WriteInt("recv_col4",g_lvrecv.GetColumnWidth(3));

	g_config->WriteInt("send_col2",g_lvsend.GetColumnWidth(1));
	g_config->WriteInt("send_col3",g_lvsend.GetColumnWidth(2));
	g_config->WriteInt("send_col4",g_lvsend.GetColumnWidth(3));

	g_config->WriteInt("recv_qcol1",g_lvrecvq.GetColumnWidth(0));
	g_config->WriteInt("recv_qcol2",g_lvrecvq.GetColumnWidth(1));
	g_config->WriteInt("recv_qcol3",g_lvrecvq.GetColumnWidth(2));
      
	g_config->WriteInt("xfers_vis", 0);
}

void Vaste_Idd_Xfers::closeEvent(QCloseEvent *e)
{
	g_config->WriteInt("xfers_vis", 0);

	IDD_XFERS::closeEvent(e);
	this->hide();
}

void Vaste_Idd_Xfers::UpdateTitle()
{
	TAG tag("Vaste_Idd_Xfers::UpdateTitle()");
        char buf[1024];
        strcpy(buf,"Transfers");
        int numdls=g_recvs.GetSize();
        int numuls=g_sends.GetSize();
        int numq=g_files_in_download_queue;
        int a=0;

        if (numdls) sprintf(buf+strlen(buf),"%s%d DL%s",a++?", ":" (",numdls,numdls==1?"":"s");
        if (numq) sprintf(buf+strlen(buf),"%s%d DL%s Queued",a++?", ":" (",numq,numq==1?"":"s");
        if (numuls) sprintf(buf+strlen(buf),"%s%d UL%s",a++?", ":" (",numuls,numuls==1?"":"s");
        if (a) strcat(buf,")");
       
        strcat(buf," - ");
	strcat(buf, APP_NAME);
	SetWndTitle(this, buf);
	
}


void Vaste_Idd_Xfers::show()
{
	IDD_XFERS::show();
}

void Vaste_Idd_Xfers::pushBtnIDCResume()
{
	int x;
        int l=g_lvrecv.GetCount();
        for (x = 0; x < l; x ++)
        {
        	if (g_lvrecv.GetSelected(x))
              	{
                	int param=g_lvrecv.GetParam(x);
                	if (!param)
                	{
                  		char sizestr[64];
				char host[512];
                  		char name[260];
                  		g_lvrecv.GetText(x,3,host,sizeof(host));
                  		if (host[0])
                  		{
                    			g_lvrecv.GetText(x,0,name,sizeof(name));
                    			g_lvrecv.GetText(x,1,sizestr,sizeof(sizestr));
                    			g_lvrecv.DeleteItem(x);
                    			int p=g_lvrecvq.InsertItem(g_lvrecvq.GetCount(),name,0);
                    			g_lvrecvq.SetItemText(p,1,sizestr);
                    			g_lvrecvq.SetItemText(p,2,host);
			                g_files_in_download_queue++;
                    			l--; x--;
                  		}
                	}
              	}
	}
        recvQ_UpdateStatusText();
}

void Vaste_Idd_Xfers::recvQ_UpdateStatusText()
{
	char buf[256];

	sprintf(buf, "%d items in queue", g_files_in_download_queue);
	Label_IDC_QUEUEITEMS->setText(buf);

	UpdateTitle();

	lstViewRecvSelectChanged();
}

void Vaste_Idd_Xfers::pushBtnIDCDndAbortSel()
{
	int x;
	int l=g_lvrecv.GetCount();

	for (x = 0; x < l; x ++)
	{
		if (g_lvrecv.GetSelected(x))
		{
			XferRecv *a=(XferRecv *)g_lvrecv.GetParam(x);
			if (a) a->Abort(g_mql);
		}
	}
}

void Vaste_Idd_Xfers::pushBtnIDCDndClearComp()
{
	int x;
	int l=g_lvrecv.GetCount();
	
	for (x = 0; x < l; x ++)
	{
		char buf[64];
		g_lvrecv.GetText(x,3,buf,sizeof(buf));
		if (!g_lvrecv.GetParam(x) && !buf[0])
		{
			g_lvrecv.DeleteItem(x--);
			l--;
		}
	}
	
	lstViewRecvSelectChanged();
}

void Vaste_Idd_Xfers::pushBtnIDCDndClearComp3()
{
	int x;
	int l=g_lvrecv.GetCount();
	
	for (x = 0; x < l; x ++)
	{
		char buf[64];
		g_lvrecv.GetText(x,3,buf,sizeof(buf));
		if (!g_lvrecv.GetParam(x))
		{
			g_lvrecv.DeleteItem(x--);
			l--;
		}
	}
	
	lstViewRecvSelectChanged();
}

void Vaste_Idd_Xfers::pushBtnIDCDndClearAll()
{
	QMessageBox mb( APP_NAME,
          	"Abort all downloads",
        QMessageBox::Information,
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No,
        QMessageBox::Cancel | QMessageBox::Escape );
    	
	if (QMessageBox::Yes != mb.exec()) return;
    
	int x;
	int l=g_lvrecv.GetCount();
	
	for (x = 0; x < l; x ++)
	{
		XferRecv *a=(XferRecv *)g_lvrecv.GetParam(x);
		if (a) a->Abort(g_mql);
		g_lvrecv.DeleteItem(x--);
		l--;
	}
	
	lstViewRecvSelectChanged();
}

void Vaste_Idd_Xfers::chkBoxIDCDndAutoClear(int state)
{
          g_config->WriteInt("recv_autoclear", CheckBox_IDC_DndAUTOCLEAR->isChecked());
}

void Vaste_Idd_Xfers::chkBoxIDCDndAutoResume(int state)
{
          g_config->WriteInt("recv_autores", CheckBox_IDC_DndAUTORESUME->isChecked());
}

void Vaste_Idd_Xfers::pushBtnMoveToBottom( )
{
	int x;
	int l=g_lvrecvq.GetCount();
	int a=l-1;

	for (x = l-1; x >= 0; x --) 
	{
		if (g_lvrecvq.GetSelected(x))
		{
			char text[1024];
			char size[64];
			char loc[1024];

			int param=g_lvrecvq.GetParam(x);
			g_lvrecvq.GetText(x,0,text,sizeof(text));
			g_lvrecvq.GetText(x,1,size,sizeof(size));
			g_lvrecvq.GetText(x,2,loc,sizeof(loc));
			g_lvrecvq.DeleteItem(x);
			g_lvrecvq.InsertItem(a,text,param);
			g_lvrecvq.SetItemText(a,1,size);
			g_lvrecvq.SetItemText(a,2,loc);
			g_lvrecvq.SetSelected(a);
			a--;
		}
	}
	return;
}

void Vaste_Idd_Xfers::pushBtnDlNow()
{
	int x;
	int l=g_lvrecvq.GetCount();
	int b=0;

	for (x = 0; x < l; x ++)
	{
		if (g_lvrecvq.GetSelected(x))
		{              
			char name[260];
			char sizestr[64];
			char host[512];

			g_lvrecvq.GetText(x,0,name,sizeof(name));
			g_lvrecvq.GetText(x,1,sizestr,sizeof(sizestr));
			g_lvrecvq.GetText(x,2,host,sizeof(host));
			g_lvrecvq.DeleteItem(x);
			g_files_in_download_queue--;
			
			int a,n=g_lvrecv.GetCount();
			for (a = 0; a < n; a ++)
			{
				char name2[260];
				char host2[512];
				
				g_lvrecv.GetText(a,0,name2,sizeof(name2));
				g_lvrecv.GetText(a,3,host2,sizeof(host2));   
				if(!stricmp(name2,name) && !stricmp(host2,host))
				{
					if (g_lvrecv.GetParam(a)) break;
					g_lvrecv.DeleteItem(a--);
					n--;
				}
			}
			
			if (a == n)
			{
				g_recvs.Add(new XferRecv(g_mql,host,sizestr,name,g_config->ReadString("downloadpath","/tmp/vaste")));
			}                

			b=1;
			x--;
			l--;
		}
	}
	
	if (b)
	{
		XferDlg_SetSel(0);
	}
	
	recvQ_UpdateStatusText();
}

void Vaste_Idd_Xfers::pushBtnMoveToTop()
{
	int x;
	int l=g_lvrecvq.GetCount();
	int a=0;
	
	for (x = 0; x < l; x ++)
	{
		if (g_lvrecvq.GetSelected(x))
		{
			char text[1024];
			char size[64];
			char loc[1024];
			
			int param=g_lvrecvq.GetParam(x);
			g_lvrecvq.GetText(x,0,text,sizeof(text));
			g_lvrecvq.GetText(x,1,size,sizeof(size));
			g_lvrecvq.GetText(x,2,loc,sizeof(loc));
			g_lvrecvq.DeleteItem(x);
			g_lvrecvq.InsertItem(a,text,param);
			g_lvrecvq.SetItemText(a,1,size);
			g_lvrecvq.SetItemText(a,2,loc);
			g_lvrecvq.SetSelected(a);
			a++;
		}
	}	
}

void Vaste_Idd_Xfers::pushBtnRemoveQueue()
{
	int x;
	int l=g_lvrecvq.GetCount();

	for (x = 0; x < l; x ++)
	{
		if (g_lvrecvq.GetSelected(x))
		{
			g_lvrecvq.DeleteItem(x--);
			l--;
			g_files_in_download_queue--;
		}
	}
	
	recvQ_UpdateStatusText();
}

void Vaste_Idd_Xfers::pushBtnClearQueue()
{
	int x;
	int l=g_lvrecvq.GetCount();

	for (x = 0; x < l; x ++)
	{
		g_lvrecvq.DeleteItem(x--);
		l--;
		g_files_in_download_queue--;
	}
	
	recvQ_UpdateStatusText();
}

void Vaste_Idd_Xfers::pushBtnSendAbortSel()
{
	int x;
	int l=g_lvsend.GetCount();
            
	for (x = 0; x < l; x ++)
	{
		if (g_lvsend.GetSelected(x))
		{
			void *tt=(void *)g_lvsend.GetParam(x);

			if (tt)
			{
				int y,n=g_uploads.GetSize();
				for (y = 0; y < n && (void*)g_uploads.Get(y) != tt; y ++);
				if (y < n)
				{
					g_uploads.Set(y,NULL);
					free(tt);
					g_lvsend.SetItemParam(x,0);
					g_lvsend.SetItemText(x,3,"Aborted");
				}
				else
				{
					XferSend *t=(XferSend *)tt;
					t->Abort(g_mql);
					if (t->get_idx() >= UPLOAD_BASE_IDX)
					{
						int wu=t->get_idx()-UPLOAD_BASE_IDX;
						if (wu >= 0 && wu < n)
						{
							free(g_uploads.Get(wu));
							g_uploads.Set(wu,NULL);
						}
					}
				}
			}
		}
	}
}

void Vaste_Idd_Xfers::pushBtnSendClearComp()
{
	int x;
	int l=g_lvsend.GetCount();
	for (x = 0; x < l; x ++)
	{
		if (!g_lvsend.GetParam(x))
		{
			g_lvsend.DeleteItem(x--);
			l--;
		}
	}
	
	lstViewSendSelectChanged();
}
void Vaste_Idd_Xfers::pushBtnSendClearComp2()
{
	QMessageBox mb( APP_NAME,
          	"Abort and Clear all uploads",
        QMessageBox::Information,
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No,
        QMessageBox::Cancel | QMessageBox::Escape );
    if (QMessageBox::Yes != mb.exec() ) return;
	int x;
	int l=g_lvsend.GetCount();
	
	for (x = 0; x < l; x ++)
	{
		void *tt=(void *)g_lvsend.GetParam(x);
		if (tt)
		{
			int y,n=g_uploads.GetSize();
			for (y = 0; y<n && (void*)g_uploads.Get(y) != tt; y ++);
			if (y < n)
			{
				g_uploads.Set(y,NULL);
				free(tt);
				g_lvsend.SetItemParam(x,0);
				g_lvsend.SetItemText(x,3,"Aborted");
			}
			else
			{
				XferSend *t=(XferSend *)tt;
				t->Abort(g_mql);
				if (t->get_idx() >= UPLOAD_BASE_IDX)
				{
					int wu=t->get_idx()-UPLOAD_BASE_IDX;
					if (wu >= 0 && wu < n)
					{
						free(g_uploads.Get(wu));
						g_uploads.Set(wu,NULL);
					}
				}
			}
		}

		g_lvsend.DeleteItem(x);
		x--;
		l--;
	}
	
	lstViewSendSelectChanged();
}

void Vaste_Idd_Xfers::chkBoxSendAutoClear(int state)
{
	g_config->WriteInt("send_autoclear", CheckBox_IDC_SENDAUTOCLEAR->isChecked());
}

void Vaste_Idd_Xfers::lstViewRecvDClicked()
{
	int x;
	int l=g_lvrecv.GetCount();

	for (x = 0; x < l && !g_lvrecv.GetSelected(x); x ++);
	
	if (x < l)
	{
		if (!g_lvrecv.GetParam(x))
		{
			char text[32];
			g_lvrecv.GetText(x,3,text,sizeof(text));
			if (text[0])
			{
				pushBtnIDCResume();
			}
			else
			{
				doExecuteOfDownload(x,0);
			}
		}
	}
}

void Vaste_Idd_Xfers::lstViewRecvSelectChanged()
{
	TAG tag("Vaste_Idd_Xfers::lstViewRecvSelectChanged()");
	int s2=g_lvrecv.GetCount();
	int s = false;
	
	for(int x=0; (x<s2) && !s; x++)
		s = s || g_lvrecv.GetSelected(x);
    	
	PushButton_IDC_RESUME->setEnabled(s);
	PushButton_IDC_DndABORTSEL->setEnabled(s);
	PushButton_IDC_DndCLEARCOMP->setEnabled(s2);
	PushButton_IDC_DndCLEARCOMP3->setEnabled(s2);
	PushButton_IDC_DndCLEARALL->setEnabled(s2);
}

void Vaste_Idd_Xfers::lstViewRecvQDclicked()
{
	pushBtnDlNow();
}

void Vaste_Idd_Xfers::lstViewRecvQSelectedChanged()
{
	int s2=g_lvrecvq.GetCount();
	int s = false;
	
	for(int x=0; (x<s2) && !s; x++)
		s = s || g_lvrecvq.GetSelected(x);
    	
	PushButton_IDC_DLNOW->setEnabled(s);
    	PushButton_IDC_MOVETOTOP->setEnabled(s);
    	PushButton_IDC_MOVETOBOTTOM->setEnabled(s);
    	PushButton_IDC_REMOVEQUEUE->setEnabled(s);
	PushButton_IDC_CLEARQUEUE->setEnabled(s2);
}

void Vaste_Idd_Xfers::lstViewSendDClicked()
{
}

void Vaste_Idd_Xfers::lstViewSendSelectChanged()
{
	int s2=g_lvsend.GetCount();//ListView_GetItemCount(l->hwndFrom);
	int s=0;
	for (int x=0; (x<s2) && !s; x++)
		s = s || g_lvsend.GetSelected(x);
	PushButton_IDC_SendABORTSEL->setEnabled(s);
    	PushButton_IDC_SendCLEARCOMP2->setEnabled(s2);
    	PushButton_IDC_SendCLEARCOMP->setEnabled(s2);
} 

void Vaste_Idd_Xfers::doExecuteOfDownload(int x, int copy)
{
  XferRecv *tr;
  char *t=g_config->ReadString("downloadpath","");
  if (!t || !*t) return;

  if ((tr=(XferRecv*)g_lvrecv.GetParam(x)))
  {
    char *fn=NULL;
    if (copy) fn=tr->getOutputFileCopy();
    if (!fn) fn=tr->getActualOutputFile();

    if (fn && *fn)
    {
      char *file=(char*)malloc(strlen(t)+2+strlen(fn));
      sprintf(file,"%s/%s",t,fn);
      fopen(file, "w+");
      free(file); 
      return;
    }
  }

  char n[512];
  n[0]=0;
  g_lvrecv.GetText(x,0,n,sizeof(n));
  if (*n)
  {
    char *buf=(char*)malloc(strlen(t)+2+strlen(n));
    if (buf)
    {
      sprintf(buf,"%s/%s",t,n);
      fopen(buf, "w+");
      free(buf);
    }
  }
}

void Vaste_Idd_Xfers::lnEditMaxSimulDLTextChanged(const QString &str)
{
	TAG tag("lnEditMaxSinulDLTextChanged()");
	bool ok;
	int  b = 0;

	b = str.toInt(&ok);
	if (b)
	{
              g_max_simul_dl = b;
              g_config->WriteInt("recv_maxdl", b);
	}
}
