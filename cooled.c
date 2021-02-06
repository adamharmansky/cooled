#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* file;
size_t file_size = 0;
int line_number = 0;
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

char
is_number(char c)
{
	return (c>=48)&&(c<58);
}

void
enumerate(int line, int number)
{
	size_t line_start = get_line_start(line) - file;
	size_t last_line = number>0?get_line_start(line+number) - file:file_size;
	printf("%d\t", line);
	line++;
	for(size_t i = line_start; i < last_line; i++)
	{
		putchar(file[i]);
		if(file[i] == '\n')
		{
			if(!(line%20)){printf("--MORE--");getchar();}
			if(line == line_number) printf("\x1b[33m>>\x1b[0m\t");
			else printf("%d\t", line);
			line++;
		}
	}
	printf("\n");
}

void
insert_mode()
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

extern int interpret_command(char*, size_t);
//I just needed to put this here so the compiler doesn't have a mental breakdown trying to understand my recursive functions

int
look_for_more(char* command, size_t command_length)
{
	int command_offset = 0;
	while(command[command_offset] == ' ')
	{
		command_offset++;
		command_length--;
	}
	if(*command != '\n' && *command != 0)
		interpret_command(command + command_offset, command_length);
}

int
interpret_command(char* command, size_t command_length)
{
	if(*command == 'i')          //insert a line
		insert_mode();
	else if(*command == 'n')     //enumerate lines
		enumerate(0,0);
	else if(*command == ' ')     //the feature I miss the most in ed: list a comprehensible amount of lines before and after the cursor ED YOU NEED THIS
		enumerate(line_number<10?0:line_number-10,15);
	else if(*command == 'q')     //quit
	{
		free(file);
		fclose(the_file);
		return 2;
	}
	else if(is_number(*command)) //set line number and interpret
	{
		sscanf(command, "%d", &line_number);
		int command_offset = 0;
		while(is_number(*(command+command_offset)) || *command == ' ')
		{
			command_offset++;
			command_length--;
		}
		look_for_more(command + command_offset, command_length);
	}
	else if(*command == '$')     //end of file and interpret
	{
		line_number = get_line_count();
		look_for_more(command+1, command_length);
	}
	else
		printf(
		"owo vim-senpai your commands are too compwicated :(\n(maybe try \"h\"?)\n"
		);
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
		int operation_result = interpret_command(command, command_length);
		if(operation_result == 2) return 0;
		free(command);
	}
}
