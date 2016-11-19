test -e /test/file/path
[ -e /test/file/path ]
test -e /test/file/path || echo WORLD
echo hello || [ -e /test/file/path ]
echo world && echo hello && [ -f /test/file/path ]
[ -d /test/file/path ] || echo hello || world
