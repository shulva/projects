from langchain_text_splitters import CharacterTextSplitter
from langchain_text_splitters import RecursiveCharacterTextSplitter


splitter1 = CharacterTextSplitter(
    chunk_size=1,
    chunk_overlap=0
)
 
text = '666666\n\n333\n\n22'
print(splitter1.split_text(text))

splitter = RecursiveCharacterTextSplitter(
    chunk_size=5,
    chunk_overlap=1
)

# 下面text是一段测试RecursiveCharacterTextSplitter的文本
text = "This is a test text for the RecursiveCharacterTextSplitter. It is a long text with many words."
print(splitter.split_text(text))

