.PHONY: build run clean debug

BUILD_TYPE ?= Debug

build:
	@mkdir -p build
	@cd build && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) .. && make
	@ln -sf build/compile_commands.json compile_commands.json

run: build
	@./build/example

debug:
	@$(MAKE) BUILD_TYPE=Debug build
	@lldb ./build/example

clean:
	@rm -rf build
	@rm -f compile_commands.json
