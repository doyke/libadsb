#!/usr/bin/env python

import logging

# Useful binary/dec/hex convert tool
# http://home.paulschou.net/tools/xlate/

# Sniffed message
# 8d42405058af749269b978081a0a
#adsb = [0x8d, 0x42, 0x40, 0x50, 0x58, 0xaf, 0x74, 0x92, 0x69, 0xb9, 0x78]

# H4x0r crafted messages
# 8ddeadbf58af749269b978a95724
#adsb = [0x8d, 0xde, 0xad, 0xbf, 0x58, 0xaf, 0x74, 0x92, 0x69, 0xb9, 0x78]
# 8dcafebb58af749269b9783949e0
#adsb = [0x8d, 0xca, 0xfe, 0xbb, 0x58, 0xaf, 0x74, 0x92, 0x69, 0xb9, 0x78]
# 8db000b558af749269b9782cec6b
#adsb = [0x8d, 0xb0, 0x00, 0xb5, 0x58, 0xaf, 0x74, 0x92, 0x69, 0xb9, 0x78]
# 8d31337058af749269b9787117c7
#adsb = [0x8d, 0x31, 0x33, 0x70, 0x58, 0xaf, 0x74, 0x92, 0x69, 0xb9, 0x78]

# BUG: in PG PlanePlotter, messages with ICAO24 starting with 0 are not displayed
# 8d03133758af749269b978f4e3ab

# Test vectors - 112 bits
adsb_112bits_tests = []
adsb_112bits_tests.append([0x8d, 0x42, 0x40, 0x50, 0x58, 0xaf, 0x74, 0x92, 0x69, 0xb9, 0x78, 0x081a0a])
adsb_112bits_tests.append([0x8d, 0xde, 0xad, 0xbf, 0x58, 0xaf, 0x74, 0x92, 0x69, 0xb9, 0x78, 0xa95724])
adsb_112bits_tests.append([0x8d, 0xca, 0xfe, 0xbb, 0x58, 0xaf, 0x74, 0x92, 0x69, 0xb9, 0x78, 0x3949e0])
adsb_112bits_tests.append([0x8d, 0xb0, 0x00, 0xb5, 0x58, 0xaf, 0x74, 0x92, 0x69, 0xb9, 0x78, 0x2cec6b])
adsb_112bits_tests.append([0x8d, 0x31, 0x33, 0x70, 0x58, 0xaf, 0x74, 0x92, 0x69, 0xb9, 0x78, 0x7117c7])

# Test vectors - 56 bits
# http://www.vk2tvk.org/Wireless/Franks/AvTrans.html
# 5da5db4ef5f740
# 0 1 0 1 1   1 0 1   1 0 1 0 0 1 0 1 1 1 0 1 1 0 1 1 0 1 0 0 1 1 1 0   1 1 1 1 0 1 0 1 1 1 1 1 0 1 1 1 0 1 0 0 0 0 0 0  
adsb_56bits_tests = []
adsb_56bits_tests.append([0x5d, 0xa5, 0xdb, 0x4e, 0xf5f740])

def adsb_112bits_crc(adsb_payload_11_bytes):
    POLY = 0xFFFA0480

    data = \
        (adsb_payload_11_bytes[0] << 24) | \
        (adsb_payload_11_bytes[1] << 16) | \
        (adsb_payload_11_bytes[2] <<  8) | \
        (adsb_payload_11_bytes[3] <<  0)

    data1 = \
        (adsb_payload_11_bytes[4] << 24) | \
        (adsb_payload_11_bytes[5] << 16) | \
        (adsb_payload_11_bytes[6] <<  8) | \
        (adsb_payload_11_bytes[7] <<  0)

    data2 = \
        (adsb_payload_11_bytes[8] << 24) | \
        (adsb_payload_11_bytes[9] << 16) | \
        (adsb_payload_11_bytes[10] <<  8)

    logging.info('init dataX', hex(data), hex(data1), hex(data2))

    result = 0x00000000

    for i in range(0, 88):
        logging.info('data', hex(data))
        if (data & 0x80000000) <> 0:
            data = data ^ POLY
            logging.info('data (if)', hex(data))

        data = data << 1
        logging.info('data (<<)', hex(data))

        logging.info('data1', hex(data1))
        if (data1 & 0x80000000) <> 0:
            data = data | 1
            logging.info('data (if)', hex(data))

        data1 = data1 << 1
        logging.info('data1 (<<)', hex(data1))

        logging.info('data2', hex(data2))
        if (data2 & 0x80000000) <> 0:
            data1 = data1 | 1
            logging.info('data1 (if)', hex(data1))

        data2 = data2 << 1
        logging.info('data2 (<<)', hex(data2))

    result = result ^ data
    logging.debug(hex(data >> 8), hex(result >> 8))
    
    return result >> 8

def adsb_56bits_crc(adsb_payload_4_bytes):
    POLY = 0xFFFA0480

    data = \
        (adsb_payload_4_bytes[0] << 24) | \
        (adsb_payload_4_bytes[1] << 16) | \
        (adsb_payload_4_bytes[2] <<  8) | \
        (adsb_payload_4_bytes[3] <<  0)

    logging.info('init dataX', hex(data))

    result = 0x00000000

    for i in range(0, 32):
        logging.info('data', hex(data))
        if (data & 0x80000000) <> 0:
            data = data ^ POLY
            logging.info('data (if)', hex(data))

        data = data << 1
        logging.info('data (<<)', hex(data))

    result = result ^ data
    logging.debug(hex(data >> 8), hex(result >> 8))
    
    return result >> 8


def main():
    logging.basicConfig(level=logging.DEBUG)
    
    for adsb_msg in adsb_112bits_tests:
        res = adsb_112bits_crc(adsb_msg[0:11])
        ver =  adsb_msg[11]
        if res <> ver:
            print 'FAIL', hex(res), hex(ver)
        else:
            print 'PASS', hex(res), hex(ver)

    for adsb_msg in adsb_56bits_tests:
        res = adsb_56bits_crc(adsb_msg[0:4])
        ver =  adsb_msg[4]
        if res <> ver:
            print 'FAIL', hex(res), hex(ver)
        else:
            print 'PASS', hex(res), hex(ver)

if __name__ == "__main__" :
    main()

