$searchdir = $args[0]
$ext = $args[1]

$files = Get-ChildItem -Filter "*.$ext" -Recurse $searchdir

foreach($file in $files)
{
    echo $file.FullName
}