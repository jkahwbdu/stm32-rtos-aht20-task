# -*- coding: utf-8 -*-
"""Build the submission zip: sources + report + docs + hex, no Keil objects."""
import os
import zipfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
DIST = os.path.join(ROOT, 'dist')
NAME = 'RTOS_AHT20_Task_632407030418.zip'

EXCLUDE_DIRS = {'__pycache__', '_pdf', '.git', 'dist'}
EXCLUDE_FILES = {'.DS_Store', 'Thumbs.db', 'desktop.ini'}
INCLUDE_BUILD = {
    'MDK-ARM/RTOS_AHT20_Task.uvprojx',
    'MDK-ARM/RTOS_AHT20_Task.uvoptx',
    'MDK-ARM/startup_stm32f103xb.s',
    'MDK-ARM/RTOS_AHT20_Task/RTOS_AHT20_Task.hex',
    'MDK-ARM/build.log',
    'build_log.txt',
}


def want(rel):
    rel = rel.replace('\\', '/')
    if rel in INCLUDE_BUILD:
        return True
    if rel.startswith('MDK-ARM/'):
        return False
    if rel.startswith('docs/images/'):
        return True
    return True


def main():
    os.makedirs(DIST, exist_ok=True)
    out = os.path.join(DIST, NAME)
    count = 0
    with zipfile.ZipFile(out, 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for dirpath, dirnames, filenames in os.walk(ROOT):
            dirnames[:] = [d for d in dirnames if d not in EXCLUDE_DIRS]
            for fn in filenames:
                if fn in EXCLUDE_FILES:
                    continue
                full = os.path.join(dirpath, fn)
                rel = os.path.relpath(full, ROOT).replace('\\', '/')
                if not want(rel):
                    continue
                if rel == 'dist/' + NAME:
                    continue
                z.write(full, os.path.join('RTOS_AHT20_Task_632407030418', rel))
                count += 1
    print('zip:', out)
    print('files:', count, 'size: %.2f MB' % (os.path.getsize(out) / 1048576.0))


if __name__ == '__main__':
    main()
