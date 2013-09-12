Name: RobotSteve
Summary: Programmieren lernen
License: GPL-2.0
Group: Amusements/Teaching/Other
Version: 0.9.2
Release: 1
Source0: %{name}-%{version}.tar.gz
Packager: Fabian Vogt <fabian@ritter-vogt.de>
BuildRoot: %{_tmppath}/%{name}-%{version}-build
BuildRequires: libqt4-devel

%description

%prep
%setup

%build
qmake
make

%install
make INSTALL_ROOT=%{buildroot} install

%files
%{_bindir}/RobotSteve
%{_datadir}/applications/RobotSteve.desktop
%{_datadir}/icons/RobotSteve.png
%{_datadir}/mime/packages/RobotSteve.xml
