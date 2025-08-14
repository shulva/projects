#/bin/bash
marco()
{
echo "$(pwd)" > ~/memory.sh
echo "$(pwd)" 
}
polo()
{
cd "$(cat ~/memory.sh)"
}
