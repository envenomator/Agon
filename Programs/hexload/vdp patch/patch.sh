#! /bin/bash
echo "Removing old patch directory"
rm -rf agon-vdp-patched
echo "Git clone Quark VPD"
git clone https://github.com/breakintoprogram/agon-vdp.git
echo "Renaming directory"
mv agon-vdp agon-vdp-patched
mkdir agon-vdp-patched/video
echo "Copying files"
cp ihexload.h agon-vdp-patched/
cp ihexload.cpp agon-vdp-patched/
echo "Patching video.ino"
sed -i 's/#include \"agon.h\"/#include \"agon.h\"\n#include \"ihexload.h\"/g' agon-vdp-patched/video.ino
sed -i 's/HardwareSerial DBGSerial\x280\x29;//g' agon-vdp-patched/video.ino
sed -i 's/DBGSerial/Serial/g' agon-vdp-patched/video.ino
sed -i 's/Serial.begin(500000, SERIAL_8N1, 3, 1);//g' agon-vdp-patched/video.ino
sed -i 's/\tESPSerial.end();/\tSerial.begin(115200, SERIAL_8N1, 3, 1);\n\tESPSerial.end();/g' agon-vdp-patched/video.ino
sed -i 's/vdu_sys_sprites();.*$/vdu_sys_sprites\(\);\t\t\t\/\/ Sprite system control\n\t\t\t\tbreak;\n\t\t\tcase 0x1C:\t\t\t\t\/\/ HEXLOAD VDU 23,28\n\t\t\t\tvdu_sys_hexload\(\);/g' agon-vdp-patched/video.ino
sed -i 's/Agon Quark VPD Version %d\.%02d/Agon Quark VPD Version %d\.%02d Hexload patch/g' agon-vdp-patched/video.ino
echo "Relocating video code"
cd agon-vdp-patched
mv $(ls --ignore video) video
cd ..
echo "Patch complete"
