epson-inkjet-printer-201401w
L455_L456_L36x_L22x_L31x_L13x Series - Epson Inkjet Printer Linux Driver
sudo apt install lsb-base build-essential automake autoconf libtool system-conf$
cd epson-inkjet-printer-filter-1.0.0
autoreconf -f -i
./configure --prefix=/opt/epson-inkjet-printer-201401w
make -j4
sudo make install
sudo cp -a epson-inkjet-printer-201401w-1.0.0/ppds /opt/epson-inkjet-printer-20$
sudo cp -a epson-inkjet-printer-201401w-1.0.0/watermark /opt/epson-inkjet-print$
sudo cupsctl ¨Cremote-any
sudo usermod -a -G lpadmin pi
