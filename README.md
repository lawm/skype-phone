# Freetalk SP2014 / ... / Belkin ..... patches

These are changes to be overlaid on top of original GPL source
for the Freetalk SP2014 / ... / Belkin .... Skype phone.

It disables unneeded packages, updates URLs and fixes compilation
on a modern Ubuntu system

More info at https://lawm.github.io/2018/06/16/skype-phone.html


Download and apply:

    download F1PP010EN-SKv1_GPL.tar.gz
    git clone https://github.com/lawm/skype-phone.git

    mkdir patched
    tar xzf F1PP010EN-SKv1_GPL.tar.gz -C patched

    cp -r skype-phone/buildroot patched/


Delete corrupt file:

    rm patched/buildroot/dl/binutils-2.16.1.tar.bz2


Build buildroot, including toolchain:

    cd patched/buildroot/config/onephone_mips32/
    make


Build test app:

    cd -
    cd apps
    make
