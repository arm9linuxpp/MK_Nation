
 XXXX PB2
[lwm2m_init:65] Entering
[lwm2m_configure:279] endpointName: "urn:imei:866901063238648", msisdn: "", altPath: "", numObject: 9
[lwm2m_step:384] timeoutP: 60
[lwm2m_step:389] State: STATE_INITIAL
[object_getServers:1040] Entering
[lwm2m_data_new:160] size: 1
[lwm2m_data_encode_bool:519] value: false
[lwm2m_data_decode_bool:529] Entering
[lwm2m_data_decode_bool:580] result: 1, value: false
[lwm2m_data_free:178] size: 1
[lwm2m_data_new:160] size: 1
[lwm2m_data_encode_int:289] value: 123
[lwm2m_data_decode_int:299] Entering
[lwm2m_data_decode_int:362] result: 1, value: 123
[lwm2m_data_new:160] size: 1
[lwm2m_data_encode_int:289] value: 123
[lwm2m_data_decode_int:299] Entering
[lwm2m_data_decode_int:362] result: 1, value: 123
[lwm2m_data_free:178] size: 1
[lwm2m_data_new:160] size: 2
[lwm2m_data_encode_int:289] value: 300
[lwm2m_data_encode_string:214] "U"
[lwm2m_data_decode_int:299] Entering
[lwm2m_data_decode_int:362] result: 1, value: 300
[lwm2m_data_free:178] size: 2
[lwm2m_data_free:178] size: 1
[registration_start:1028] State: STATE_REGISTER_REQUIRED
[lwm2m_data_new:160] size: 1
[lwm2m_data_free:178] size: 1
[lwm2m_data_new:160] size: 1
[lwm2m_data_free:178] size: 1
[observe_step:521] Entering
[registration_step:2056] State: STATE_REGISTERING
[object_getRegisterPayloadBufferLength:788] Entering
[object_getRegisterPayload:886] Entering
[lwm2m_data_new:160] size: 1
[lwm2m_data_encode_string:214] "coaps://lwm2m.mudip.com:7685"
[lwm2m_data_free:178] size: 1
[lwm2m_data_new:160] size: 1
[lwm2m_data_encode_int:289] value: 0
[lwm2m_data_decode_int:299] Entering
[lwm2m_data_decode_int:362] result: 1, value: 0
[lwm2m_data_free:178] size: 1
[transaction_new:161] method: 2, altPath: "", mID: 1290, token_len: 4
[transaction_new:162] NULL
[transaction_new:242] Exiting on success. new transac=00000163EC8468A0
[transaction_send:373] Entering: transaction=00000163EC8468A0
6 DTLSv12: initialize HASH_SHA256
6 clear MAC
6 add MAC data: (12 bytes): 010000340000000000000034
6 add MAC data: (52 bytes): FEFDE66502CA7318E05A198CEDFC7F80E5E2B344393E8EA2B55E41F04F267B96376500000006C0A8C0A400FF01000004 ...
6 send handshake packet of type: client_hello (1)
6 send header: (13 bytes):
6 00000000 16 FE FF 00 00 00 00 00  00 00 00 00 40 
6 send unencrypted: (12 bytes):
6 00000000 01 00 00 34 00 00 00 00  00 00 00 34 
6 send unencrypted: (52 bytes):
6 00000000 FE FD E6 65 02 CA 73 18  E0 5A 19 8C ED FC 7F 80 
6 00000010 E5 E2 B3 44 39 3E 8E A2  B5 5E 41 F0 4F 26 7B 96 
6 00000020 37 65 00 00 00 06 C0 A8  C0 A4 00 FF 01 00 00 04 
6 00000030 00 17 00 00 
Sending 77 bytes to [lwm2m.mudip.com]:7685
QtIpV4Ud::sendUDP: 16feff00000000000000000040010000340000000000000034fefde66502ca7318e05a198cedfc7f80e5e2b344393e8ea2b55e41f04f267b96376500000006c0a8c0a400ff0100000400170000[transaction_step:460] Entering
[lwm2m_step:491] Final timeoutP: 2
[lwm2m_step:493] Final state: STATE_REGISTERING
 -> State: STATE_REGISTERING
