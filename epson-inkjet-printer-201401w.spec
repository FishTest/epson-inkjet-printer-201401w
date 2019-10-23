# Copyright (C) Seiko Epson Corporation 2014.

%define pkgtarname epson-inkjet-printer-201401w
%define drivername epson-driver
%define filtername epson-inkjet-printer-filter
%define supplier %{pkgtarname}
%define distribution LSB
%define lsbver 3.2
%define filterver 1.0.0

%define driverstr epson-inkjet-printer
%define extraversion -1lsb%{lsbver}
%define supplierstr Seiko Epson Corporation

AutoReqProv: no

Name: %{pkgtarname}
Version: 1.0.0
Release: 1lsb%{lsbver}
Source0: %{filtername}-%{filterver}.tar.gz
Source1: %{pkgtarname}-%{version}.tar.gz
License: LGPL and SEIKO EPSON CORPORATION SOFTWARE LICENSE AGREEMENT
Vendor: Seiko Epson Corporation
URL: http://download.ebz.epson.net/dsc/search/01/search/?OSC=LX
Packager: Seiko Epson Corporation <linux-printer@epson.jp>
BuildRoot: %{_tmppath}/%{pkgtarname}-%{version}-%{release}
Group: Applications/System
Requires: lsb >= %{lsbver}
BuildRequires: lsb-build-cc, lsb-build-c++, lsb-appchk
BuildRequires: gzip
Summary: L455_L456_L36x_L22x_L31x_L13x Series - Epson Inkjet Printer Driver

%description
This software is a filter program used with Common UNIX Printing
System (CUPS) from the Linux. This can supply the high quality print
with Seiko Epson Color Ink Jet Printers.

This printer driver is supporting the following printers.

L456
L455
L366
L365
L362
L360
L312
L310
L222
L220
L132
L130

For detail list of supported printer, please refer to below site:
http://download.ebz.epson.net/dsc/search/01/search/?OSC=LX

# Packaging settings
%install_into_opt

%prep
rm -rf $RPM_BUILD_DIR/%{filtername}-%{filterver}
rm -rf $RPM_BUILD_DIR/%{pkgtarname}-%{version}

tar xzvf %{_sourcedir}/%{filtername}-%{filterver}.tar.gz -C $RPM_BUILD_DIR/
tar xzvf %{_sourcedir}/%{pkgtarname}-%{version}.tar.gz -C $RPM_BUILD_DIR/

%build
cd $RPM_BUILD_DIR/%{filtername}-%{filterver}
autoreconf -f -i
%configure
make 

%install
rm -rf %{buildroot}
install -d %{buildroot}%{_cupsserverbin}/filter
install -d %{buildroot}%{_docdir}

cd $RPM_BUILD_DIR/%{filtername}-%{filterver}
install src/`echo %{filtername} | tr - _` %{buildroot}%{_cupsserverbin}/filter
for file in AUTHORS COPYING COPYING.LIB COPYING.EPSON ; do
	install -m 644 $file %{buildroot}%{_docdir}
done

cd $RPM_BUILD_DIR/%{pkgtarname}-%{version}
install -m 644 Manual.txt README %{buildroot}%{_docdir}

case `uname -m` in
	x86_64)	X86LIB=64 ;;
	*)	;;
esac

for folder in ppds lib"$X86LIB" resource watermark ; do
	install -d %{buildroot}%{_prefix}/$folder
	install -m 644 -t %{buildroot}%{_prefix}/$folder $folder/*
done

%adjust_ppds

cd %{buildroot}%{_prefix}/ppds
for ppd in `ls -1F *.ppd` ; do
	gzip -9 $ppd
done

%pre
%create_opt_dirs

%post
/sbin/ldconfig
%set_ppd_links
%update_ppds_fast
%restart_cups

%postun
/sbin/ldconfig
%not_on_rpm_update
%remove_ppd_links
%restart_cups
%end_not_on_rpm_update

%clean
cd $RPM_BUILD_DIR/%{filtername}-%{filterver}
make clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%{_prefix}/*
