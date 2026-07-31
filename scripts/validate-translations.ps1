$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $PSScriptRoot
$englishFile = Join-Path $projectRoot "translations\boss_slayer.phrases.txt"
$chineseFile = Join-Path $projectRoot "translations\chi\boss_slayer.phrases.txt"
$translationFiles = @($englishFile, $chineseFile)

function Get-PhraseKeys {
    param([string]$Path)

    $keys = New-Object System.Collections.Generic.List[string]

    foreach ($line in [IO.File]::ReadAllLines($Path)) {
        if ($line -match '^\s{4}"([^"]+)"\s*$') {
            $keys.Add($Matches[1])
        }
    }

    return $keys.ToArray()
}

function Get-PhraseMap {
    param(
        [string]$Path,
        [string]$Language
    )

    $phrases = [ordered]@{}
    $currentKey = $null
    $languagePattern = '^\s*"' + [regex]::Escape($Language) + '"\s+"(.*)"\s*$'

    foreach ($line in [IO.File]::ReadAllLines($Path)) {
        if ($line -match '^\s{4}"([^"]+)"\s*$') {
            $currentKey = $Matches[1]
            $phrases[$currentKey] = ""
            continue
        }

        if ($currentKey -and $line -match $languagePattern) {
            $phrases[$currentKey] = $Matches[1]
        }
    }

    return $phrases
}

function Expand-PhraseSample {
    param(
        [string]$Template,
        [object[]]$Values
    )

    $expanded = $Template.Replace("%%", "%")

    for ($index = 0; $index -lt $Values.Count; $index++) {
        $argument = $index + 1
        $value = [string]$Values[$index]

        $token = "{" + $argument + "}"
        $expanded = $expanded.Replace($token, $value)
    }

    return $expanded
}

foreach ($file in $translationFiles) {
    if (-not (Test-Path -LiteralPath $file -PathType Leaf)) {
        throw "Translation file not found: $file"
    }

    $lines = [IO.File]::ReadAllLines($file)
    $firstContentLine = $lines | Where-Object { $_.Trim().Length -gt 0 } | Select-Object -First 1

    if ($firstContentLine.Trim() -ne '"Phrases"') {
        throw "Translation root must be `"Phrases`": $file"
    }

    $depth = 0

    for ($index = 0; $index -lt $lines.Length; $index++) {
        $line = $lines[$index]
        $trimmed = $line.Trim()

        if ($line -match '^\s*"[^"]+"\s*\{.*\}\s*$') {
            $lineNumber = $index + 1
            throw "Inline translation sections are not supported ($($file):$lineNumber)."
        }

        if (($line -match '^\s*"(en|chi)"\s*"') -and ($line -match '(?<!%)%(?!%)')) {
            $lineNumber = $index + 1
            throw "Literal percent signs must be escaped as %% ($($file):$lineNumber)."
        }

        if ($trimmed -eq '{') {
            $depth++
        }
        elseif ($trimmed -eq '}') {
            $depth--

            if ($depth -lt 0) {
                $lineNumber = $index + 1
                throw "Unexpected closing brace ($($file):$lineNumber)."
            }
        }
    }

    if ($depth -ne 0) {
        throw "Unbalanced translation braces: $file"
    }
}

$englishKeys = Get-PhraseKeys -Path $englishFile
$chineseKeys = Get-PhraseKeys -Path $chineseFile
$keyDifference = Compare-Object -ReferenceObject $englishKeys -DifferenceObject $chineseKeys

if ($keyDifference) {
    $details = $keyDifference | Out-String
    throw "English and Simplified Chinese phrase keys differ:`n$details"
}

$perkMenuSamples = @(
    [pscustomobject]@{ Key = "Firepower"; Max = 5; Values = @(10) },
    [pscustomobject]@{ Key = "Toughness"; Max = 4; Values = @(6) },
    [pscustomobject]@{ Key = "Boss Healing"; Max = 3; Values = @(10, 10) },
    [pscustomobject]@{ Key = "Melee Fury"; Max = 4; Values = @(8) },
    [pscustomobject]@{ Key = "Healing Boost"; Max = 4; Values = @(6) },
    [pscustomobject]@{ Key = "Medical Supply"; Max = 1; Values = @(180) },
    [pscustomobject]@{ Key = "Throwable Supply"; Max = 1; Values = @(180) },
    [pscustomobject]@{ Key = "Fate Reroll"; Max = 1; Values = @() },
    [pscustomobject]@{ Key = "Field Rescue"; Max = 4; Values = @(8) },
    [pscustomobject]@{ Key = "Ammo Reclamation"; Max = 3; Values = @(2) },
    [pscustomobject]@{ Key = "First Aid Feedback"; Max = 3; Values = @(5) },
    [pscustomobject]@{ Key = "Precision Hunter"; Max = 3; Values = @(1) },
    [pscustomobject]@{ Key = "Supply Conditioning"; Max = 1; Values = @(2, 10) },
    [pscustomobject]@{ Key = "Survival Rhythm"; Max = 2; Values = @(10, 30, 40) }
)

$languageFiles = @(
    [pscustomobject]@{ Path = $englishFile; Language = "en" },
    [pscustomobject]@{ Path = $chineseFile; Language = "chi" }
)

$maximumMenuBytes = 0

foreach ($languageFile in $languageFiles) {
    $phraseMap = Get-PhraseMap -Path $languageFile.Path -Language $languageFile.Language
    $itemTemplate = $phraseMap["Build Menu Item"]

    foreach ($sample in $perkMenuSamples) {
        $name = $phraseMap["Perk $($sample.Key) Name"]
        $shortTemplate = $phraseMap["Perk $($sample.Key) Short"]
        $shortText = Expand-PhraseSample -Template $shortTemplate -Values $sample.Values
        $displayText = Expand-PhraseSample `
            -Template $itemTemplate `
            -Values @($name, 0, $sample.Max, $shortText)
        $byteCount = [Text.Encoding]::UTF8.GetByteCount($displayText)

        if ($byteCount -gt $maximumMenuBytes) {
            $maximumMenuBytes = $byteCount
        }

        if ($byteCount -gt 63) {
            throw "Build menu text exceeds the safe 63-byte UTF-8 limit ($($languageFile.Language), $($sample.Key), $byteCount bytes): $displayText"
        }
    }
}

Write-Host "Translations validated: $($englishKeys.Count) synchronized phrase keys; longest build-menu item: $maximumMenuBytes/63 bytes."
