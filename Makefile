#code written by Ztirom45
#LICENCE: GPL4

all:
	pio run -t upload
	pio device monitor

build:
	pio run
monitor:
	pio device monitor
lsp:
	#create compile_commands with:
	pio run -t compiledb