QtIpV4Ud::rxUdpDataEvent in !
Message:  "16fefd0000000000000000002f030000230000000000000023fefd201d820a49a41fb4207a30cd5dbb8df9226f910f17aad5c5420e70e6109f441065"  From:  "176.9.148.44" 7685
 XXXX onUdsReceive len:  60
5 received message (60 bytes), starting with 'handshake', epoch 0
5 handshake message epoch 0
5 got 'handshake' epoch 0 sequence 0 (60 bytes)
6 dtls_handle_message: FOUND PEER
6 bitfield is 0 sequence base 0 rseqn 0
6 init bitfield is 1 sequence base 0
6 receive header: (13 bytes):
6 00000000 16 FE FD 00 00 00 00 00  00 00 00 00 2F 
6 receive unencrypted: (47 bytes):
6 00000000 03 00 00 23 00 00 00 00  00 00 00 23 FE FD 20 1D 
6 00000010 82 0A 49 A4 1F B4 20 7A  30 CD 5D BB 8D F9 22 6F 
6 00000020 91 0F 17 AA D5 C5 42 0E  70 E6 10 9F 44 10 65 
6 received handshake packet of type: hello_verify_request (3)
6 handle handshake packet of type: hello_verify_request (3)
6 clear MAC
6 add MAC data: (12 bytes): 010000540001000000000054
6 add MAC data: (84 bytes): FEFDE66502CA7318E05A198CEDFC7F80E5E2B344393E8EA2B55E41F04F267B96376500201D820A49A41FB4207A30CD5D ...
6 send handshake packet of type: client_hello (1)
6 send header: (13 bytes):
6 00000000 16 FE FF 00 00 00 00 00  00 00 01 00 60 
6 send unencrypted: (12 bytes):
6 00000000 01 00 00 54 00 01 00 00  00 00 00 54 
6 send unencrypted: (84 bytes):
6 00000000 FE FD E6 65 02 CA 73 18  E0 5A 19 8C ED FC 7F 80 
6 00000010 E5 E2 B3 44 39 3E 8E A2  B5 5E 41 F0 4F 26 7B 96 
6 00000020 37 65 00 20 1D 82 0A 49  A4 1F B4 20 7A 30 CD 5D 
6 00000030 BB 8D F9 22 6F 91 0F 17  AA D5 C5 42 0E 70 E6 10 
6 00000040 9F 44 10 65 00 06 C0 A8  C0 A4 00 FF 01 00 00 04 
6 00000050 00 17 00 00 
Sending 109 bytes to [lwm2m.mudip.com]:7685
QtIpV4Ud::sendUDP: 16feff00000000000000010060010000540001000000000054fefde66502ca7318e05a198cedfc7f80e5e2b344393e8ea2b55e41f04f267b96376500201d820a49a41fb4207a30cd5dbb8df9226f910f17aad5c5420e70e6109f4410650006c0a8c0a400ff0100000400170000[lwm2m_step:384] timeoutP: 60
[lwm2m_step:389] State: STATE_REGISTERING
[registration_getStatus:1094] State: STATE_REGISTERING
[registration_getStatus:1101] 123 status: STATE_REG_PENDING
[registration_getStatus:1125] reg_status: STATE_REG_PENDING
[observe_step:521] Entering
[registration_step:2056] State: STATE_REGISTERING
[transaction_step:460] Entering
[lwm2m_step:491] Final timeoutP: 2
[lwm2m_step:493] Final state: STATE_REGISTERING
 -> State: STATE_REGISTERING
QtIpV4Ud::rxUdpDataEvent in !
Message:  "16fefd0000000000000001005d020000510001000000000051fefd65d29761f7d13c15464b6bc9c5b577dca1a184ace3bf39031206ccd015eb2a6b208ba3b2f7e259961bcd0146b55b5a365bcd7b0b05a870c0b8995444e9553984c4c0a800000900170000ff0100010016fefd0000000000000002000c0e0000000002000000000000"  From:  "176.9.148.44" 7685
 XXXX onUdsReceive len:  131
