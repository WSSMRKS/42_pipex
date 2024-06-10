# solve relink issues




# Directories #
SRCDIR = src/
HDRDIR = inc/
LIBFTDIR = libft
# Names #
NAME = pipex
# no bonus
BONUS_NAME = pipex_bonus
# Compiler & COptions & CFlags #
CFLAGS =  -g -Werror -Wall -Wextra
TESTFLAGS = -g3
COPTIONS = -g3 -Werror -Wall -Wextra -O0 -c
# ADD SEARCHFLAGS FOR LIBFT ,...
CC = cc
# Source Files #
SRC = src/pipex.c src/pipex_parsing.c src/pipex_utils.c
HEADERS = pipex.h
LIBFT_SRC = libft/libft.a
# Object Files
SRC_OBJ = $(SRC:.c=.o)
BONUS_OBJ = $(BONUS_SRC:.c=.o)
MAIN_OBJ = $(MAIN_SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)

# Targets #
all: $(NAME)	# Compile all project parts including bonus.

$(NAME): $(LIBFT_SRC) $(SRC_OBJ) # Compile mandatory part.
	$(CC) $(SRC_OBJ) $(MLX_SRC) $(LIBFT_SRC) $(CFLAGS) -o $(NAME)

$(LIBFT_SRC): # Compile libft
	$(MAKE) all -C libft/

exes: $(NAME) clean # Compile all project parts including bonus clean up after compilation.

# Compile .c to .o #
%.o: %.c
	$(CC) $^ $(COPTIONS) -o $@

# Checkers, Testers #

# clean, fclean, re
clean:	# Clean project folders, leave executables.
	rm -f $(SRC_OBJ)
	rm -f $(MAIN_OBJ)
	rm -f $(BONUS_OBJ)
	rm -f $(TEST_OBJ)
  ifeq ("$(wildcard $(LIBFTDIR))", "")
	echo "libft: Directory does not exist."
  else
	$(MAKE) fclean -C libft/
	echo "libft folder cleaned"
  endif

fclean: clean	# Fully clean project folders.
	rm -f $(NAME) $(MAIN_NAME) $(BONUS_NAME)
	echo "\"$(NAME)\" deleted"

re: fclean	# Recompile whole project.
	$(MAKE) all

name:
	echo "$(NAME)"

# help:
# 	echo "Possible Commands:"
# 	echo "all --> Compile whole project including bonus"
# 	echo "name --> Display project name"
# 	echo "exes --> Compile project and delete object files"
# 	echo "help --> Display this help message"
# 	echo "mlx --> Download mlx_linux library"
# 	echo "dl_tester --> Download the tester"
# 	echo "test --> Run all available tests"
# 	echo "test --> run all available tests"
# 	echo "clean --> Delete all object files"
# 	echo "fclean --> Delete everything besides source files"
# 	echo "re --> recompile everything (fclean, all)"
# 	echo "libft --> Compile libft and copy libft.h and libft.a to project folder"

help:	# Print this helpful message
	@awk 'BEGIN { \
	FS = ":.*#"; printf "Usage:\n\tmake <target>\n\nTargets:\n"; } \
	/^[a-zA-Z_0-9-]+:.*?#/ { \
	printf "%-16s%s\n", $$1, $$2 } ' Makefile
# Syntax for this to work: target:	# Description

.PHONY: all fclean clean re name help exes
