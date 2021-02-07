#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define GENERIC_ERROR "sowwy, user-senpai, thewe was an ewwor\n"
#define FILE_NOT_OPEN_ERROR ">///< user-senpai, pwease open a file first\n"
#define COMMAND_NOT_FOUND_ERROR ">///< vim-senpai your commands are too compwicated :(\n(maybe try \"h\"?)\n"
#define TOO_MANY_ARGUMENTS_ERROR ">///< user-senpai, your [int argc] is too big >///<"

#define COMMAND_PROMPT "%d\t>", line_number
#define INSERT_MODE_PROMPT "%d\t:", line_number

#define LINE_NUMBER_INDICATOR "%d\t", line
#define CURRENT_LINE_INDICATOR "\x1b[33m>>\x1b[0m\t"
#define PAGING_INDICATOR "--MORE--"

char* file;
size_t file_size = 0;
int line_number = 0;
char* file_name;
int indent_count = 0;

char*
get_line_start(int line)
{
	char* x = file;
	for(;line > 0;x++)
	{
		if(*x == '\n') line--;
		if(x > file+file_size) return file + file_size;
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
	if(file_name == NULL) return 1;
	FILE* the_file = fopen(file_name, "rw");
	if(the_file == NULL)
	{
		int error = errno;
		printf(GENERIC_ERROR);
		perror(strerror(errno));
		return error;
	}
	fseek(the_file, 0, SEEK_END);
	file_size = ftell(the_file);
	file = malloc(file_size);
	fseek(the_file, 0, SEEK_SET);
	for(int i = 0; i < file_size; i++) file[i] = fgetc(the_file);
	fclose(the_file);
}
int
write_file()
{
	if(file_name == NULL)
	{
		printf(FILE_NOT_OPEN_ERROR);
		return 1;
	}
	FILE* the_file = fopen(file_name, "w");
	if(the_file == NULL)
	{
		int error_number = errno;
		printf(GENERIC_ERROR);
		perror(strerror(error_number));
		return 1;
	}
	for(size_t i = 0; i < file_size; i++) fputc(file[i], the_file);
	fclose(the_file);
}

char
is_number(char c)
{
	return (c>=48)&&(c<58);
}

void
enumerate(int line, int number)
{
	int first_line = line;
	size_t line_start = get_line_start(line) - file;
	size_t last_line = number>0?get_line_start(line+number) - file:file_size;
	printf(LINE_NUMBER_INDICATOR);
	line++;
	for(size_t i = line_start; i < last_line; i++)
	{
		putchar(file[i]);
		if(file[i] == '\n')
		{
			if(!((line-first_line)%20)){printf(PAGING_INDICATOR);getchar();}
			if(line == line_number) printf(CURRENT_LINE_INDICATOR);
			else printf(LINE_NUMBER_INDICATOR);
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
		printf(INSERT_MODE_PROMPT);
		for(int i = 0; i < indent_count; i++) printf("\t");
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

		//indent
		line_size += indent_count;
		line = realloc(line, line_size);
		memmove(line+indent_count, line, line_size-indent_count);
		for(int i = 0; i < indent_count; i++) line[i] = '\t';

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

int
delete_line(int line)
{
	char* line_start = get_line_start(line);
	size_t line_size = get_line_start(line+1)-line_start;
	size_t rest_of_file_size = file + file_size - line_start;
	memmove(line_start, line_start+line_size, rest_of_file_size);
	file_size -= line_size;
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
	else if(*command == 'd')
		delete_line(line_number);
	else if(*command == 'n')     //enumerate lines
		enumerate(0,0);
	else if(*command == ' ')     //the feature I miss the most in ed: list a comprehensible amount of lines before and after the cursor ED YOU NEED THIS
		enumerate(line_number<10?0:line_number-10,15);
	else if(*command == 'q')     //quit
	{
		free(file);
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
	else if(*command == 'w')
	{
		int command_offset = 1;
		int name_size = command_length - 1;
		while(command[command_offset] == ' ')
		{
			command_offset++;
			name_size --;
		}
		if(command[command_offset] != '\n' && command[command_offset] != 0)
		{
			file_name = malloc(name_size-1);
			memcpy(file_name, command+command_offset, name_size - 1);
		}
		int return_value = write_file();
		if(return_value != 0)
		{
			printf(GENERIC_ERROR);
		}
	}
	else if(*command == '>')
	{
		indent_count++;
		look_for_more(command + 1, command_length - 1);
	}
	else if(*command == '<')
	{
		indent_count--;
		look_for_more(command + 1, command_length - 1);
	}
	else
		printf(COMMAND_NOT_FOUND_ERROR);
}

int
main(int argc, char** argv)
{
	if(argc == 2)
	{
		file_name = argv[1];
		load_file();
	}
	else if(argc > 2)
	{
		printf(TOO_MANY_ARGUMENTS_ERROR);
		return 1;
	}
	for(;;)
	{
		printf(COMMAND_PROMPT);
		char* command;
		size_t command_length = 0;
		command_length = getline(&command, &command_length, stdin);
		int operation_result = interpret_command(command, command_length);
		if(operation_result == 2) return 0;
		free(command);
	}
}
