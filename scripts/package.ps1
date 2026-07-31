$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $PSScriptRoot
$buildScript = Join-Path $PSScriptRoot "build.ps1"
$definitionsFile = Join-Path $projectRoot "include\boss_slayer\definitions.inc"
$compiledPlugin = Join-Path $projectRoot "dist\boss_slayer.smx"
$translationSource = Join-Path $projectRoot "translations"
$stagingRoot = Join-Path $projectRoot "dist\package"
$serverRoot = Join-Path $stagingRoot "left4dead2"
$pluginDestination = Join-Path $serverRoot "addons\sourcemod\plugins\boss_slayer.smx"
$translationDestination = Join-Path $serverRoot "addons\sourcemod\translations"

& $buildScript

$versionLine = Select-String -LiteralPath $definitionsFile -Pattern '#define BSR_VERSION "([^"]+)"'
if (-not $versionLine) {
    throw "Could not read BSR_VERSION from $definitionsFile"
}

$version = $versionLine.Matches[0].Groups[1].Value
$archivePath = Join-Path $projectRoot "dist\boss-slayer-v$version.zip"

$resolvedStagingRoot = [System.IO.Path]::GetFullPath($stagingRoot)
$expectedStagingParent = [System.IO.Path]::GetFullPath(
    (Join-Path $projectRoot "dist")
) + [System.IO.Path]::DirectorySeparatorChar

if (-not $resolvedStagingRoot.StartsWith(
    $expectedStagingParent,
    [System.StringComparison]::OrdinalIgnoreCase
)) {
    throw "Unsafe package staging path: $resolvedStagingRoot"
}

if (Test-Path -LiteralPath $resolvedStagingRoot) {
    Remove-Item -LiteralPath $resolvedStagingRoot -Recurse -Force
}

New-Item -ItemType Directory -Path (Split-Path -Parent $pluginDestination) -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $translationDestination "chi") -Force | Out-Null

Copy-Item -LiteralPath $compiledPlugin -Destination $pluginDestination -Force
Copy-Item `
    -LiteralPath (Join-Path $translationSource "boss_slayer.phrases.txt") `
    -Destination (Join-Path $translationDestination "boss_slayer.phrases.txt") `
    -Force
Copy-Item `
    -LiteralPath (Join-Path $translationSource "chi\boss_slayer.phrases.txt") `
    -Destination (Join-Path $translationDestination "chi\boss_slayer.phrases.txt") `
    -Force
Copy-Item -LiteralPath (Join-Path $projectRoot "README.md") -Destination $stagingRoot -Force
Copy-Item -LiteralPath (Join-Path $projectRoot "LICENSE") -Destination $stagingRoot -Force
Copy-Item -LiteralPath (Join-Path $projectRoot "CHANGELOG.md") -Destination $stagingRoot -Force
$docsDestination = Join-Path $stagingRoot "docs"
New-Item -ItemType Directory -Path $docsDestination -Force | Out-Null
Copy-Item `
    -Path (Join-Path $projectRoot "docs\*") `
    -Destination $docsDestination `
    -Recurse `
    -Force

Compress-Archive -Path (Join-Path $stagingRoot "*") -DestinationPath $archivePath -Force

Write-Host "Release package created: $archivePath"
