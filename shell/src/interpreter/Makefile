OBJS += \
	$(BUILD)/interpreter.o

$(BUILD)/%.o: shell/src/interpreter/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
