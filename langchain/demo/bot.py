from random import choice
from langchain.prompts.prompt import PromptTemplate
from langchain_core.prompts import ChatPromptTemplate
from langchain_chroma import Chroma
from langchain_huggingface.embeddings import HuggingFaceEmbeddings
from langchain_core.output_parsers import StrOutputParser
from langchain_core.runnables import RunnablePassthrough
from langchain_openai import ChatOpenAI,OpenAI
from fastapi import FastAPI
from langserve import add_routes
from langchain_core.runnables import RunnableParallel
from langchain.output_parsers import DatetimeOutputParser,PydanticOutputParser
# 迁移到 Pydantic V2 的导入
from pydantic import BaseModel, Field, field_validator # <--- 修改这里，使用 field_validator

from langchain_core.utils.function_calling import convert_to_openai_function
from langchain.output_parsers.openai_functions import JsonOutputFunctionsParser
from langchain_core.messages import HumanMessage
from openai import OpenAI
import json


import os
os.environ['HF_ENDPOINT'] = 'https://hf-mirror.com/'

#---------------------------------------------------------------------------

class Joke(BaseModel):
    setup: str = Field(description="question to set up a joke")
    punchline: str = Field(description="answer to resolve the joke")

    # 使用 Pydantic V2 的 field_validator
    @field_validator("setup")
    @classmethod # field_validator 修饰的通常是类方法
    def question_ends_with_question_mark(cls, value: str) -> str: # 参数通常是 value
        if not value.endswith("?"):
            raise ValueError("Badly formed question!")
        return value

output_parser = PydanticOutputParser(pydantic_object=Joke)

prompt_template = PromptTemplate(
    template = "Tell me a Joke\n{format_instructions}",
    input_variables = [],
    partial_variables={"format_instructions": output_parser.get_format_instructions()},
)

embedding_function = HuggingFaceEmbeddings(
    model_name="sentence-transformers/all-MiniLM-L6-v2"
)

vectorstore = Chroma(persist_directory="./chroma_db", embedding_function=embedding_function)

retriever = vectorstore.as_retriever(search_type='similarity', search_kwargs={'k': 3})


#function calling---------------------------------------------------

def get_weather(date, location):
    print("after query，{}{}'s weather is 20℃".format(date, location))
    return "20℃"

deepseek_api_key = os.environ["DEEPSEEK_API_KEY"]
llm = OpenAI(
    base_url='https://api.deepseek.com/v1',
    api_key=deepseek_api_key,
)

tools = [
    {
        "type": "function",
        "function": {
            "name": "get_weather",
            "description": "Get the weather for a specified location on a specified date",
            "parameters": {
                "type": "object",
                "properties": {
                    "location": {
                        "type": "string",
                        "description": "The city and state, e.g. 北京"
                    },
                    "date": {
                        "type": "string",
                        "description": "the date to get weather, e.g. 2024-01-01"
                    }
                },
                "required": ["location", "date"]
            }
        }
    }
]

messages = []
messages.append({"role": "user", "content": "what's the xian's weather like in 2024-01-01"})

completion =llm.chat.completions.create(
  model="deepseek-chat",
  messages=messages,
  tools = tools
)

args = json.loads(completion.choices[0].message.tool_calls[0].function.arguments)
location = args["location"]
date = args["date"]
temperature = get_weather(date, location)

messages.append(    {
        "role": "assistant", 
        "tool_calls":[
            {
                "id": completion.choices[0].message.tool_calls[0].id,
                "type": "function", # 通常是 "function"
                "function": {
                    "name": completion.choices[0].message.tool_calls[0].function.name,
                    "arguments": completion.choices[0].message.tool_calls[0].function.arguments
                }
            }
        ]
    })

messages.append(
    {
        "role": "tool",
        "tool_call_id":completion.choices[0].message.tool_calls[0].id,
        "name":completion.choices[0].message.tool_calls[0].function.name,
        "content":temperature
    }
)

completion2 =llm.chat.completions.create(
  model="deepseek-chat",
  messages=messages,
  tools = tools
)

print(completion2.choices[0].message)

'''
RunnableParallel ----------------------------------------------------------------

chain2 = ChatPromptTemplate.from_template("write a 2-line joke about {topic}") | llm 
chain3 = ChatPromptTemplate.from_template("write a 3-line poem about {topic}") | llm 

combo_chain = RunnableParallel(
    joke=chain2,
    poem=chain3
)
output = combo_chain.invoke({"topic": "bear"})
print(output)

--------------------------------------------------------------------------
rag_chain = (
        prompt_template.format(question = "today is ?")
        | llm
        | output_parser
)

app = FastAPI(
    title="agent",
    version="0.1.0"
)

add_routes(
    app,
    rag_chain,
    path='/consumer_ai',
)

if __name__ == '__main__':
    import uvicorn

    uvicorn.run(app, host="localhost", port=8000)
'''