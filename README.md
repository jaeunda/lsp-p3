# Linux System Programming Project 3: ssu-ext2
`ssu-ext2` is a shell-based program that analyzes an ext2 file system image and interacts with its contents using built-in commands.


## Implementation
- Parses ext2 disk images **without using ext2-specific libraries or headers**.
- Fully reconstructs and navigates the directory tree using **linked list structures**.
- Provides a custom shell interface with internal commands to explore directories and read file contents.
- Outputs structured, permission-aware views of the file system.
- Gracefully handles invalid input, missing arguments, or wrong paths.


## Commands
### `tree <PATH> [OPTIONS]`
Displays the directory structure of the given path (relative to root `.`).
- `-r`: recursively shows subdirectories
- `-s`: includes size of files/directories
- `-p`: shows permissions
All options can be combined.


### `print <PATH> [OPTIONS]`
Prints the contents of the given file.
- `-n <line_num>`: displays only the first `line_num` lines.


### `help [COMMAND]`
Display usage information for all commands, or a specific one.


### `exit`
Terminates the shell interface.


## Build
- Compile
```
$ make
```
- Execute
```
$ ./ssu_ext2
20232372>
20232372> exit
$
```
- Remove
```
$ make clean
```
