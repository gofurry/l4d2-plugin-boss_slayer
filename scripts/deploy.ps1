$ErrorActionPreference = "Stop"

$configPath = Join-Path $PSScriptRoot "config.local.ps1"
$buildScript = Join-Path $PSScriptRoot "build.ps1"

if (-not (Test-Path -LiteralPath $configPath)) {
    throw "Missing scripts\config.local.ps1."
}

. $configPath

$projectRoot = Split-Path -Parent $PSScriptRoot
$compiledPlugin = Join-Path $projectRoot "dist\boss_slayer.smx"
$deployedPlugin = Join-Path $PluginDirectory "boss_slayer.smx"

& $buildScript

if (-not (Test-Path -LiteralPath $PluginDirectory -PathType Container)) {
    throw "SourceMod plugins directory not found: $PluginDirectory"
}

Copy-Item -LiteralPath $compiledPlugin -Destination $deployedPlugin -Force

$sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $compiledPlugin).Hash
$deployedHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $deployedPlugin).Hash

if ($sourceHash -ne $deployedHash) {
    throw "Deployed file checksum failed: $deployedPlugin"
}

Write-Host "Deploy succeeded: $deployedPlugin"
Write-Host "In the local development server run: sm plugins reload boss_slayer"
