import logging
import traceback
import subprocess
import pyzmail
import imapclient
import smtplib
import pyautogui
import time
import datetime
import threading
import email
import email.parser
from email.header import Header,decode_header
from email.message import EmailMessage
from pathlib import Path
logging.basicConfig(filename='torrentStarterLog.txt', level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

my_email = '1329068252@qq.com'
my_password = 'yikypytdaarejcig'
imap_server = 'imap.qq.com'
bt_path = Path('C:\\Program Files\\BitComet\BitComet.exe')
cwd = Path.cwd()
now = datetime.datetime.now()
day_time = now.strftime('SINCE %d-%b-%Y')  #只接受日内的

message = EmailMessage()

message['Subject'] = 'Python 邮件自动工作'
message['From'] = "1329068252@qq.com"         #utf-8 会被加密，从而不符合qq邮箱的From Header要求
message['To'] = Header("worker", 'utf-8')     # 接收者

def email_back(file_name):
    smtp = smtplib.SMTP('smtp.qq.com', 587)
    smtp.starttls()
    smtp.login('1329068252@qq.com', 'yikypytdaarejcig')
    smtp.ehlo()

    message.set_content(file_name+" start")
    smtp.sendmail('1329068252@qq.com', '1329068252@qq.com', message.as_string())
    smtp.quit()


def parse_instruction(instruction_email, parse_message):
    lines = instruction_email
    if 'saierhao' in lines:
        print("password correct")

        for part in parse_message.walk():
            if part.get_content_disposition() is not None:

                file_name = part.get_filename()
                file_name, decode = decode_header(file_name)[0]
                print(decode)
                file_name = file_name.decode(decode) if decode else str(file_name)
                print("downloading file:"+file_name)

                attachment = part.get_payload(decode=True)
                file_path = cwd / Path(file_name)
                print(str(file_path))
                torrent = open(file_path, 'wb')
                torrent.write(attachment)

                download_torrent = subprocess.Popen([bt_path, file_path], shell=True)
                thread_one = threading.Thread(target=pyautogui.click, args=[1935, 1385], kwargs={'duration': 1})
                thread_one.start()

                thread_two = threading.Thread(target=email_back, kwargs={'file_name': file_name})
                thread_two.start()

def get_instruction_email():
    imap = imapclient.IMAPClient(imap_server, ssl=True)
    imap.login(my_email, my_password)
    imap.select_folder('INBOX')

    uid = imap.search(day_time)

    print("length of email is "+str(len(uid)))

    raw_message = imap.fetch(uid, 'RFC822')  # RFC822 是接受邮件全部内容,包括附件

    # pprint.pprint(raw_message)
    for value in raw_message.values():
        if value is not None:
            pyz_message = pyzmail.PyzMessage.factory(value[b'RFC822'])

            if pyz_message.html_part is not None:
                body = pyz_message.html_part.get_payload().decode(pyz_message.html_part.charset)
            if pyz_message.text_part is not None:
                # If there's both a html and text part, use the text part.
                body = pyz_message.text_part.get_payload().decode(pyz_message.text_part.charset)

            email_message = email.message_from_bytes(value[b'RFC822'])
            msg = email.parser.Parser().parsestr(email_message.as_string())
            parse_instruction(body, msg)

    print('mission complete')

    imap.logout()
    print("imap logout")


while True:
    print("searching instructions..")
    try:
        get_instruction_email()
    except Exception as err:
        logging.debug(traceback.format_exc())

    print('Done processing instructions. Pausing for 15 minutes.')
    time.sleep(60*15)
