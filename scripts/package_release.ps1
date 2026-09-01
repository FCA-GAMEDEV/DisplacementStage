# ============================================================
#  package_release.ps1
#  Gera o pacote DisplacementStage-v1.0.0-x64.zip
#
#  NOTA: Usa -LiteralPath em todos os cmdlets de arquivo para
#  evitar que [] no nome do diretorio pai seja tratado como
#  wildcard pelo PowerShell.
#
#  Uso (a partir da raiz do projeto):
#    pwsh -ExecutionPolicy Bypass -File scripts\package_release.ps1
# ============================================================

param(
    [string]$Version = "v1.0.0"
)

$ErrorActionPreference = "Stop"

# -- Caminhos base -------------------------------------------
$root       = Split-Path -Parent $PSScriptRoot
$buildDir   = Join-Path $root "build"
$releaseDir = Join-Path $buildDir "Release"
$binDataDir = Join-Path $root "bin\data"     # fallback: dados canonicos do repositorio
$outName    = "DisplacementStage-$Version-x64"
$outDir     = Join-Path $root $outName
$zipPath    = Join-Path $root "$outName.zip"

Write-Host ""
Write-Host "=================================================="  -ForegroundColor Cyan
Write-Host "  DisplacementStage -- Empacotador de Release"       -ForegroundColor Cyan
Write-Host "  Versao : $Version"                                  -ForegroundColor Cyan
Write-Host "  Raiz   : $root"                                     -ForegroundColor Cyan
Write-Host "=================================================="  -ForegroundColor Cyan
Write-Host ""

# -- Verificar se o build Release existe ---------------------
$exePath = Join-Path $releaseDir "DisplacementStage.exe"
if (-not (Test-Path -LiteralPath $exePath)) {
    Write-Host "[ERRO] DisplacementStage.exe nao encontrado em:" -ForegroundColor Red
    Write-Host "       $exePath"                                  -ForegroundColor Red
    Write-Host "       Execute primeiro: cmake --build build --config Release" -ForegroundColor Yellow
    exit 1
}

# -- Limpar pasta de saida anterior --------------------------
if (Test-Path -LiteralPath $outDir) {
    Write-Host "[INFO] Removendo pasta anterior: $outName" -ForegroundColor Yellow
    Remove-Item -LiteralPath $outDir -Recurse -Force
}
if (Test-Path -LiteralPath $zipPath) {
    Write-Host "[INFO] Removendo zip anterior: $outName.zip" -ForegroundColor Yellow
    Remove-Item -LiteralPath $zipPath -Force
}

New-Item -ItemType Directory -Path $outDir | Out-Null
Write-Host "[OK]   Pasta de saida criada: $outDir" -ForegroundColor Green

# -- Copiar executavel ---------------------------------------
Copy-Item -LiteralPath $exePath -Destination $outDir
Write-Host "[OK]   DisplacementStage.exe copiado" -ForegroundColor Green

# -- Copiar DLLs de terceiros --------------------------------
$dlls = @("glfw3.dll", "glew32.dll")
foreach ($dll in $dlls) {
    $src = Join-Path $releaseDir $dll
    if (Test-Path -LiteralPath $src) {
        Copy-Item -LiteralPath $src -Destination $outDir
        Write-Host "[OK]   $dll copiado" -ForegroundColor Green
    } else {
        Write-Host "[AVISO] $dll nao encontrado em build\Release\" -ForegroundColor Yellow
    }
}

# -- Copiar pasta data/ com estrutura correta ----------------
# Para cada subpasta, tenta primeiro build\Release\data\,
# depois bin\data\ como fallback (fonte canonica do repositorio).
$dataSubfolders = @("fonts", "kernels", "shaders", "textures", "dmap")
$dstDataRoot    = Join-Path $outDir "data"
New-Item -ItemType Directory -Path $dstDataRoot | Out-Null

foreach ($sub in $dataSubfolders) {
    $srcInBuild = Join-Path $releaseDir "data\$sub"
    $srcInBin   = Join-Path $binDataDir $sub
    $dstSub     = Join-Path $dstDataRoot $sub

    # Escolher fonte: build/Release/data/ tem prioridade; bin/data/ e fallback
    if (Test-Path -LiteralPath $srcInBuild) {
        $srcSub = $srcInBuild
        $label  = "build\Release\data\$sub"
    } elseif (Test-Path -LiteralPath $srcInBin) {
        $srcSub = $srcInBin
        $label  = "bin\data\$sub (fallback)"
    } else {
        $srcSub = $null
        $label  = ""
    }

    if ($srcSub) {
        Copy-Item -LiteralPath $srcSub -Destination $dstDataRoot -Recurse

        # Remover arquivos de backup (nome contem " - ", ex: "arquivo - Copia.cl")
        $backups = Get-ChildItem -LiteralPath $dstSub -Recurse -File |
                   Where-Object { $_.Name -like "* - *" }
        foreach ($bk in $backups) {
            Remove-Item -LiteralPath $bk.FullName -Force
            Write-Host "[INFO] Excluido backup: $($bk.Name)" -ForegroundColor Yellow
        }
        Write-Host "[OK]   data\$sub\ copiado de $label" -ForegroundColor Green
    } else {
        if ($sub -eq "dmap") {
            # dmap: criar vazia para gravacoes em runtime (nao e erro)
            New-Item -ItemType Directory -Path $dstSub | Out-Null
            Write-Host "[OK]   data\dmap\ criada vazia (para gravacoes em runtime)" -ForegroundColor Green
        } else {
            Write-Host "[AVISO] data\$sub\ nao encontrado em build nem em bin\data\" -ForegroundColor Yellow
        }
    }
}

# -- Copiar LEIA-ME.txt --------------------------------------
$leiaMe = Join-Path $root "LEIA-ME.txt"
if (Test-Path -LiteralPath $leiaMe) {
    Copy-Item -LiteralPath $leiaMe -Destination $outDir
    Write-Host "[OK]   LEIA-ME.txt copiado" -ForegroundColor Green
}

# -- Compactar em .zip ---------------------------------------
Write-Host ""
Write-Host "[...] Compactando $outName.zip ..." -ForegroundColor Cyan

# Compress-Archive tem problemas com colchetes em caminhos.
# Usamos a API .NET ZipFile que aceita caminhos literais sem interpretacao de wildcards.
Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::CreateFromDirectory($outDir, $zipPath,
    [System.IO.Compression.CompressionLevel]::Optimal, $true)

Write-Host "[OK]   Zip gerado: $zipPath" -ForegroundColor Green

# -- Resumo final --------------------------------------------
$zipSizeMB = [math]::Round((Get-Item -LiteralPath $zipPath).Length / 1MB, 2)
Write-Host ""
Write-Host "=================================================="            -ForegroundColor Cyan
Write-Host "  Pacote pronto!  $outName.zip  ($zipSizeMB MB)"              -ForegroundColor Green
Write-Host "=================================================="            -ForegroundColor Cyan
Write-Host ""
Write-Host "Proximos passos:"
Write-Host "  1. Teste: extraia o .zip em outra pasta e execute DisplacementStage.exe"
Write-Host "  2. Acesse: https://github.com/FCA-GAMEDEV/DisplacementStage/releases/new"
Write-Host "  3. Tag: $Version | Titulo: DisplacementStage $Version (Windows x64)"
Write-Host "  4. Anexe: $outName.zip"
Write-Host "  5. Publique!"
Write-Host ""
