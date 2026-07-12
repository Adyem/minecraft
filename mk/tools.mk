PYTHON          ?= python3
LINT_SCRIPT      = scripts/lint_sources.py
COVERAGE_SCRIPT  = scripts/coverage_report.py
GOINFRE_LOCAL_PREFIX ?= /goinfre/$(USER)/local

ifeq ($(OS),Windows_NT)
    MKDIR   = mkdir -p
    RMDIR   = rm -rf
    RM      = rm -f
else
    MKDIR   = mkdir -p
    RMDIR   = rm -rf
    RM      = rm -f
endif
