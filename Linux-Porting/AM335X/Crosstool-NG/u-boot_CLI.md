fatload mmc 0:1 0x80200000 zImage

fatload mmc 0:1 0x80f00000 am225x-boneblack.dtb

setenv bootargs console=ttyO0,115200

bootz 0x80200000 - 0x80f00000