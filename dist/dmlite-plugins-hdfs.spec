Name:		dmlite-plugins-hdfs
Version:	0.7.3
Release:	3%{?dist}
Summary:	HDFS plugin for dmlite
Group:		Applications/Internet
License:	ASL 2.0
URL:		https://svnweb.cern.ch/trac/lcgdm
# The source of this package was pulled from upstream's vcs. Use the
# following commands to generate the tarball:
# svn export http://svn.cern.ch/guest/lcgdm/dmlite-plugins-hdfs/tags/dmlite-plugins-hdfs_0_6_0 dmlite-plugins-hdfs-0.6.1
# tar -czvf dmlite-plugins-hdfs-0.6.1.tar.gz dmlite-plugins-hdfs-0.6.1
Source0:	%{name}-%{version}.tar.gz
Buildroot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:	cmake%{?_isa}
BuildRequires:	dmlite-devel >= 0.7.2
BuildRequires:  dmlite-private-devel >= 0.7.2
BuildRequires:	hadoop-libhdfs
BuildRequires:	java-devel

Requires:	dmlite-libs >= 0.7.2
Requires: 	java-devel

%description
This package provides the HDFS plugin for DMLITE. It provides a HDFS
implementation of all the DMLITE interfaces (NS,pool, IO, etc).

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

%files
%defattr(-,root,root,-)
%{_libdir}/dmlite/plugin_hdfs.so
%doc LICENSE README RELEASE-NOTES
%config(noreplace) %{_sysconfdir}/dmlite.conf.d/*

%changelog
* Wed Jul 08 2015 Andrea Manzi <andrea.manzi@cern.ch> - 0.7.3-3
- set HDFSReplication default value to 2
- fix cleaning of tmp files when copy to HDFS fails

* Tue May 19 2015 Andrea Manzi <andrea.manzi@cern.ch> - 0.7.3-2
- usage of hdfsConnectAsUserNewInstance
- added HDFSReplication parameter

* Mon Apr 13 2015 Andrea Manzi <andrea.manzi@cern.ch> - 0.7.3-1
- enable write via xrootd

* Fri Apr 10 2015 Andrea Manzi <andrea.manzi@cern.ch> - 0.7.2-1
- fixed issue on removeReplica

* Wed Nov 26 2014 Andrea Manzi <andrea.manzi@cern.ch> - 0.7.1-1
- added simple Authn 
- added some missing functions for NS
- fixed issue with metalink

- removed getDatanode and the usage of the libhdfs extensions. Fully replaced by usage of HdfsGateway
* Fri Jul 11 2014 Andrea Manzi <andrea.manzi@cern.ch> - 0.7.0-1
- adapted to dmlite 0.7.0
- added new logging
- removed getDatanode and the usage of the libhdfs extensions. Fully replaced by usage of HdfsGateway

* Tue May 6 2014 Andrea Manzi <andrea.manzi@cern.ch> - 0.6.5-1
- added Multi gateway support
- implemented file buffering for write operations
- added HdfsTmpFolder var

* Mon Mar 10 2014 Andrea Manzi <andrea.manzi@cern.ch> - 0.6.4-1
- implemented pread on HDFSIOHandler
- removed O_RDWR mode on HDFSIOHandler ( cause not supported by HDFS)
- added logging with syslog

* Tue Dec 17 2013 Andrea Manzi <andrea.manzi@cern.ch> - 0.6.3-1
- Added HDFSGateway mode
- compliant with dmlite-lib 0.6.2

* Mon Oct 21 2013 Andrea Manzi <andrea.manzi@cern.ch> - 0.6.2-1
- Adapted HDFSIOHandler to latest dmlite
- removed check for EOF

* Mon May 6 2013 Andrea Manzi <andrea.manzi@cern.ch> - 0.6.1-1
- adapted to dmlite 0.6.1 and changes to libhdfs

* Wed Dec 19 2012 Andrea Manzi <andrea.manzi@cern.ch> - 0.6.0-1
- update to new upstream release

* Tue Oct 16 2012 Andrea Manzi <andrea.manzi@cern.ch> - 0.5.0-1
- update to new upstream release
- renamed component to dmlite-plugins-hdfs (was dmlite-plugins-hadoop)

* Wed Apr 11 2012 Ricardo Rocha <ricardo.rocha@cern.ch> - 0.3.0-1
- Initial build
