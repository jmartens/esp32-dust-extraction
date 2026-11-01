<#
.SYNOPSIS
  Containerised ESP-IDF workflow for Windows (PowerShell). Linux/macOS: use `make`.
.DESCRIPTION
  Runs idf.py in the pinned espressif/idf image (tag in scripts/idf-version).
  Build output lives in a named Docker volume, never in the host `build/` directory.
.EXAMPLE
  ./scripts/idf-docker.ps1 build
  ./scripts/idf-docker.ps1 test
  ./scripts/idf-docker.ps1 flash -Port 'rfc2217://host.docker.internal:4000?ign_set_control'
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory, Position = 0)]
    [ValidateSet('build', 'test', 'clean', 'shell', 'menuconfig', 'flash', 'monitor')]
    [string]$Command,
    [string]$Target = 'esp32',
    [string]$Port = '',
    [string]$Device = ''
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$version = (Get-Content (Join-Path $PSScriptRoot 'idf-version') -Raw).Trim()
$image = if ($env:IDF_IMAGE) { $env:IDF_IMAGE } else { "espressif/idf:$version" }
$buildVolume = "open-extraction-idf-build-$version"
$ccacheVolume = 'open-extraction-idf-ccache'

function Invoke-Docker {
    # Native stderr (e.g. git warnings) must not abort; rely on the exit code.
    $ErrorActionPreference = 'Continue'
    docker @args
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

function Invoke-Idf {
    param([string[]]$Run, [string]$WorkDir = '/project/firmware', [switch]$Interactive, [string[]]$Env = @())
    $dockerArgs = @('run', '--rm')
    if ($Interactive) { $dockerArgs += '-it' }
    $dockerArgs += @('-e', 'HOME=/tmp', '-e', 'GIT_CONFIG_COUNT=1', '-e', 'GIT_CONFIG_KEY_0=safe.directory', '-e', 'GIT_CONFIG_VALUE_0=*', '-e', 'IDF_CCACHE_ENABLE=1', '-e', 'CCACHE_DIR=/ccache')
    foreach ($e in $Env) { $dockerArgs += @('-e', $e) }
    $dockerArgs += @('-v', "${root}:/project", '-v', "${buildVolume}:/build", '-v', "${ccacheVolume}:/ccache")
    if ($Device) { $dockerArgs += @('--device', $Device) }
    $dockerArgs += @('-w', $WorkDir, $image)
    Invoke-Docker @dockerArgs @Run
}

$flashPort = if ($Port) { $Port } else { $Device }
$buildDir = "/build/$Target"

switch ($Command) {
    'build' {
        Invoke-Idf -Run @('idf.py', '-B', $buildDir, 'set-target', $Target)
        Invoke-Idf -Run @('idf.py', '-B', $buildDir, 'build')
    }
    'test' {
        Invoke-Idf -WorkDir '/project/firmware/test' -Env @('BUILD_DIR=/build/host-test') -Run @('bash', './run_coverage.sh')
    }
    'clean' { Invoke-Docker volume rm -f $buildVolume }
    'shell' { Invoke-Idf -Interactive -Run @('bash') }
    'menuconfig' { Invoke-Idf -Interactive -Run @('idf.py', '-B', $buildDir, 'menuconfig') }
    'flash' {
        if (-not $flashPort) { throw 'Pass -Port rfc2217://host.docker.internal:4000?ign_set_control (or -Device on Linux)' }
        Invoke-Idf -Run @('idf.py', '-B', $buildDir, '-p', $flashPort, 'flash')
    }
    'monitor' {
        if (-not $flashPort) { throw 'Pass -Port rfc2217://host.docker.internal:4000?ign_set_control (or -Device on Linux)' }
        Invoke-Idf -Interactive -Run @('idf.py', '-B', $buildDir, '-p', $flashPort, 'monitor')
    }
}
