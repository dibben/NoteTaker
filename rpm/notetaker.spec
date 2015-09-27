%if 0%{?suse_version}
%define _qt5_qmake /usr/%_lib/qt5/bin/qmake
%endif
%define lrelease lrelease
%if 0%{?fedora_version}
%define  debug_package %{nil}
%endif

Name:                   notetaker
Summary:                Qt-based Notes editor
Group:                  Productivity/Text/Notes
Version:                0.1
Release:                1
License:                GPL-2.0+
Url:                    http://github.com/dibben/NoteTaker
Source0:                %{name}-%{version}.tar.gz
BuildRoot:              %{_tmppath}/%{name}-%{version}-build


BuildRequires:          libstdc++-devel 
BuildRequires:          gcc-c++ 
BuildRequires:          make
BuildRequires:          pkgconfig
BuildRequires:          pkgconfig(Qt5Core)
BuildRequires:          pkgconfig(Qt5Gui)
BuildRequires:          pkgconfig(Qt5Network)
BuildRequires:          pkgconfig(hunspell)
%if 0%{?suse_version}
BuildRequires:          update-desktop-files
BuildRequires:          pkgconfig(Qt5Test)
BuildRequires:          libqt5-qttools-devel
%endif
%if 0%{?fedora} <= 19
BuildRequires:          libqt5-qttools-devel
%endif
%if 0%{?fedora} >= 20
BuildRequires:          qt5-qttools-devel
%endif
%if 0%{?fedora}
BuildRequires:          desktop-file-utils
%endif


%description
A Qt-based notes editor that syncs with simplenote

 
%prep
%setup -q -n %{name}-%{version}

%build
%{_qt5_qmake} -r
make

%install
make INSTALL_ROOT="%buildroot" install

%files 
%defattr(-,root,root,755)
%_bindir/%{name}
%{_datadir}/applications/%{name}.desktop
#%{_datadir}/icons/hicolor/*/apps/%{name}.*



%changelog

* Sat Sep 26 2015 David Dibben <dibben@ieee.org> 0.1
- Initial release
