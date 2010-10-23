#! /bin/sh
# Creates necessary Makefiles, and configure, etc.
  
set -ex
    
${ACLOCAL:-aclocal}
#${AUTOHEADER:-autoheader}
${AUTOMAKE:-automake} --add-missing --copy --force
${AUTOCONF:-autoconf}
