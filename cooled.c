#include <stdio.h>
#include <stdlib.h>

struct line
{
	size_t size;
	char* contents;
};

struct line* document;
int line_count;

int
shift(void* whence, int elements, size_t element_size)
{
	for(int i = 0; i < elements; i++)
		for(int j = 0; j < element_size; j++)
			*(whence + i*element_size + j) = *(whence + (i + 1)*element_size + j);
}

int
append_line(char* contents, size_t size, int whence)
{
	document = realloc(document, ++line_count * sizeof(struct line));
	shift(document + whence*sizeof(struct line), line_count - whence - 1, sizeof(struct line));
	//ah, yes, fragmentation go brrrrr
	char* x = malloc(size);
	document[whence].contents = x;
	for(int i = 0; i < size; i++) *(x+i) = contents[i];
}

int
main(int argc, char** argv)
{
	for(;;)
	{
		
	}
}
