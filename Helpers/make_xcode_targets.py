from argparse import ArgumentParser
import os
import getpass
import re
from typing import List

USERNAME = getpass.getuser()
SCHEMES = os.path.join('xcuserdata', f'{USERNAME}.xcuserdatad', 'xcschemes', 'xcschememanagement.plist')

with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'SingleTarget.plist')) as template_file:
   SINGLE_TARGET_TEMPLATE = template_file.read()

with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'NoTargets.plist')) as template_file:
   NO_TARGETS_TEMPLATE = template_file.read()

with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'ExecutableTarget.xcscheme')) as template_file:
   SCHEME_TEMPLATE = template_file.read()

def remove_schemes(project: str):
   pass

def set_executable(project: str, exe: str):
   pass

def make_xcode_targets(project: str, executables: List[str]):
   for subdir in os.listdir(project):
      if '.xcodeproj' in subdir:
         name = os.path.splitext(subdir)[0]
         fbuild_uuid = None
         native_uuid = None

         with open(os.path.join(project, subdir, 'project.pbxproj')) as proj_file:
            lines = proj_file.readlines()

            state = ''
            for line in lines:
               if state == 'PBXLegacyTarget':
                  fbuild_uuid = re.match(r'\s*([0-9]{24}) /\*.*\*/ = \{', line).groups()[0]
                  state = ''
               elif state == 'PBXNativeTarget':
                  native_uuid = re.match(r'\s*([0-9]{24}) /\*.*\*/ = \{', line).groups()[0]
                  state = ''
               elif 'Begin PBXLegacyTarget section' in line:
                  state = 'PBXLegacyTarget'
               elif 'Begin PBXNativeTarget section' in line:
                  state = 'PBXNativeTarget'

         path = os.path.join(project, subdir, SCHEMES)
         with open(path, 'w') as schemes_file:
            if 'Dependencies' not in path:
               schemes_file.write(SINGLE_TARGET_TEMPLATE
                  .replace('{native_uuid}', native_uuid)
                  .replace('{fbuild_target}', name))
            else:
               schemes_file.write(NO_TARGETS_TEMPLATE
                  .replace('{native_uuid}', native_uuid)
                  .replace('{fbuild_uuid}', fbuild_uuid)
                  .replace('{fbuild_target}', name))

         if name in executables:
            # Ding ding ding, add a scheme
            schemes_dir = os.path.join(project, subdir, 'xcshareddata', 'xcschemes')
            os.makedirs(schemes_dir, exist_ok=True)
            with open(os.path.join(schemes_dir, f'{name}.xcscheme'), 'w') as scheme_file:
               scheme_file.write(SCHEME_TEMPLATE.replace('{target}', name))
      elif os.path.isdir(os.path.join(project, subdir)): # Recurse
         make_xcode_targets(os.path.join(project, subdir), executables)


if __name__ == '__main__':
   parser = ArgumentParser()
   parser.add_argument('project', help='XCode project directory')
   parser.add_argument('executables', nargs='*', help='Targets to create an executable reference for')

   args = parser.parse_args()

   make_xcode_targets(args.project, args.executables)
