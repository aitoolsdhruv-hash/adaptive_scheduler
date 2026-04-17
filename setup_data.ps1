# Adaptive Task Scheduler - Data Environment Setup
# Generates a standard mock environment for 8-thread testing

$DataDir = "data"

if (!(Test-Path $DataDir)) {
    New-Item -ItemType Directory -Path $DataDir
}

Write-Host ">>> Generating I/O Files (source_0 to source_7)..." -ForegroundColor Cyan
for ($i = 0; $i -lt 8; $i++) {
    $filePath = Join-Path $DataDir "source_$i.bin"
    if (!(Test-Path $filePath)) {
        # Create 50MB dummy file
        fsutil file createnew $filePath 52428800 | Out-Null
    }
}

Write-Host ">>> Generating Search Folders (folder_0 to folder_99)..." -ForegroundColor Cyan
for ($i = 0; $i -lt 100; $i++) {
    $folderPath = Join-Path $DataDir "folder_$i"
    if (!(Test-Path $folderPath)) {
        New-Item -ItemType Directory -Path $folderPath | Out-Null
        
        # Randomly place a target file in some folders
        if ((Get-Random -Minimum 0 -Maximum 10) -eq 1) {
            New-Item -ItemType File -Path (Join-Path $folderPath "target.txt") -Value "Match found!" | Out-Null
        } else {
            New-Item -ItemType File -Path (Join-Path $folderPath "other.txt") -Value "Nothing here." | Out-Null
        }
    }
}

Write-Host ">>> Generating Mock /proc hierarchy..." -ForegroundColor Cyan
$ProcDir = Join-Path $DataDir "proc"
if (!(Test-Path $ProcDir)) { New-Item -ItemType Directory -Path $ProcDir }

for ($i = 101; $i -le 110; $i++) {
    $pidDir = Join-Path $ProcDir $i.ToString()
    if (!(Test-Path $pidDir)) { 
        New-Item -ItemType Directory -Path $pidDir | Out-Null 
        $statusPath = Join-Path $pidDir "status"
        $statusContent = "Name: MockProcess`nState: R (running)`nPID: $i`nMemory: $(Get-Random -Min 100 -Max 2000) kB"
        Set-Content -Path $statusPath -Value $statusContent
    }
}

Write-Host "`n[SUCCESS] Environment setup complete. You can now test up to 8 threads and 100 workload units." -ForegroundColor Green
