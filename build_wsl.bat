where /Q wsl
IF ERRORLEVEL 1 (
	@echo wsl not found
) ELSE (
	del /Q ".sconsign.dblite"
	wsl -d Ubuntu-18.04 bash build_linux.sh
	if errorlevel 1 ( echo Failed to compile Linux godot-cpp for x64. Code: %errorlevel% && exit /b %errorlevel% )
)