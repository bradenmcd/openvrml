<html><head><title>Alpha Channel Test</title></head><body bgcolor="#88a">
<?
$dir = opendir(".");
while($fileName = readdir($dir)) 
if (!(strcmp(substr($fileName, -4), ".png")))
    print("<img src=\"" . urlencode($fileName) . "\" alt=\"$fileName\" />\n");
?>
</body></html>
