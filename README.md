## Ayatana Settings
####Info
Ayatana Settings allows you to configure all your Ayatana system indicators.
####Build dependencies ^[1]^

- python-polib
- python-setuptools

####Runtime dependencies ^[1]^

- gtk3
- gobject-introspection
- python-psutil

[1] The package names may slightly vary among various Linux flavours 
####Installation
	git clone https://github.com/AyatanaIndicators/ayatana-settings.git
	cd ayatana-settings
	python setup.py build
	sudo python setup.py install --root=/ --optimize=1