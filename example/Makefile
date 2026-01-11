.PHONY: build run clean

build:
	@mkdir -p build
	@cd build && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. && make
	@ln -sf build/compile_commands.json compile_commands.json

run: build
	@./build/example

clean:
	@rm -rf build
	@rm -f compile_commands.json
