AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = waste

waste_SOURCES = srvmain.cpp sharedmain.cpp util.cpp srchwnd.cpp main.cpp \
                prefs.cpp asyncdns.cpp connection.cpp mqueue.cpp \
                mqueuelist.cpp config.cpp filedb.cpp listen.cpp xfers.cpp \
                xferwnd.cpp netkern.cpp netq.cpp d_chat.cpp wxmain.cpp \
                listview.cpp m_ping.cpp m_file.cpp m_upload.cpp m_keydist.cpp \
                m_chat.cpp m_lcaps.cpp m_search.cpp wxmainframe.cpp sha.cpp \
                shbuf.cpp blowfish.cpp rsa/r_random.cpp wxappresources.cpp \
                rsa/rsa.cpp rsa/md5.cpp wxchat.cpp wxxml.cpp wxnetstat.cpp \
                wxprefs.cpp wxxfers.cpp wxbrowser.cpp wxuploads.cpp \
                rsa/nn.cpp keygen.cpp wxpubkeys.cpp rsa/r_keygen.cpp \
                wxkgrand.cpp rsa/prime.cpp license.cpp
waste_LDADD = @WX_LIBS@

CLEANFILES = wxxml.cpp
wxxml.cpp: wxwaste.xrc
	wxrc -v -c --output=$@ $?
