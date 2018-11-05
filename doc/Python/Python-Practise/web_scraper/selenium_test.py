# -*- coding: utf-8 -*-

from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from bs4 import BeautifulSoup
from docx import Document
from docx.enum.text import WD_ALIGN_PARAGRAPH   # 用来居中显示标题
import sys
import time
from imp import reload

print(sys.getdefaultencoding())

BROWSER_PATH = \
    'C:\Program Files (x86)\Google\Chrome\Application\chromedriver.exe'
DEST_URL = "https://wenku.baidu.com/view/aa31a84bcf84b9d528ea7a2c.html"

doc_content_list = []


def find_doc(driver, i=1):
    html = driver.page_source
    soup1 = BeautifulSoup(html, 'lxml')
    title_result = soup1.find('div', attrs={'class': 'doc-title'})
    doc_title = title_result.get_text()  # 得到文档标题
    print(doc_title)

    try:
        elem = driver.find_element_by_xpath("/html/body/div[2]/div[2]/div[9]/div[3]/div[1]/div[2]/text()")
        elem.click()
        global doc_content_list
        doc_content_list = []
    except:
        pass

    content_result = soup1.find_all('p', attrs={'class': 'txt'})
    for each in content_result:
        each_text = each.get_text()
        if '            ' in each_text:
            text = each_text.replace('            ', '')
        else:
            text = each_text
        doc_content_list.append(text)  # 得到正文内容
        # print(doc_content_list)

    return doc_title, doc_content_list


def save(doc_title, doc_content_list):
    document = Document()
    heading = document.add_heading(doc_title, 0)
    heading.alignment = WD_ALIGN_PARAGRAPH.CENTER

    for each in doc_content_list:
        document.add_paragraph(each)

    # document.save(u'百度文库-%s.docx' % doc_title)
    document.save(u'1.docx')
    driver.quit()
    t_title = doc_title.decode('utf-8')
    print(u"\n\n已全部写入文件    %s.docx,   请查收。" % t_title)


if __name__ == '__main__':
    options = webdriver.ChromeOptions()
    options.add_argument('user-agent="Mozilla/5.0 (Linux; Android 4.0.4; \
        Galaxy Nexus Build/IMM76B) AppleWebKit/535.19 (KHTML, like Gecko) \
        Chrome/18.0.1025.133 Mobile Safari/535.19"')
    driver = webdriver.Chrome(BROWSER_PATH, chrome_options=options)
    driver.get(DEST_URL)

    m = 1
    title, content = find_doc(driver, m)
    save(title, content)