5 received message (131 bytes), starting with 'handshake', epoch 0
5 handshake message epoch 0
5 got 'handshake' epoch 0 sequence 1 (106 bytes)
6 dtls_handle_message: FOUND PEER
6 bitfield is 1 sequence base 0 rseqn 1
6 update bitfield is 3 new sequence base 1
6 receive header: (13 bytes):
6 00000000 16 FE FD 00 00 00 00 00  00 00 01 00 5D 
6 receive unencrypted: (93 bytes):
6 00000000 02 00 00 51 00 01 00 00  00 00 00 51 FE FD 65 D2 
6 00000010 97 61 F7 D1 3C 15 46 4B  6B C9 C5 B5 77 DC A1 A1 
6 00000020 84 AC E3 BF 39 03 12 06  CC D0 15 EB 2A 6B 20 8B 
6 00000030 A3 B2 F7 E2 59 96 1B CD  01 46 B5 5B 5A 36 5B CD 
6 00000040 7B 0B 05 A8 70 C0 B8 99  54 44 E9 55 39 84 C4 C0 
6 00000050 A8 00 00 09 00 17 00 00  FF 01 00 01 00 
6 received handshake packet of type: server_hello (2)
6 handle handshake packet of type: server_hello (2)
6 add MAC data: (93 bytes): 020000510001000000000051FEFD65D29761F7D13C15464B6BC9C5B577DCA1A184ACE3BF39031206CCD015EB2A6B208B ...
5 got 'handshake' epoch 0 sequence 2 (25 bytes)
6 dtls_handle_message: FOUND PEER
6 bitfield is 3 sequence base 1 rseqn 2
6 update bitfield is 7 new sequence base 2
6 receive header: (13 bytes):
6 00000000 16 FE FD 00 00 00 00 00  00 00 02 00 0C 
6 receive unencrypted: (12 bytes):
6 00000000 0E 00 00 00 00 02 00 00  00 00 00 00 
6 received handshake packet of type: server_hello_done (14)
6 handle handshake packet of type: server_hello_done (14)
6 add MAC data: (12 bytes): 0E0000000002000000000000
[lwm2m_data_new:160] size: 1
[lwm2m_data_encode_opaque:251] length: 24
[lwm2m_data_free:178] size: 1
6 add MAC data: (12 bytes): 1000001A000200000000001A
6 add MAC data: (26 bytes): 001875726E3A696D65693A383636393031303633323338363438
6 send handshake packet of type: client_key_exchange (16)
6 send header: (13 bytes):
6 00000000 16 FE FD 00 00 00 00 00  00 00 02 00 26 
6 send unencrypted: (12 bytes):
6 00000000 10 00 00 1A 00 02 00 00  00 00 00 1A 
6 send unencrypted: (26 bytes):
6 00000000 00 18 75 72 6E 3A 69 6D  65 69 3A 38 36 36 39 30 
6 00000010 31 30 36 33 32 33 38 36  34 38 
Sending 51 bytes to [lwm2m.mudip.com]:7685
QtIpV4Ud::sendUDP: 16fefd000000000000000200261000001a000200000000001a001875726e3a696d65693a383636393031303633323338363438[lwm2m_data_new:160] size: 1
[lwm2m_data_encode_opaque:251] length: 16
[lwm2m_data_free:178] size: 1
6 psk: (16 bytes):
6 00000000 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 49 
6 client_random: (32 bytes): E66502CA7318E05A198CEDFC7F80E5E2B344393E8EA2B55E41F04F267B963765
6 server_random: (32 bytes): 65D29761F7D13C15464B6BC9C5B577DCA1A184ACE3BF39031206CCD015EB2A6B
6 pre_master_secret: (36 bytes): 0010000000000000000000000000000000000010000102030405060708090A0B0C0D0E49
6 extended_master_secret: (48 bytes): 81C1AEE61A34C289609A050904EAA119254926FB1B618BB1B417FD6273CF0491DC42F34B04A5C04D771FA5 ...
6 key_block (40 bytes):
6   client_MAC_secret: (0 bytes): 
6   server_MAC_secret: (0 bytes): 
6   client_write_key: (16 bytes): 09F18191F55AD21A67F0F9E1EDBC5BE9
6   server_write_key: (16 bytes): EA2CA2DC3EDFB6263E6FA030ADD2CD17
6   client_IV: (4 bytes): A1BF0458
6   server_IV: (4 bytes): 8854964E
6 send header: (13 bytes):
6 00000000 14 FE FD 00 00 00 00 00  00 00 03 00 01 
6 send unencrypted: (1 bytes):
6 00000000 01 
Sending 14 bytes to [lwm2m.mudip.com]:7685
QtIpV4Ud::sendUDP: 14fefd00000000000000030001016 server finished MAC: (12 bytes): E9C7F1867DB18BCB7689564B
6 add MAC data: (12 bytes): 1400000C000300000000000C
6 add MAC data: (12 bytes): E9C7F1867DB18BCB7689564B
6 send handshake packet of type: finished (20)
6 dtls_prepare_record(): encrypt using TLS_PSK_WITH_AES_128_CCM_8
6 nonce:: (16 bytes): A1BF0458000100000000000000000000
6 key:: (16 bytes): 09F18191F55AD21A67F0F9E1EDBC5BE9
6 message:: (40 bytes): 00010000000000000537909EA4DCFDCBBF5A898DDDD921B92E37D18FA8EF2B032180DAC9493CA79D
6 send header: (13 bytes):
6 00000000 16 FE FD 00 01 00 00 00  00 00 00 00 28 
6 send unencrypted: (12 bytes):
6 00000000 14 00 00 0C 00 03 00 00  00 00 00 0C 
6 send unencrypted: (12 bytes):
6 00000000 E9 C7 F1 86 7D B1 8B CB  76 89 56 4B 
Sending 53 bytes to [lwm2m.mudip.com]:7685
QtIpV4Ud::sendUDP: 16fefd0001000000000000002800010000000000000537909ea4dcfdcbbf5a898dddd921b92e37d18fa8ef2b032180dac9493ca79d[lwm2m_step:384] timeoutP: 60
[lwm2m_step:389] State: STATE_REGISTERING
[registration_getStatus:1094] State: STATE_REGISTERING
[registration_getStatus:1101] 123 status: STATE_REG_PENDING
[registration_getStatus:1125] reg_status: STATE_REG_PENDING
[observe_step:521] Entering
[registration_step:2056] State: STATE_REGISTERING
[transaction_step:460] Entering
[lwm2m_step:491] Final timeoutP: 2
[lwm2m_step:493] Final state: STATE_REGISTERING
 -> State: STATE_REGISTERING
