#/bin/sh
#LoraYin 2003/12/31


test_mkdir(){
if [ ! -d $1 ]
then
    mkdir $1
fi
}


echo "===Begin install PadLockSL lib ==="

test_mkdir /usr/local/lib/qt-3.2.3
test_mkdir /usr/local/lib/qt-3.2.3/lib

cp -df lib/lib*so* /usr/local/lib/qt-3.2.3/lib 
rm -f /usr/local/lib/libqt-mt.so
ln -s /usr/local/lib/qt-3.2.3/lib/libqt-mt.so.3.2.3 /usr/local/lib/libqt-mt.so
ln -s /usr/local/lib/qt-3.2.3/lib/libqt-mt.so.3.2.3 /usr/local/lib/libqt-mt.so.3
ln -s /usr/local/lib/qt-3.2.3/lib/libqt-mt.so.3.2.3 /usr/local/lib/libqt-mt.so.3.2

echo "===Begin install PadLockSL  ==="

test_mkdir /usr/local/padlocksl
test_mkdir /usr/local/padlocksl/docs
test_mkdir /usr/local/padlocksl/icons
test_mkdir /usr/local/padlocksl/skins
cp -f PadLockSL /usr/local/padlocksl
cp -rdf docs/* /usr/local/padlocksl/docs/
cp -rdf icons/* /usr/local/padlocksl/icons/
cp -rdf skins/* /usr/local/padlocksl/skins/
cp -f padlocksl /usr/local/bin
chmod 777 /usr/local/padlocksl
chmod 655 /usr/local/padlocksl/docs
chmod 655 /usr/local/padlocksl/icons
chmod 655 /usr/local/padlocksl/skins
chmod 755 /usr/local/padlocksl/PadLockSL
chmod 755 /usr/local/bin/padlocksl 


echo "===Install End==="

