BOARD=esp32
SERIAL_PORT=$(shell ls -d /dev/* | grep USB | head -n1)

.PHONY: all build_prod build post_build clean flash serial
all: build post_build flash

production: build_prod post_build flash

build_prod:
	west build -b $(BOARD)

build:
	west build -b $(BOARD) -- -DOVERLAY_CONFIG=dbg.conf

post_build:
	-mv build/compile_commands.json ./ 

serial:
	minicom -D $(SERIAL_PORT) -b 115200 -C log.tmp

flash:
	west flash

clean:
	rm -rf build/
