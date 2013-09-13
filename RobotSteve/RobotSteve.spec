Name: RobotSteve
Summary: Programmieren lernen
License: GPL-2.0
Group: Amusements/Teaching/Other
Version: 0.9.5
Release: 1
Source0: %{name}-%{version}.tar.gz
Packager: Fabian Vogt <fabian@ritter-vogt.de>
BuildRoot: %{_tmppath}/%{name}-%{version}-build
BuildRequires: libqt4-devel
BuildRequires: shared-mime-info
BuildRequires: update-desktop-files

%description

%prep
%setup

%build
qmake
make

%install
make INSTALL_ROOT=%{buildroot} install
%suse_update_desktop_file RobotSteve

%post
%mime_database_post

%postun
%mime_database_postun

%files
%doc license.txt Unterschiede.txt Beispiele.txt
%{_bindir}/RobotSteve
%{_datadir}/applications/RobotSteve.desktop
%{_datadir}/icons/RobotSteve.png
%{_datadir}/mime/packages/RobotSteve.xml
