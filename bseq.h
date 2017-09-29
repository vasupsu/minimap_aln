#ifndef MM_BSEQ_H
#define MM_BSEQ_H

#include <stdint.h>

struct bseq_file_s;
typedef struct bseq_file_s bseq_file_t;

typedef struct {
	int l_name, l_seq, rid;
	char *name, *seq, *qual;
} bseq1_t;

bseq_file_t *bseq_open(const char *fn);
void bseq_close(bseq_file_t *fp);
void bseq_seek(bseq_file_t *fp, size_t ofs);
bseq1_t *bseq_read(bseq_file_t *fp, int chunk_size, int *n_, size_t *sizeToRead);
bseq1_t *bseq_read2(bseq_file_t *fp, int n_);
int bseq_eof(bseq_file_t *fp);

#endif
