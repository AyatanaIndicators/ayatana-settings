# Ayatana Settings

## Info

Ayatana Settings allows you to configure all your Ayatana system indicators.

## Build dependencies

 - cmake-extras
 - intltool
 - gtk4
 - glib2
 - glibc
 - hicolor-icon-theme

## Runtime dependencies

 - gtk4
 - glib2
 - glibc
 - hicolor-icon-theme

## Installation

```
git clone https://github.com/AyatanaIndicators/ayatana-settings.git
cd ayatana-settings
mkdir build
cd build
cmake ..
make
sudo make install
```

**The install prefix defaults to `/usr`, change it with `-DCMAKE_INSTALL_PREFIX=/some/path`**
