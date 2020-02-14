#ifndef __RECORD_H__
#define __RECORD_H__

#include "keros.h"



void *record_start(void);
int record_stop(void *hd);
uni_record_data_t *record_data_get(void *hd, int timeout_ms);
void *encoder_init(void);
int encoder_uninit(void *hd);
int coder_data(void *hd, char *in, int inlen, char **out, int *outlen);
void record_data_destroy(uni_record_data_t *data);
void record_set_filename(char *name);


#endif

