$bro=$args[0]
$bro2=$args[1]
rm -r $bro2/BaseShaders
cp -r $bro/Application/Renderer/BaseShaders -Destination $bro2
cd $bro2/BaseShaders
$files = Get-ChildItem $bro2/BaseShaders

foreach($file in $files)
{
    $extn = [IO.Path]::GetExtension($file)
    if ($extn -eq ".glsl" ){
    $baseName = (Get-Item $file).Basename
    $spvName = $baseName + ".spv"
    glslangValidator -V $file -o  $spvName
    }
}

    

