#ifndef VECTOR_H
#define VECTOR_H

typedef struct
{
	int	elm;
	int	num;
	int	cap;
	void *	buf;
} vector_t;

void	vector_init(vector_t *v, int elm);
void *	vector_push(vector_t *v, const void *e);

static inline void *vector_at(const vector_t *v, int n)
{
	return (char *) v->buf + v->elm * n;
}

#endif
