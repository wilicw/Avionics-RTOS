.PHONY: all build clean flash
all: build flash

build:
	west build -b esp32
	-mv build/compile_commands.json ./ 

flash: build
	west flash

clean:
	rm -rf build/
