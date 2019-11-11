@echo off
:: For a window to open, a valid GL context needs to exist for the current session.
:: When you rd into a computer it doesn't copy over the gl context for whatever reason

:: attach remote session to the console

for /f "skip=1 tokens=3" %%s in ('query user %USERNAME%') do (
  %windir%\System32\tscon.exe %%s /dest:console
)

:: run program with the console's contexts 'n such

cd C:\dev\IwEngine\Sandbox\bin\Debug.windows.x86_64\
Sandbox.exe

:: then reconnect to the remote session