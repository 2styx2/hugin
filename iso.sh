#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/hugin.kernel isodir/boot/hugin.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "hugin" {
	multiboot /boot/hugin.kernel
}
EOF
grub-mkrescue -o hugin.iso isodir
