# 
from html.parser import HTMLParser
import re

class MyHTMLParser(HTMLParser):
    smap = {'p': '',
            'code' : '',
            'font' : '',
            'i' : '',
            'small' : '',
            'sub' : '_',
            'table' : '\\tabular{llll}{',
            'td' : '',
            'th' : '\\bold{',
            'tr' : '',
            }

    emap = {'p': '',
            'code' : '',
            'font' : '',
            'i' : '',
            'small' : '',
            'sub' : '',
            'table' : '} \n',
            'td' : '',
            'th' : '}',
            'tr' : ' \\cr',
            }

    def __init__(self):
        super().__init__()
        self.a_tag = False
        self.tr_tag = False
        
    def handle_starttag(self, tag, attrs):
        if tag == 'a':
            self.a_tag = True
            if attrs[0][0] == 'href': 
                print('(see below)', end='')
            else:
                if not attrs[0][0] == 'name':
                    raise TypeError("Link tag error")
        elif tag == 'tr':
            self.tr_tag = True
        elif tag == 'td' or tag == 'th':
            if self.tr_tag:
                self.tr_tag = False
                print(self.smap[tag], end='')
            else:
                print(f' \\tab {self.smap[tag]}', end='')
        else:
            print(self.smap[tag], end='')

    def handle_endtag(self, tag):
        if tag == 'a':
            self.a_tag = False
        else:
            print(self.emap[tag], end='')

    def handle_data(self, data):
        if re.search("Grayed out expressions are not supported", data):
            return
        if self.a_tag and not data == '(link)':
            raise TypeError("Link error")
        if (data):
            print(data.replace("\\", "\\\\")
                  .replace("%", "\%")
                  .replace("{", "\{")
                  .replace("}", "\}")
                  .replace("≡", "="), end='')

parser = MyHTMLParser()
    
with open("RE2Syntax.md") as fp:
    data = fp.read()

print("""% Generated file <Girish Palya>
\\name{re2_syntax}
\\alias{re2_syntax}
\\alias{re2_regular_expressions_syntax}
\\title{RE2 Regular Expression Syntax}
\\description{""")
parser.feed(data)
print("}\n")
