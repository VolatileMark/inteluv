KVER ?= $(shell uname -r)
CERT_KEY ?= /var/lib/shim-signed/mok/MOK.priv
CERT_X509 ?= /var/lib/shim-signed/mok/MOK.der
HASH_ALGO ?= sha512

sign-file = $(shell find /usr/src -name sign-file)
build-dir = /lib/modules/$(KVER)/build

kobj-target = inteluv.ko
obj-m += inteluv.o

sign: module
	$(sign-file) $(HASH_ALGO) $(CERT_KEY) $(CERT_X509) $(kobj-target)

module:
	$(MAKE) -C $(build-dir) M=$(PWD) modules

clean:
	$(MAKE) -C $(build-dir) M=$(PWD) clean
	rm -f inteluv_test

test:
	gcc inteluv_test.c -o inteluv_test

.PHONY: sign module clean
