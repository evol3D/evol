$from=$args[0]
$to=$args[1]
cp $from/subprojects/physics_debug/platform/windows/freeglut/bin/x64/freeglut.dll -Destination $to
echo "Copied DLLS successfully. from: $from to: $to ."