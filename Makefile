# ExtraVersion
#
#   make            AmigaOS 4 build (cross compiler in docker): build/os4/ExtraVersion
#   make host       Linux build for the tests: build/host/ExtraVersion
#   make test       host build + tests (tests/run_tests.py)
#   make test-os4   AmigaOS 4 build + the same tests in the OS4 guest
#   make lha        release archive: dist/ExtraVersion.lha (tools/make_lha.sh)
#
# Native AmigaOS 4 build (SDK with GCC):
#   make DOCKER= CC=gcc STRIP=strip

IMAGE   = walkero/amigagccondocker:os4-gcc15-3.6.1
DOCKER  = sudo docker run --rm -u $(shell id -u):$(shell id -g) -v $(CURDIR):/src -w /src $(IMAGE)
CC      = $(DOCKER) ppc-amigaos-gcc
STRIP   = $(DOCKER) ppc-amigaos-strip
CFLAGS  = -O2 -Wall -Wextra -std=gnu99
# -N: no page alignment of the segments (as the original makefile); without
# it GCC 15's linker pads the 10 KB program to 70 KB
LDFLAGS = -N -Wl,--no-warn-rwx-segments
GUESTPY = $(HOME)/build/amiga-fleet-venv/bin/python
HOSTCC  = cc
HOSTCFLAGS = -O1 -g -Wall -Wextra -std=gnu99 -fsanitize=address,undefined

SRC = src/extraversion.c

all: build/os4/ExtraVersion

build/os4/ExtraVersion: $(SRC)
	mkdir -p build/os4
	$(CC) $(CFLAGS) -mcrt=newlib $(LDFLAGS) -o $@ $(SRC)

host: build/host/ExtraVersion

build/host/ExtraVersion: $(SRC)
	mkdir -p build/host
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $(SRC)

test: host
	python3 -I tests/run_tests.py

test-os4: all
	$(GUESTPY) -I tests/run_tests.py --guest

lha: all
	tools/make_lha.sh

clean:
	rm -rf build/os4 build/host build/stage

.PHONY: all host test test-os4 lha clean
