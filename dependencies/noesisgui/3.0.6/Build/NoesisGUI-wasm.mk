# GNU Make solution 

PROJECTS = NoesisApp Samples.IntegrationGLUT Samples.HelloWorld Samples.Buttons Samples.TicTacToe Samples.QuestLog Samples.Scoreboard

.SUFFIXES:
.PHONY: all clean help $(PROJECTS)

all: $(PROJECTS)

clean:
	@$(MAKE) --no-print-directory -C ..\Src\NoesisApp\Projects\wasm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\IntegrationGLUT\Projects\wasm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\HelloWorld\Projects\wasm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Buttons\Projects\wasm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\TicTacToe\Projects\wasm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\QuestLog\Projects\wasm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Scoreboard\Projects\wasm clean

help:
	@echo Usage: make [CONFIG=name] [V=1] [target]
	@echo CONFIGURATIONS:
	@echo - Debug
	@echo - Profile
	@echo - Release
	@echo TARGETS:
	@echo - all [default]
	@echo - clean
	@echo - NoesisApp
	@echo - Samples.IntegrationGLUT
	@echo - Samples.HelloWorld
	@echo - Samples.Buttons
	@echo - Samples.TicTacToe
	@echo - Samples.QuestLog
	@echo - Samples.Scoreboard

NoesisApp: 
	@$(MAKE) --no-print-directory -C ..\Src\NoesisApp\Projects\wasm

Samples.IntegrationGLUT: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\IntegrationGLUT\Projects\wasm

Samples.HelloWorld: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\HelloWorld\Projects\wasm

Samples.Buttons: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Buttons\Projects\wasm

Samples.TicTacToe: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\TicTacToe\Projects\wasm

Samples.QuestLog: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\QuestLog\Projects\wasm

Samples.Scoreboard: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Scoreboard\Projects\wasm

