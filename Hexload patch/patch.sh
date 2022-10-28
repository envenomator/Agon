#! /bin/bash
echo "Removing old patch directories"
rm -rf agon-mos-patched
rm -rf agon-vdp-patched
echo "Git clone Quark locally"
git clone https://github.com/breakintoprogram/agon-mos.git
git clone https://github.com/breakintoprogram/agon-vdp.git
echo "Renaming directories"
mv agon-mos agon-mos-patched
mv agon-vdp agon-vdp-patched
mkdir agon-vdp-patched/video
echo "Copying files"
cp mos_hexload.h agon-mos-patched/src/
cp mos_hexload.c agon-mos-patched/src/
cp mos_bytestream.asm agon-mos-patched/src/
cp ihexload.h agon-vdp-patched/
cp ihexload.cpp agon-vdp-patched/
echo "Patching MOS.zdsproj"
sed -i 's/0BE0000/0BD0000/g' agon-mos-patched/MOS.zdsproj
echo "Patching mos.c"
sed -i 's/#include \"ff.h\"/#include \"ff.h\"\n#include \"mos_hexload.h\"/g' agon-mos-patched/src/mos.c
sed -i 's/&mos_cmdSET },/\&mod_cmdSET },\n\t{ \"HL\",\t\t\&mos_HEXLOAD },/g' agon-mos-patched/src/mos.c
echo "Patching main.c"
sed -i 's/Agon Quark MOS Version %d\.%02d/Agon Quark MOS Version %d\.%02d Hexload patch/g' agon-mos-patched/main.c
echo "Patching video.ino"
sed -i 's/#include \"agon.h\"/#include \"agon.h\"\n#include \"ihexload.h\"/g' agon-vdp-patched/video.ino
sed -i 's/HardwareSerial DBGSerial\x280\x29;//g' agon-vdp-patched/video.ino
sed -i 's/DBGSerial/Serial/g' agon-vdp-patched/video.ino
sed -i 's/500000/115200/g' agon-vdp-patched/video.ino
sed -i 's/vdu_sys_sprites.*$/vdu_sys_sprites\(\);\t\t\t\/\/ Sprite system control\n\t\t\t\tbreak;\n\t\t\tcase 0x1C:\t\t\t\t\/\/ HEXLOAD VDU 23,28\n\t\t\t\tvdu_sys_hexload\(\);/g' agon-vdp-patched/video.ino
sed -i 's/Agon Quark VPD Version %d\.%02d/Agon Quark VPD Version %d\.%02d Hexload patch/g' agon-vdp-patched/video.ino
echo "Relocating video code"
cd agon-vdp-patched
mv $(ls --ignore video) video
cd ..
echo "Patch complete"
