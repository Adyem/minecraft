ifeq ($(OS),Windows_NT)
SHELL := C:/Progra~1/Git/bin/bash.exe
.SHELLFLAGS := -lc
export SHELL
else
SHELL := /bin/bash
endif

include mk/project.mk
include mk/tools.mk
include mk/output.mk
include mk/sources.mk
include mk/directories.mk
include mk/libft.mk
include mk/compiler.mk
include mk/objects.mk

.SILENT:

ifeq ($(OS),Windows_NT)
SUBMODULE_UPDATE_CMD = tools\update_libft.cmd
else
SUBMODULE_UPDATE_CMD = sh tools/update_libft.sh
endif

all: dirs $(TARGET)

tests: dirs $(TEST_NAME)

dirs:
	@-$(MKDIR) $(OBJ_DIR)
	@-$(MKDIR) $(OBJ_DIR_DEBUG)
	@-$(MKDIR) $(OBJ_DIR_TEST)

$(BUILD_LOG_DIR):
	@-$(MKDIR) $(BUILD_LOG_DIR)

install_cobc:
	@if ! command -v cobc >/dev/null 2>&1; then \
		apt-get update && apt-get install -y gnucobol; \
	else \
		printf 'cobc already installed.\n'; \
	fi

tests_with_cobc: install_cobc
	$(MAKE) tests
	$(MAKE) test

submodule_init:
	@$(SUBMODULE_UPDATE_CMD)

submodule_update:
	@$(SUBMODULE_UPDATE_CMD)

debug:
	$(MAKE) all DEBUG=1

$(TARGET): $(OBJS) $(LIBFT_LINK_LIB)
	@printf '\033[1;36m[FT_VOX BUILD] Linking %s\033[0m\n' "$@"
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT_LINK_FLAGS) -o $@ $(LDFLAGS)

$(LIBFT_FULL_LIB): FORCE
	@$(MAKE) -C $(LIBFT_DIR) $(LIBFT_PARALLEL_FLAGS) $(notdir $@) COMPILE_FLAGS="$(LIBFT_COMPILE_FLAGS)" BUILD_OUTPUT_SUFFIX="$(LIBFT_BUILD_OUTPUT_SUFFIX)" LIBFT_ARCHIVE_SUFFIX="$(LIBFT_ARCHIVE_SUFFIX)" LIBFT_BATCH_OUTPUT=$(LIBFT_CHILD_BATCH_OUTPUT)

$(LIBFT_FULL_DEBUG_LIB): FORCE
	@$(MAKE) -C $(LIBFT_DIR) $(LIBFT_PARALLEL_FLAGS) $(notdir $@) COMPILE_FLAGS="$(LIBFT_COMPILE_FLAGS)" BUILD_OUTPUT_SUFFIX="$(LIBFT_BUILD_OUTPUT_SUFFIX)" LIBFT_ARCHIVE_SUFFIX="$(LIBFT_ARCHIVE_SUFFIX)" LIBFT_BATCH_OUTPUT=$(LIBFT_CHILD_BATCH_OUTPUT)

$(TEST_NAME): $(TEST_OBJS) $(OBJS_NO_MAIN) $(TARGET) $(LIBFT_FULL_LIB)
	@printf '\033[1;36m[FT_VOX BUILD] Linking %s\033[0m\n' "$@"
	@$(CC) $(CFLAGS) $(TEST_OBJS) $(OBJS_NO_MAIN) $(LIBFT_LINK_FLAGS) -o $@ $(LDFLAGS)

$(OBJS) $(TEST_OBJS): | $(LIBFT_LINK_LIB)

$(OBJ_DIR)/%.o: %.cpp
	@if [ "$(FT_VOX_BATCH_OUTPUT)" = "1" ]; then \
		log_file=".ft_vox_build_$$(printf '%s' "$@" | tr '/.' '__')_$$$$.log"; \
		{ \
			$(MKDIR) $(dir $@); \
			$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@; \
			$(FIXDEP); \
			built=$$(find $(OBJ_DIR) -type f -name '*.o' 2>/dev/null | wc -l); \
			printf '\033[1;36m[FT_VOX PROGRESS] %s (%d/%d)\033[0m\n' "$<" $$built $(TOTAL_OBJS); \
		} > "$$log_file" 2>&1; \
		status=$$?; \
		while ! mkdir $(FT_VOX_OUTPUT_LOCK) 2>/dev/null; do sleep 0.02; done; \
		cat "$$log_file"; \
		rmdir $(FT_VOX_OUTPUT_LOCK); \
		$(RM) "$$log_file"; \
		if [ $$status -ne 0 ]; then exit $$status; fi; \
	else \
		$(MKDIR) $(dir $@); \
		$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@; \
		$(FIXDEP); \
		built=$$(find $(OBJ_DIR) -type f -name '*.o' 2>/dev/null | wc -l); \
		printf '\033[1;36m[FT_VOX PROGRESS] %s (%d/%d)\033[0m\n' "$<" $$built $(TOTAL_OBJS); \
	fi

