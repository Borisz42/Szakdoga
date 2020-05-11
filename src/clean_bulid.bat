:: Author: Alle

:: DELETE directories
:: The option /d /r is undocumented, but can be a useful combination, the wildcard will only match against Folder/Directory names (not filenames).
:: /r  Recurse into subfolders (see notes below)
:: /s 	Delete specified files from all subdirectories.
:: /q 	Quiet mode, do not ask if ok to delete on global wildcard

FOR /d /r . %%d in (x64, obj, Debug, ipch, .vs, FractalCollision.tlog) DO @if exist "%%d" rd /s/q "%%d"

:: DELETE files
:: /F  Ignore read-only setting and delete anyway (FORCE) 
:: /S  Delete from all Subfolders (DELTREE)
DEL /s /f *.sdf
DEL /s /f *.VC.db
DEL /s /f /AH *.suo
DEL /s /f *.log
DEL /s /f *.iobj
DEL /s /f *.ipdb
DEL /s /f *.pdb
DEL /s /f *.res
