#How to scrape businesses’ info with Python and Beautiful Soup

我们将使用Beautiful Soup python软件包来获取有关阿姆斯特丹市中心“咖啡店”所在位置的信息。

##关于虚拟环境的注意事项

建立虚拟环境，我们将使用 [virtualenvwrapper](https://virtualenvwrapper.readthedocs.org/en/latest/)。 首先，安装并配置。

    $ source ~/.bash_profile

或者重启终端。下面是一些常用的命令。

###虚拟环境的常用命令

| 命令 | 描述 |
| ------ | ------ |
|mkvirtualenv | 为项目创建一个新的虚拟环境|
|workon | 列出或更改为现有虚拟环境|
|lsvirtualenv | 显示可用的所有虚拟环境|
|showvirtualenv | 显示有关当前虚拟环境的详细信息|
|rmvirtualenv | 删除已有虚拟环境|
|deactivate | 关闭虚拟环境 |

##创建项目和虚拟环境

创建一个新的文件夹，用于存放项目和建立虚拟环境。在该虚拟环境下，安装Beautiful Soup web抓取包。

    $ mkdir web_scraper && cd web_scraper
    $ touch scraper.py requirements.txt
    $ mkvirtualenv coffeeshops

第一次创建虚拟环境后，使用

    $ workon coffeeshops

转换到虚拟环境`“coffeeshops”`中。可用`“lsvirtualenv”`列出所有可用的虚拟环境。

### 安装依赖包

`requirements.txt`文件将包含为了运行而需要安装的所有软件包。 要在requirements.txt文件中安装所有列出的包，请运行

    $ pip install -r requirements.txt

但是现在我们的`requirements.txt`文件完全是空的！ 第一步将安装`Beautiful Soup`包：

    $ pip install beautifulsoup4

>注意： 这儿使用了pip安装和管理python包。其实也可以使用其它工具easy_install或iPython。

安装软件包后，我们必须将已安装的软件包添加到requirements.txt。 在node.js中有“ - save”选项。 这个命令有点像python：

    $ pip freeze > requirements.txt

##抓取咖啡店位置

我们将抓取 http://www.coffeeshopdirect.com/Map.html 这个网页上的咖啡店信息。 创建文件scraper.py，并运行如下代码：

    $ python scraper.py

###代码

    from bs4 import BeautifulSoup
    import urllib
    import json

    r = urllib.urlopen(‘http://www.coffeeshop.freeuk.com/Map.html').read()
    soup = BeautifulSoup(r, ‘html.parser’)
    # Grab all the links
    coffeeshops = []
    for link in soup.find_all(‘area’):
     coffeeshop = {}
     # Link for coffeshop
     coffeeshop[‘full_link’] = ‘http://www.coffeeshop.freeuk.com/' + link.get(‘href’)

     # Http to link
     coffeeshop_site = urllib.urlopen(coffeeshop[‘full_link’]).read()
     coffeeshop_soup = BeautifulSoup(coffeeshop_site, ‘html.parser’)
     # Check that title is there
     title_elm = coffeeshop_soup.select(‘.goldBig’)

    # Make sure the title element selector exists
    if title_elm is not None and len(title_elm) > 0:
     coffeeshop[‘title’] = coffeeshop_soup.select(‘.goldBig’)[0].get_text()
    # Select the iFrame url
    coffeeshop[‘iframe_url’] = coffeeshop_soup.select(‘#iCr > iframe’)[0].get(‘src’)

     # Add to coffeeshop to list
     coffeeshops.append(coffeeshop)
     print(coffeeshop)
    print(coffeeshops)
    # Write to JSON document
    with open(‘coffeeshops.json’, ‘w’) as outfile:
     json.dump(coffeeshops, outfile)
    # Prettify JSON output
    # https://jsonformatter.curiousconcept.com/

[完整的代码](https://github.com/cleechtech/coffeeshopsinamsterdam/blob/master/web_scraper/scraper.py)。 运行此脚本后，你将会得到阿姆斯特丹市所有咖啡店的信息，输出格式是JSON文件格式。