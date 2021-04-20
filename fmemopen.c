/*
 * Copyright (c) 2017  Joachim Nilsson <troglobit@gmail.com>
 * https://raw.githubusercontent.com/libconfuse/libconfuse/master/src/fmemopen.c
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
extern void *malloc(int size);

struct ops
{
	char *buf;
	unsigned int len, pos;
};
typedef unsigned long size_t;

typedef struct ops ops_t;

static int readfn(void *arg, char *buf, int len)
{
	int sz;
	ops_t *ops = (ops_t *)arg;

	sz = (int)(ops->len - ops->pos);
	if (sz < 0)
		sz = 0;
	if (len > sz)
		len = sz;

	memcpy(buf, &ops->buf[ops->pos], len);
	ops->pos += len;

	return len;
}

static int writefn(void *arg, const char *buf, int len)
{
	int sz;
	ops_t *ops = (ops_t *)arg;

	sz = (int)(ops->len - ops->pos);
	if (sz < 0)
		sz = 0;
	if (len > sz)
		len = sz;

	memcpy(&ops->buf[ops->pos], buf, len);
	ops->pos += len;

	return len;
}

static int closefn(void *arg)
{
	free(arg);
	return 0;
}
static int seekfn(void *arg, unsigned long offset, int whence)
{
	ops_t *ops = malloc(sizeof(*ops));
	ops->pos += offset;
	return ops->pos;
}
int *fmemopen(void *buf, size_t len, const char *type)
{
	ops_t *ops = (ops_t *)malloc(sizeof(*ops));

	if (!ops)
		return 0;

	memset(ops, 0, sizeof(*ops));
	ops->buf = buf;
	ops->len = len;
	ops->pos = 0;

	return funopen(ops, readfn, writefn, seekfn, closefn);
}