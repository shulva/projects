echo "password:" > password.txt
git add .
git commit -m "..."

git -rm --cached password.txt
git commit --amend -CHEAD
