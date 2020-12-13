#!/bin/sh
source ../../utils/kern/dinterbuild.env || exit 1
cat << EOF > Makefile
obj-m += DinterR_mod.o

kbuild:
	make -C $KERNEL_DIR M=`pwd`

clean:
	make -C $KERNEL_DIR M=`pwd` clean
	rm -f Makefile
EOF
make kbuild
