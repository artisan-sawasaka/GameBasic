@echo off

:: ------------------------------
:: 変数定義
:: ------------------------------
set TOOL=%CD%\..\Tool\AutoMaster\AutoMaster.exe
set XLSX_DIR=%CD%\master
set BINARY_DIR=%CD%\..\data\master


:: ------------------------------
:: バイナリディレクトリ生成
:: ------------------------------
if exist %BINARY_DIR% rmdir /S /Q %BINARY_DIR%
mkdir %BINARY_DIR%


:: ------------------------------
:: マスターデータ作成
:: ------------------------------
%TOOL% -x %XLSX_DIR% -b %BINARY_DIR%