$(OBJ_DIR)/%.o: %.mm
	@$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@
	@$(FIXDEP)
	@built=$$(find $(OBJ_DIR) -type f -name '*.o' 2>/dev/null | wc -l); \
	printf '\033[1;36m[FT_VOX PROGRESS] %s (%d/%d)\033[0m\n' "$<" $$built $(TOTAL_OBJS)

$(OBJ_DIR_TEST)/%.o: %.cpp
	@if [ "$(FT_VOX_BATCH_OUTPUT)" = "1" ]; then \
		log_file=".ft_vox_build_$$(printf '%s' "$@" | tr '/.' '__')_$$$$.log"; \
		{ \
			$(MKDIR) $(dir $@); \
			$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@; \
			$(FIXDEP); \
			built=$$(find $(OBJ_DIR_TEST) -type f -name '*.o' 2>/dev/null | wc -l); \
			printf '\033[1;36m[FT_VOX TEST PROGRESS] %s (%d/%d)\033[0m\n' "$<" $$built $(TOTAL_TEST_OBJS); \
		} > "$$log_file" 2>&1; \
		status=$$?; \
		while ! mkdir $(FT_VOX_OUTPUT_LOCK) 2>/dev/null; do sleep 0.02; done; \
		cat "$$log_file"; \
		rmdir $(FT_VOX_OUTPUT_LOCK); \
		$(RM) "$$log_file"; \
		if [ $$status -ne 0 ]; then exit $$status; fi; \
	else \
		$(MKDIR) $(dir $@); \
		$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@; \
		$(FIXDEP); \
		built=$$(find $(OBJ_DIR_TEST) -type f -name '*.o' 2>/dev/null | wc -l); \
		printf '\033[1;36m[FT_VOX TEST PROGRESS] %s (%d/%d)\033[0m\n' "$<" $$built $(TOTAL_TEST_OBJS); \
	fi

-include $(DEPS)

clean:
	-$(RMDIR) $(OBJ_DIR) $(OBJ_DIR_DEBUG) $(OBJ_DIR_TEST)
	-$(RM) test_example_compiler.c test_example_compiler.bin test_example_compiler.txt
	-$(RM) test_example_invalid_compiler.c test_example_invalid_compiler.bin test_example_invalid_compiler.log
	-$(RM) test_runtime_file.txt
	-$(RMDIR) $(BUILD_LOG_DIR) $(FT_VOX_OUTPUT_LOCK)
	-$(RM) $(FT_VOX_OUTPUT_LOGS)

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) $(LIBFT_PARALLEL_FLAGS) fclean
	-$(RM) $(NAME) $(NAME_DEBUG) $(TEST_NAME)
	-$(RMDIR) $(OBJ_DIR) $(OBJ_DIR_DEBUG) $(OBJ_DIR_TEST) data

re:
	@$(MAKE) fclean
	@$(MAKE) all

test: $(TEST_NAME)
	@if [ -x "$(GOINFRE_LOCAL_PREFIX)/bin/cobc" ]; then \
		PATH="$(GOINFRE_LOCAL_PREFIX)/bin:$$PATH" \
		LD_LIBRARY_PATH="$(GOINFRE_LOCAL_PREFIX)/lib:$(GOINFRE_LOCAL_PREFIX)/lib64$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH}" \
		CPATH="$(GOINFRE_LOCAL_PREFIX)/include$${CPATH:+:$$CPATH}" \
		LIBRARY_PATH="$(GOINFRE_LOCAL_PREFIX)/lib:$(GOINFRE_LOCAL_PREFIX)/lib64$${LIBRARY_PATH:+:$$LIBRARY_PATH}" \
		./$(TEST_NAME); \
	else \
		./$(TEST_NAME); \
	fi

both: all debug

re_both: re both

lint:
	$(PYTHON) $(LINT_SCRIPT)

coverage:
	@-$(RMDIR) $(OBJ_DIR)
	@-$(RMDIR) $(OBJ_DIR_DEBUG)
	@-$(RMDIR) $(OBJ_DIR_TEST)
	@-$(RM) $(TARGET)
	@-$(RM) $(NAME_DEBUG)
	@-$(RM) $(TEST_NAME)
	$(MAKE) tests OPT_LEVEL=0 COVERAGE=1
	$(MAKE) test COVERAGE=1
	$(PYTHON) $(COVERAGE_SCRIPT) --object-dir $(OBJ_DIR) --threshold-lines $(COVERAGE_LINE_THRESHOLD) --threshold-branches $(COVERAGE_BRANCH_THRESHOLD)

ci-coverage:
	$(MAKE) coverage

ci-build:
	$(MAKE) fclean
	$(MAKE) all OPT_LEVEL=2
	$(MAKE) debug

ci-test:
	$(MAKE) test

ci-lint: lint

ci:
	$(MAKE) ci-build
	$(MAKE) ci-test
	$(MAKE) ci-lint
	$(MAKE) ci-coverage

.PHONY: all dirs clean fclean re debug both re_both tests test lint coverage \
        ci-build ci-test ci-lint ci-coverage ci submodule_init submodule_update \
        ft_vox install_cobc tests_with_cobc FORCE

FORCE:
