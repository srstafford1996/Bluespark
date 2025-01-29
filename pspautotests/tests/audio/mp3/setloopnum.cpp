#include <common.h>
#include <pspiofilemgr.h>
#include <pspmp3.h>
#include <psputility.h>
#include <psputils.h>

extern "C" int sceMp3LowLevelInit(int handle, int unk);
extern "C" int sceMp3LowLevelDecode(int handle, const void *src, int *srcConsumed, short *samples, int *sampleBytesWritten);

static u8 mp3Buf[8192] __attribute__((aligned(64)));
static short pcmBuf[4608] __attribute__((aligned(64)));

static int initHeader(int handle, const u8 *header, int size, int offset = 0) {
	u8 *dst = NULL;
	int result = sceMp3GetInfoToAddStreamData(handle, &dst, NULL, NULL);
	if (result < 0) {
		sceMp3ReleaseMp3Handle(handle);
		return result;
	}

	memcpy(dst + offset, header, size);
	sceKernelDcacheWritebackInvalidateRange(mp3Buf, sizeof(mp3Buf));

	return sceMp3Init(handle);
}

static int initHeader(SceMp3InitArg *mp3Init, const u8 *header, int size, int offset = 0) {
	memset(mp3Buf, 0, sizeof(mp3Buf));
	int handle = sceMp3ReserveMp3Handle(mp3Init);
	if (handle < 0) {
		return handle;
	}

	int result = initHeader(handle, header, size, offset);
	if (result < 0) {
		sceMp3ReleaseMp3Handle(handle);
		return result;
	}
	return handle;
}

static void testSetLoopNum(const char *title, int handle, int value) {
	int result = sceMp3SetLoopNum(handle, value);
	if (result >= 0) {
		int loop = sceMp3GetLoopNum(handle);
		checkpoint("%s: %08x (%d)", title, result, loop);
	} else {
		checkpoint("%s: %08x", title, result);
	}
}

extern "C" int main(int argc, char *argv[]) {
	sceUtilityLoadModule(PSP_MODULE_AV_AVCODEC);
	sceUtilityLoadModule(PSP_MODULE_AV_MP3);

	sceMp3InitResource();

	int fd = sceIoOpen("sample.mp3", PSP_O_RDONLY, 0777);

	short *out = NULL;
	int handle;

	SceMp3InitArg mp3Init;
	mp3Init.mp3StreamStart = 0;
	mp3Init.mp3StreamEnd = sceIoLseek32(fd, 0, SEEK_END);
	mp3Init.unk1 = 0;
	mp3Init.unk2 = 0;
	mp3Init.mp3Buf = mp3Buf;
	mp3Init.mp3BufSize = sizeof(mp3Buf);
	mp3Init.pcmBuf = pcmBuf;
	mp3Init.pcmBufSize = sizeof(pcmBuf);

	checkpointNext("Handles");
	testSetLoopNum("  Unreserved", 0, 0);
	handle = sceMp3ReserveMp3Handle(NULL);
	testSetLoopNum("  NULL info arg", handle, 0);
	sceMp3ReleaseMp3Handle(handle);
	handle = sceMp3ReserveMp3Handle(&mp3Init);
	testSetLoopNum("  Basic info arg", handle, 0);
	sceMp3ReleaseMp3Handle(handle);
	testSetLoopNum("  Negative", -1, 0);
	testSetLoopNum("  2", 2, 0);

	static const u8 baseHeader[] = {
		0xFF, 0xFB, 0x10, 0x00,
	};

	checkpointNext("After init");
	handle = sceMp3ReserveMp3Handle(NULL);
	sceMp3LowLevelInit(handle, 0);
	testSetLoopNum("  Low level init", handle, 0);
	sceIoLseek32(fd, 0, SEEK_SET);
	sceIoRead(fd, mp3Buf, 4096);
	int consumed, written;
	sceMp3LowLevelDecode(handle, mp3Buf, &consumed, pcmBuf, &written);
	testSetLoopNum("  Low level decode", handle, 0);
	sceMp3ReleaseMp3Handle(handle);
	handle = sceMp3ReserveMp3Handle(&mp3Init);
	sceMp3SetLoopNum(handle, 0);
	initHeader(handle, baseHeader, sizeof(baseHeader));
	testSetLoopNum("  After set then init", handle, 0);
	sceMp3ReleaseMp3Handle(handle);
	handle = initHeader(&mp3Init, baseHeader, sizeof(baseHeader));
	testSetLoopNum("  After init", handle, 0);
	sceMp3ReleaseMp3Handle(handle);
	handle = initHeader(&mp3Init, baseHeader, sizeof(baseHeader));
	sceMp3Decode(handle, &out);
	testSetLoopNum("  After decode", handle, 0);
	sceMp3ReleaseMp3Handle(handle);

	checkpointNext("Loop values");
	static const u32 values[] = { -2, -1, 0, 1, 2, 0x7FFFFFFF, 0x80000000 };
	for (size_t i = 0; i < ARRAY_SIZE(values); ++i) {
		char temp[64];
		snprintf(temp, sizeof(temp), "  Value %d", values[i]);
		handle = sceMp3ReserveMp3Handle(&mp3Init);
		if (handle < 0) {
			checkpoint("%s: Failed (%08x)", temp, handle);
		} else {
			testSetLoopNum(temp, handle, values[i]);
		}
		sceMp3ReleaseMp3Handle(handle);
	}

	handle = sceMp3ReserveMp3Handle(&mp3Init);
	sceMp3TermResource();
	checkpointNext("After term");
	testSetLoopNum("  Prev allocated handle", handle, 0);

	return 0;
}
