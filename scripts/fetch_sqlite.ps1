# Robust download script for SQLite amalgamation
# Usage: .\scripts\fetch_sqlite.ps1
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$urls = @(
    'https://www.sqlite.org/2024/sqlite-amalgamation-3410200.zip',
    'https://www.sqlite.org/2023/sqlite-amalgamation-3410200.zip'
)
$destDir = Join-Path $PSScriptRoot '..\third_party\sqlite' | Resolve-Path -ErrorAction SilentlyContinue
if (-not $destDir) { $destDir = Join-Path $PSScriptRoot '..\third_party\sqlite' }
if (-not (Test-Path $destDir)) { New-Item -ItemType Directory -Path $destDir | Out-Null }
$zipPath = Join-Path $destDir 'sqlite-amalgamation.zip'
$success = $false
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
foreach ($url in $urls) {
    try {
        Write-Host "Attempting download: $url"
        Invoke-WebRequest -Uri $url -OutFile $zipPath -UseBasicParsing -TimeoutSec 120
        Write-Host "Downloaded to $zipPath"
        Add-Type -AssemblyName System.IO.Compression.FileSystem
        [System.IO.Compression.ZipFile]::ExtractToDirectory($zipPath, $destDir)
        Remove-Item $zipPath -Force
        Write-Host "Extracted into $destDir"
        $success = $true
        break
    } catch {
        $msg = if ($_.Exception) { $_.Exception.Message } else { $_.ToString() }
        Write-Warning ("Download failed for " + $url + ": " + $msg)
    }
}
if (-not $success) { throw "All downloads failed. Please download the SQLite amalgamation manually into $destDir" }
Write-Host "SQLite amalgamation available in $destDir"