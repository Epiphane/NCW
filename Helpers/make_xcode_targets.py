from argparse import ArgumentParser
import os
import getpass
import re
from typing import List

USERNAME = getpass.getuser()
SCHEMES = os.path.join('xcuserdata', f'{USERNAME}.xcuserdatad', 'xcschemes', 'xcschememanagement.plist')

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))

with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'SingleTarget.plist')) as template_file:
   SINGLE_TARGET_TEMPLATE = template_file.read()

with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'NoTargets.plist')) as template_file:
   NO_TARGETS_TEMPLATE = template_file.read()

with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'ExecutableTarget.xcscheme')) as template_file:
   SCHEME_TEMPLATE = template_file.read()

BUILD_SETTINGS = [
   '            CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;',
   '            CLANG_WARN_BOOL_CONVERSION = YES;',
   '            CLANG_WARN_COMMA = YES;',
   '            CLANG_WARN_CONSTANT_CONVERSION = YES;',
   '            CLANG_WARN_EMPTY_BODY = YES;',
   '            CLANG_WARN_ENUM_CONVERSION = YES;',
   '            CLANG_WARN_INFINITE_RECURSION = YES;',
   '            CLANG_WARN_INT_CONVERSION = YES;',
   '            CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;',
   '            CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;',
   '            CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;',
   '            CLANG_WARN_STRICT_PROTOTYPES = YES;',
   '            CLANG_WARN_SUSPICIOUS_MOVE = YES;',
   '            CLANG_WARN_UNREACHABLE_CODE = YES;',
   '            CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;',
   '            ENABLE_STRICT_OBJC_MSGSEND = YES;',
   '            ENABLE_TESTABILITY = YES;',
   '            GCC_NO_COMMON_BLOCKS = YES;',
   '            GCC_WARN_64_TO_32_BIT_CONVERSION = YES;',
   '            GCC_WARN_ABOUT_RETURN_TYPE = YES;',
   '            GCC_WARN_UNDECLARED_SELECTOR = YES;',
   '            GCC_WARN_UNINITIALIZED_AUTOS = YES;',
   '            GCC_WARN_UNUSED_FUNCTION = YES;',
   '            GCC_WARN_UNUSED_VARIABLE = YES;',
   '            ONLY_ACTIVE_ARCH = YES;',
]

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

         project_lines = []
         with open(os.path.join(project, subdir, 'project.pbxproj')) as proj_file:
            lines = proj_file.readlines()

            state = ''
            i = 0
            while i < len(lines):
               line = lines[i].strip('\n')
               # Protect strings
               line = re.sub(r'= ([A-Za-z0-9\/\+\_\.-]+)([\s;])', r'= "\1"\2', line)
               project_lines.append(line.strip('\n'))

               if state == 'PBXLegacyTarget':
                  fbuild_uuid = re.match(r'\s*([0-9]{24}) /\*.*\*/ = \{', line).groups()[0]
                  state = ''
               elif state == 'PBXNativeTarget':
                  native_uuid = re.match(r'\s*([0-9]{24}) /\*.*\*/ = \{', line).groups()[0]
                  state = ''
               elif state == 'XCBuildConfiguration Debug' and 'buildSettings' in line:
                  project_lines += BUILD_SETTINGS
                  state = 'XCBuildConfiguration Release'
               elif state == 'XCBuildConfiguration Release' and 'buildSettings' in line:
                  project_lines += BUILD_SETTINGS
                  state = ''
               elif 'Begin PBXLegacyTarget section' in line:
                  state = 'PBXLegacyTarget'
               elif 'Begin PBXNativeTarget section' in line:
                  state = 'PBXNativeTarget'
               elif 'Begin XCBuildConfiguration section' in line:
                  state = 'XCBuildConfiguration Debug'

               i += 1

         with open(os.path.join(project, subdir, 'project.pbxproj'), 'w') as new_proj_file:
            new_proj_file.write('\n'.join(project_lines))

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
               scheme_file.write(SCHEME_TEMPLATE.replace('{target}', name).replace('{root}', REPO_ROOT))
      elif os.path.isdir(os.path.join(project, subdir)): # Recurse
         make_xcode_targets(os.path.join(project, subdir), executables)


if __name__ == '__main__':
   parser = ArgumentParser()
   parser.add_argument('project', help='XCode project directory')
   parser.add_argument('executables', nargs='*', help='Targets to create an executable reference for')

   args = parser.parse_args()

   make_xcode_targets(args.project, args.executables)
