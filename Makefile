SoC=esp32s3
SERIAL_PORT=$(shell ls -d /dev/* | grep usbmodem | head -n1)

.PHONY: all build post_build clean flash serial
all: build post_build flash

set_target: sdkconfig
	idf.py set-target $(SoC)

build:
	idf.py build

post_build:
	-mv build/compile_commands.json ./ 

serial:
	idf.py monitor -p $(SERIAL_PORT)

flash:
	idf.py flash -p $(SERIAL_PORT)

clean:
	rm -rf build/
