#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright 2016 Google Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Spatial Media Metadata Injector 

Tool for examining and injecting spatial media metadata in MP4/MOV files.
"""

import os
import re
import sys

path = os.path.dirname(sys.modules[__name__].__file__)
path = os.path.join(path, '..')
sys.path.insert(0, path)
from spatialmedia import metadata_utils

class person:
    def __init__(self):
        self.crop = None
        self.file = ['', '']
        self.inject = False
        self.spatial_audio = False
        self.stereo_mode = 'none'

def console(contents):
  print(contents)


def main():
  console("kaokao ")
  args = person()
  if len(sys.argv) > 2:
    args.inject = True
    args.file[0] = sys.argv[1]
    args.file[1] = sys.argv[2]
  else:
    args.inject = False
    args.file[0] = sys.argv[1]

  if args.inject:
    if len(args.file) != 2:
      console("Injecting metadata requires both an input file and output file.")
      return

    metadata = metadata_utils.Metadata()
    metadata.video = metadata_utils.generate_spherical_xml(args.stereo_mode,
                                                           args.crop)

    if args.spatial_audio:
      metadata.audio = metadata_utils.SPATIAL_AUDIO_DEFAULT_METADATA

    if metadata.video:
      metadata_utils.inject_metadata(args.file[0], args.file[1], metadata,
                                     console)
    else:
      console("Failed to generate metadata.")
    return

  if len(args.file) > 0:
    for input_file in args.file:
      if(input_file != ''):
        metadata_utils.parse_metadata(input_file, console)
    return
	
if __name__ == "__main__":
	main()