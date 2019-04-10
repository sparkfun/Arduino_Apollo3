# -*- mode: python -*-

block_cipher = None


a = Analysis(['artemis_uart_loader.py'],
             pathex=['C:\\Users\\nathan.seidle\\Dropbox\\Apps\\Arduino\\hardware\\SparkFun\\Arduino_Apollo3\\tools\\apollo3_scripts'],
             binaries=[],
             datas=[],
             hiddenimports=[],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher,
             noarchive=False)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,
          [],
          name='artemis_uart_loader',
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=True,
          runtime_tmpdir=None,
          console=True )
