@echo off

FOR /F "tokens=*" %%G IN ('DIR /B ..\Dll\*.h') DO (
"SourceTools\uncrustify.exe" -f "..\Dll\%%G" -c "SourceTools\uncrustify.cfg" -o ..\Dll\indentoutput.tmp
move /Y ..\Dll\indentoutput.tmp "..\Dll\%%G" >nul
)

FOR /F "tokens=*" %%G IN ('DIR /B ..\Dll\*.cpp') DO (
"SourceTools\uncrustify.exe" -f "..\Dll\%%G" -c "SourceTools\uncrustify.cfg" -o ..\Dll\indentoutput.tmp
move /Y ..\Dll\indentoutput.tmp "..\Dll\%%G" >nul
)

FOR /F "tokens=*" %%G IN ('DIR /B ..\Dll\*.fos') DO (
"SourceTools\uncrustify.exe" -l CPP -f "..\Dll\%%G" -c "SourceTools\uncrustify.cfg" -o ..\Dll\indentoutput.tmp
move /Y ..\Dll\indentoutput.tmp "..\Dll\%%G" >nul
)

FOR /F "tokens=*" %%G IN ('DIR /B ..\Dll\AngelScript\*.h') DO (
"SourceTools\uncrustify.exe" -f "..\Dll\AngelScript\%%G" -c "SourceTools\uncrustify.cfg" -o ..\Dll\AngelScript\indentoutput.tmp
move /Y ..\Dll\AngelScript\indentoutput.tmp "..\Dll\AngelScript\%%G" >nul
)

FOR /F "tokens=*" %%G IN ('DIR /B .\*.h') DO (
"SourceTools\uncrustify.exe" -f "%%G" -c "SourceTools\uncrustify.cfg" -o indentoutput.tmp
move /Y indentoutput.tmp "%%G" >nul
)

FOR /F "tokens=*" %%G IN ('DIR /B .\*.cpp') DO (
"SourceTools\uncrustify.exe" -f "%%G" -c "SourceTools\uncrustify.cfg" -o indentoutput.tmp
move /Y indentoutput.tmp "%%G" >nul
)
