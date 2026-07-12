LIBFT_DIR    = Libft
LIBFT_UNAME_S := $(shell uname -s)

LIBFT_COMPILE_FLAGS = -Wall -Wextra -Werror -std=c++17 \
                      -Wmissing-declarations -Wshadow -Wformat=2 -Wundef \
                      -Wfloat-equal -Wodr \
                      -DLIBFT_INTERNAL_HEADERS -DGAME_USE_VOXEL_REGION_BACKEND $(OPT_FLAGS)

ifeq ($(OS),Windows_NT)
    LIBFT_COMPILE_FLAGS += -Wold-style-cast -Wconversion -Wuseless-cast \
                           -Wzero-as-null-pointer-constant -Wmaybe-uninitialized
else ifeq ($(LIBFT_UNAME_S),Darwin)
    LIBFT_COMPILE_FLAGS += -Wno-format-nonliteral -Wno-tautological-compare
else
    LIBFT_COMPILE_FLAGS += -Wold-style-cast -Wconversion -Wuseless-cast \
                           -Wzero-as-null-pointer-constant -Wmaybe-uninitialized
endif

LIBFT_FULL_LIB           = $(LIBFT_DIR)/Full_Libft.a
LIBFT_FULL_DEBUG_LIB     = $(LIBFT_DIR)/Full_Libft_debug.a
LIBFT_LINK_LIB           = $(LIBFT_FULL_LIB)
LIBFT_LINK_FLAGS         = $(LIBFT_LINK_LIB)
LIBFT_BUILD_OUTPUT_SUFFIX = _ft_vox_opt$(OPT_LEVEL)
LIBFT_ARCHIVE_SUFFIX      = _ft_vox

LIBFT_JOBS ?= 4
LIBFT_PARALLEL_FLAGS = $(if $(filter -j%,$(MAKEFLAGS)),,-j$(LIBFT_JOBS))
LIBFT_CHILD_BATCH_OUTPUT = $(if $(filter -j%,$(MAKEFLAGS)),$(FT_VOX_BATCH_OUTPUT),$(if $(filter 1,$(LIBFT_JOBS)),0,1))
