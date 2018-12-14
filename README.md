# TextureMapper

Cross-platform (Linux, Windows) GUI tool for texture work in the Mass Effect games. Written in C++ using Qt5.

This is the third iteration of the project. Each iteration was an exercise I set for myself for learning purposes. This is the first one I am completely satisfied with. The other two :

- [The first](https://github.com/CreeperLava/MassEffectTextureMapper) (Java, JavaFX). Very inefficient memory-wise, slow, uses too many third-party libraries. Partly due to my inexperience at the time, partly because it's Java.
- [The second](https://github.com/CreeperLava/TextureMapperRuby) (Ruby, GTK3). Ruby is great, but a script language, and thus not compileable and easily redistributable.

**Introduction**

This program is intended to facilitate modding of the Mass Effect trilogy (ME1, ME2, ME3).

Each texture in the trilogy is uniquely identified with a hash (a hexadecimal number ie 0x12345678). Many textures are reused across several games, sometimes exactly the same, sometimes only with a resize or a small color change between one game and the other.

AlvaroMe and myself have built a database of such textures, partly using image comparison software, partly by comparing textures manually. There are currently around 8300 groups of duplicate textures (each texture in a group being very similar to the others in the same group).

**Goal**

The goal of the tool is to show texture mod authors where their textures can be reused, in the same game or in others. That way you get the most out of the textures you made.

You can for example point the TextureMapper to a folder where your textures are, and tell it to port them from ME3 to ME2. It will go through your textures, find the hashes within their names, and from this build a list of the duplicates in ME2 of your ME3 textures. Optionally, it can even copy your textures into a ME2 folder, and rename them to their ME2 name and hash.

**Requirements**

The tool works both on Linux and Windows. Linux requires Qt5. Windows has no requirements, unless you're compiling from source.

**Usage**

Download the executable from [github](https://github.com/CreeperLava/TextureMapper/releases/latest). Run the executable by double-clicking it.

There are 2 modes to the application :

If you just want to know the duplicates of a given hash, just paste or write down your hash in the left text box, select your game and click Go. The 2 options (Standalone, Copy), don't apply to that mode.

If you want to port existing textures to another game (eg find duplicates of ME3 textures in ME2) or the same game (eg find duplicates of ME3 textures in ME3 itself, since that's a thing) :

1. Browse for the files you wish to port from the top left dialog. The selected files will be displayed in the left pane, one per line. Once you've selected files, you can't edit the left pane anymore, until you hit the Clear button.

2. Select the options you need.

Standalone : show all the duplicates of a texture, including itself; otherwise show all the duplicates, excluding itself.

Copy : makes as many copies of your texture as there are duplicates, and renames those copies accordingly. There will be no overwriting of the files you selected, the ported textures will be pasted in a subfolder named ME1, ME2 or ME3 depending on the game you next choose.

3. Select the game you wish to port your textures to. The tool doesn't care from which game your hashes actually come from, only in which game it should find matching duplicates. You could mix hashes from ME1, ME2 and ME3 and it would work just fine.

4. Select the folder you want the ported textures to be copied into from the top right dialog. By default it is set to where you selected the files.

5. Click Go. The tool will match your hashes with the database, and display the results in the right pane. If you selected the copy option, it will copy the matches in a subfolder.
