@echo off
rem +--------------------------------------------+
rem |         Batch Version Builder v1.2         |
rem |         Coding by Md5Chap                  |
rem |         Dedicated to the public            |
rem +--------------------------------------------+

VERIFY OTHER 2>nul
SETLOCAL ENABLEEXTENSIONS
IF NOT ERRORLEVEL 1 goto nextBLD
echo ERROR: This batch is only supported on WinNt>=5.0
goto endthis
:nextBLD
pushd %~dp0

set bmaj=1
set bmin=0
set brev=0
set bcus=0
Set bbui=0

if exist 0 goto :EOF

if exist version (
  for /F "tokens=1,2,3,4,5 delims=." %%i in (Version) do (
    set bmaj=%%i
    set bmin=%%j
    set brev=%%k
    set bcus=%%l
    Set bbui=%%m
  )
)

if "x%bmaj%x"=="xx" set bmaj=1
if "x%bmin%x"=="xx" set bmin=0
if "x%brev%x"=="xx" set brev=0
if "x%bcus%x"=="xx" set bcus=0
if "x%bbui%x"=="xx" set bbui=0

set /A bbui=bbui+1
(echo %bmaj%.%bmin%.%brev%.%bcus%.%bbui%)>Version

(echo #pragma once)>build.hpp

(echo #define __VER_IMAJOR %bmaj%)>>build.hpp
(echo #define __VER_VMAJOR %bmaj%)>>build.hpp
(echo #define __VER_SMAJOR "%bmaj%")>>build.hpp

(echo #define __VER_IMINOR %bmin%)>>build.hpp
(echo #define __VER_VMINOR %bmin%)>>build.hpp
(echo #define __VER_SMINOR "%bmin%")>>build.hpp

(echo #define __VER_IREVIS %brev%)>>build.hpp
(echo #define __VER_VREVIS %brev%)>>build.hpp
(echo #define __VER_SREVIS "%brev%")>>build.hpp

if X%bcus%X==X0X goto next1
(echo #define __VER_VCUSTM %bcus%)>>build.hpp
(echo #define __VER_SCUSTM "%bcus%")>>build.hpp
(echo #define __VER_SCUST2 " %bcus%")>>build.hpp
(echo #define __VER_SCUST3 "%bcus% ")>>build.hpp
(echo #define __VER_SCUST4 " %bcus% ")>>build.hpp
goto next2
:next1
(echo #define __VER_VCUSTM )>>build.hpp
(echo #define __VER_SCUSTM "")>>build.hpp
(echo #define __VER_SCUST2 "")>>build.hpp
(echo #define __VER_SCUST3 "")>>build.hpp
(echo #define __VER_SCUST4 "")>>build.hpp
:next2

(echo #define __VER_IBUILD %bbui%)>>build.hpp
(echo #define __VER_VBUILD %bbui%)>>build.hpp
(echo #define __VER_SBUILD "%bbui%")>>build.hpp

(echo #define __VER_IFULL %bmaj%,%bmin%)>>build.hpp
(echo #define __VER_VFULL %bmaj%.%bmin%)>>build.hpp
(echo #define __VER_SFULL "%bmaj%.%bmin%")>>build.hpp

(echo #define __VER_IFULLR %bmaj%,%bmin%,%brev%)>>build.hpp
(echo #define __VER_VFULLR %bmaj%.%bmin%.%brev%)>>build.hpp
(echo #define __VER_SFULLR "%bmaj%.%bmin%.%brev%")>>build.hpp

(echo #define __VER_IFULLRB %bmaj%,%bmin%,%brev%,%bbui%)>>build.hpp
(echo #define __VER_VFULLRB %bmaj%.%bmin%.%brev%.%bbui%)>>build.hpp
(echo #define __VER_SFULLRB "%bmaj%.%bmin%.%brev%.%bbui%")>>build.hpp

popd
:endthis
