执行git stash后出现了:
Saved working directory and index state WIP on main: 9caf2d8 readme change
执行git log --all -- oneline后:可以看到顶部有两个
a41b480 (refs/stash) WIP on main: 9caf2d8 readme change
30f8936 index on main: 9caf2d8 readme change
git stash pop之后就没有了
git stash 的用途:[Example-git stash](../../../zk/zk/2-Tools/2-b%20(Git).md)
