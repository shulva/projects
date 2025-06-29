from langchain.prompts.prompt import PromptTemplate
from langchain.prompts.few_shot import FewShotPromptTemplate
from langchain.prompts.example_selector import LengthBasedExampleSelector

from langchain.prompts import ChatPromptTemplate,HumanMessagePromptTemplate,SystemMessagePromptTemplate
from langchain.prompts import AIMessagePromptTemplate,FewShotChatMessagePromptTemplate
from langchain.prompts.prompt import PromptTemplate


#字符串提示模板
def string_prompt(prompt:str)->str:
    prompt_city = PromptTemplate(input_variables=["city"],
    template="简单介绍一下{city}这个城市")
    return prompt_city.format(city=prompt)

#少样本字符串提示模板
def few_shot_prompt(prompt:str):  #函数参数自己定义，这里只是示范
    examples = [
    {
        "question": "从一副标准扑克牌中随机抽取一张牌，计算抽到红桃的概率。",
        "answer": "概率为 1/4。"
    },
    {
        "question": "在一组人中，60%是女性。如果随机选择一个人，计算她是女性并且是左撇子的概率，已知左撇子的概率为 10%。",
        "answer": "概率为 6%。"
    },
    {
        "question": "一枚硬币被抛两次，计算至少一次出现正面的概率。",
        "answer": "概率为 3/4。"
    },
    {
        "question": "在一批产品中，90%是正常的，10%有缺陷。如果一个产品有缺陷，被检测到的概率是95%。计算一个被检测为有缺陷的产品实际上是有缺陷的概率。",
        "answer": "概率为 32/143。"
    },
    {
        "question": "一个骰子被掷 3 次，计算得到至少一次 6 的概率。",
        "answer": "概率为 91/216。"
    },
    {
        "question": "在 0 到 1 的区间内均匀分布的随机变量 X，计算 X 小于 0.3 的概率。",
        "answer": "概率为 0.3。"
    },
    {
        "question": "一枚硬币被抛 5 次，计算正面朝上的次数为 3 的概率。",
        "answer": "概率为 10/32。"
    },
    {
        "question": "考试成绩近似服从正态分布，平均分为 70 分，标准差为 10 分。计算得分高于 80 分的学生的概率。",
        "answer": "概率为约 15.87%。"
    }
        ]


    example_prompt = PromptTemplate(input_variables=["question","answer"], template="Question: {question}\n{answer}")
    example_prompt.format(**examples[0])

    '''
    在使用few_shot_prompt_template构造 prompt 时，先使用example_prompt将examples格式化为字符串，默认情况下示例之间使用换行符（\n\n）隔开，我们暂时先将这部分格式化后的字符串称为 formatted_examples_str。

    prefix和suffix分别表示拼接在formatted_examples_str前面和后面的模板内容，接收 input_variables 里的变量，生成具体的字符串。

    也就是说，最终的 prompt = prefix + formatted_examples_str + suffix，各部分之间默认使用换行符（\n\n）隔开。
    '''

    few_shot_prompt = FewShotPromptTemplate(examples=examples,
                                            example_prompt=example_prompt,
                                            prefix = "You are an assistant good at {field}, learn the below examples and then answer the last question",
                                            suffix = "Question:{question}",
                                            input_variables=["field","question"]
                                            )

    few_shot_prompt.format(field = "math",question ="1+1=?")

'''
由于模型对单次的 token 数量有限制，如果有示例集很大，将全量提示集加入最终的 prompt 中，可能会导致 token 数量溢出。同时，如果将一些与问题相关性较低的示例加入到 prompt 中，也会影响模型的回复质量。

所以，更好的做法是根据提出的问题筛选出最相关的若干示例样本加到 prompt 中。LangChain 提供了ExampleSelector 来实现这个效果。
'''

def chat_model_prompt()->str:
    chat_prompt = ChatPromptTemplate.from_messages(
        [
            SystemMessagePromptTemplate.from_template("你是一个专业的翻译助手，擅长将{source_lang}翻译成{dest_lang}, 对输入的文本进行翻译")
            HumanMessagePromptTemplate.from_template("{text}")
        ]
    )

    final_prompt = chat_prompt.format(source_lang="中文", dest_lang="英语",text="快乐编程")
    return final_prompt

def chat_model_few_shot_prompt()->str:
    examples = [{"input":"2+2","output":"4"},{"input":"2+3","output":"5"},]
    example_prompt = ChatPromptTemplate.from_messages(
        [
            HumanMessagePromptTemplate.from_template("{input}"),
            SystemMessagePromptTemplate.from_template("{output}")
        ]
    )

    # 创建FewShotChatMessagePromptTemplate实例
    few_shot_prompt = FewShotChatMessagePromptTemplate(
        example_prompt=example_prompt,
        examples=examples
    )

    final_prompt = ChatPromptTemplate.from_messages(
        SystemMessagePromptTemplate(content = "you are a math teacher"),
        few_shot_prompt,
        ("human","{input}")
    )

def pipeline_prompt()->str:

    full_template ="""
    {introduction}
    {example}
    {start}
    """

    full_prompt = PromptTemplate.from_template(full_template)

#---------------------------------------------------------------------
    introduction_template = "you are a {person}"
    introduction_prompt =  PromptTemplate.from_template(introduction_template)

    example_template ="""Here's an example of an interaction:
    Q: {example_q}
    A: {example_a}"""
    example_prompt = PromptTemplate.from_template(example_template)

    start_template ="""Now, do this as the prompt
    Q: {input}
    A:"""
    start_prompt = PromptTemplate.from_template(start_template)

    input_prompt= [
        ("introduction",introduction_prompt),
        ("example",example_prompt),
        ("start",start_prompt),
    ]

    pipeline_prompt = PipelinePromptTemplate(
        final_prompt=full_prompt,
        pipeline_prompt=input_prompt
    )

    pipeline_prompt.format(
        person = "me",
        example_q = "what is your favorite car?",
        example_a = "BYD",
        input = "what is your favorite city?"
    )