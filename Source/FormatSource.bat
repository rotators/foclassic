@echo off

for %%D in (..\Dll ..\Dll\AngelScript .) do (
	for %%E in (h cpp fos) do (
		for %%F in (%%D\*.%%E) do (
			"SourceTools\uncrustify.exe" -c "SourceTools\uncrustify.cfg" -l CPP -f "%%F" -o FormatSource.tmp --if-changed
			if exist FormatSource.tmp (echo          FormatSource prevails. && move /Y FormatSource.tmp "%%F" >nul)
		)
	)
)
