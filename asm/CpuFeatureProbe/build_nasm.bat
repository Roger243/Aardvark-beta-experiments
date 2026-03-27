@echo off
setlocal

if "%~1"=="" (
  set OUTDIR=build
) else (
  set OUTDIR=%~1
)

if not exist "%OUTDIR%" mkdir "%OUTDIR%"

nasm -f win64 cpuid_info.asm -o "%OUTDIR%\cpuid_info.obj"
nasm -f win64 simd_check.asm -o "%OUTDIR%\simd_check.obj"
nasm -f win64 module.asm -o "%OUTDIR%\module.obj"

echo CpuFeatureProbe objects generated in %OUTDIR%
