@echo off

:: ------------------------------
:: 変数定義
:: ------------------------------
set TOOL=%CD%\..\..\Tool\FilePacker\FilePacker.exe
set SRC=%CD%\src
set DST_DIR=%CD%\..\..\data\image
set DST=%DST_DIR%\image.dat

:: ------------------------------
:: マスターデータ作成
:: ------------------------------
rmdir /S /Q %DST_DIR%
mkdir %DST_DIR%
%TOOL% -i %SRC% -o %DST%

pause
