#/bin/bash
find . -name '*.h' -print0 | xargs -0 wc -l
find . -name '*.sh' -print0 | xargs -0 wc -l
find . -name '*.c' -print0 | xargs -0 wc -l
find . -name '*.s' -print0 | xargs -0 wc -l
