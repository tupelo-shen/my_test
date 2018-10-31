#!/usr/bin/python
# -*- coding: utf-8 -*-

from bs4 import BeautifulSoup
import urllib
import json

r = urllib.urlopen('www.coffeeshopdirect.com/Map.html').read()
soup = BeautifulSoup(r, 'html.parser')


# Grab all the links
coffeeshops = []
for link in soup.find_all('area'):
    coffeeshop = {}

    # Link for coffeshop
    coffeeshop['full_link'] = 'http://www.coffeeshop.freeuk.com/' + \
        link.get('href')

    # Http to link
    coffeeshop_site = urllib.urlopen(coffeeshop['full_link']).read()
    coffeeshop_soup = BeautifulSoup(coffeeshop_site, 'html.parser')

    # Check that title is there
    title_elm = coffeeshop_soup.select('.goldBig')

    if title_elm is not None and len(title_elm) > 0:
        coffeeshop['title'] = coffeeshop_soup.select('.goldBig')[0].get_text()

    coffeeshop['iframe_url'] = \
        coffeeshop_soup.select('#iCr > iframe')[0].get('src')

    # Add to coffeeshop to list
    coffeeshops.append(coffeeshop)
    print(coffeeshop)

print(coffeeshops)

# Write to JSON document
with open('coffeeshops.json', 'w') as outfile:
    json.dump(coffeeshops, outfile)

# Prettify JSON output
# https://jsonformatter.curiousconcept.com/
