// Test harness for kirk engine
// From: http://code.google.com/p/kirk-engine/source/browse/trunk/test_harness/main.c

#include <common.h>
#include <pspkernel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "../libkirk/kirk_engine.h"
#include "kirk_engine.h"

unsigned char sample_kirk1_ecdsa[320] =
{
    0x10, 0x6D, 0x65, 0x5D, 0xF7, 0xB4, 0xC0, 0x41, 0x5D, 0xAB, 0x17, 0x3C, 0xAE, 0x6D, 0xD8, 0xF2, 
    0x66, 0x4F, 0xE1, 0xF2, 0xE9, 0xD6, 0x63, 0x36, 0xF7, 0x33, 0x0B, 0xCA, 0xB9, 0x55, 0x6D, 0xB6, 
    0xEB, 0xE8, 0x05, 0xDC, 0xF5, 0x57, 0xE2, 0xF8, 0xC8, 0x1F, 0xD9, 0x5C, 0xB6, 0x0B, 0x60, 0x1B, 
    0xF0, 0x86, 0x2D, 0xDB, 0x1F, 0xCB, 0x4E, 0xAF, 0xCD, 0xE3, 0x88, 0xA6, 0x3C, 0x1D, 0x57, 0xDC, 
    0x5E, 0x94, 0xEE, 0xAC, 0x2E, 0x6C, 0x9F, 0x2E, 0x81, 0xC7, 0x1C, 0x58, 0x3A, 0xF1, 0x81, 0x74, 
    0xCB, 0xB5, 0xC6, 0x10, 0x4B, 0x09, 0xD1, 0x79, 0x08, 0xB8, 0xB7, 0x44, 0x92, 0x08, 0x5C, 0x23, 
    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x26, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x7E, 0x50, 0x53, 0x50, 0x07, 0x10, 0x01, 0x00, 0x08, 0x01, 0x65, 0x78, 0x61, 0x6D, 0x70, 0x6C, 
    0x65, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xEE, 0x92, 0x8E, 0xAE, 0x50, 0xCA, 0x4E, 0x01, 0x19, 0x9C, 0x1C, 0xA6, 0x90, 0xB8, 0x61, 0x70, 
    0x0A, 0x95, 0x54, 0x57, 0x8F, 0xC5, 0xEF, 0x2C, 0xA2, 0x49, 0xA6, 0xB5, 0x08, 0xDF, 0x8C, 0x78, 
    0x2E, 0x38, 0x94, 0x28, 0x9B, 0xE1, 0x1F, 0xC4, 0xCE, 0x61, 0xDB, 0x32, 0x0B, 0x5E, 0x05, 0xF0, 
} ;

//hex_dump("KIRK 11 buff", rndbig,0x24);
void hex_dump(char* description, u8* data_ptr, int data_size) {
	int n;
	printf("%s: ", description);
	for (n = 0; n < data_size; n++) {
		printf("%02X ", data_ptr[n]);
	}
	printf("\n");
}

u8 rnd[0x14];
u8 rndbig[0x80];
int res;

void test_sha1() {
	u8 correct_sha1[0x14] = { 0xDE, 0x8A, 0x84, 0x7B, 0xFF, 0x8C, 0x34, 0x3D, 0x69, 0xB8, 0x53, 0xA2, 0x15, 0xE6, 0xEE, 0x77, 0x5E,0xF2, 0xEF, 0x96 };

	memset(rnd,0,0x14);
	memset(rndbig,0,0x80);
	rndbig[0]= 0x20;
	hex_dump("KIRK 11 buff", rndbig,0x24);
	
	// 4 bytes header
	// +
	// 32 times: \0
	res = sceUtilsBufferCopyWithRange(rnd, 0x14, rndbig, (4) + 0x20, KIRK_CMD_SHA1_HASH);
	hex_dump("SHA1 Result", rnd,0x14);
	hex_dump("SHA1 Expected", correct_sha1, 0x14);
	if (memcmp(rnd,correct_sha1, 0x14) == 0) {
		printf("SHA1 of 0x20 00s is correct!\n");
	} else {
		printf("SHA1 failed! (0x%x)\n", res);
	}
}

