import pprint
from bilibili_api import user, Credential,settings,search,sync
import asyncio
import webbrowser

settings.http_client = settings.HTTPClient.HTTPX
filename = "/Users/shulva/Desktop/eat.html"


async def Method1() -> None: #通过公共搜索关键字+up主名字的方式搜索

    print("请输入想要搜索的内容")
    info = input()

    CREDENTIAL = Credential(
        sessdata="54843f3b%2C1744168060%2C587ff%2Aa1CjBYJ43oO26w3q8iGFOmQXnMDCouhQQ4dWI_8mvSsGT3RxU4GYR_1G_klGXRy4gBgoESVl8zbjJqa0p2RGYwRWpiTlN4U2VWTGhWcUNBeVE1SjJoWHZDMmw5aDhzYVFlVlVMdHFSQVhVb2hjbGlRUzdGTHg0VHlnenpMZmNHanJMbm9BcWRENGZ3IIEC"
    )
    shulva = user.User(342277214, credential=CREDENTIAL)
    lists = await shulva.get_followings()
    file = open(filename, 'w')

    for up in lists["list"]:
        temp = info+' '+up["uname"]
        answer = sync(search.search_by_type(search_type=search.SearchObjectType.VIDEO,keyword=temp
                                            ,page=1))

        if 'result' in answer.keys():
            for video in answer['result']:
                if video['author'] == up['uname'] and video['typename'] =='美食制作' and info in video['title']:
                    print(video['author']+' '+video['arcurl']+' '+video['title']+' '+video['typename'])
                    dynamic_content = "<p>{author}: <a href=\"{url}\">{title}</a> </p>".format(author=video['author'],url=video['arcurl'],title=video['title'])
                    html_content = "<html><body>{content}</body></html>".format(content=dynamic_content)
                    file.write(html_content)

        await asyncio.sleep(1)

    file.close()
    webbrowser.open_new_tab('file://'+filename)

async def Method2() -> None: #获取关注的up主的投稿进行搜索，有352风控校验失败的问题没解决
    print("请输入想要搜索的内容")
    info = input()

    CREDENTIAL = Credential(
        sessdata="0de58bf5%2C1743874362%2C101be%2Aa1CjAhQQNY1sYyP1iSUPYsFCu5ki7eLgHmZ8FsRU1KGnLXZ5t61Hk3h2Ni7wocCldF0-wSVmptZDdYWmRjeE1sd3ZZWHA0WUxWMndGM05OV1JSOTM4RjFvdmN3OUFjZUQyNUJFRXkwcUdNd0J3bTJaY0U3cU9KYU5IanlmbmJoXzIwQlg4MVpSem1RIIEC"
    )
    shulva = user.User(342277214, credential=CREDENTIAL)
    lists = await shulva.get_followings()

    for up in lists["list"]:
        id = int(up['mid'])
        up = user.User(id)
        answer = await up.get_videos(keyword=info)
        print(answer)
        await asyncio.sleep(1)


if __name__ == '__main__':
    asyncio.run(Method1())
    print("查询完成！")

