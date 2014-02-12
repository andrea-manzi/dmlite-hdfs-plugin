Name:		dmlite
Version:	0.4.0
Release:	1%{?dist}
Summary:	Common libraries for grid data management and storage
Group:		Applications/Internet
License:	ASL 2.0
URL:		https://svnweb.cern.ch/trac/lcgdm/wiki/Dpm/Dev/Dmlite
# The source of this package was pulled from upstream's vcs. Use the
# following commands to generate the tarball:
# svn export http://svn.cern.ch/guest/lcgdm/dmlite/tags/dmlite_0_4_0 dmlite-0.4.0
# tar -czvf dmlite-0.4.0.tar.gz dmlite-0.4.0
Source0:	%{name}-%{version}.tar.gz
Buildroot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%if %{?fedora}%{!?fedora:0} >= 10 || %{?rhel}%{!?rhel:0} >= 6
BuildRequires:	boost-devel >= 1.41.0
%else
BuildRequires:	boost141-devel
%endif
BuildRequires:	cmake
BuildRequires:	cppunit-devel
BuildRequires:	doxygen
BuildRequires:	graphviz
BuildRequires:	openssl-devel

%description
This package provides a set of common libraries and plugins that implement
logic for data management and storage on the grid.

%package libs
Summary:	Common libraries for all dmlite packages
Group:		Applications/Internet

%description libs
This package provides the libraries used by dmlite components.

%package devel
Summary:	Development libraries and headers for dmlite
Group:		Applications/Internet
Requires:	%{name}-libs%{?_isa} = %{version}-%{release}
%if %{?fedora}%{!?fedora:0} >= 10 || %{?rhel}%{!?rhel:0} >= 6
Requires:	boost-devel >= 1.41.0
%else
Requires:	boost141-devel
%endif

%description devel
This package provides headers and development libraries for dmlite.

%package docs
Summary:	API documentation for dmlite
Group:		Applications/Internet

%description docs
Man pages and HTML documentation for dmlite.

%package tests
Summary:	Tests for dmlite
Group:		Applications/Internet

%description tests
Tests for dmlite and plug-ins.

%prep
%setup -q -n %{name}-%{version}

%build
%cmake . -DCMAKE_INSTALL_PREFIX=/

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}

make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

%post libs -p /sbin/ldconfig

%postun libs -p /sbin/ldconfig

%files libs
%defattr(-,root,root,-)
%config(noreplace) %{_sysconfdir}/dmlite.conf
%{_libdir}/libdmlite.so.*
%doc README LICENSE

%files devel
%defattr(-,root,root,-)
%{_includedir}/dmlite
%{_libdir}/libdmlite.so

%files docs
%defattr(-,root,root,-)
%{_mandir}/man3/*
%{_defaultdocdir}/%{name}-%{version}

%files tests
%defattr(-,root,root,-)
%{_datadir}/tests/*

%changelog
* Fri Jul 13 2012 Ricardo Rocha <ricardo.rocha@cern.ch> - 0.3.0-1
- Update for new upstream release

* Tue Jun 05 2012 Ricardo Rocha <ricardo.rocha@cern.ch> - 0.2.0-3
- Removed subversion build dep
- Added patches for proper tests compilation (missing include, wrong cmake dep)

* Sun May 20 2012 Ricardo Rocha <ricardo.rocha@cern.ch> - 0.3.0-1
- Update for new upstream release

* Tue Feb 28 2012 Ricardo Rocha <ricardo.rocha@cern.ch> - 0.2.0-2
- Split plugins into multiple packages, added dependencies
- Updated package descriptions

* Tue Jan 31 2012 Alejandro Alvarez <alejandro.alvarez.ayllon@cern.ch> - 0.2.0-1
- Added documentation to the build process

* Mon Jan 23 2012 Alejandro Alvarez <alejandro.alvarez.ayllon@cern.ch> - 0.1.0-1
- Added cppunit-devel as a build dependency

* Tue Jan 20 2012 Alejandro Alvarez <alejandro.alvarez.ayllon@cern.ch> - 0.1.0-1
- Created spec file
