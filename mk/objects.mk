OBJS = $(SRC:%.cpp=$(OBJ_DIR)/%.o)
ifneq ($(MM_SRC),)
OBJS += $(MM_SRC:%.mm=$(OBJ_DIR)/%.o)
endif
OBJS_NO_MAIN = $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

TEST_OBJS = $(TEST_SRC:%.cpp=$(OBJ_DIR_TEST)/%.o)

TOTAL_OBJS      := $(words $(OBJS))
TOTAL_TEST_OBJS := $(words $(TEST_OBJS))
DEPS            = $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)
