param([switch]$Open)

$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $PSScriptRoot
$validator = Join-Path $PSScriptRoot "validate-translations.ps1"
$englishFile = Join-Path $projectRoot "translations\boss_slayer.phrases.txt"
$chineseFile = Join-Path $projectRoot "translations\chi\boss_slayer.phrases.txt"
$distDirectory = Join-Path $projectRoot "dist"
$outputFile = Join-Path $distDirectory "translation-audit.html"

& $validator

function Read-PhraseCatalog {
    param(
        [string]$Path,
        [string]$Language
    )

    $phrases = [ordered]@{}
    $currentKey = $null
    $formatPattern = '^\s*"#format"\s+"(.*)"\s*$'
    $languagePattern = '^\s*"' + [regex]::Escape($Language) + '"\s+"(.*)"\s*$'

    foreach ($line in [IO.File]::ReadAllLines($Path)) {
        if ($line -match '^\s{4}"([^"]+)"\s*$') {
            $currentKey = $Matches[1]
            $phrases[$currentKey] = [pscustomobject]@{
                Format = ""
                Text = ""
            }
            continue
        }

        if (-not $currentKey) {
            continue
        }

        if ($line -match $formatPattern) {
            $phrases[$currentKey].Format = $Matches[1]
        }
        elseif ($line -match $languagePattern) {
            $phrases[$currentKey].Text = $Matches[1]
        }
    }

    return $phrases
}

function Get-DisplayText {
    param([string]$Text)

    return $Text.Replace("%%", "%").Replace('\n', "`n")
}

function Get-HtmlText {
    param([string]$Text)

    return [Net.WebUtility]::HtmlEncode((Get-DisplayText -Text $Text))
}

$english = Read-PhraseCatalog -Path $englishFile -Language "en"
$chinese = Read-PhraseCatalog -Path $chineseFile -Language "chi"
$rows = New-Object Text.StringBuilder
$index = 0

foreach ($key in $english.Keys) {
    $index++
    $englishPhrase = $english[$key]
    $chinesePhrase = $chinese[$key]
    $formatMatches = $englishPhrase.Format -eq $chinesePhrase.Format
    $hasReplacementCharacter = $englishPhrase.Text.Contains([char]0xFFFD) -or $chinesePhrase.Text.Contains([char]0xFFFD)
    $status = if (-not $formatMatches) {
        "FORMAT MISMATCH"
    }
    elseif ($hasReplacementCharacter) {
        "INVALID CHARACTER"
    }
    elseif (-not $englishPhrase.Text -or -not $chinesePhrase.Text) {
        "MISSING TEXT"
    }
    else {
        "OK"
    }
    $statusClass = if ($status -eq "OK") { "ok" } else { "error" }
    $englishDisplay = Get-DisplayText -Text $englishPhrase.Text
    $chineseDisplay = Get-DisplayText -Text $chinesePhrase.Text
    $englishBytes = [Text.Encoding]::UTF8.GetByteCount($englishDisplay)
    $chineseBytes = [Text.Encoding]::UTF8.GetByteCount($chineseDisplay)

    [void]$rows.AppendLine("<tr data-search='$(Get-HtmlText -Text "$key $($englishPhrase.Text) $($chinesePhrase.Text)")'>")
    [void]$rows.AppendLine("<td>$index</td>")
    [void]$rows.AppendLine("<td class='key'>$(Get-HtmlText -Text $key)</td>")
    [void]$rows.AppendLine("<td class='format'>$(Get-HtmlText -Text $englishPhrase.Format)</td>")
    [void]$rows.AppendLine("<td>$(Get-HtmlText -Text $englishPhrase.Text)<span class='bytes'>$englishBytes bytes</span></td>")
    [void]$rows.AppendLine("<td>$(Get-HtmlText -Text $chinesePhrase.Text)<span class='bytes'>$chineseBytes bytes</span></td>")
    [void]$rows.AppendLine("<td class='$statusClass'>$status</td>")
    [void]$rows.AppendLine("</tr>")
}

$generatedAt = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
$html = @"
<!doctype html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Boss Slayer Translation Audit</title>
<style>
body { margin: 0; background: #111827; color: #e5e7eb; font-family: "Segoe UI", "Microsoft YaHei", sans-serif; }
main { max-width: 1500px; margin: 0 auto; padding: 24px; }
h1 { margin: 0 0 8px; }
p { color: #cbd5e1; }
input { width: 100%; box-sizing: border-box; margin: 16px 0; padding: 12px; border: 1px solid #475569; border-radius: 8px; background: #0f172a; color: #fff; font-size: 16px; }
table { width: 100%; border-collapse: collapse; background: #1f2937; }
th, td { padding: 10px; border: 1px solid #374151; text-align: left; vertical-align: top; white-space: pre-wrap; }
th { position: sticky; top: 0; background: #0f172a; z-index: 1; }
.key, .format { font-family: Consolas, monospace; }
.bytes { display: block; margin-top: 6px; color: #94a3b8; font-size: 12px; }
.ok { color: #86efac; font-weight: 700; }
.error { color: #fca5a5; font-weight: 700; }
.hidden { display: none; }
</style>
</head>
<body>
<main>
<h1>Boss Slayer 全量翻译审校</h1>
<p>共 $($english.Count) 个 phrase，生成于 $generatedAt。页面会把 <code>%%</code> 显示为游戏中的单个 <code>%</code>，把 <code>\n</code> 显示为换行；格式参数保留为 <code>{1}</code> 供核对。</p>
<input id="search" type="search" placeholder="搜索 key、英文或中文……">
<table>
<thead><tr><th>#</th><th>Phrase key</th><th>#format</th><th>English</th><th>简体中文</th><th>检查</th></tr></thead>
<tbody>
$rows
</tbody>
</table>
</main>
<script>
const input = document.getElementById('search');
const rows = Array.from(document.querySelectorAll('tbody tr'));
input.addEventListener('input', () => {
  const query = input.value.trim().toLowerCase();
  rows.forEach(row => row.classList.toggle('hidden', !row.dataset.search.toLowerCase().includes(query)));
});
</script>
</body>
</html>
"@

New-Item -ItemType Directory -Path $distDirectory -Force | Out-Null
[IO.File]::WriteAllText($outputFile, $html, (New-Object Text.UTF8Encoding($false)))

Write-Host "Translation audit created: $outputFile"

if ($Open) {
    Start-Process -FilePath $outputFile
}
