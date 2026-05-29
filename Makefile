# terminal-based
build:
	clang main.c core.c imageScale.c -o ascii_filter -lm -Werror

# web-based (wasm)
WASM_CC = emcc
WASM_FLAGS = -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "UTF8ToString", "HEAPU8"]' -s ALLOW_MEMORY_GROWTH=1 -lm
WASM_SRC = wasm_core.c core.c imageScale.c
WASM_OUT = index.js

web:
	$(WASM_CC) $(WASM_SRC) -o $(WASM_OUT) $(WASM_FLAGS)
	@echo "\n[OK] Type 'python3 -m http.server [server number]' to run the web!"

clean-web:
	rm -f index.js index.wasm
	@echo "\n[OK] Finished cleaning!"