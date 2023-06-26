# inteluv
A kernel module that allows the user to undervolt Intel CPUs when SecureBoot is enabled.

## WARNING
This module bypasses security checks put in place by the kernel, by allowing userspace applications to write to certain MSRs.  
A whitelist has been put in place to restrict access to said MSRs, but I do not make ANY guarantees about the safety of this module.  
**PLEASE USE AT YOUR OWN RISK.**

## DEPENDENCIES

### Fedora
```bash
$ sudo dnf install \
      kernel-devel \
      kernel-headers
```

## BUILDING

First thing first, you'll have to enroll your own MOK key. If you don't know how to do that, either check [this](ENROLL.md) out or 
follow the [Arch Wiki](https://wiki.archlinux.org/title/Unified_Extensible_Firmware_Interface/Secure_Boot#shim) guide.  

Edit the file named `SIGNING` and replace the fields `KEY=` with the path to your private key (usually ending with \*.priv or \*.key) 
and `X509=` with the path to your public key (usually ending with \*.der or \*.x509). You can leave `HASH=` as is.

After that, make sure you're logged in as root (running the other commands as sudo wont do).

```bash
$ sudo -i
```

Once you've entered your password, run the following commands in order:

```bash
# cd /path/to/inteluv
# make
# make install
```

And we're done! The module is installed. You should now be able to run `modprobe inteluv` as root and `/dev/inteluv` should now exist.  

## SETTING UP DKMS

// TODO

## USING WITH intel-undervolt

To allow `intel-undervolt` to use the new module, we're going to have to patch it and build it from source. 
To do that, run the following commands.

```bash
$ git clone https://github.com/kitsunyan/intel-undervolt.git
$ cd intel-undervolt
$ patch -p1 < /path/to/inteluv/intel-undervolt.patch
$ ./configure --enable-systemd --bindir=/usr/local/bin && make
$ sudo make install # To install intel-undervolt to your system
```

Now try running `sudo intel-undervolt read`. You should get all 0.0mV (or -0.0mV).