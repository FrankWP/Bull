@echo off

set VS2008_DEVENV="C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe"
set BUILD_LOG_FILE=".\buildlog.txt"
set BUILD_TYPE=build
set CONFIG_NAME=Debug

if {%1%} == {} (
    echo δָ��������ʹ��Ĭ��ֵ��
) else (
    if {%2%} == {} goto usage_error

    if /i "%1%" == "build" (
        set BUILD_TYPE=build
    ) else (
        if /i "%1%" == "rebuild" (set BUILD_TYPE=rebuild) else (goto usage_error)
    )

    if /i "%2%" == "Debug" (
        set CONFIG_NAME=Debug
    ) else (
        if /i "%2%" == "Release" (set CONFIG_NAME=Release) else (goto usage_error)
    )
)

echo ���������%BUILD_TYPE%, %CONFIG_NAME%

set SLN_01=wxWidgets-2.9.1\build\msw\wx_vc9.sln


if exist %BUILD_LOG_FILE% del %BUILD_LOG_FILE%

echo.

echo ���ڱ��룺%SLN_01% ...
%VS2008_DEVENV% %SLN_01% /%BUILD_TYPE% %CONFIG_NAME% /out %BUILD_LOG_FILE%



echo.
echo ������ɣ���鿴�ļ�%BUILD_LOG_FILE%������������Ƿ���ȷ��
echo.

xcopy /E /Y "wxWidgets-2.9.1\include\*" "..\window\include\"
xcopy /E /Y "wxWidgets-2.9.1\lib\*" "..\window\lib\"

set BUILD_LOG_FILE=".\buildlog-c.txt"
rem #if exist %BUILD_LOG_FILE% del %BUILD_LOG_FILE%

rem #echo ��������%SLN_01% ...

rem #%VS2008_DEVENV% %SLN_01% /clean %CONFIG_NAME% /out %BUILD_LOG_FILE%

goto end

rem ========================================================================

:usage_error

echo.
echo ʹ�÷�����
echo     %0% [build^|rebuild] [release^|debug]
echo.
echo ����ָ����������Ĭ��Ϊ��build, release
echo.

:end

PAUSE

