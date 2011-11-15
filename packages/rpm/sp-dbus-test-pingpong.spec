Name: sp-dbus-test-pingpong
Version: 0.6.0
Release: 1%{?dist}
Summary: A minimal client/server for testing D-Bus throughput/latency
Group: Development/Tools
License: GPLv2+
URL: http://www.gitorious.org/+maemo-tools-developers/maemo-tools/sp-dbus-test-pingpong
Source: %{name}_%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build
BuildRequires: dbus-1-glib-devel, dbus-1-devel

%description
 A client/server pair for testing D-Bus throughput/latency by pushing
 a continuous stream of messages and measuring the mininum/average/maximum
 latency, as well as the amount of messages pushes in a given time.
 
%prep
%setup -q -n %{name}

%build
make

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(755,root,root,-)
%{_bindir}/dping
%{_bindir}/dpong
%defattr(644,root,root,-)
%{_mandir}/man1/*.1.gz
%doc README COPYING 