int main(int argc, char *argv[])
{
	u8 keypair[0x3C];
	u8 newpoint[0x28];
	u8 mult_test[0x3c];
	u8 test17_fullsig[0x64];
	KIRK_CMD16_BUFFER ecdsa_sign;
	ECDSA_SIG signature;
	KIRK_CMD17_BUFFER ecdsa_test;
	// My private RIF
	
	// From NPEH90049 Demo NPUMDIMG Header SHA1 of 0xD8 bytes
	u8 test17_hash[0x14]= { 0x2C, 0x39, 0xC1, 0x46, 0x22, 0xD5, 0x55, 0x02, 0x3A, 0x03, 0xB1, 0x2D, 0x17, 0x00, 0x00, 0x36, 0x8C, 0x28, 0xBD, 0x50 };

	// From NPEH90049 Demo NPUMDIMG Header at offset 0xD8
	u8 test17_sig[0x28] = {
		0x4B, 0xBC, 0xBC, 0xB5, 0x01, 0x70, 0xCD, 0x23,
		0x20, 0x6F, 0x51, 0x9A, 0xBE, 0xD7, 0xD8, 0xCC,
		0x04, 0x56, 0x4C, 0x9E, 0x17, 0xE0, 0x1E, 0x2E,
		0x63, 0x12, 0x38, 0x60, 0x58, 0x0B, 0x21, 0x84,
		0x9F, 0x52, 0x13, 0xF1, 0x31, 0x2C, 0x6A, 0xBC
	};

	// Public NPUMDIMG Key from np9660.prx
	u8 rif_public[0x28] = {
		0x01, 0x21, 0xEA, 0x6E, 0xCD, 0xB2, 0x3A, 0x3E,
		0x23, 0x75, 0x67, 0x1C, 0x53, 0x62, 0xE8, 0xE2,
		0x8B, 0x1E, 0x78, 0x3B, 0x1A, 0x27, 0x32, 0x15,
		0x8B, 0x8C, 0xED, 0x98, 0x46, 0x6C, 0x18, 0xA3,
		0xAC, 0x3B, 0x11, 0x06, 0xAF, 0xB4, 0xEC, 0x3B
	};

	printf("Starting Test Harness...\n");
	
	// In the real world, you should use a secure way of generated a nice chunk of random data.
	// There are good OS-specific ways available on each platform normally. Use those!
	//
	// The two values for the fuse id can be grabbed from your personal device
	// by reading BC100090 for the first value and BC100094 for the second value.
	// Process them as u32 so the endian order stays correct.
	//kirk_init2((u8*)"This is my test seed",20,0x12345678, 0xabcd );
	//kirk_init();

	test_sha1();

	// Test Random Generator
	printf("\nGenerating 2 random numbers...\n");
	sceUtilsBufferCopyWithRange(rndbig, 0x77, 0, 0, KIRK_CMD_PRNG);
	hex_dump("Big Random Number", rndbig, 0x77);
			
	sceUtilsBufferCopyWithRange(rnd,0x14,0,0,KIRK_CMD_PRNG);
	hex_dump("Random Number", rnd, 0x14);
	
	// Test Key Pair Generator
	printf("\nGenerating a new ECDSA keypair...\n");
	sceUtilsBufferCopyWithRange(keypair,0x3C,0,0,KIRK_CMD_ECDSA_GEN_KEYS);
	hex_dump("Private Key", keypair, 0x14);
	hex_dump("Public Key", keypair+0x14, 0x28);
	
	// Test Point Multiplication
	printf("\nMultiplying the Public Key by the Random Number...\n");
	memcpy(mult_test,rnd,0x14);
	memcpy(mult_test+0x14,keypair+0x14,0x28);
	sceUtilsBufferCopyWithRange(newpoint,0x28,mult_test,0x3C,KIRK_CMD_ECDSA_MULTIPLY_POINT);
	hex_dump("New point", newpoint, 0x28);
	
	printf("Testing a known valid ECDSA signature...\n");
	memcpy(test17_fullsig, rif_public,0x28);
	memcpy(test17_fullsig+0x28, test17_hash,0x14);
	memcpy(test17_fullsig+0x3C, test17_sig,0x28);
	res=sceUtilsBufferCopyWithRange(0,0,test17_fullsig,0x64,KIRK_CMD_ECDSA_VERIFY);
	printf("Signature check returned %d\n", res);
	if(res) {
			printf("Signature FAIL!\n");
	} else {
			printf("Signature VALID!\n");
	}
	printf("\nTesting ECDSA signing with ECDSA key pair...\n");
	
	// @TODO! Reenable.
	//encrypt_kirk16_private(ecdsa_sign.enc_private,keypair);
	
	hex_dump("Encrypted Private", ecdsa_sign.enc_private, 0x20);
	//Test with a message hash of all 00s
	memset(ecdsa_sign.message_hash,0,0x14);
	sceUtilsBufferCopyWithRange(signature.r,0x28,ecdsa_sign.enc_private,0x34,KIRK_CMD_ECDSA_SIGN);
	
	printf("\nChecking signature and Message hash...\n");
	hex_dump("Signature R", signature.r, 0x14);
	hex_dump("Signature S", signature.s, 0x14);
	hex_dump("Message hash", ecdsa_sign.message_hash,0x14);
	
	printf("\nUsing Public key...\n");
	hex_dump("Public.x", keypair+0x14,0x14);
	hex_dump("Public.y", keypair+0x28,0x14);
	// Build ecdsa verify message block
	memcpy(ecdsa_test.public_key.x,keypair+0x14,0x14);
	memcpy(ecdsa_test.public_key.y,keypair+0x28,0x14);
	memcpy(ecdsa_test.message_hash,ecdsa_sign.message_hash,0x14);
	memcpy(ecdsa_test.signature.r,signature.r,0x14);
	memcpy(ecdsa_test.signature.s,signature.s,0x14);
	
	res=sceUtilsBufferCopyWithRange(0,0,(u8*)ecdsa_test.public_key.x,0x64,KIRK_CMD_ECDSA_VERIFY);
	printf("Signature check returned %d\n", res);
	if (res) {
		printf("Signature FAIL!\n");
	} else {
		printf("Signature VALID!\n");
	}       
	printf("\nTesting Kirk1 ECDSA Verification and Decryption...\n");
	res=sceUtilsBufferCopyWithRange(sample_kirk1_ecdsa,320,(u8*)sample_kirk1_ecdsa,320,KIRK_CMD_DECRYPT_PRIVATE);
	printf("Signature check returned %d\n", res);
	if (res) {
		printf("Signature FAIL!\n");
	} else {
		printf("Signature VALID!\n");
	}
	printf("%s\n",sample_kirk1_ecdsa);
	return 0;
}