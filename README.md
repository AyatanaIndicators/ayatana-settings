# Ayatana Settings

### Info
Ayatana Settings allows you to configure all your Ayatana system indicators.
### Build dependencies <sup>[1]</sup>

- python-polib <sup>[2]</sup>
- python-setuptools <sup>[2]</sup>

### Runtime dependencies <sup>[1]</sup>

- gtk3
- gobject-introspection
- python-psutil <sup>[2]</sup>

### Installation <sup>[2]</sup>
	git clone https://github.com/AyatanaIndicators/ayatana-settings.git
	cd ayatana-settings
	python setup.py build
	sudo python setup.py install --root=/ --optimize=1
	
[1] The package names may slightly vary among various Linux flavours

[2] Make sure "python" refers to Python 3 (e.g. python in Arch, python3 in Debian) 
