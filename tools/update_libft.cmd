@echo off
setlocal

echo Updating Libft from its tracked branch...
git submodule update --init --remote --merge --recursive Libft
if errorlevel 1 exit /b %errorlevel%

echo Libft submodule status:
git submodule status --recursive Libft
