import bs4
from langchain_community.document_loaders import WebBaseLoader
from langchain.text_splitter import RecursiveCharacterTextSplitter
from langchain_community.vectorstores import Chroma
from langchain_huggingface.embeddings import HuggingFaceEmbeddings

loader = WebBaseLoader(
    web_path="https://www.gov.cn/jrzg/2013-10/25/content_2515601.htm",
    bs_kwargs=dict(parse_only=bs4.SoupStrainer(
            class_=("p1")
        ))
)
docs = loader.load()
#print(docs)

'''
如果直接将整个原始文档加入上下文，容易超出模型的限制，而且模型在大量的输入中也比较难找到关键的信息，
所以，我们要将原始文档切割成多个小文档块
'''

text_splitter = RecursiveCharacterTextSplitter(chunk_size=1000, chunk_overlap=200)
splits = text_splitter.split_documents(docs)

embedding_function = HuggingFaceEmbeddings(
    model_name = "sentence-transformers/all-MiniLM-L6-v2"
)

# 向量化存储
db = Chroma.from_documents(documents=splits, embedding=embedding_function, persist_directory="./chroma_db")
print("vector db established")

