i implemented a new networking protocol for multiplexing endpoints within a network node with unlimited amount of values.

this is meant to be a better version of udp ports and tcp ports, which are unsigned 16-bit values and are limited to 2^16 values.

my protocol uses variable-length (unlimited length) array of unsigned bytes which represent a logical endpoint within a network node
example for endpoints are {0x11, 0x22, 0x33}, {0x50}, {0x00, 0x00, 0x01}, {0xff, 0xff}.

in my actual implementation, i used a trie data structure to handle the various endpoint efficiently.

the protocol is called "endpoint multiplexing protocol" (EMP), and can be found under `src/protocols/emp`