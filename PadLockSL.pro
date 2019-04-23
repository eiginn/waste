SOURCES	+= asyncdns.cpp \
	config.cpp \
	listen.cpp \
	load_config.cpp \
	common.cpp \
	connection.cpp \
	filedb.cpp \
	xferwnd.cpp \
	xfers.cpp \
	vq_chat.cpp \
	vaste_kernal.cpp \
	vaste_mainwindow.cpp \
	vaste_idd_search.cpp \
	vaste_idd_net.cpp \
	vaste_idd_xfers.cpp \
	vaste_chat.cpp \
	util.cpp \
	ui_treeview.cpp \
	m_chat.cpp \
	m_file.cpp \
	m_keydist.cpp \
	m_lcaps.cpp \
	m_ping.cpp \
	m_search.cpp \
	m_upload.cpp \
	mqueue.cpp \
	mqueuelist.cpp \
	netkern.cpp \
	netq.cpp \
	prefdlgs.cpp \
	preference.cpp \
	prefinterface.cpp \
	qt_srvmain.cpp \
	qulf_dialog.cpp \
	sha.cpp \
	tag.cpp \
	srchwnd.cpp \
	ui_listview.cpp \
    	vaste_keygen.cpp \
	rsa/md5c.c \
	rsa/nn.c \
	rsa/prime.c \
	rsa/r_keygen.c \
	rsa/r_random.c \
	rsa/rsa.c \
	AES/src/aescrypt.c \
	AES/src/aeskey.c \
	AES/src/aestab.c \
	AES/src/c5p.c 
HEADERS	+= asyncdns.h \
	common.h \
	connection.h \
	filedb.h \
	xferwnd.h \
	xfers.h \
	vq_chat.h \
	vaste_mainwindow.h \
	vaste_idd_search.h \
	vaste_chat.h \
	vaste_idd_net.h \
	vaste_idd_xfers.h \
	util.h \
	ui_treeview.h \
	m_chat.h \
	m_file.h \
	itemlist.h \
	itemstack.h \
	m_keydist.h \
	m_lcaps.h \
	m_ping.h \
	m_search.h \
	m_upload.h \
	mqueue.h \
	mqueuelist.h \
	netkern.h \
	netq.h \
	platform.h \
	prefdlgs.h \
	preference.h \
	prefinterface.h \
	qulf_dialog.h \
	sha.h \
	shbuf.h \
	sockets.h \
	tag.h \
	srchwnd.h \
	ui_listview.h \
	rsa/global.h \
	rsa/md5.h \
	rsa/nn.h \
	rsa/prime.h \
	rsa/r_random.h \
	rsa/rsa.h \
	rsa/rsaref.h \
	AES/include/C5P.h \
	AES/include/aes.h \
	AES/include/aesopt.h
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
TARGET = PadLockSL 
FORMS	= idd_chatchan.ui \
	idd_chatpriv.ui \
	idd_chatroomcreate.ui \
	idd_net.ui \
	idd_recv.ui \
	idd_recvq.ui \
	idd_search.ui \
	idd_send.ui \
	idd_xfers.ui \
	idd_help.ui \
	idd_about.ui \
	main_window.ui
TEMPLATE	=app
CONFIG	+= qt warn_on debug thread
LANGUAGE	= C++
DEFINES += QTUI _LINUX
INCLUDEPATH += AES/include
