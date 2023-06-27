# ENROLLING YOUR OWN MOK

> **Note** This guide assumes you are using shim as your first-stage bootloader (a bootloader that loads the bootloader that loads your OS). 
Most distributions (like Fedora) already use this by default. Check out the [Arch Wiki](https://wiki.archlinux.org/title/Unified_Extensible_Firmware_Interface/Secure_Boot#shim) 
for more information. 

## Generating your own MOK

First, choose a safe place to store your keys. A good location would be one that is:

- Made by the user.
- Accessible/modifiable only to root.
- Located on an encrypted drive/partition.

None of this is needed of course, but it is good practice.  
For example I chose `/var/lib/shim-signed/mok`.  
Now run the following commands as root:

```bash
⋕ MOKDIR="/var/lib/shim-signed/mok" # replace this path with the one you chose.
⋕ mkdir -p $MOKDIR && cd $MOKDIR
⋕ openssl req -new -x509 -newkey rsa:2048 -keyout MOK.priv -outform DER -out MOK.der -days 36500 -noenc -subj "/CN=My MOK" # replace "My MOK" with whatever you want
```

And voilà, your key has now been generated.  

## Enrolling the key

To enroll your keys run the following command:

```bash
⋕ mokutil --import $MOKDIR/MOK.der
```

Or if you want to enroll the DKMS key

```bash
⋕ mokutil --import /var/lib/dkms/mok.pub
```

You will be prompted for a one time password. Now reboot your computer.  
You should now see a blue screen that says "Press any key to perform MOK management". Now do the following:

1. press any key, as instructed; a menu should pop up
2. select "Enroll MOK"
3. select "Continue"
4. select "Yes"
5. enter the one time password you entered before (when you ran `mokutil --import ...`)
6. you should now be back on the first menu; select reboot

You can verify if the key was enrolled successfully by running the following command as root:

```bash
⋕ mokutil --test-key $MOKDIR/MOK.der
```

Or if you enrolled the DKMS key:

```bash
⋕ mokutil --test-key /var/lib/dkms/mok.pub
```

If the output says `/path/to/your/key is already enrolled`, you have successfully enrolled your own MOK!
