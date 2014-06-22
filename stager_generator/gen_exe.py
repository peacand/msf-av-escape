#!/usr/bin/python2.6

###################
# Michael Molho   #
#     2014        #
###################

import sys
import struct

if len(sys.argv) != 4:
    sys.stderr.write('Usage ' + sys.argv[0] + ' <template file> <ip> <port> \n')
    sys.exit(1)

template = sys.argv[1]
ip = sys.argv[2]
port = sys.argv[3]

raw = open(template, 'rb').read()

ref = "AAA.AAA.AAA.AAA" + '\0' + "AAAAA" + '\0'
ip_offset = raw.find(ref)

if ip_offset > 0:
    raw = list(raw)
    raw[ip_offset:ip_offset+len(ip)+1] = ip + '\0'
    port_offset = ip_offset + 16
    raw[port_offset:port_offset+len(port)+1] = port + '\0'
    xor_offset = port_offset + 8
    use_xor = struct.unpack('<I', ''.join(raw[xor_offset:xor_offset+4]))[0]
    raw = ''.join(raw)
    sys.stdout.write(raw)
    sys.stderr.write("IP => " + ip + "\nPORT => " + port + "\nSucceed !\n")
else:
    sys.stderr.write("Error : IP/Port pattern not found ... \n")

