# KirikiriTools
Tools for the Kirikiri visual novel engine.

## KirikiriDescrambler
Some Kirikiri games have their plaintext scripts (.ks/.tjs) scrambled or compressed. Such files can be recognized by the signature "FE FE XX FF FE" at the start, XX being 00, 01 or 02. KirikiriDescrambler turns them into regular text files which can be placed right back in the game - no rescrambling needed.

## KirikiriUnencryptedArchive
A Kirikiri plugin (.tpm) which makes games accept unencrypted .xp3 archives. By using this plugin, it's no longer necessary to identify and replicate the encryption for each game you work on; just create an unencrypted archive with the Xp3Pack tool (included in this repository), throw the .tpm in the game's "plugin" folder, and you're done.

## KirikiriUnencryptedArchiveLauncher
Kirikiri games with the "CxDec" protection don't load .tpm plugins (they can be recognized by the absence of any .tpm files in the "plugin" folder). Because of this restriction, the KirikiriUnencryptedArchive plugin won't be loaded and unencrypted archives will be ignored. As a workaround, place this launcher .exe in the game's folder and use that rather than starting the game directly. It will find the Kirikiri .exe and launch it with the plugin already in memory.

For Steam games, make sure to uncomment the two SetEnvironmentVariable() calls in main.cpp and fill in the AppId (the number in the game's Steam store URL).

## Xp3Pack
Creates unencrypted .xp3 archives for use with the KirikiriUnencryptedArchive plugin. Unlike other packing tools, it sets all hashes in the file table to zero; this serves as a marker for the plugin to bypass the game's decryption for those files.

Typical usage is to place Xp3Pack.exe in the game folder, create a "patch" subfolder containing the files you want to include, and run "Xp3Pack patch" from the command line. This will create a patch.xp3 in the game folder. If the game already has its own patch.xp3, name your folder "patch2" and run "Xp3Pack patch2" instead. If the game already has a patch2.xp3, name your folder "patch3", and so on.
