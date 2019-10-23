Epson Inkjet Printer Linux Driver(L455_L456_L36x_L22x_L31x_L13x Series)

1.Install Software

sudo apt install lsb-base build-essential automake autoconf libtool system-config-printer libcups2-dev libcupsimage2-dev cups alien foomatic-filters foomatic-db-engine printer-driver-escpr

2.Compile driver

cd epson-inkjet-printer-filter-1.0.0

autoreconf -f -i

./configure --prefix=/opt/epson-inkjet-printer-201401w

make -j4

3.Install driver

sudo make install

cd ..

sudo cp -a epson-inkjet-printer-201401w-1.0.0/ppds /opt/epson-inkjet-printer-201401w

sudo cp -a epson-inkjet-printer-201401w-1.0.0/watermark /opt/epson-inkjet-printer-201401w

4.Config cups

sudo cupsctl –remote-any

sudo usermod -a -G lpadmin pi