QtIpV4Ud::rxUdpDataEvent in !
Message:  "14fefd000000000000000300010116fefd000100000000000000280001000000000000d445901218981cd0c9959f2ef6c28c6bdd77c6a07df325d2c42a06f1f18e00f2"  From:  "176.9.148.44" 7685
 XXXX onUdsReceive len:  67
5 received message (67 bytes), starting with 'change_cipher_spec', epoch 0
5 got 'change_cipher_spec' epoch 0 sequence 3 (14 bytes)
6 dtls_handle_message: FOUND PEER
6 bitfield is 7 sequence base 2 rseqn 3
6 update bitfield is f new sequence base 3
6 receive header: (13 bytes):
6 00000000 14 FE FD 00 00 00 00 00  00 00 03 00 01 
6 receive unencrypted: (1 bytes):
6 00000000 01 
5 got 'handshake' epoch 1 sequence 0 (53 bytes)
6 dtls_handle_message: FOUND PEER
6 bitfield is 0 sequence base 0 rseqn 0
6 nonce: (16 bytes): 8854964E000100000000000000000000
6 key: (16 bytes): EA2CA2DC3EDFB6263E6FA030ADD2CD17
6 ciphertext: (32 bytes): D445901218981CD0C9959F2EF6C28C6BDD77C6A07DF325D2C42A06F1F18E00F2
6 decrypt_verify(): found 24 bytes cleartext
6 cleartext: (24 bytes): 1400000C000300000000000C6F835E792D74D8014E9E38E0
6 init bitfield is 1 sequence base 0
6 receive header: (13 bytes):
6 00000000 16 FE FD 00 01 00 00 00  00 00 00 00 28 
6 receive unencrypted: (24 bytes):
6 00000000 14 00 00 0C 00 03 00 00  00 00 00 0C 6F 83 5E 79 
6 00000010 2D 74 D8 01 4E 9E 38 E0  
6 received handshake packet of type: finished (20)
6 handle handshake packet of type: finished (20)
6 d:: (12 bytes): 6F835E792D74D8014E9E38E0
6 v:: (12 bytes): 6F835E792D74D8014E9E38E0
6 Handshake complete
[lwm2m_step:384] timeoutP: 60
[lwm2m_step:389] State: STATE_REGISTERING
[registration_getStatus:1094] State: STATE_REGISTERING
[registration_getStatus:1101] 123 status: STATE_REG_PENDING
[registration_getStatus:1125] reg_status: STATE_REG_PENDING
[observe_step:521] Entering
[registration_step:2056] State: STATE_REGISTERING
[transaction_step:460] Entering
[lwm2m_step:491] Final timeoutP: 2
[lwm2m_step:493] Final state: STATE_REGISTERING
 -> State: STATE_REGISTERING
