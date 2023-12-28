#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

void vector_init(vector_t *v, int elm)
{
	v->num = 0;
	v->elm = elm;
	v->cap = 0;
	v->buf = NULL;
}

void *vector_push(vector_t *v, const void *e)
{
	void *p;

	if (v->num + 1 > v->cap)
	{
		size_t size;

		if (v->cap == 0)
		{
			v->cap = 1;
		}
		else
		{
			v->cap = v->cap * 2;
		}

		size = v->elm * v->cap;

		if (v->buf == NULL)
		{
			v->buf = malloc(size);
		}
		else
		{
			v->buf = realloc(v->buf, size);
		}
	}

	if (v->buf == NULL)
	{
		return NULL;
	}

	p = (char *) v->buf + v->elm * v->num;

	v->num = v->num + 1;

	if (e != NULL)
	{
		memcpy(p, e, v->elm);
	}

	return p;
}
