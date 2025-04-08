@echo off

for /f %%p in ('cd') do set _currentpath=%%p

echo %_currentpath%
cd ..\..
for /f %%p in ('cd') do set _ngpcpath=%%p
cd %_currentpath%

echo %_ngpcpath%

path=%PATH%;%_ngpcpath%\t900\bin
SET THOME=%_ngpcpath%\t900\
PROMPT $P$_NGPC$G
make
if ERRORLEVEL 1 GOTO debug
copy *.ngp D:\Development\Ahchay.com\ahchay.com\NGPC\Projects
for %%g in (*.ngp) do %%g

goto end
:debug
:end