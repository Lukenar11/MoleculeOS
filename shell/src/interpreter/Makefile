OBJS += \
	$(BUILD)/interpreter.o \
	$(BUILD)/commands.o

$(BUILD)/%.o: shell/src/interpreter/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
