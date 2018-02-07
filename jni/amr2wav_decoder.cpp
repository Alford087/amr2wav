#include <jni.h>
#include <interf_dec.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "wavwriter.h"
#include <interf_dec.h>

namespace amr_decode{

#ifndef _Included_com_nd_media_amr_Amr2WavDecoder
#define _Included_com_nd_media_amr_Amr2WavDecoder

#ifdef __cplusplus
extern "C" {
#endif


/* From WmfDecBytesPerFrame in dec_input_format_tab.cpp */
const int sizes[] = { 12, 13, 15, 17, 19, 20, 26, 31, 5, 6, 5, 5, 0, 0, 0, 0 };


char* jstringTostring(JNIEnv* env, jstring jstr){
	char* rtn = NULL;
	jclass clsstring = env->FindClass("java/lang/String");
	jstring strencode = env->NewStringUTF("utf-8");
	jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
	jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
	jsize alen = env->GetArrayLength(barr);
	jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
	if (alen > 0){
		rtn = (char*)malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	env->ReleaseByteArrayElements(barr, ba, 0);
	return rtn;
}


JNIEXPORT jint JNICALL Java_com_nd_media_amr_Amr2WavDecoder_convert
  (JNIEnv* env, jobject jclass, jstring amrPath, jstring wavPath){

  FILE* in;
  char header[6];
  int n;
  void *wav, *amr;
//  if (argc < 3) {
// 	fprintf(stderr, "%s in.amr out.wav\n", argv[0]);
//  	return 1;
//  }
   char *amrPathTemp = jstringTostring(env,amrPath);
//  char *nameStr= env->GetStringUTFChars(amrPath,NULL);
  in = fopen(amrPathTemp, "rb");
  if (!in) {
//    perror(argv[1]);
  	return 1;
  }
  n = fread(header, 1, 6, in);
  if (n != 6 || memcmp(header, "#!AMR\n", 6)) {
//  	fprintf(stderr, "Bad header\n");
  	return 1;
  }

  const char *wavPathTemp =jstringTostring(env,wavPath);
  wav = wav_write_open(wavPathTemp, 8000, 16, 1);
  if (!wav) {
//  	fprintf(stderr, "Unable to open %s\n", argv[2]);
  	return 1;
  }

  amr = Decoder_Interface_init();
  while (1) {
  	uint8_t buffer[500], littleendian[320], *ptr;
  	int size, i;
  	int16_t outbuffer[160];
  	/* Read the mode byte */
  	n = fread(buffer, 1, 1, in);
  	if (n <= 0)
  		break;
  	/* Find the packet size */
  	size = sizes[(buffer[0] >> 3) & 0x0f];
  	n = fread(buffer + 1, 1, size, in);
  	if (n != size)
  		break;

  	/* Decode the packet */
  	Decoder_Interface_Decode(amr, buffer, outbuffer);

  	/* Convert to little endian and write to wav */
  	ptr = littleendian;
  	for (i = 0; i < 160; i++) {
  		*ptr++ = (outbuffer[i] >> 0) & 0xff;
  		*ptr++ = (outbuffer[i] >> 8) & 0xff;
  	}
  	wav_write_data(wav, littleendian, 320);
  }
  fclose(in);
  Decoder_Interface_exit(amr);
  wav_write_close(wav);
}
 
#ifdef __cplusplus
}
#endif
#endif

}
