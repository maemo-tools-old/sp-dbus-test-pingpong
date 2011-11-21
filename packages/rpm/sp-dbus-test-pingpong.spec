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
%defattr(-,root,root,-)
%{_bindir}/dping
%{_bindir}/dpong
%{_mandir}/man1/*.1.gz
%doc README COPYING 

%changelog
* Mon Aug 08 2011 Eero Tamminen <eero.tamminen@nokia.com> 0.6
  *  - sp-dbus-test-pingpong: not open-source

* Wed Oct 13 2010 Eero Tamminen <eero.tamminen@nokia.com> 0.5
  * Attach to DBus Session Bus instead of System Bus.
  * Fix latency values.

* Thu Oct 07 2010 Eero Tamminen <eero.tamminen@nokia.com> 0.4
  * Integrate for Harmattan - 

* Mon Feb 09 2009 Eero Tamminen <eero.tamminen@nokia.com> 0.3
  * Fixes for time-handling related issue(s) applied. 

* Wed Nov 12 2008 Eero Tamminen <eero.tamminen@nokia.com> 0.2
  * The optional roundtrip latency mode has been implemented (can be
    invoked by giving -r to dping). 

* Tue Nov 04 2008 Eero Tamminen <eero.tamminen@nokia.com> 0.1
  * Initial Release.
  