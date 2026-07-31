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
$translationSource = Join-Path $projectRoot "translations"
$translationDirectory = Join-Path $SourceModRoot "translations"
$englishTranslation = Join-Path $translationSource "boss_slayer.phrases.txt"
$chineseTranslation = Join-Path $translationSource "chi\boss_slayer.phrases.txt"
$deployedEnglishTranslation = Join-Path $translationDirectory "boss_slayer.phrases.txt"
$deployedChineseTranslation = Join-Path $translationDirectory "chi\boss_slayer.phrases.txt"

& $buildScript

if (-not (Test-Path -LiteralPath $PluginDirectory -PathType Container)) {
    throw "SourceMod plugins directory not found: $PluginDirectory"
}

if (-not (Test-Path -LiteralPath $englishTranslation -PathType Leaf)) {
    throw "English translation file not found: $englishTranslation"
}

if (-not (Test-Path -LiteralPath $chineseTranslation -PathType Leaf)) {
    throw "Simplified Chinese translation file not found: $chineseTranslation"
}

Copy-Item -LiteralPath $compiledPlugin -Destination $deployedPlugin -Force
New-Item -ItemType Directory -Path (Split-Path -Parent $deployedChineseTranslation) -Force | Out-Null
Copy-Item -LiteralPath $englishTranslation -Destination $deployedEnglishTranslation -Force
Copy-Item -LiteralPath $chineseTranslation -Destination $deployedChineseTranslation -Force

$sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $compiledPlugin).Hash
$deployedHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $deployedPlugin).Hash

if ($sourceHash -ne $deployedHash) {
    throw "Deployed file checksum failed: $deployedPlugin"
}

$translationPairs = @(
    @($englishTranslation, $deployedEnglishTranslation),
    @($chineseTranslation, $deployedChineseTranslation)
)

foreach ($pair in $translationPairs) {
    $sourceTranslationHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $pair[0]).Hash
    $deployedTranslationHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $pair[1]).Hash

    if ($sourceTranslationHash -ne $deployedTranslationHash) {
        throw "Deployed translation checksum failed: $($pair[1])"
    }
}

Write-Host "Deploy succeeded: $deployedPlugin"
Write-Host "Translations deployed: $translationDirectory"
Write-Host "In the local development server run: sm plugins reload boss_slayer"
Write-Host "After changing translations run: sm_reload_translations"
