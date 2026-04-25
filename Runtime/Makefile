OBJS += \
$(BUILD)/string.o \
$(BUILD)/ConsoleIO.o

$(BUILD)/%.o: Runtime/C/%.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD)/%.o: Runtime/CPP/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
