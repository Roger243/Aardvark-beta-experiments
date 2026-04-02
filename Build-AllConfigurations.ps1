param(
    [string]$Project = ".\Sample.vcxproj",
    [string]$ConfigurationList = "Debug,Release",
    [string]$PlatformList = "Win32,x64,ARM,ARM64"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path $Project)) {
    throw "Project file not found: $Project"
}

$msbuild = Get-Command msbuild -ErrorAction SilentlyContinue
if (-not $msbuild) {
    throw "MSBuild not found in PATH. Use a Visual Studio Developer PowerShell/Command Prompt."
}

$configurations = $ConfigurationList.Split(',') | ForEach-Object { $_.Trim() } | Where-Object { $_ }
$platforms = $PlatformList.Split(',') | ForEach-Object { $_.Trim() } | Where-Object { $_ }

$failures = @()

foreach ($configuration in $configurations) {
    foreach ($platform in $platforms) {
        Write-Host "`n=== Building $configuration|$platform ===" -ForegroundColor Cyan

        $args = @(
            $Project,
            '/t:Build',
            "/p:Configuration=$configuration",
            "/p:Platform=$platform",
            '/m',
            '/v:m'
        )

        & $msbuild.Source @args
        if ($LASTEXITCODE -ne 0) {
            $failures += "$configuration|$platform"
        }
    }
}

if ($failures.Count -gt 0) {
    Write-Host "`nBuild failures:" -ForegroundColor Red
    $failures | ForEach-Object { Write-Host " - $_" -ForegroundColor Red }
    exit 1
}

Write-Host "`nAll configurations built successfully." -ForegroundColor Green
exit 0
