# SPDX-License-Identifier: GPL-3.0-or-later
# Static guard for source references left behind by the slim crypto/key-manager removal.

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$scanRoots = @(
    (Join-Path $repoRoot 'src'),
    (Join-Path $repoRoot 'CMakeModules'),
    (Join-Path $repoRoot 'CMakeLists.txt')
)
$extensions = @('.c', '.cc', '.cpp', '.cxx', '.h', '.hh', '.hpp', '.hxx', '.cmake', '.txt')

$rules = @(
    @{
        Name = 'include of a removed crypto/loader header'
        Pattern = '^\s*#\s*include\s*[<"][^">]*(?:key_manager|partition_data_manager|xts_encryption_layer|xts_archive|nax)\.h[">]'
    },
    @{
        Name = 'reference to a removed key-manager/partition API type'
        Pattern = '\b(?:KeyManager|PartitionDataManager|Package2Type|SHA256Hash|SignatureType|TitleKeyType|TicketData|RSA4096Ticket|RSA2048Ticket|ECDSATicket|RSAKeyPair|KeyCategory|S128KeyType|S256KeyType|KeyIndex)\b'
    },
    @{
        Name = 'reference to a removed key-derivation/ticket API'
        Pattern = '\b(?:GetSignatureTypeDataSize|GetSignatureTypePaddingSize|GenerateKeyEncryptionKey|DeriveKeyblobKey|DeriveKeyblobMACKey|DeriveMasterKey|DecryptKeyblob|DeriveSDSeed|DeriveSDKeys|GetTicketblob|GetBISKey)\b'
    },
    @{
        Name = 'obsolete key-file setup path'
        Pattern = '(?i)\b(?:prod|title)\.keys\b'
    },
    @{
        Name = 'CMake reference to a removed translation unit'
        Pattern = '(?i)(?:key_manager|partition_data_manager|xts_encryption_layer|xts_archive|nax)\.(?:cpp|h)\b'
    }
)

$files = [System.Collections.Generic.List[System.IO.FileInfo]]::new()
foreach ($root in $scanRoots) {
    if (-not (Test-Path -LiteralPath $root)) {
        continue
    }

    $item = Get-Item -LiteralPath $root
    if ($item.PSIsContainer) {
        foreach ($file in Get-ChildItem -LiteralPath $root -Recurse -File) {
            if ($extensions -contains $file.Extension.ToLowerInvariant()) {
                $files.Add($file)
            }
        }
    }
    elseif ($extensions -contains $item.Extension.ToLowerInvariant()) {
        $files.Add($item)
    }
}

$findings = [System.Collections.Generic.List[string]]::new()
foreach ($file in $files) {
    $relativePath = [System.IO.Path]::GetRelativePath($repoRoot, $file.FullName).Replace('\', '/')
    $lineNumber = 0

    foreach ($line in [System.IO.File]::ReadLines($file.FullName)) {
        $lineNumber++

        foreach ($rule in $rules) {
            if ($line -match $rule.Pattern) {
                $findings.Add(('{0}:{1}: [{2}] {3}' -f $relativePath, $lineNumber, $rule.Name, $line.Trim()))
            }
        }
    }
}

$removedPaths = @(
    'src/core/crypto/key_manager.cpp',
    'src/core/crypto/key_manager.h',
    'src/core/crypto/partition_data_manager.cpp',
    'src/core/crypto/partition_data_manager.h',
    'src/core/crypto/xts_encryption_layer.cpp',
    'src/core/crypto/xts_encryption_layer.h',
    'src/core/file_sys/xts_archive.cpp',
    'src/core/file_sys/xts_archive.h',
    'src/core/loader/nax.cpp',
    'src/core/loader/nax.h'
)
foreach ($path in $removedPaths) {
    if (Test-Path -LiteralPath (Join-Path $repoRoot $path)) {
        $findings.Add(('{0}: removed source file unexpectedly exists' -f $path))
    }
}

Write-Host "Slim source audit scanned $($files.Count) source/build files."
if ($findings.Count -gt 0) {
    Write-Host 'Potential stale references found:'
    $findings | ForEach-Object { Write-Host "  $_" }
    throw "Slim source audit failed with $($findings.Count) finding(s)."
}

Write-Host 'Slim source audit passed: no removed key-manager, partition-manager, legacy loader, or CMake references remain.'
