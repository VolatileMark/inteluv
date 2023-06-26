KDIR ?= /lib/modules/$(shell uname -r)/build
CERT_KEY ?= $(shell grep SIGNING -e "KEY=" | cut -d "=" -f 2)
CERT_X509 ?= $(shell grep SIGNING -e "X509=" | cut -d "=" -f 2)
HASH_ALGO ?= $(shell grep SIGNING -e "HASH=" | cut -d "=" -f 2)

sign-file = $(shell find /usr/src -name sign-file)
version = $(shell cat VERSION)

kobj-target = inteluv.ko
obj-m += inteluv.o

sign: module
	$(sign-file) $(HASH_ALGO) $(CERT_KEY) $(CERT_X509) $(kobj-target)

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f inteluv_test

test:
	gcc inteluv_test.c -o inteluv_test

.PHONY: sign module clean
