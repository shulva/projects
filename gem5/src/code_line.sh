#/bin/bash
find . -name '*.hh' -print0 | xargs -0 wc -l
find . -name '*.py' -print0 | xargs -0 wc -l
find . -name '*.cc' -print0 | xargs -0 wc -l
find . -name '*.s' -print0 | xargs -0 wc -l
