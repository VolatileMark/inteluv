# inteluv
A kernel module that allows the user to undervolt Intel CPUs when SecureBoot is enabled.

## WARNING
This module bypasses security checks put in place by the kernel, by allowing userspace applications to write to certain MSRs.  
A whitelist has been put in place to restrict access to said MSRs, but I do not make ANY guarantees about the safety of this module.  
**PLEASE USE AT YOUR OWN RISK.**
