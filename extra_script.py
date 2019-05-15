print "====Before_Build===="
Import("env")
import gzip
import shutil 
from pip._internal import main as pipmain

def install_package(package):
    pipmain(["install",package])
install_package("html_utils_becothal")
from html_utils import HTML

website_root = "./TestHTML/"
website_index = "index"
website_index_joined = website_root + website_index + "_joined.html"
website_index += ".html"
def before_uploadfs():
    print "----uploadfs----"
    html_file = HTML()
    html_file.readFile(website_root,website_index)
    html_file.inlineCSS()
    html_file.writeFile(website_index_joined)
    with open(website_index_joined) as f_in, gzip.open("./data/index.html.gz","wb") as f_out:
        shutil.copyfileobj(f_in,f_out)
    del(website_index_joined)

print "Current build targets", map(str, BUILD_TARGETS) 
if("uploadfs" in BUILD_TARGETS):
    before_uploadfs()
else: 
    print "No extraScript for this Target"



#env.AddPreAction("uploadfs",before_buildfs)

