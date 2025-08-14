## Example: a typical script with several problems
## you can use https://www.shellcheck.net/ to check shell script

# ------------------------------- Correc code:
#!/bin/bash
## Example: a typical script with several problems
for f in *.m3u
do
  [[ -e  "$f" ]] || break
  grep -qi "hq.*mp3" "$f" \
    && echo -e "Playlist $f contains a HQ file in mp3 format"
done

# ------------------------------- Problematic code:
##!/bin/sh
#for f in $(ls *.m3u)
#do
#	grep -qi hq.*mp3 $f \
#	&& echo -e 'Playlist $f contains a HQ file in mp3 format'
#done
