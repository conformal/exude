# $exude$

%define name		exude
%define version		0.3.1
%define release		1

Name: 		%{name}
Summary: 	Simple to use memory debugger library for C
Version: 	%{version}
Release: 	%{release}
License: 	ISC
Group: 		System Environment/Libraries
URL:		http://opensource.conformal.com/wiki/exude
Source: 	%{name}-%{version}.tar.gz
Buildroot:	%{_tmppath}/%{name}-%{version}-buildroot
Prefix: 	/usr
Requires:	clog >= 0.3.4

%description
exude is a simple to use memory debugger. The idea is to enhance standard
memory allocation calls with simple replacements that add just a little more
functionality, such as keeping track if memory was previously allocated, to
prevent common errors such as double free, memory leaks etc.

%prep
%setup -q

%build
make

%install
make install DESTDIR=$RPM_BUILD_ROOT LOCALBASE=/usr

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
/usr/lib/libexude.so.*


%package devel
Summary: Libraries and header files to develop applications using exude 
Group: Development/Libraries
Requires: clens-devel >= 0.0.5, clog-devel >= 0.3.4

%description devel
This package contains the libraries, include files, and documentation to
develop applications with exude.

%files devel
%defattr(-,root,root)
%doc /usr/share/man/man?/*
/usr/include/exude.h
/usr/lib/libexude.so
/usr/lib/libexude.a

%changelog
* Tue Jul 26 2011 - davec 0.3.1-1
- Don't link against clens directly from library
* Tue Jul 03 2011 - davec 0.3.0-1
- Create
