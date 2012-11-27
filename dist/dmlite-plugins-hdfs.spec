Name:		dmlite-plugins-hdfs
Version:	0.6.0
Release:	1%{?dist}
Summary:	HDFS plugin for dmlite
Group:		Applications/Internet
License:	ASL 2.0
URL:		https://svnweb.cern.ch/trac/lcgdm
# The source of this package was pulled from upstream's vcs. Use the
# following commands to generate the tarball:
# svn export http://svn.cern.ch/guest/lcgdm/dmlite-plugins-hdfs/tags/dmlite-plugins-hdfs_0_5_0 dmlite-plugins-hdfs-0.5.0
# tar -czvf dmlite-plugins-hdfs-0.5.0.tar.gz dmlite-plugins-hdfs-0.5.0
Source0:	%{name}-%{version}.tar.gz
Buildroot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:	cmake%{?_isa}
BuildRequires:	dmlite-devel%{?_isa}
BuildRequires:	hadoop-libhdfs
BuildRequires:	java-devel

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
* Tue Nov 27 2012 Andrea Manzi <andrea.manzi@cern.ch> - 0.6.0
- hdfs-ns implementation

* Tue Oct 16 2012 Andrea Manzi <andrea.manzi@cern.ch> - 0.5.0
- update to new upstream release
- renamed component to dmlite-plugins-hdfs (was dmlite-plugins-hadoop)

* Wed Apr 11 2012 Ricardo Rocha <ricardo.rocha@cern.ch> - 0.3.0-1
- Initial build
