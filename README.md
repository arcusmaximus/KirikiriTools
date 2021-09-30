# KirikiriTools
Tools for the Kirikiri visual novel engine. [Download page](https://github.com/arcusmaximus/KirikiriTools/releases)

If you're looking to translate a Kirikiri game, take a look at [VNTextPatch](https://github.com/arcusmaximus/VNTranslationTools).

## KirikiriDescrambler
Some Kirikiri games have their plaintext scripts (.ks/.tjs) scrambled or compressed. Such files can be recognized by the following signatures at the start:
```
FE FE 00 FF FE
FE FE 01 FF FE
FE FE 02 FF FE
```
KirikiriDescrambler turns these into regular text files which can be placed right back in the game - no rescrambling needed.

## KirikiriUnencryptedArchive
A DLL (named "version.dll") that makes games accept unencrypted .xp3 archives. By using this file, it's no longer necessary to identify and replicate the encryption for each game you work on; just create an unencrypted archive with the Xp3Pack tool (included in this repository), throw the version.dll in the game's folder, and you're done.

The DLL produces debug messages that can be seen with Microsoft's [DebugView](https://docs.microsoft.com/en-us/sysinternals/downloads/debugview) tool - this can be handy for confirming it's working.

## Xp3Pack
Creates unencrypted .xp3 archives for use with the KirikiriUnencryptedArchive DLL. Unlike other packing tools, it sets all hashes in the file table to zero; this serves as a marker for the DLL to bypass the game's decryption for those files.

Typical usage is to place Xp3Pack.exe in the game folder, create a "patch" subfolder containing the files you want to include, and run "Xp3Pack patch" from the command line. This will create a patch.xp3 in the game folder. If the game already has its own patch.xp3, name your folder "patch2" and run "Xp3Pack patch2" instead. If the game already has a patch2.xp3, name your folder "patch3", and so on.
