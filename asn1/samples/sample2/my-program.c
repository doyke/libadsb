#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include "MyPacket.h"
 
 
 
#define INTARRAYSIZE 10
#define MAXPKTSIZE   256
 
 
typedef uint8_t u8;
 
/* Create a structure that matches the main ASN.1 declaration */
 
typedef struct MyPacket_ {
        u8              myFlags;
        u8              myInts[INTARRAYSIZE];
        char           *myName;
 
} MyPacket;
 
u8              Encode_DER_Packet(MyPacket * Pkt, u8 * buf);
u8              Decode_DER_Packet(u8 * buf, MyPacket * Pkt);
void print_byte(u8 byte);
 
int
main()
{
        MyPacket        Pkt,Pkt2;
        u8              buf[MAXPKTSIZE];
        int             i;
 
        /*
         * Fill in a packet
         */
 
        for (i = 0; i < INTARRAYSIZE; i++)
                Pkt.myInts[i] = i * 11 + 5;
 
        /*
         * Set the first, second and 4th bits
         */
 
        Pkt.myFlags = 0;
        Pkt.myFlags |= (1 << 0);
        Pkt.myFlags |= (1 << 1);
        Pkt.myFlags |= (1 << 3);
 
        /*
         * Set the myName string
         */
 
        Pkt.myName = calloc(1, 256);
        strcpy(Pkt.myName, "Mister Z");
 
        /*
         * Encode Pkt into DER
         */
 
        i = Encode_DER_Packet(&Pkt, buf);
 
        /*
         * Decode DER data to Pkt2
         */
 
        i = Decode_DER_Packet(buf,&Pkt2);
 
        /*
         * Display contents of Pkt2
         */
 
        printf("\n\nStructure Pkt2: \n");
        printf("\nMyFlags : ");
        print_byte(Pkt2.myFlags);
        printf("\nMyName  : %s", Pkt2.myName);
        printf("\nMyInts  : ");
        for (i=0;i<INTARRAYSIZE;i++)
                printf("%d ",Pkt2.myInts[i]);
 
        printf("\n");
 
        if (Pkt.myName)
          free (Pkt.myName);
 
        if (Pkt2.myName)
          free (Pkt2.myName);
 
        return 0;
}
 
/*
 * Encodes a MyPacket structure into DER data
 * Returns the size of the resulting DER data
 */
 
u8
Encode_DER_Packet(MyPacket * Pkt, u8 * buf)
{
 
        u8              oid[] = { 1, 3, 6, 1, 5, 9, 0, 0, 0, 0 };
 
        /*
         * MyPacket_t and MyInt_t declared in MyPacket.h and MyInt.h
         */
 
        MyPacket_t     *myPacket;
 
        MyInt_t        *myInt;
        asn_enc_rval_t  er;     /* Encoder return value */
 
        u8              ret, i;
 
        myPacket = calloc(1, sizeof *myPacket);
 
        /*
         * Fill in myObjectId so that the other end knows which
         * protocol is this
         */
 
        ret = OBJECT_IDENTIFIER_set_arcs(&myPacket->myObjectId, oid,
                                         sizeof(oid[0]),
                                         sizeof(oid) / sizeof(oid[0]));
 
        if (ret != 0)
                goto fail;
 
        /*
         * Fill in myInts
         */
 
 
        for (i = 0; i < INTARRAYSIZE; i++)
          {
                  myInt = calloc(1, sizeof *myInt);
                  *myInt = Pkt->myInts[i];
                  ret = ASN_SEQUENCE_ADD(&myPacket->myInts, myInt);
                  if (ret != 0)
                          goto fail;
          }
 
        /*
         * Fill in myName
         */
 
        myPacket->myName.buf = calloc(1, strlen(Pkt->myName) + 1);
        strcpy(myPacket->myName.buf, Pkt->myName);
        myPacket->myName.size = strlen(Pkt->myName);
 
        /*
         * Fill in myFlags
         */
 
        myPacket->myFlags.buf = calloc(1, 1);
        myPacket->myFlags.size = 1 /* 1 byte */ ;
 
        for (i = 0; i < 8; i++)
                if (Pkt->myFlags & (1 << i))
                        myPacket->myFlags.buf[0] |= (1 << (7 - i));
 
        /*
         * Encode to DER and place result in buf
         */
 
        er = der_encode_to_buffer(&asn_DEF_MyPacket, myPacket, buf,
                                  MAXPKTSIZE);
        if (er.encoded == -1)
          {
                  fprintf(stderr, "Cannot encode %s: %s\n",
                          er.failed_type->name, strerror(errno));
                  goto fail;
          }
        else
          {
                  printf("Structure successfully encoded\n");
                  /*xer_fprint(stdout, &asn_DEF_MyPacket, myPacket);*/
 
                  asn_DEF_MyPacket.free_struct (&asn_DEF_MyPacket, myPacket, 0);
                  return er.encoded;
          }
 
      fail:
        fprintf(stderr, "Cannot encode %s: %s\n", er.failed_type->name, strerror(errno));
        asn_DEF_MyPacket.free_struct(&asn_DEF_MyPacket, myPacket, 0);
        return -1;
 
}
 