[lwm2m_step:384] timeoutP: 2
[lwm2m_step:389] State: STATE_REGISTERING
[registration_getStatus:1094] State: STATE_REGISTERING
[registration_getStatus:1101] 123 status: STATE_REG_PENDING
[registration_getStatus:1125] reg_status: STATE_REG_PENDING
[observe_step:521] Entering
[registration_step:2056] State: STATE_REGISTERING
[transaction_step:460] Entering
[transaction_send:373] Entering: transaction=00000163EC8468A0
6 dtls_prepare_record(): encrypt using TLS_PSK_WITH_AES_128_CCM_8
6 nonce:: (16 bytes): A1BF0458000100000000000100000000
6 key:: (16 bytes): 09F18191F55AD21A67F0F9E1EDBC5BE9
6 message:: (220 bytes): 0001000000000001B5B9386D2C7C6C225D759C40D39933B67B48DABE8C5E87FAA78DA0D598E8E200BBD7088ED2EDB589210 ...
6 send header: (13 bytes):
6 00000000 17 FE FD 00 01 00 00 00  00 00 01 00 DC 
6 send unencrypted: (204 bytes):
6 00000000 44 02 05 0A 0A 05 62 97  B2 72 64 11 28 39 6C 77 
6 00000010 6D 32 6D 3D 31 2E 31 0D  0E 65 70 3D 75 72 6E 3A 
6 00000020 69 6D 65 69 3A 38 36 36  39 30 31 30 36 33 32 33 
6 00000030 38 36 34 38 03 62 3D 55  06 6C 74 3D 33 30 30 FF 
6 00000040 3C 2F 3E 3B 72 74 3D 22  6F 6D 61 2E 6C 77 6D 32 
6 00000050 6D 22 3B 63 74 3D 31 31  30 2C 3C 2F 31 3E 3B 76 
6 00000060 65 72 3D 31 2E 31 2C 3C  2F 31 2F 30 3E 2C 3C 2F 
6 00000070 32 2F 30 3E 2C 3C 2F 33  2F 30 3E 2C 3C 2F 34 2F 
6 00000080 30 3E 2C 3C 2F 35 2F 30  3E 2C 3C 2F 36 2F 30 3E 
6 00000090 2C 3C 2F 37 2F 30 3E 2C  3C 2F 33 31 30 32 34 3E 
6 000000A0 3B 76 65 72 3D 31 2E 30  2C 3C 2F 33 31 30 32 34 
6 000000B0 2F 31 30 3E 2C 3C 2F 33  31 30 32 34 2F 31 31 3E 
6 000000C0 2C 3C 2F 33 31 30 32 34  2F 31 32 3E 
Sending 233 bytes to [lwm2m.mudip.com]:7685
QtIpV4Ud::sendUDP: 17fefd000100000000000100dc0001000000000001b5b9386d2c7c6c225d759c40d39933b67b48dabe8c5e87faa78da0d598e8e200bbd7088ed2edb589210a79ef1a408d058a500f864b08d38cd381793e6929a74bcb663e75c3ef90c433692fa009679650248020dc4d4b697d3fa2a5002726a60c00e374668bd69209eabe631cf7e54f6561bcac0abd312f1fd1b73195c381ee199468aeb2b76c0bdaaa921a67f44526f3e9e946d4af5fc09dc128dc1a1f0fa172fea7aeed4cf7276848582ab664c51a4f7bfdd8a799e7580741d2cc28224736b93c6c1dc496daa47cd9f4fd5ee5a0b04efdac18db[lwm2m_step:491] Final timeoutP: 1
[lwm2m_step:493] Final state: STATE_REGISTERING
 -> State: STATE_REGISTERING
