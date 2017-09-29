//#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bseq.h"
#include "kseq.h"
KSEQ_INIT(FILE *, fread)

extern unsigned char seq_nt4_table[256];

struct bseq_file_s {
	int is_eof;
	FILE *fp;
	kseq_t *ks;
};

bseq_file_t *bseq_open(const char *fn)
{
	bseq_file_t *fp;
	FILE * f;
	f = fn && strcmp(fn, "-")? fopen(fn, "r") : fdopen(fileno(stdin), "r");
	if (f == 0) return 0;
	fp = (bseq_file_t*)calloc(1, sizeof(bseq_file_t));
	fp->fp = f;
	fp->ks = kseq_init(fp->fp);
	printf ("ofs %lu\n", ftell(f));
	return fp;
}

void bseq_close(bseq_file_t *fp)
{
	kseq_destroy(fp->ks);
	fclose(fp->fp);
	free(fp);
}

void bseq_seek(bseq_file_t *fp, size_t ofs)
{
	fseek (fp->fp, ofs, SEEK_SET);
	printf ("ofs %lu\n", ftell(fp->fp));
}

bseq1_t *bseq_read(bseq_file_t *fp, int chunk_size, int *n_, size_t *sizeToRead)
{
//	printf ("chunk_size %d\n", chunk_size);
	int size = 0, m, n;
	bseq1_t *seqs;
	kseq_t *ks = fp->ks;
	m = n = 0; seqs = 0;
	while ((chunk_size > 0) && (kseq_read(ks) >= 0)) {
		bseq1_t *s;
		assert(ks->seq.l <= INT32_MAX);
		if (n >= m) {
			m = m? m<<1 : 256;
			seqs = (bseq1_t*)realloc(seqs, m * sizeof(bseq1_t));
		}
		s = &seqs[n];
		s->name = strdup(ks->name.s);
		s->seq = strdup(ks->seq.s);
		s->l_seq = ks->seq.l;
		s->l_name = ks->name.l;
		if (sizeToRead != NULL)
		{
	                s->qual = strdup(ks->qual.s);//vasu
			size += ks->name.l + ks->comment.l + ks->plus_len + 2*seqs[n].l_seq + 4 + 2 + (ks->comment.l > 0);
		}
		else
			size += seqs[n].l_seq;
		n++;
//		printf ("%d - *%s* *%s* - %d, %d, %d, %d\n", size, ks->name.s, ks->comment.s, ks->name.l, ks->comment.l, ks->plus_len, ks->seq.l);
		if (size >= chunk_size) break;
	}
	if (sizeToRead != NULL)
		*sizeToRead -= size;
	if (n == 0) fp->is_eof = 1;
	*n_ = n;
	return seqs;
}
bseq1_t *bseq_read2(bseq_file_t *fp, int n_)
{
	int size = 0, m, n;
	bseq1_t *seqs;
	kseq_t *ks = fp->ks;
	m = n = 0; seqs = 0;
	while (kseq_read(ks) >= 0) {
		bseq1_t *s;
		assert(ks->seq.l <= INT32_MAX);
		if (n >= m) {
			m = m? m<<1 : 256;
			seqs = (bseq1_t*)realloc(seqs, m * sizeof(bseq1_t));
		}
		s = &seqs[n];
		s->name = strdup(ks->name.s);
		s->seq = strdup(ks->seq.s);
                s->qual = strdup(ks->qual.s);//vasu
		s->l_seq = ks->seq.l;
		s->l_name = ks->name.l;
		size += ks->name.l + ks->comment.l + ks->plus_len + 2*seqs[n].l_seq + 4 + 2 + (ks->comment.l > 0);
		n++;
//		printf ("%d - *%s* *%s* - %d, %d, %d, %d\n", size, ks->name.s, ks->comment.s, ks->name.l, ks->comment.l, ks->plus_len, ks->seq.l);
		if (n >= n_) break;
	}
	if (n == 0) fp->is_eof = 1;
	assert (n_ == n);
	return seqs;
}

int bseq_eof(bseq_file_t *fp)
{
	return fp->is_eof;
}
