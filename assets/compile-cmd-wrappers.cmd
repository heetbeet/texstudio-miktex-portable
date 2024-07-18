<# :
    @echo off
    set "_cmd=%~f0"
    powershell -nologo -nop -exec bypass "iex (Get-Content '%_cmd%' -Raw)"
    goto :EOF
#>

# Make sure we have access to tcc.exe
$tempFolderPath = Join-Path $Env:Temp "tcc-latent-path"
if (Test-Path -Path "$tempFolderPath/tcc/tcc.exe" -PathType Leaf) {
    # we already have tcc
} else {
    # We have to download tcc
    New-Item -Type Directory -Force -Path $tempFolderPath | Out-Null
    Invoke-WebRequest "http://download.savannah.gnu.org/releases/tinycc/tcc-0.9.27-win64-bin.zip" -OutFile "$tempFolderPath\tcc-0.9.27-win64-bin.zip"
    Expand-Archive -Path "$tempFolderPath\tcc-0.9.27-win64-bin.zip" -DestinationPath "$tempFolderPath"
}

$tempBuild = Join-Path $Env:Temp "Portable-MikTex-and-TexStudio-wrappers"
Remove-Item -Recurse -Force $tempBuild -ErrorAction Ignore
New-Item -Type Directory -Force -Path $tempBuild | Out-Null

$thisdir = Split-Path $Env:_cmd
& "$tempFolderPath/tcc/tcc.exe" "$thisdir/cmd-wrapper.c" -luser32 -lkernel32 -o "$tempBuild/MiKTeX.exe"
& "$tempFolderPath/tcc/tcc.exe" -DNOSHELL "$thisdir/cmd-wrapper.c" -luser32 -lkernel32 -o "$tempBuild/TeXstudio.exe"

Write-Host ""
#Write-Host "Compiled output: $tempBuild/MiKTeX.exe"
Write-Host "Compiled output: $tempBuild/TeXstudio.exe"

& explorer $tempBuild