QtIpV4Ud::rxUdpDataEvent in !
Message:  "17fefd000100000000000100260001000000000001d874dc9cfc1f696c1a461cf22f006a2a2405a5bfebb654536cb7d3b908ff"  From:  "176.9.148.44" 7685
 XXXX onUdsReceive len:  51
5 received message (51 bytes), starting with 'application_data', epoch 1
5 got 'application_data' epoch 1 sequence 1 (51 bytes)
6 dtls_handle_message: FOUND PEER
6 bitfield is 1 sequence base 0 rseqn 1
6 nonce: (16 bytes): 8854964E000100000000000100000000
6 key: (16 bytes): EA2CA2DC3EDFB6263E6FA030ADD2CD17
6 ciphertext: (30 bytes): D874DC9CFC1F696C1A461CF22F006A2A2405A5BFEBB654536CB7D3B908FF
6 decrypt_verify(): found 22 bytes cleartext
6 cleartext: (22 bytes): 6441050A0A0562978272640A794C7157533745776D69
6 update bitfield is 3 new sequence base 1
6 receive header: (13 bytes):
6 00000000 17 FE FD 00 01 00 00 00  00 00 01 00 26 
6 receive unencrypted: (22 bytes):
6 00000000 64 41 05 0A 0A 05 62 97  82 72 64 0A 79 4C 71 57 
6 00000010 53 37 45 77 6D 69 
5 ** application data:
[lwm2m_handle_packet:488] Entering
[lwm2m_handle_packet:496] Parsed: ver 1, type 2, tkl 4, code 2.01, mid 1290, Content type: 0
[lwm2m_handle_packet:497] Payload: 
[transaction_handleResponse:296] Entering
[prv_handleRegistrationReply:670] 123 Registration successful
[lwm2m_data_new:160] size: 1
[lwm2m_data_free:178] size: 1
[transaction_remove:282] Entering. transaction=00000163EC8468A0
[transaction_free:258] Entering. transaction=00000163EC8468A0
[lwm2m_step:384] timeoutP: 60
[lwm2m_step:389] State: STATE_REGISTERING
[registration_getStatus:1094] State: STATE_REGISTERING
[registration_getStatus:1101] 123 status: STATE_REGISTERED
[registration_getStatus:1125] reg_status: STATE_REGISTERED
[observe_step:521] Entering
[registration_step:2056] State: STATE_READY
[transaction_step:460] Entering
[lwm2m_step:491] Final timeoutP: 60
[lwm2m_step:493] Final state: STATE_READY
 -> State: STATE_READY