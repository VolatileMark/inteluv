KDIR ?= /lib/modules/$(shell uname -r)/build
ODIR ?= $(abspath .)
EXTRAFLAGS ?=
CERT_KEY ?= $(shell grep SIGNING -e "KEY=" | cut -d "=" -f 2)
CERT_X509 ?= $(shell grep SIGNING -e "X509=" | cut -d "=" -f 2)
HASH_ALGO ?= $(shell grep SIGNING -e "HASH=" | cut -d "=" -f 2)

sign-file = $(shell find /usr/src -name sign-file)
version = $(shell cat VERSION)
dkms = $(shell which dkms)
pwd = $(shell pwd)

kobj-target = inteluv.ko
obj-m += inteluv.o

module:
	$(MAKE) $(EXTRAFLAGS) -C $(KDIR) M=$(ODIR) src=$(pwd) modules

sign:
	$(sign-file) $(HASH_ALGO) $(CERT_KEY) $(CERT_X509) $(kobj-target)

clean:
	$(MAKE) -C $(KDIR) M=$(pwd) clean
	rm -f inteluv_test

test:
	gcc inteluv_test.c -o inteluv_test

dkms:
	sed s/@@VERSION@@/$(version)/g dkms.conf.in > dkms.conf
	$(dkms) add $(pwd)

dkms-setup:
	rm -f /etc/dkms/framework.conf.d/dkms-signing-keys.conf
	echo "mok_signing_key=\"$(CERT_KEY)\"" >> /etc/dkms/framework.conf.d/dkms-signing-keys.conf
	echo "mok_certificate=\"$(CERT_X509)\"" >> /etc/dkms/framework.conf.d/dkms-signing-keys.conf

.PHONY: sign module clean test dkms dkms-setup
