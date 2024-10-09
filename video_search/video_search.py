from bilibili_api import user, Credential
import asyncio
import ssl

ssl._create_default_https_context = ssl._create_unverified_context

async def main() -> None:
    CREDENTIAL = Credential(
        sessdata="0de58bf5%2C1743874362%2C101be%2Aa1CjAhQQNY1sYyP1iSUPYsFCu5ki7eLgHmZ8FsRU1KGnLXZ5t61Hk3h2Ni7wocCldF0-wSVmptZDdYWmRjeE1sd3ZZWHA0WUxWMndGM05OV1JSOTM4RjFvdmN3OUFjZUQyNUJFRXkwcUdNd0J3bTJaY0U3cU9KYU5IanlmbmJoXzIwQlg4MVpSem1RIIEC"
    )
    shulva = user.User(342277214, credential=CREDENTIAL)
    lists = await shulva.get_followings()
    for man in lists["list"]:
        print(man["uname"])


if __name__ == '__main__':
    asyncio.run(main())
