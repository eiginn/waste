#/bin/sh
#LoraYin 2003/12/31


echo "===Begin remove PadLockSL lib ==="
BEGINDIR=`pwd`
rm -f /usr/local/lib/libqt-mt.so*
rm -rf /usr/local/lib/qt-3.2.3

echo "===Begin remove PadLockSL  ==="
rm -rf /usr/local/padlocksl
rm -f /usr/local/bin/padlocksl


echo "===Uninstall End==="

