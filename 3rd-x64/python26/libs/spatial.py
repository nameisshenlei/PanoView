
import os
import re
import sys

path = os.path.dirname(sys.modules[__name__].__file__)
path = os.path.join(path, '..')
sys.path.insert(0, path)
from spatialmedia import metadata_utils

global consolestr
consolestr = ""
def console(contents):
  #print("haohao")
  global consolestr
  consolestr = consolestr + contents
  #print(contents)

def inject(file1, file2):
  global consolestr
  consolestr = ""
  metadata = metadata_utils.Metadata()
  metadata.video = metadata_utils.generate_spherical_xml('none', None)
  #metadata.audio = metadata_utils.SPATIAL_AUDIO_DEFAULT_METADATA

  if metadata.video:
    metadata_utils.inject_metadata(file1, file2, metadata, console)
  else:
    console("failed")
  return consolestr

def detect(infile):
  global consolestr
  consolestr = ""
  metadata_utils.parse_metadata(infile, console)
  #print(consolestr)
  return consolestr
  

#main("E:/myshare/python/py2exe_test/callpython/test.mp4", "E:/myshare/python/py2exe_test/callpython/test2.mp4")