# GNU Make solution 

PROJECTS = NoesisApp Gui.XamlPlayer Samples.Integration Samples.HelloWorld Samples.Buttons Samples.Login Samples.QuestLog Samples.Scoreboard Samples.DataBinding Samples.ApplicationTutorial Samples.Touch Samples.Commands Samples.UserControl Samples.CustomControl Samples.BlendTutorial Samples.Menu3D Samples.Localization Samples.Inventory Samples.TicTacToe Samples.Gallery

.SUFFIXES:
.PHONY: all clean help $(PROJECTS)

all: $(PROJECTS)

clean:
	@$(MAKE) --no-print-directory -C ..\Src\NoesisApp\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Gui\XamlPlayer\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Integration\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\HelloWorld\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Buttons\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Login\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\QuestLog\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Scoreboard\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\DataBinding\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\ApplicationTutorial\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Touch\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Commands\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\UserControl\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\CustomControl\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\BlendTutorial\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Menu3D\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Localization\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Inventory\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\TicTacToe\Projects\android_arm clean
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Gallery\Projects\android_arm clean

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
	@echo - Gui.XamlPlayer
	@echo - Samples.Integration
	@echo - Samples.HelloWorld
	@echo - Samples.Buttons
	@echo - Samples.Login
	@echo - Samples.QuestLog
	@echo - Samples.Scoreboard
	@echo - Samples.DataBinding
	@echo - Samples.ApplicationTutorial
	@echo - Samples.Touch
	@echo - Samples.Commands
	@echo - Samples.UserControl
	@echo - Samples.CustomControl
	@echo - Samples.BlendTutorial
	@echo - Samples.Menu3D
	@echo - Samples.Localization
	@echo - Samples.Inventory
	@echo - Samples.TicTacToe
	@echo - Samples.Gallery

NoesisApp: 
	@$(MAKE) --no-print-directory -C ..\Src\NoesisApp\Projects\android_arm

Gui.XamlPlayer: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Gui\XamlPlayer\Projects\android_arm

Samples.Integration: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Integration\Projects\android_arm

Samples.HelloWorld: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\HelloWorld\Projects\android_arm

Samples.Buttons: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Buttons\Projects\android_arm

Samples.Login: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Login\Projects\android_arm

Samples.QuestLog: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\QuestLog\Projects\android_arm

Samples.Scoreboard: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Scoreboard\Projects\android_arm

Samples.DataBinding: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\DataBinding\Projects\android_arm

Samples.ApplicationTutorial: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\ApplicationTutorial\Projects\android_arm

Samples.Touch: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Touch\Projects\android_arm

Samples.Commands: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Commands\Projects\android_arm

Samples.UserControl: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\UserControl\Projects\android_arm

Samples.CustomControl: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\CustomControl\Projects\android_arm

Samples.BlendTutorial: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\BlendTutorial\Projects\android_arm

Samples.Menu3D: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Menu3D\Projects\android_arm

Samples.Localization: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Localization\Projects\android_arm

Samples.Inventory: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Inventory\Projects\android_arm

Samples.TicTacToe: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\TicTacToe\Projects\android_arm

Samples.Gallery: NoesisApp
	@$(MAKE) --no-print-directory -C ..\Src\Packages\Samples\Gallery\Projects\android_arm

