# Fetch Limine
git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1
rm -rf limine/.git limine/.gitignore
make -C limine

# Fetch SSFN renderer and Tiny Printf
wget -O ssfn.h https://gitlab.com/bztsrc/scalable-font2/-/raw/master/ssfn.h?ref_type=heads
wget -O printf.h https://raw.githubusercontent.com/mpaland/printf/master/printf.h
wget -O printf.c https://raw.githubusercontent.com/mpaland/printf/master/printf.c