/*
 * Decodes a DER encoded data into a MyPacket structure
 * Returns the size of the resulting MyPacket structure
 */
 
u8
Decode_DER_Packet(u8 * buf, MyPacket * Pkt)
{
 
        u8             *oid;
        u8              oid_size;
 
        /*
         * MyPacket_t and MyInt_t declared in MyPacket.h and MyInt.h
         */
 
        MyPacket_t     *myPacket = 0;
        MyInt_t        *myInt;
        asn_dec_rval_t  er;     /* decoder return value */
        u8              ret, i;
 
        /*
         * Decode the buffer into the myPacket structure
         * The asn1c compiler provides the generic BER decoder which
         * is implicitly capable of decoding BER, CER and DER encoded data.
         */
 
        er = ber_decode(0, &asn_DEF_MyPacket, (void **) &myPacket, buf,
                        MAXPKTSIZE);
 
        if (er.code != RC_OK)
                goto fail;
        else
          {
                  printf("Structure successfully decoded\n");
                  /*xer_fprint(stdout, &asn_DEF_MyPacket, myPacket);*/
          }
 
        /*
         * Extract the OBJECT_IDENTIFIER into oid / oid_size
         */
 
        oid_size = OBJECT_IDENTIFIER_get_arcs(&myPacket->myObjectId, 0,
                                         sizeof(oid[0]), 0);
        oid = malloc(oid_size * sizeof(oid[0]));
 
	if (oid) {
	    (void) OBJECT_IDENTIFIER_get_arcs(&myPacket->myObjectId, oid,
					      sizeof(oid[0]), oid_size);
	    free (oid);
	    oid = NULL;
	}
 
        /*
         * Extract MyInts
         */
 
        for (i = 0; i < myPacket->myInts.list.count; i++)
                Pkt->myInts[i] = *myPacket->myInts.list.array[i];
 
        /*
         * Extract MyFlags
         */
 
        Pkt->myFlags = 0;
 
        for (i = 0; i < 8; i++)
                if (myPacket->myFlags.buf[0] & (1 << i))
                        Pkt->myFlags |= (1 << (7 - i));
 
        /*
         * Extract myName
         */
 
        Pkt->myName = calloc(1, myPacket->myName.size + 1);
        strcpy(Pkt->myName, myPacket->myName.buf);
        asn_DEF_MyPacket.free_struct (&asn_DEF_MyPacket, myPacket, 0);
 
        return er.consumed;
 
      fail:
        fprintf(stderr, "Decode failure %s\n", strerror(errno));
        asn_DEF_MyPacket.free_struct(&asn_DEF_MyPacket, myPacket, 0);
        return -1;
 
}
 
void print_byte(u8 byte){
 
        u8 i;
        for (i=0; i<8;i++)
 
                if (byte & (1 << i) )
                        printf("1");
                else
                        printf("0");
}

