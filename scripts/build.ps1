$ErrorActionPreference = "Stop"

$configPath = Join-Path $PSScriptRoot "config.local.ps1"

if (-not (Test-Path -LiteralPath $configPath)) {
    throw "Missing scripts\config.local.ps1. Copy config.example.ps1 and configure local paths."
}

. $configPath

$projectRoot = Split-Path -Parent $PSScriptRoot
$sourceFile = Join-Path $projectRoot "src\boss_slayer.sp"
$projectInclude = Join-Path $projectRoot "include"
$distDirectory = Join-Path $projectRoot "dist"
$outputFile = Join-Path $distDirectory "boss_slayer.smx"

if (-not (Test-Path -LiteralPath $CompilerPath -PathType Leaf)) {
    throw "SourcePawn compiler not found: $CompilerPath"
}

if (-not (Test-Path -LiteralPath $SourceModInclude -PathType Container)) {
    throw "SourceMod include directory not found: $SourceModInclude"
}

if (-not (Test-Path -LiteralPath $sourceFile -PathType Leaf)) {
    throw "Plugin entry point not found: $sourceFile"
}

New-Item -ItemType Directory -Path $distDirectory -Force | Out-Null

Write-Host "Building Boss Slayer..."

& $CompilerPath `
    $sourceFile `
    "-i$SourceModInclude" `
    "-i$projectInclude" `
    "-o$outputFile"

if ($LASTEXITCODE -ne 0) {
    throw "Compilation failed. spcomp exit code: $LASTEXITCODE"
}

if (-not (Test-Path -LiteralPath $outputFile -PathType Leaf)) {
    throw "Compiler did not create output: $outputFile"
}

Write-Host "Build succeeded: $outputFile"
