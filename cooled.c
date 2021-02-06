#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* file;
size_t file_size = 0;
int line_number = 1;
FILE* the_file;

char*
get_line_start(int line)
{
	char* x = file;
	for(;line > 0;x++)
	{
		if(*x == '\n') line--;
		if(x > x+file_size) return 0;
	}
	return x;
}

int
get_line_count()
{
	int x = 0;
	for(int i = 0; i < file_size; i++) if(file[i] == '\n') x++;
	return x;
}
int
load_file()
{
	fseek(the_file, 0, SEEK_END);
	file_size = ftell(the_file);
	file = malloc(file_size);
	fseek(the_file, 0, SEEK_SET);
	for(int i = 0; i < file_size; i++) file[i] = fgetc(the_file);
}

int
main(int argc, char** argv)
{
	if(argc == 2)
	{
		the_file = fopen(argv[1], "rw");
		load_file();
	}
	for(;;)
	{
		printf("%d\t>", line_number);
		char* command;
		size_t command_length = 0;
		getline(&command, &command_length, stdin);
		if(*command == 'a')
		{
			for(;;)
			{
				//draw the prompt
				printf("%d\t:", line_number);
				char* line;
				size_t line_size = 0;
				//read the input
				line_size = getline(&line, &line_size, stdin);
				//if this is pressed, insert mode ends
				if(*line == '.' && line[1] == '\n')
				{
					free(line);
					break;
				}
				//the new size, we don't want to change the size yet, it will indicate where the end of the file is
				size_t new_size = file_size + line_size;
				file = realloc(file, new_size);
				//if the line is in the middle of the document, we will write on the beggining of the line that was there before
				char* where = line_number>=get_line_count() ? file + file_size : get_line_start(line_number);
				//if the line is in the middle of the document, shift all the contents, so we can fit the new string in there
				if(line_number < get_line_count())
				{
					char* start = get_line_start(line_number);
					char rest_of_file[file+file_size-start];
					memcpy(rest_of_file, start, file+file_size-start);
					memcpy(start + line_size, rest_of_file, file+file_size-start);
				}
				file_size = new_size;
				memcpy(where, line, line_size);
				line_number++;
				free(line);
			}
		}
		else if(*command == 'n')
		{
			int line = 1;
			printf("%d\t", line);
			line++;
			for(size_t i = 0; i < file_size; i++)
			{
				putchar(file[i]);
				if(file[i] == '\n')
				{
					printf("%d\t", line);
					line++;
				}
			}
			printf("\n");
		}
		else if(*command == 'q')
		{
			free(file);
			fclose(the_file);
			return 0;
		}
		//please, don't get mad at me :(
		else if(
			*command == '0' ||
			*command == '1' ||
			*command == '2' ||
			*command == '3' ||
			*command == '4' ||
			*command == '5' ||
			*command == '6' ||
			*command == '7' ||
			*command == '8' ||
			*command == '9')
		{
			sscanf(command, "%d", &line_number);
		}
		free(command);
	}
}
