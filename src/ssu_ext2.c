#include "ssu_ext2.h"

int main(int argc, char *argv[]){
	// init
	if (argc != 2){
		fprintf(stderr, "Usage Error: ./ssu_ext2 <EXT2_IMAGE>\n\n");
		exit(1);
	} else {
		strcpy(img_path, argv[1]);
	}
	// prompt
	while (prompt());
}
int prompt(){
	char input_buffer[BUFFER_SIZE];

	printf("20232372> ");
	fgets(input_buffer, BUFFER_SIZE, stdin);
	input_buffer[strlen(input_buffer) -1] = '\0';

	if (!strcmp(input_buffer, "")) return TRUE;
	else if (!strcmp(input_buffer, "exit")) return FALSE;
	else if (!strcmp(input_buffer, "help")) print_usage(ALL);

	// instruction
	int argc;
	char **argv = divide_line(input_buffer, &argc, " \t");
	
	// tree
	if (argc >= 2 && !strcmp(argv[0], "tree")){
		tree(argc, argv);
	}
	// print
	else if (argc >= 2 && !strcmp(argv[0], "print")){
		print(argc, argv);
	}
	// help
	else if (argc == 2 && !strcmp(argv[0], "help")){
		if (!strcmp(argv[1], "tree")) print_usage(TREE);
		else if (!strcmp(argv[1], "print")) print_usage(PRINT);
		else if (!strcmp(argv[1], "help")) print_usage(HELP);
		else if (!strcmp(argv[1], "exit")) print_usage(EXIT);
		else {
			fprintf(stderr, "Invalid command -- '%s'\n", argv[1]);
			print_usage(ALL);
		}
	}
	else {
		print_usage(ALL);
	}
	return TRUE;
}
void print_usage(int command){
	printf("Usage: \n");
	if (command == ALL || command == TREE){
		printf("\t> tree <PATH> [OPTION] ... : display the directory structure if <PATH> is a directory\n");
		printf("\t\t-r: display the directory structure recursively if <PATH> is a directory\n");
		printf("\t\t-s: display the directory structure if <PATH> is a directory, including the size of each file\n");
		printf("\t\t-p: display the directory structure if <PATH> is a directory, including the permissions of each directory and file\n");
	}

	if (command == ALL || command == PRINT){
		printf("\t> print <PATH> [OPTION] ... : print the contents on the standard output if <PATH> is file\n");
		printf("\t\t-n <line_number>: print only the first <line_number> lines of its contents on the standard output if <PATH> is file\n");
	}

	if (command == ALL || command == HELP){
		printf("\t > help [COMMAND]: show commands for program\n");
	}

	if (command == ALL || command == EXIT){
		printf("\t > exit: exit program\n");
	}
}
