@REM @echo off
@REM setlocal enabledelayedexpansion

@REM :: 计算当前目录的总大小（单位：字节）
@REM for /f "tokens=3" %%a in ('dir /s /-c ^| find "个文件"') do set size=%%a

@REM :: 去除逗号（如果有）
@REM set size=%size:,=%

@REM :: 计算200MB的字节数
@REM set /a limit=200*1024*1024

@REM :: 比较大小
@REM if %size% GTR %limit% (
@REM     echo 当前目录大小超过200MB，按回车键程序退出。
@REM     pause
@REM     exit /b
@REM )

@REM echo 当前目录大小未超过200MB，继续执行。
@REM :: 在此处添加你想执行的命令


del *.bak /s
del *.ddk /s
del *.edk /s
del *.lst /s
del *.lnp /s
del *.mpf /s
del *.mpj /s
del *.obj /s
del *.omf /s
:: 不允许删除JLINK的设置
del *.opt /s  

del *.plg /s
del *.rpt /s
del *.tmp /s
del *.__i /s
del *.crf /s
del *.o /s
del *.d /s
del *.axf /s
del *.tra /s
del *.dep /s           
del JLinkLog.txt /s

del *.iex /s
del *.htm /s
del *.sct /s
del *.map /s
exit
