@echo off

FOR /F "tokens=*" %%G IN ('DIR /B .\*.h') DO (
"SourceTools\uncrustify.exe" -f "%%G" -c "SourceTools\uncrustify.cfg" -o indentoutput.tmp
move /Y indentoutput.tmp "%%G" >nul
)

FOR /F "tokens=*" %%G IN ('DIR /B .\*.cpp') DO (
"SourceTools\uncrustify.exe" -f "%%G" -c "SourceTools\uncrustify.cfg" -o indentoutput.tmp
move /Y indentoutput.tmp "%%G" >nul
)
