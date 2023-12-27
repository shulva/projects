import webbrowser

import imapclient
import pyzmail
import bs4

# pdf-568

imap = imapclient.IMAPClient('imap.qq.com', ssl=True)
imap.login('1329068252@qq.com', 'yikypytdaarejcig')
imap.select_folder('INBOX', readonly=True)

list_uid = imap.search('SINCE 01-Sep-2023')  # ALL实在太多了

raw_message = imap.fetch(list_uid, ['BODY[]'])

for value in raw_message.values():
    if value is not None:
        try:
            message = pyzmail.PyzMessage.factory(value[b'BODY[]'])
            if message.html_part is not None:
                html_content = message.html_part.get_payload().decode(message.html_part.charset)
                bs4_object = bs4.BeautifulSoup(html_content, 'html.parser')

                unsubscribe = [bs4_object.select('a:contains("unsubscribe")'),
                               bs4_object.select('link:contains("unsubscribe")'),
                               bs4_object.select('a:contains("Unsubscribe")'),
                               bs4_object.select('link:contains("Unsubscribe")')]

                for i in range(len(unsubscribe)):
                    if len(unsubscribe[i]) != 0:
                        for j in range(len(unsubscribe[i])):
                            url = unsubscribe[i][j].get('href')
                            print(url)
                            webbrowser.open(url)

        except Exception as err:
            print("exception happened:" + str(err))

        unsubscribe = []

imap.logout()
