@echo off

:platform
if "%PROCESSOR_ARCHITECTURE%"=="x86" setdll32 /t:msedge.exe 2>nul
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" setdll64 /t:msedge.exe 2>nul
if "%errorlevel%"=="32" set bits=32&goto x86
if "%errorlevel%"=="64" set bits=64&goto x64
goto eof

:x86
@echo ***********************************************************************
@echo *          The program will automatically inject 32-bit browsers      *
@echo *                     Press any key to continue                       *
@echo ***********************************************************************
@echo *                     程序自动注入32位的浏览器                        *
@echo *                          按任意键继续                               *
@echo ***********************************************************************
echo+
@pause .
goto runing

:x64
echo+
@echo ***********************************************************************
@echo *          The program will automatically inject 64-bit browsers      *
@echo *                     Press any key to continue                       *
@echo ***********************************************************************
@echo *                    程序自动注入64位的浏览器                      *
@echo *                          按任意键继续                               *
@echo ***********************************************************************
echo+
@pause .

:runing
setdll%bits% /d:version%bits%.dll msedge.exe 2>nul
if "%errorlevel%"=="0" echo 成功！(Done) &goto eof
echo 失败！(Fail)

:eof
pause .
@del /s/q setdll*.exe 2>nul 1>nul
if "%bits%"=="32" del /s/q version64.dll 2>nul 1>nul
if "%bits%"=="64" del /s/q version32.dll 2>nul 1>nul