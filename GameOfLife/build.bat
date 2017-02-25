@echo off

cl -nologo -Zi main.c -Fegameoflife.exe -Iw:/SoftwareRenderer user32.lib gdi32.lib opengl32.lib -link -SUBSYSTEM:CONSOLE