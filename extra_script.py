print("====Before_Build====")
Import("env")
import gzip
import shutil
import os
import subprocess
try:
    import configparser
except ImportError:
    import ConfigParser as configparser
import pip


def install_package(package):
    subprocess.call(["pip","install","--upgrade",package])
install_package("html_utils_becothal")
from html_utils import HTML


def before_uploadfs():
    print("----uploadfs----")
    environment = "env:esp8285"
    config = configparser.ConfigParser()
    config.read("platformio.ini")
    if config.get(environment,"web_ui_path") == "": 
        print("web_ui_path is not set")
        return
    inlinedWebUI = config.get(environment,"web_ui_path")[:config.get(environment,"web_ui_path").find(".html")] + "_inlined.html"
    spiff_dir = config.get(environment,"spiff_dir")
    html_file = HTML()
    html_file.read_file(config.get(environment,"web_ui_path"))
    html_file.remove_comments("", "<!--", "-->")
    html_file.inline_css()
    html_file.inline_js()
    html_file.remove_comments("", "<!--", "-->")
    html_file.images_to_base64()
    #html_file.uglify() // Cannot be used. 
    html_file.write_file(inlinedWebUI)
    if not os.path.exists(spiff_dir):
        os.mkdir(spiff_dir)
    with open(inlinedWebUI,"rb") as f_in, gzip.open(spiff_dir + "index.html.gz","wb") as f_out:
        shutil.copyfileobj(f_in,f_out) 
    os.remove(inlinedWebUI)

print("Current build targets", map(str, BUILD_TARGETS)) 
if("uploadfs" in BUILD_TARGETS or "buildfs" in BUILD_TARGETS):
    before_uploadfs()
else: 
    print("No extraScript for this Target")



#env.AddPreAction("uploadfs",before_buildfs)

