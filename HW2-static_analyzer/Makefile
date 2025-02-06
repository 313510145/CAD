.PHONY: all check run run_valgrind clean

CXX = g++
CXX_FLAG = -Wall -Wextra -std=c++14 -g -static
INCLUDE_PATH = -Iheader

CHECKCC = valgrind
CHECKCC_FLAG = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose

CPPCHECK = ../../cppcheck-2.13.0/cppcheck
CPPCHECK_FLAG = --enable=all --inconclusive --suppress=missingIncludeSystem

MEMORY_DIRECTORY = memory
OBJECT_DIRECTORY = object
OUTPUT_DIRECTORY = output

SOURCE_DIRECTORY = source
TESTCASE_DIRECTORY = testcase
TESTCASE_LIB_DIRECTORY = $(TESTCASE_DIRECTORY)/library
TESTCASE_VERILOG_DIRECTORY = $(TESTCASE_DIRECTORY)/verilog
TESTCASE_PATTERN_DIRECTORY = $(TESTCASE_DIRECTORY)/pattern

SOURCE = $(wildcard $(SOURCE_DIRECTORY)/*.cpp)
OBJECT = $(patsubst $(SOURCE_DIRECTORY)/%.cpp, $(OBJECT_DIRECTORY)/%.o, $(SOURCE))
DEPENDENCY = $(patsubst $(SOURCE_DIRECTORY)/%.cpp, $(OBJECT_DIRECTORY)/%.d, $(SOURCE))

TESTCASE_LIB = $(wildcard $(TESTCASE_LIB_DIRECTORY)/*.lib)
TESTCASE_VERILOG = $(wildcard $(TESTCASE_VERILOG_DIRECTORY)/*.v)

define GET_TESTCASE_LIB_BASE
testcase_lib_base=$$(basename $$testcase_lib); \
testcase_lib_base=$${testcase_lib_base%.*}
endef

define GET_TESTCASE_VERILOG_BASE
testcase_verilog_base=$$(basename $$testcase_verilog); \
testcase_verilog_base=$${testcase_verilog_base%.*}
endef

TARGET = HW2

all: $(TARGET)

$(MEMORY_DIRECTORY):
	mkdir $(MEMORY_DIRECTORY)

$(OBJECT_DIRECTORY):
	mkdir $(OBJECT_DIRECTORY)

$(OUTPUT_DIRECTORY):
	mkdir $(OUTPUT_DIRECTORY)

$(TARGET): main.cpp $(OBJECT)
	$(CXX) $(CXX_FLAG) $(INCLUDE_PATH) $^ -o $@

-include $(DEPENDENCY)

$(OBJECT_DIRECTORY)/%.o: $(SOURCE_DIRECTORY)/%.cpp | $(OBJECT_DIRECTORY)
	$(CXX) $(CXX_FLAG) $(INCLUDE_PATH) -MMD -c $< -o $@

check: $(CPPCHECK)
	./$(CPPCHECK) $(CPPCHECK_FLAG) $(INCLUDE_PATH) .

run: $(TARGET) | $(OUTPUT_DIRECTORY)
	for testcase_lib in $(TESTCASE_LIB); do \
		for testcase_verilog in $(TESTCASE_VERILOG); do \
			$(GET_TESTCASE_VERILOG_BASE); \
			./$(TARGET) $$testcase_lib $$testcase_verilog $(TESTCASE_PATTERN_DIRECTORY)/$${testcase_verilog_base}.pat $(OUTPUT_DIRECTORY)/; \
		done \
	done

run_valgrind: $(TARGET) | $(MEMORY_DIRECTORY) $(OUTPUT_DIRECTORY)
	for testcase_lib in $(TESTCASE_LIB); do \
		for testcase_verilog in $(TESTCASE_VERILOG); do \
			$(GET_TESTCASE_LIB_BASE); \
			$(GET_TESTCASE_VERILOG_BASE); \
			$(CHECKCC) $(CHECKCC_FLAG) --log-file=$(MEMORY_DIRECTORY)/$${testcase_lib_base}_$${testcase_verilog_base}.log ./$(TARGET) $$testcase_lib $$testcase_verilog $(TESTCASE_PATTERN_DIRECTORY)/$${testcase_verilog_base}.pat $(OUTPUT_DIRECTORY)/; \
		done \
	done

clean:
	rm -rf $(TARGET) $(MEMORY_DIRECTORY) $(OBJECT_DIRECTORY) $(OUTPUT_DIRECTORY)
